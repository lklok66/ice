// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(module, exports, require){
        var AsyncStatus = require("Ice/AsyncStatus").Ice.AsyncStatus;
        var BasicStream = require("Ice/BasicStream").Ice.BasicStream;
        var ConnectionRequestHandler = require("Ice/ConnectionRequestHandler").Ice.ConnectionRequestHandler;
        var Debug = require("Ice/Debug").Ice.Debug;
        var ExUtil = require("Ice/ExUtil").Ice.ExUtil;
        var LocalExceptionWrapper = require("Ice/LocalExceptionWrapper").Ice.LocalExceptionWrapper;
        var OutgoingAsync = require("Ice/OutgoingAsync").Ice.OutgoingAsync;
        var Protocol = require("Ice/Protocol").Ice.Protocol;
        var ReferenceMode = require("Ice/ReferenceMode").Ice.ReferenceMode;
        var LocalException = require("Ice/Exception").Ice.LocalException;
        var Promise = require("Ice/Promise").Ice.Promise;

        var ConnectRequestHandler = function(ref, proxy)
        {
            this._reference = ref;
            this._response = ref.getMode() === ReferenceMode.ModeTwoway;
            this._proxy = proxy;
            this._batchAutoFlush = ref.getInstance().initializationData().properties.getPropertyAsIntWithDefault(
                "Ice.BatchAutoFlush", 1) > 0 ? true : false;
            this._initialized = false;
            this._flushing = false;
            this._batchRequestInProgress = false;
            this._batchRequestsSize = Protocol.requestBatchHdr.length;
            this._batchStream =
                new BasicStream(ref.getInstance(), Protocol.currentProtocolEncoding, this._batchAutoFlush);
            this._updateRequestHandler = false;

            this._connection = null;
            this._compress = false;
            this._exception = null;
            this._requests = [];
            this._updateRequestHandler = false;
            this._pendingPromises = [];
        };

        ConnectRequestHandler.prototype.connect = function()
        {
            var self = this;
            this._reference.getConnection().then(
                function(connection, compress)
                {
                    self.setConnection(connection, compress);
                }).exception(
                    function(ex)
                    {
                        self.setException(ex);
                    });

            if(this.initialized())
            {
                Debug.assert(this._connection !== null);
                return new ConnectionRequestHandler(this._reference, this._connection, this._compress);
            }
            else
            {
                // The proxy request handler will be updated when the connection is set.
                this._updateRequestHandler = true;
                return this;
            }
        };

        ConnectRequestHandler.prototype.prepareBatchRequest = function(os)
        {
            if(!this.initialized())
            {
                this._batchRequestInProgress = true;
                this._batchStream.swap(os);
                return;
            }

            this._connection.prepareBatchRequest(os);
        };

        ConnectRequestHandler.prototype.finishBatchRequest = function(os)
        {
            if(!this.initialized())
            {
                Debug.assert(this._batchRequestInProgress);
                this._batchRequestInProgress = false;

                this._batchStream.swap(os);

                if(!this._batchAutoFlush &&
                this._batchStream.size + this._batchRequestsSize > this._reference.getInstance().messageSizeMax())
                {
                    ExUtil.throwMemoryLimitException(this._batchStream.size + this._batchRequestsSize,
                                                    this._reference.getInstance().messageSizeMax());
                }

                this._requests.push(new Request(this._batchStream));
                return;
            }
            this._connection.finishBatchRequest(os, this._compress);
        };

        ConnectRequestHandler.prototype.abortBatchRequest = function()
        {
            if(!this.initialized())
            {
                Debug.assert(this._batchRequestInProgress);
                this._batchRequestInProgress = false;

                var dummy = new BasicStream(this._reference.getInstance(), Protocol.currentProtocolEncoding,
                                            this._batchAutoFlush);
                this._batchStream.swap(dummy);
                this._batchRequestsSize = Protocol.requestBatchHdr.length;

                return;
            }
            this._connection.abortBatchRequest();
        };

        ConnectRequestHandler.prototype.sendAsyncRequest = function(out)
        {
            if(!this.initialized())
            {
                this._requests.push(new Request(out));
                return AsyncStatus.Queued;
            }
            return this._connection.sendAsyncRequest(out, this._compress, this._response);
        };

        ConnectRequestHandler.prototype.flushAsyncBatchRequests = function(out)
        {
            if(!this.initialized())
            {
                this._requests.push(new Request(out));
                return AsyncStatus.Queued;
            }
            return this._connection.flushAsyncBatchRequests(out);
        };

        ConnectRequestHandler.prototype.getReference = function()
        {
            return this._reference;
        };

        ConnectRequestHandler.prototype.getConnection = function()
        {
            if(this._exception !== null)
            {
                throw this._exception;
            }
            else
            {
                Debug.assert(this._initialized);
                return this._connection;
            }
        };

        ConnectRequestHandler.prototype.onConnection = function()
        {
            //
            // Called by ObjectPrx.ice_getConnection
            //

            if(this._exception !== null)
            {
                return Promise.fail(this._exception);
            }
            else if(this._connection !== null)
            {
                Debug.assert(this._initialized);
                return Promise.succeed(this._connection);
            }
            else
            {
                var promise = new Promise();
                this._pendingPromises.push(promise);
                return promise;
            }
        };

        //
        // Implementation of Reference_GetConnectionCallback
        //

        ConnectRequestHandler.prototype.setConnection = function(connection, compress)
        {
            Debug.assert(this._exception === null && this._connection === null);
            Debug.assert(this._updateRequestHandler || this._requests.length === 0);

            this._connection = connection;
            this._compress = compress;

            //
            // If this proxy is for a non-local object, and we are using a router, then
            // add this proxy to the router info object.
            //
            var ri = this._reference.getRouterInfo();
            if(ri !== null)
            {
                var self = this;
                var promise = ri.addProxy(this._proxy).then(
                    function()
                    {
                        //
                        // The proxy was added to the router info, we're now ready to send the
                        // queued requests.
                        //
                        self.flushRequests();
                    }).exception(
                        function(ex)
                        {
                            self.setException(ex);
                        });

                if(!promise.completed())
                {
                    return; // The request handler will be initialized once addProxy completes.
                }
            }

            //
            // We can now send the queued requests.
            //
            this.flushRequests();
        };

        ConnectRequestHandler.prototype.setException = function(ex)
        {
            Debug.assert(!this._initialized && this._exception === null);
            Debug.assert(this._updateRequestHandler || this._requests.length === 0);

            this._exception = ex;
            this._proxy = null; // Break cyclic reference count.

            //
            // If some requests were queued, we notify them of the failure.
            //
            if(this._requests.length > 0)
            {
                this.flushRequestsWithException(ex);
            }

            for(var i = 0; i < this._pendingPromises.length; ++i)
            {
                this._pendingPromises[i].fail(ex);
            }
            this._pendingPromises = [];
        };

        ConnectRequestHandler.prototype.initialized = function()
        {
            if(this._initialized)
            {
                Debug.assert(this._connection !== null);
                return true;
            }
            else
            {
                if(this._exception !== null)
                {
                    throw this._exception;
                }
                else
                {
                    return this._initialized;
                }
            }
        };

        ConnectRequestHandler.prototype.flushRequests = function()
        {
            Debug.assert(this._connection !== null && !this._initialized);

            //
            // We set the _flushing flag to true to prevent any additional queuing. Callers
            // might block for a little while as the queued requests are being sent but this
            // shouldn't be an issue as the request sends are non-blocking.
            //
            this._flushing = true;

            var sentCallbacks = [];
            try
            {
                while(this._requests.length > 0)
                {
                    var request = this._requests[0];
                    if(request.out !== null)
                    {
                        if((this._connection.sendAsyncRequest(request.out, this._compress, this._response) &
                            AsyncStatus.InvokeSentCallback) > 0)
                        {
                            sentCallbacks.push(request.out);
                        }
                    }
                    else if(request.batchOut !== null)
                    {
                        if((this._connection.flushAsyncBatchRequests(request.batchOut) &
                            AsyncStatus.InvokeSentCallback) > 0)
                        {
                            sentCallbacks.push(request.batchOut);
                        }
                    }
                    else
                    {
                        var os = new BasicStream(request.os.instance(), Protocol.currentProtocolEncoding);
                        this._connection.prepareBatchRequest(os);
                        try
                        {
                            request.os.pos = 0;
                            os.writeBlob(request.os.readBlob(request.os.size));
                        }
                        catch(ex)
                        {
                            this._connection.abortBatchRequest();
                            throw ex;
                        }
                        this._connection.finishBatchRequest(os, this._compress);
                    }
                    this._requests.shift();
                }
            }
            catch(ex)
            {
                if(ex instanceof LocalExceptionWrapper)
                {
                    Debug.assert(this._exception === null && this._requests.length > 0);
                    this._exception = ex.inner;
                    this.flushRequestsWithExceptionWrapper(ex);
                }
                else if(ex instanceof LocalException)
                {
                    Debug.assert(this._exception === null && this._requests.length > 0);
                    this._exception = ex;
                    this.flushRequestsWithException(ex);
                }
                else
                {
                    throw ex;
                }
            }

            // TODO: sentCallbacks
            if(sentCallbacks.length > 0)
            {
            }

            //
            // We've finished sending the queued requests and the request handler now send
            // the requests over the connection directly. It's time to substitute the
            // request handler of the proxy with the more efficient connection request
            // handler which does not have any synchronization. This also breaks the cyclic
            // reference count with the proxy.
            //
            // NOTE: _updateRequestHandler is immutable once _flushing = true
            //
            if(this._updateRequestHandler && this._exception === null)
            {
                this._proxy.__setRequestHandler(
                    new ConnectionRequestHandler(this._reference, this._connection, this._compress));
            }

            Debug.assert(!this._initialized);
            if(this._exception === null)
            {
                this._initialized = true;
                this._flushing = false;
            }
            this._proxy = null; // Break cyclic reference count.

            for(var i = 0; i < this._pendingPromises.length; ++i)
            {
                this._pendingPromises[i].succeed(this._connection);
            }
            this._pendingPromises = [];
        };

        ConnectRequestHandler.prototype.flushRequestsWithException = function(ex)
        {
            for(var i = 0; i < this._requests.length; ++i)
            {
                var request = this._requests[i];
                if(request.out !== null)
                {
                    request.out.__finishedEx(ex, false);
                }
                else if(request.batchOut !== null)
                {
                    request.batchOut.__finishedEx(ex, false);
                }
            }
            this._requests = [];
        };

        ConnectRequestHandler.prototype.flushRequestsWithExceptionWrapper = function(ex)
        {
            for(var i = 0; i < this._requests.length; ++i)
            {
                var request = this._requests[i];
                if(request.out !== null)
                {
                    request.out.__finishedWrapper(ex);
                }
                else if(request.batchOut !== null)
                {
                    request.batchOut.__finishedEx(ex.inner, false);
                }
            }
            this._requests = [];
        };

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.ConnectRequestHandler = ConnectRequestHandler;

        var Request = function(arg)
        {
            if(arg instanceof BasicStream)
            {
                this.os = new BasicStream(arg.instance(), Protocol.currentProtocolEncoding);
                this.os.swap(arg);
            }
            else if(arg instanceof OutgoingAsync)
            {
                this.out = arg;
            }
            else
            {
                Debug.assert(arg instanceof BatchOutgoingAsync);
                this.batchOut = arg;
            }
        };
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ConnectRequestHandler"));
