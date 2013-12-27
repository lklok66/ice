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
        var Debug = require("Ice/Debug").Ice.Debug;
        var ExUtil = require("Ice/ExUtil").Ice.ExUtil;
        var HashMap = require("Ice/HashMap").Ice.HashMap;
        var LocalExceptionWrapper = require("Ice/LocalExceptionWrapper").Ice.LocalExceptionWrapper;
        var Promise = require("Ice/Promise").Ice.Promise;
        var Protocol = require("Ice/Protocol").Ice.Protocol;
        var SocketOperation = require("Ice/SocketOperation").Ice.SocketOperation;
        var Timer = require("Ice/Timer").Ice.Timer;
        var TimeUtil = require("Ice/TimeUtil").Ice.TimeUtil;
        var TraceUtil = require("Ice/TraceUtil").Ice.TraceUtil;
        var ProtocolVersion = require("Ice/Version").Ice.ProtocolVersion;
        var EncodingVersion = require("Ice/Version").Ice.EncodingVersion;

        var _merge = require("Ice/Util").merge;

        var Ice = {};

        _merge(Ice, require("Ice/Exception").Ice);
        _merge(Ice, require("Ice/LocalException").Ice);

        var StateNotInitialized = 0;
        var StateNotValidated = 1;
        var StateActive = 2;
        var StateHolding = 3;
        var StateClosing = 4;
        var StateClosed = 5;
        var StateFinished = 6;

        var ConnectionI = function(communicator, instance, reaper, transceiver, endpoint, incoming, adapter)
        {
            this._communicator = communicator;
            this._instance = instance;
            this._reaper = reaper;
            this._transceiver = transceiver;
            this._desc = transceiver.toString();
            this._type = transceiver.type();
            this._endpoint = endpoint;
            this._incoming = incoming;
            this._adapter = adapter;
            var initData = instance.initializationData();
            this._logger = initData.logger; // Cached for better performance.
            this._traceLevels = instance.traceLevels(); // Cached for better performance.
            this._timer = instance.timer();
            this._writeTimeoutId = 0;
            this._writeTimeoutScheduled = false;
            this._readTimeoutId = 0;
            this._readTimeoutScheduled = false;

            this._hasMoreData = { value: false };

            this._warn = initData.properties.getPropertyAsInt("Ice.Warn.Connections") > 0;
            this._warnUdp = instance.initializationData().properties.getPropertyAsInt("Ice.Warn.Datagrams") > 0;
            this._acmAbsoluteTimeoutMillis = 0;

            this._nextRequestId = 1;
            this._batchAutoFlush = initData.properties.getPropertyAsIntWithDefault("Ice.BatchAutoFlush", 1) > 0 ? true : false;
            this._batchStream = new BasicStream(instance, Protocol.currentProtocolEncoding, this._batchAutoFlush);
            this._batchStreamInUse = false;
            this._batchRequestNum = 0;
            this._batchRequestCompress = false;
            this._batchMarker = 0;

            this._sendStreams = [];

            this._readStream = new BasicStream(instance, Protocol.currentProtocolEncoding);
            this._readHeader = false;
            this._writeStream = new BasicStream(instance, Protocol.currentProtocolEncoding);

            this._readStreamPos = -1;
            this._writeStreamPos = -1;

            this._dispatchCount = 0;

            this._state = StateNotInitialized;
            this._shutdownInitiated = false;
            this._validated = false;

            this._readProtocol = new ProtocolVersion();
            this._readProtocolEncoding = new EncodingVersion();

            this._asyncRequests = new HashMap(); // Map<int, OutgoingAsync>

            this._exception = null;

            this._startPromise = null;
            this._closePromises = [];
            this._holdPromises = [];
            this._finishedPromises = [];

            if(this._adapter !== null)
            {
                this._servantManager = this._adapter.getServantManager();
            }
            else
            {
                this._servantManager = null;
            }

            if(this._endpoint.datagram())
            {
                this._acmTimeout = 0;
            }
            else
            {
                if(this._adapter !== null)
                {
                    this._acmTimeout = this._adapter.getACM();
                }
                else
                {
                    this._acmTimeout = this._instance.clientACM();
                }
            }
        };

        ConnectionI.prototype.start = function()
        {
            Debug.assert(this._startPromise === null);

            try
            {
                if(this._state >= StateClosed) // The connection might already be closed if the communicator was destroyed.
                {
                    Debug.assert(this._exception !== null);
                    return Promise.fail(this._exception);
                }

                this._startPromise = new Promise();
                var self = this;
                this._transceiver.setCallbacks(
                    function() { self.transceiverConnected(); },
                    function() { self.transceiverBytesAvailable(); },
                    function() { self.transceiverBytesWritten(); },
                    function() { self.transceiverClosed(); },
                    function(ex) { self.transceiverError(ex); }
                );
                this.initialize();
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.exception(ex);
                }
                return Promise.fail(ex);
            }

            return this._startPromise;
        };

        ConnectionI.prototype.activate = function()
        {
            if(this._state <= StateNotValidated)
            {
                return;
            }

            if(this._acmTimeout > 0)
            {
                this._acmAbsoluteTimeoutMillis = TimeUtil.now() + this._acmTimeout * 1000;
            }

            this.setState(StateActive);
        };

        ConnectionI.prototype.hold = function()
        {
            if(this._state <= StateNotValidated)
            {
                return;
            }

            this.setState(StateHolding);
        };

        // DestructionReason.
        ConnectionI.ObjectAdapterDeactivated = 0;
        ConnectionI.CommunicatorDestroyed = 1;

        ConnectionI.prototype.destroy = function(reason)
        {
            switch(reason)
            {
                case ConnectionI.ObjectAdapterDeactivated:
                {
                    this.setStateEx(StateClosing, new Ice.ObjectAdapterDeactivatedException());
                    break;
                }

                case ConnectionI.CommunicatorDestroyed:
                {
                    this.setStateEx(StateClosing, new Ice.CommunicatorDestroyedException());
                    break;
                }
            }
        };

        ConnectionI.prototype.close = function(force)
        {
            var promise = new Promise();

            if(force)
            {
                this.setStateEx(StateClosed, new Ice.ForcedCloseConnectionException());
                promise.succeed();
            }
            else
            {
                //
                // If we do a graceful shutdown, then we wait until all
                // outstanding requests have been completed. Otherwise,
                // the CloseConnectionException will cause all outstanding
                // requests to be retried, regardless of whether the
                // server has processed them or not.
                //
                if(this._closePromises === null)
                {
                    this._closePromises = [];
                }
                this._closePromises.push(promise);
                this.checkClose();
            }

            return promise;
        };

        ConnectionI.prototype.checkClose = function()
        {
            //
            // If close(false) has been called, then we need to check if all
            // requests have completed and we can transition to StateClosing.
            // We also complete outstanding promises.
            //
            if(this._asyncRequests.length === 0 && this._closePromises !== null)
            {
                this.setStateEx(StateClosing, new Ice.CloseConnectionException());
                for(var i = 0; i < this._closePromises.length; ++i)
                {
                    this._closePromises[i].succeed();
                }
                this._closePromises = null;
            }
        };

        ConnectionI.prototype.isActiveOrHolding = function()
        {
            return this._state > StateNotValidated && this._state < StateClosing;
        };

        ConnectionI.prototype.isFinished = function()
        {
            if(this._state !== StateFinished || this._dispatchCount !== 0)
            {
                return false;
            }

            Debug.assert(this._state === StateFinished);
            return true;
        };

        ConnectionI.prototype.throwException = function()
        {
            if(this._exception !== null)
            {
                Debug.assert(this._state >= StateClosing);
                throw this._exception;
            }
        };

        ConnectionI.prototype.waitUntilHolding = function()
        {
            var promise = new Promise();
            this._holdPromises.push(promise);
            this.checkState();
            return promise;
        };

        ConnectionI.prototype.waitUntilFinished = function()
        {
            var promise = new Promise();
            this._finishedPromises.push(promise);
            this.checkState();
            return promise;
        };

        ConnectionI.prototype.monitor = function(now)
        {
            if(this._state !== StateActive)
            {
                return;
            }

            //
            // Active connection management for idle connections.
            //
            if(this._acmTimeout <= 0 ||
            this._asyncRequests.length > 0 || this._dispatchCount > 0 ||
            this._readStream.size > Protocol.headerSize || !this._writeStream.isEmpty() || !this._batchStream.isEmpty())
            {
                return;
            }

            if(now >= this._acmAbsoluteTimeoutMillis)
            {
                this.setStateEx(StateClosing, new Ice.ConnectionTimeoutException());
            }
        };

        ConnectionI.prototype.sendAsyncRequest = function(out, compress, response)
        {
            var requestId = 0;
            var os = out.__os();

            if(this._exception !== null)
            {
                //
                // If the connection is closed before we even have a chance
                // to send our request, we always try to send the request
                // again.
                //
                throw new LocalExceptionWrapper(this._exception, true);
            }

            Debug.assert(this._state > StateNotValidated);
            Debug.assert(this._state < StateClosing);

            //
            // Ensure the message isn't bigger than what we can send with the
            // transport.
            //
            this._transceiver.checkSendSize(os, this._instance.messageSizeMax());

            if(response)
            {
                //
                // Create a new unique request ID.
                //
                requestId = this._nextRequestId++;
                if(requestId <= 0)
                {
                    this._nextRequestId = 1;
                    requestId = this._nextRequestId++;
                }

                //
                // Fill in the request ID.
                //
                os.pos = Protocol.headerSize;
                os.writeInt(requestId);
            }

            var status;
            try
            {
                status = this.sendMessage(OutgoingMessage.create(out, out.__os(), compress, requestId));
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.setStateEx(StateClosed, ex);
                    Debug.assert(this._exception !== null);
                    throw this._exception;
                }
                else
                {
                    throw ex;
                }
            }

            if(response)
            {
                //
                // Add to the async requests map.
                //
                this._asyncRequests.set(requestId, out);
            }

            return status;
        };

        ConnectionI.prototype.prepareBatchRequest = function(os)
        {
            if(this._exception !== null)
            {
                //
                // If there were no batch requests queued when the connection failed, we can safely
                // retry with a new connection. Otherwise, we must throw to notify the caller that
                // some previous batch requests were not sent.
                //
                if(this._batchStream.isEmpty())
                {
                    throw new LocalExceptionWrapper(this._exception, true);
                }
                else
                {
                    throw this._exception;
                }
            }

            Debug.assert(this._state > StateNotValidated);
            Debug.assert(this._state < StateClosing);

            if(this._batchStream.isEmpty())
            {
                try
                {
                    this._batchStream.writeBlob(Protocol.requestBatchHdr);
                }
                catch(ex)
                {
                    if(ex instanceof Ice.LocalException)
                    {
                        this.setStateEx(StateClosed, ex);
                    }
                    throw ex;
                }
            }

            this._batchStreamInUse = true;
            this._batchMarker = this._batchStream.size;
            this._batchStream.swap(os);

            //
            // The batch stream now belongs to the caller, until
            // finishBatchRequest() or abortBatchRequest() is called.
            //
        };

        ConnectionI.prototype.finishBatchRequest = function(os, compress)
        {
            try
            {
                //
                // Get the batch stream back.
                //
                this._batchStream.swap(os);

                if(this._exception !== null)
                {
                    throw this._exception;
                }

                var flush = false;
                if(this._batchAutoFlush)
                {
                    //
                    // Throw memory limit exception if the first message added causes us to go over
                    // limit. Otherwise put aside the marshalled message that caused limit to be
                    // exceeded and rollback stream to the marker.
                    try
                    {
                        this._transceiver.checkSendSize(this._batchStream.buffer, this._instance.messageSizeMax());
                    }
                    catch(ex)
                    {
                        if(ex instanceof Ice.LocalException)
                        {
                            if(this._batchRequestNum > 0)
                            {
                                flush = true;
                            }
                            else
                            {
                                throw ex;
                            }
                        }
                        else
                        {
                            throw ex;
                        }
                    }
                }

                if(flush)
                {
                    //
                    // Temporarily save the last request.
                    //
                    var sz = this._batchStream.size - this._batchMarker;
                    this._batchStream.pos = this._batchMarker;
                    var lastRequest = this._batchStream.readBlob(sz);
                    this._batchStream.resize(this._batchMarker, false);

                    try
                    {
                        //
                        // Fill in the number of requests in the batch.
                        //
                        this._batchStream.pos = Protocol.headerSize;
                        this._batchStream.writeInt(this._batchRequestNum);

                        this.sendMessage(OutgoingMessage.createForStream(this._batchStream,
                                                                         this._batchRequestCompress, true));
                    }
                    catch(ex)
                    {
                        if(ex instanceof Ice.LocalException)
                        {
                            this.setStateEx(StateClosed, ex);
                            Debug.assert(this._exception !== null);
                            throw this._exception;
                        }
                        else
                        {
                            throw ex;
                        }
                    }

                    //
                    // Reset the batch stream.
                    //
                    this._batchStream = new BasicStream(this._instance, Protocol.currentProtocolEncoding, this._batchAutoFlush);
                    this._batchRequestNum = 0;
                    this._batchRequestCompress = false;
                    this._batchMarker = 0;

                    //
                    // Check again if the last request doesn't exceed the maximum message size.
                    //
                    if(Protocol.requestBatchHdr.length + lastRequest.length >  this._instance.messageSizeMax())
                    {
                        ExUtil.throwMemoryLimitException(
                            Protocol.requestBatchHdr.length + lastRequest.length,
                            this._instance.messageSizeMax());
                    }

                    //
                    // Start a new batch with the last message that caused us to go over the limit.
                    //
                    this._batchStream.writeBlob(Protocol.requestBatchHdr);
                    this._batchStream.writeBlob(lastRequest);
                }

                //
                // Increment the number of requests in the batch.
                //
                ++this._batchRequestNum;

                //
                // We compress the whole batch if there is at least one compressed
                // message.
                //
                if(compress)
                {
                    this._batchRequestCompress = true;
                }

                //
                // The batch stream is not in use anymore.
                //
                Debug.assert(this._batchStreamInUse);
                this._batchStreamInUse = false;
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.abortBatchRequest();
                }
                throw ex;
            }
        };

        ConnectionI.prototype.abortBatchRequest = function()
        {
            this._batchStream = new BasicStream(this._instance, Protocol.currentProtocolEncoding, this._batchAutoFlush);
            this._batchRequestNum = 0;
            this._batchRequestCompress = false;
            this._batchMarker = 0;

            Debug.assert(this._batchStreamInUse);
            this._batchStreamInUse = false;
        };

        var __flushBatchRequests_name = "flushBatchRequests";

        ConnectionI.prototype.flushBatchRequests = function()
        {
            var promise = new Promise();
            /* TODO: Derive a class from Promise?
            var result:IceInternal.ConnectionBatchOutgoingAsync =
                new IceInternal.ConnectionBatchOutgoingAsync(this, this._communicator, this._instance, __flushBatchRequests_name);
            try
            {
                result.__send();
            }
            catch(ex:LocalException)
            {
                result._this._exception(ex);
            }
            return result;
            */
            return promise;
        };

        ConnectionI.prototype.flushAsyncBatchRequests = function(outAsync)
        {
            if(this._exception !== null)
            {
                throw this._exception;
            }

            var status;
            if(this._batchRequestNum === 0)
            {
                status = AsyncStatus.Sent;
                if(!outAsync.__sent(this))
                {
                    status |= AsyncStatus.InvokeSentCallback;
                }
                return status;
            }

            //
            // Fill in the number of requests in the batch.
            //
            this._batchStream.pos = Protocol.headerSize;
            this._batchStream.writeInt(this._batchRequestNum);

            this._batchStream.swap(outAsync.__os());

            
            try
            {
                status = this.sendMessage(OutgoingMessage.create(outAsync, outAsync.__os(), this._batchRequestCompress,
                                                                 0));
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.setStateEx(StateClosed, ex);
                    Debug.assert(this._exception !== null);
                    throw this._exception;
                }
                else
                {
                    throw ex;
                }
            }

            //
            // Reset the batch stream.
            //
            this._batchStream = new BasicStream(this._instance, Protocol.currentProtocolEncoding, this._batchAutoFlush);
            this._batchRequestNum = 0;
            this._batchRequestCompress = false;
            this._batchMarker = 0;
            return status;
        };

        ConnectionI.prototype.sendResponse = function(os, compressFlag)
        {
            Debug.assert(this._state > StateNotValidated);

            try
            {
                if(--this._dispatchCount === 0)
                {
                    if(this._state === StateFinished)
                    {
                        this._reaper.add(this);
                    }
                    this.checkState();
                }

                if(this._state >= StateClosed)
                {
                    Debug.assert(this._exception !== null);
                    throw this._exception;
                }

                this.sendMessage(OutgoingMessage.createForStream(os, compressFlag !== 0, true));

                if(this._state === StateClosing && this._dispatchCount === 0)
                {
                    this.initiateShutdown();
                }
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.setStateEx(StateClosed, ex);
                }
                else
                {
                    throw ex;
                }
            }
        };

        ConnectionI.prototype.sendNoResponse = function()
        {
            Debug.assert(this._state > StateNotValidated);
            try
            {
                if(--this._dispatchCount === 0)
                {
                    if(this._state === StateFinished)
                    {
                        this._reaper.add(this);
                    }
                    this.checkState();
                }

                if(this._state >= StateClosed)
                {
                    Debug.assert(this._exception !== null);
                    throw this._exception;
                }

                if(this._state === StateClosing && this._dispatchCount === 0)
                {
                    this.initiateShutdown();
                }
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.setStateEx(StateClosed, ex);
                }
                else
                {
                    throw ex;
                }
            }
        };

        ConnectionI.prototype.endpoint = function()
        {
            return this._endpoint;
        };

        ConnectionI.prototype.setAdapter = function(adapter)
        {
            if(this._state <= StateNotValidated || this._state >= StateClosing)
            {
                return;
            }
            Debug.assert(this._state < StateClosing);

            this._adapter = adapter;

            if(this._adapter !== null)
            {
                this._servantManager = this._adapter.getServantManager();
                if(this._servantManager === null)
                {
                    this._adapter = null;
                }
            }
            else
            {
                this._servantManager = null;
            }
        };

        ConnectionI.prototype.getAdapter = function()
        {
            return this._adapter;
        };

        ConnectionI.prototype.getEndpoint = function()
        {
            return this._endpoint;
        };

        ConnectionI.prototype.createProxy = function(ident)
        {
            //
            // Create a reference and return a reverse proxy for this
            // reference.
            //
            return this._instance.proxyFactory().referenceToProxy(
                this._instance.referenceFactory().createFixed(ident, this));
        };

        ConnectionI.prototype.message = function(operation)
        {
            if(this._state >= StateClosed)
            {
                return;
            }

            if((operation & SocketOperation.Write) !== 0)
            {
                // TODO: Anything to do here?
                Debug.assert(this._writeStream.buffer.remaining === 0);
            }

            //
            // Keep reading until no more data is available.
            //
            this._hasMoreData.value = true;
            do
            {
                var info = null;

                try
                {
                    if((operation & SocketOperation.Read) !== 0 && !this._readStream.isEmpty())
                    {
                        if(this._readHeader) // Read header if necessary.
                        {
                            if(!this._transceiver.read(this._readStream.buffer, this._hasMoreData))
                            {
                                //
                                // We didn't get enough data to complete the header.
                                //
                                return;
                            }

                            Debug.assert(this._readStream.buffer.remaining === 0);
                            this._readHeader = false;

                            var pos = this._readStream.pos;
                            if(pos < Protocol.headerSize)
                            {
                                //
                                // This situation is possible for small UDP packets.
                                //
                                throw new Ice.IllegalMessageSizeException();
                            }

                            this._readStream.pos = 0;
                            var magic0 = this._readStream.readByte();
                            var magic1 = this._readStream.readByte();
                            var magic2 = this._readStream.readByte();
                            var magic3 = this._readStream.readByte();
                            if(magic0 !== Protocol.magic[0] || magic1 !== Protocol.magic[1] ||
                            magic2 !== Protocol.magic[2] || magic3 !== Protocol.magic[3])
                            {
                                var bme = new Ice.BadMagicException();
                                /* TODO: Fix for sequence<byte> mapping
                                const m:flash.utils.ByteArray = new flash.utils.ByteArray();
                                m.endian = flash.utils.Endian.LITTLE_ENDIAN;
                                m.length = 4;
                                m[0] = magic0;
                                m[1] = magic1;
                                m[2] = magic2;
                                m[3] = magic3;
                                bme.badMagic = m;
                                */
                                throw bme;
                            }

                            this._readProtocol.__read(this._readStream);
                            Protocol.checkSupportedProtocol(this._readProtocol);

                            this._readProtocolEncoding.__read(this._readStream);
                            Protocol.checkSupportedProtocolEncoding(this._readProtocolEncoding);

                            this._readStream.readByte(); // messageType
                            this._readStream.readByte(); // compress
                            var size = this._readStream.readInt();
                            if(size < Protocol.headerSize)
                            {
                                throw new Ice.IllegalMessageSizeException();
                            }
                            if(size > this._instance.messageSizeMax())
                            {
                                ExUtil.throwMemoryLimitException(size, this._instance.messageSizeMax());
                            }
                            if(size > this._readStream.size)
                            {
                                this._readStream.resize(size);
                            }
                            this._readStream.pos = pos;
                        }

                        if(this._readStream.pos != this._readStream.size)
                        {
                            if(this._endpoint.datagram())
                            {
                                throw new Ice.DatagramLimitException(); // The message was truncated.
                            }
                            else
                            {
                                if(!this._transceiver.read(this._readStream.buffer, this._hasMoreData))
                                {
                                    Debug.assert(!this._readStream.isEmpty());
                                    this.scheduleTimeout(SocketOperation.Read, this._endpoint.timeout());
                                    return;
                                }
                                Debug.assert(this._readStream.buffer.remaining === 0);
                            }
                        }
                    }

                    if(this._state <= StateNotValidated)
                    {
                        if(this._state === StateNotInitialized && !this.initialize())
                        {
                            return;
                        }

                        if(this._state <= StateNotValidated && !this.validate())
                        {
                            return;
                        }

                        this._transceiver.unregister();

                        //
                        // We start out in holding state.
                        //
                        this.setState(StateHolding);
                    }
                    else
                    {
                        Debug.assert(this._state <= StateClosing);

                        //
                        // We parse messages first, if we receive a close
                        // connection message we won't send more messages.
                        //
                        if((operation & SocketOperation.Read) !== 0)
                        {
                            info = this.parseMessage();
                        }

                        if((operation & SocketOperation.Write) !== 0)
                        {
                            this.sendNextMessage();
                        }

                        //
                        // We increment the dispatch count to prevent the
                        // communicator destruction during the callback.
                        //
                        if(info !== null && info.outAsync !== null)
                        {
                            ++this._dispatchCount;
                        }
                    }
                }
                catch(ex)
                {
                    if(ex instanceof Ice.DatagramLimitException) // Expected.
                    {
                        if(this._warnUdp)
                        {
                            this._logger.warning("maximum datagram size of " + this._readStream.pos + " exceeded");
                        }
                        this._readStream.resize(Protocol.headerSize);
                        this._readStream.pos = 0;
                        this._readHeader = true;
                        return;
                    }
                    else if(ex instanceof Ice.SocketException)
                    {
                        this.setStateEx(StateClosed, ex);
                        return;
                    }
                    else if(ex instanceof Ice.LocalException)
                    {
                        if(this._endpoint.datagram())
                        {
                            if(this._warn)
                            {
                                this._logger.warning("datagram connection exception:\n" + ex + '\n' + this._desc);
                            }
                            this._readStream.resize(Protocol.headerSize);
                            this._readStream.pos = 0;
                            this._readHeader = true;
                        }
                        else
                        {
                            this.setStateEx(StateClosed, ex);
                        }
                        return;
                    }
                    else
                    {
                        throw ex;
                    }
                }

                if(this._acmTimeout > 0)
                {
                    this._acmAbsoluteTimeoutMillis = TimeUtil.now() + this._acmTimeout * 1000;
                }

                this.dispatch(info);
            }
            while(this._hasMoreData.value);
        };

        //
        // Transceiver callbacks
        //
        ConnectionI.prototype.transceiverConnected = function()
        {
            this.message(SocketOperation.Write);
        };

        ConnectionI.prototype.transceiverBytesAvailable = function()
        {
            this.message(SocketOperation.Read);
        };

        ConnectionI.prototype.transceiverBytesWritten = function()
        {
            this.message(SocketOperation.Write);
        };

        ConnectionI.prototype.transceiverClosed = function()
        {
            this.setStateEx(StateClosed, new Ice.ConnectionLostException());
        };

        ConnectionI.prototype.transceiverError = function(ex)
        {
            this.setStateEx(StateClosed, ex);
        };

        ConnectionI.prototype.dispatch = function(info)
        {
            //
            // Notify the factory that the connection establishment and
            // validation has completed.
            //
            if(this._startPromise !== null)
            {
                this._startPromise.succeed();
                this._startPromise = null;
            }

            if(info !== null)
            {
                if(info.outAsync !== null)
                {
                    info.outAsync.__finished(info.stream);
                }

                if(info.invokeNum > 0)
                {
                    this.invokeAll(info.stream, info.invokeNum, info.requestId, info.compress, info.servantManager,
                                info.adapter);
                }
            }

            //
            // Decrease dispatch count.
            //
            if(info !== null && info.outAsync !== null)
            {
                if(--this._dispatchCount === 0)
                {
                    if(this._state === StateClosing && !this._shutdownInitiated)
                    {
                        try
                        {
                            this.initiateShutdown();
                        }
                        catch(ex)
                        {
                            if(ex instanceof Ice.LocalException)
                            {
                                this.setStateEx(StateClosed, ex);
                            }
                            else
                            {
                                throw ex;
                            }
                        }
                    }
                    else if(this._state === StateFinished)
                    {
                        this._reaper.add(this);
                    }
                    this.checkState();
                }
            }
        };

        ConnectionI.prototype.finish = function()
        {
            Debug.assert(this._state === StateClosed);
            this.unscheduleTimeout(SocketOperation.Read | SocketOperation.Write | SocketOperation.Connect);

            if(this._startPromise !== null)
            {
                this._startPromise.fail(this._exception);
                this._startPromise = null;
            }

            if(this._sendStreams.length > 0)
            {
                if(!this._writeStream.isEmpty())
                {
                    //
                    // Return the stream to the outgoing call. This is important for
                    // retriable AMI calls which are not marshalled again.
                    //
                    var message = this._sendStreams[0];
                    this._writeStream.swap(message.stream);
                }

                //
                // NOTE: for twoway requests which are not sent, finished can be called twice: the
                // first time because the outgoing is in the _sendStreams set and the second time
                // because it's either in the _requests/_asyncRequests set. This is fine, only the
                // first call should be taken into account by the implementation of finished.
                //
                for(var i = 0; i < this._sendStreams.length; ++i)
                {
                    var p = this._sendStreams[i];
                    if(p.requestId > 0)
                    {
                        this._asyncRequests.delete(p.requestId);
                    }
                    p.finished(this._exception);
                }
                this._sendStreams = [];
            }

            for(var e = this._asyncRequests.entries; e !== null; e = e.next)
            {
                e.value.__finishedEx(this._exception, true);
            }
            this._asyncRequests.clear();

            //
            // This must be done last as this will cause waitUntilFinished() to return (and communicator
            // objects such as the timer might be destroyed too).
            //
            if(this._dispatchCount === 0)
            {
                this._reaper.add(this);
            }
            this.setState(StateFinished);
        };

        ConnectionI.prototype.toString = function()
        {
            return this._desc;
        };

        ConnectionI.prototype.timedOut = function(event)
        {
            if(this._state <= StateNotValidated)
            {
                this.setStateEx(StateClosed, new Ice.ConnectTimeoutException());
            }
            else if(this._state < StateClosing)
            {
                this.setStateEx(StateClosed, new Ice.TimeoutException());
            }
            else if(this._state === StateClosing)
            {
                this.setStateEx(StateClosed, new Ice.CloseTimeoutException());
            }
        };

        ConnectionI.prototype.type = function()
        {
            return this._type;
        };

        ConnectionI.prototype.timeout = function()
        {
            return this._endpoint.timeout();
        };

        ConnectionI.prototype.getInfo = function()
        {
            if(this._state >= StateClosed)
            {
                throw this._exception;
            }
            var info = this._transceiver.getInfo();
            info.adapterName = this._adapter !== null ? this._adapter.getName() : "";
            info.incoming = this._incoming;
            return info;
        };

        ConnectionI.prototype.exception = function(ex)
        {
            this.setStateEx(StateClosed, ex);
        };

        ConnectionI.prototype.invokeException = function(ex, invokeNum)
        {
            //
            // Fatal exception while invoking a request. Since sendResponse/sendNoResponse isn't
            // called in case of a fatal exception we decrement this._dispatchCount here.
            //

            this.setStateEx(StateClosed, ex);

            if(invokeNum > 0)
            {
                Debug.assert(this._dispatchCount > 0);
                this._dispatchCount -= invokeNum;
                Debug.assert(this._dispatchCount >= 0);
                if(this._dispatchCount === 0)
                {
                    if(this._state === StateFinished)
                    {
                        this._reaper.add(this);
                    }
                    this.checkState();
                }
            }
        };

        ConnectionI.prototype.setStateEx = function(state, ex)
        {
            Debug.assert(ex instanceof Ice.LocalException);

            //
            // If setState() is called with an exception, then only closed
            // and closing states are permissible.
            //
            Debug.assert(state >= StateClosing);

            if(this._state === state) // Don't switch twice.
            {
                return;
            }

            if(this._exception === null)
            {
                this._exception = ex;

                //
                // We don't warn if we are not validated.
                //
                if(this._warn && this._validated)
                {
                    //
                    // Don't warn about certain expected exceptions.
                    //
                    if(!(this._exception instanceof Ice.CloseConnectionException ||
                        this._exception instanceof Ice.ForcedCloseConnectionException ||
                        this._exception instanceof Ice.ConnectionTimeoutException ||
                        this._exception instanceof Ice.CommunicatorDestroyedException ||
                        this._exception instanceof Ice.ObjectAdapterDeactivatedException ||
                        (this._exception instanceof Ice.ConnectionLostException && this._state === StateClosing)))
                    {
                        this.warning("connection exception", this._exception);
                    }
                }
            }

            //
            // We must set the new state before we notify requests of any
            // exceptions. Otherwise new requests may retry on a
            // connection that is not yet marked as closed or closing.
            //
            this.setState(state);
        };

        ConnectionI.prototype.setState = function(state)
        {
            //
            // We don't want to send close connection messages if the endpoint
            // only supports oneway transmission from client to server.
            //
            if(this._endpoint.datagram() && state === StateClosing)
            {
                state = StateClosed;
            }

            //
            // Skip graceful shutdown if we are destroyed before validation.
            //
            if(this._state <= StateNotValidated && state === StateClosing)
            {
                state = StateClosed;
            }

            if(this._state === state) // Don't switch twice.
            {
                return;
            }

            try
            {
                switch(state)
                {
                case StateNotInitialized:
                {
                    Debug.assert(false);
                    break;
                }

                case StateNotValidated:
                {
                    if(this._state !== StateNotInitialized)
                    {
                        Debug.assert(this._state === StateClosed);
                        return;
                    }
                    //
                    // Register to receive validation message.
                    //
                    if(!this._endpoint.datagram() && !this._incoming)
                    {
                        //
                        // Once validation is complete, a new connection starts out in the
                        // Holding state. We only want to register the transceiver now if we
                        // need to receive data in order to validate the connection.
                        //
                        this._transceiver.register();
                    }
                    break;
                }

                case StateActive:
                {
                    //
                    // Can only switch from holding or not validated to
                    // active.
                    //
                    if(this._state !== StateHolding && this._state !== StateNotValidated)
                    {
                        return;
                    }
                    this._transceiver.register();
                    break;
                }

                case StateHolding:
                {
                    //
                    // Can only switch from active or not validated to
                    // holding.
                    //
                    if(this._state !== StateActive && this._state !== StateNotValidated)
                    {
                        return;
                    }
                    if(this._state === StateActive)
                    {
                        this._transceiver.unregister();
                    }
                    break;
                }

                case StateClosing:
                {
                    //
                    // Can't change back from closed.
                    //
                    if(this._state >= StateClosed)
                    {
                        return;
                    }
                    if(this._state === StateHolding)
                    {
                        // We need to continue to read in closing state.
                        this._transceiver.register();
                    }
                    break;
                }

                case StateClosed:
                {
                    if(this._state === StateFinished)
                    {
                        return;
                    }
                    this._transceiver.unregister();
                    break;
                }

                case StateFinished:
                {
                    Debug.assert(this._state === StateClosed);
                    this._transceiver.close();
                    this._communicator = null;
                    break;
                }
                }
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    var msg = "unexpected connection exception:\n " + this._desc + "\n" + ExUtil.toString(ex);
                    this._instance.initializationData().logger.error(msg);
                }
                else
                {
                    throw ex;
                }
            }

            //
            // We only register with the connection monitor if our new state
            // is StateActive. Otherwise we unregister with the connection
            // monitor, but only if we were registered before, i.e., if our
            // old state was StateActive.
            //
            if(this._acmTimeout > 0)
            {
                if(state === StateActive)
                {
                    this._instance.connectionMonitor().add(this);
                }
                else if(this._state === StateActive)
                {
                    this._instance.connectionMonitor().remove(this);
                }
            }

            this._state = state;

            if(this._state === StateClosing && this._dispatchCount === 0)
            {
                try
                {
                    this.initiateShutdown();
                }
                catch(ex)
                {
                    if(ex instanceof Ice.LocalException)
                    {
                        this.setStateEx(StateClosed, ex);
                    }
                    else
                    {
                        throw ex;
                    }
                }
            }
            else if(this._state === StateClosed)
            {
                this.finish();
            }

            this.checkState();
        };

        ConnectionI.prototype.initiateShutdown = function()
        {
            Debug.assert(this._state === StateClosing);
            Debug.assert(this._dispatchCount === 0);
            Debug.assert(!this._shutdownInitiated);

            if(!this._endpoint.datagram())
            {
                //
                // Before we shut down, we send a close connection
                // message.
                //
                var os = new BasicStream(this._instance, Protocol.currentProtocolEncoding, false);
                os.writeBlob(Protocol.magic);
                Protocol.currentProtocol.__write(os);
                Protocol.currentProtocolEncoding.__write(os);
                os.writeByte(Protocol.closeConnectionMsg);
                os.writeByte(0); // compression status: always report 0 for CloseConnection.
                os.writeInt(Protocol.headerSize); // Message size.

                var status = this.sendMessage(OutgoingMessage.createForStream(os, false, false));
                if((status & AsyncStatus.Sent) > 0)
                {
                    //
                    // Schedule the close timeout to wait for the peer to close the connection.
                    //
                    this.scheduleTimeout(SocketOperation.Write, this.closeTimeout());
                }

                //
                // The CloseConnection message should be sufficient. Closing the write
                // end of the socket is probably an artifact of how things were done
                // in IIOP. In fact, shutting down the write end of the socket causes
                // problems on Windows by preventing the peer from using the socket.
                // For example, the peer is no longer able to continue writing a large
                // message after the socket is shutdown.
                //
                //this._transceiver.shutdownWrite();
            }
        };

        ConnectionI.prototype.initialize = function()
        {
            var s = this._transceiver.initialize(this._readStream.buffer, this._writeStream.buffer);
            if(s != SocketOperation.None)
            {
                this.scheduleTimeout(s, this.connectTimeout());
                return false;
            }

            //
            // Update the connection description once the transceiver is initialized.
            //
            this._desc = this._transceiver.toString();
            this.setState(StateNotValidated);
            return true;
        };

        ConnectionI.prototype.validate = function()
        {
            if(!this._endpoint.datagram()) // Datagram connections are always implicitly validated.
            {
                if(this._adapter !== null) // The server side has the active role for connection validation.
                {
                    if(this._writeStream.size === 0)
                    {
                        this._writeStream.writeBlob(Protocol.magic);
                        Protocol.currentProtocol.__write(this._writeStream);
                        Protocol.currentProtocolEncoding.__write(this._writeStream);
                        this._writeStream.writeByte(Protocol.validateConnectionMsg);
                        this._writeStream.writeByte(0); // Compression status (always zero for validate connection).
                        this._writeStream.writeInt(Protocol.headerSize); // Message size.
                        TraceUtil.traceSend(this._writeStream, this._logger, this._traceLevels);
                        this._writeStream.prepareWrite();
                    }

                    if(this._writeStream.pos != this._writeStream.size &&
                    !this._transceiver.write(this._writeStream.buffer))
                    {
                        this.scheduleTimeout(SocketOperation.Write, this.connectTimeout());
                        return false;
                    }
                }
                else // The client side has the passive role for connection validation.
                {
                    if(this._readStream.size === 0)
                    {
                        this._readStream.resize(Protocol.headerSize);
                        this._readStream.pos = 0;
                    }

                    if(this._readStream.pos !== this._readStream.size &&
                    !this._transceiver.read(this._readStream.buffer, this._hasMoreData))
                    {
                        this.scheduleTimeout(SocketOperation.Read, this.connectTimeout());
                        return false;
                    }

                    Debug.assert(this._readStream.pos === Protocol.headerSize);
                    this._readStream.pos = 0;
                    var m = this._readStream.readBlob(4);
                    if(m[0] !== Protocol.magic[0] || m[1] !== Protocol.magic[1] ||
                    m[2] !== Protocol.magic[2] || m[3] !== Protocol.magic[3])
                    {
                        var bme = new Ice.BadMagicException();
                        bme.badMagic = m;
                        throw bme;
                    }

                    this._readProtocol.__read(this._readStream);
                    Protocol.checkSupportedProtocol(this._readProtocol);

                    this._readProtocolEncoding.__read(this._readStream);
                    Protocol.checkSupportedProtocolEncoding(this._readProtocolEncoding);

                    var messageType = this._readStream.readByte();
                    if(messageType !== Protocol.validateConnectionMsg)
                    {
                        throw new Ice.ConnectionNotValidatedException();
                    }
                    this._readStream.readByte(); // Ignore compression status for validate connection.
                    var size = this._readStream.readInt();
                    if(size !== Protocol.headerSize)
                    {
                        throw new Ice.IllegalMessageSizeException();
                    }
                    TraceUtil.traceRecv(this._readStream, this._logger, this._traceLevels);

                    this._validated = true;
                }
            }

            this._writeStream.resize(0);
            this._writeStream.pos = 0;

            this._readStream.resize(Protocol.headerSize);
            this._readHeader = true;
            this._readStream.pos = 0;

            return true;
        };

        ConnectionI.prototype.sendNextMessage = function()
        {
            Debug.assert(this._sendStreams.length > 0);
            Debug.assert(!this._writeStream.isEmpty() && this._writeStream.pos === this._writeStream.size);

            var callbacks = []; // TODO: Use sent callbacks?
            try
            {
                while(true)
                {
                    //
                    // Notify the message that it was sent.
                    //
                    var message = this._sendStreams.shift();
                    this._writeStream.swap(message.stream);
                    if(message.sent(this, true))
                    {
                        callbacks.push(message);
                    }

                    //
                    // If there's nothing left to send, we're done.
                    //
                    if(this._sendStreams.length === 0)
                    {
                        break;
                    }

                    //
                    // If we are in the closed state, don't continue sending.
                    //
                    // The connection can be in the closed state if parseMessage
                    // (called before sendNextMessage by message()) closes the
                    // connection.
                    //
                    if(this._state >= StateClosed)
                    {
                        return callbacks;
                    }

                    //
                    // Otherwise, prepare the next message stream for writing.
                    //
                    message = this._sendStreams[0];
                    Debug.assert(!message.prepared);
                    var stream = message.stream;

                    stream.pos = 10;
                    stream.writeInt(stream.size);
                    stream.prepareWrite();
                    message.prepared = true;

                    if(message.outAsync !== null)
                    {
                        TraceUtil.trace("sending asynchronous request", stream, this._logger, this._traceLevels);
                    }
                    else
                    {
                        TraceUtil.traceSend(stream, this._logger, this._traceLevels);
                    }
                    this._writeStream.swap(message.stream);

                    //
                    // Send the message.
                    //
                    if(this._writeStream.pos != this._writeStream.size &&
                    !this._transceiver.write(this._writeStream.buffer))
                    {
                        Debug.assert(!this._writeStream.isEmpty());
                        this.scheduleTimeout(SocketOperation.Write, this._endpoint.timeout());
                        return callbacks;
                    }
                }
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.setState(StateClosed, ex);
                    return callbacks;
                }
                else
                {
                    throw ex;
                }
            }

            Debug.assert(this._writeStream.isEmpty());

            //
            // If all the messages were sent and we are in the closing state, we schedule
            // the close timeout to wait for the peer to close the connection.
            //
            if(this._state === StateClosing)
            {
                this.scheduleTimeout(SocketOperation.Write, this.closeTimeout());
            }

            return callbacks;
        };

        ConnectionI.prototype.sendMessage = function(message)
        {
            Debug.assert(this._state < StateClosed);

            Debug.assert(!message.prepared);

            var stream = message.stream;
            stream.pos = 10;
            stream.writeInt(stream.size);
            stream.prepareWrite();
            message.prepared = true;

            TraceUtil.trace("sending asynchronous request", message.stream, this._logger, this._traceLevels);

            if(this._transceiver.write(message.stream.buffer))
            {
                //
                // Entire buffer was written immediately.
                //
                var status = AsyncStatus.Sent;
                if(message.sent(this, false))
                {
                    status |= AsyncStatus.InvokeSentCallback;
                }
                if(this._acmTimeout > 0)
                {
                    this._acmAbsoluteTimeoutMillis = TimeUtil.now() + this._acmTimeout * 1000;
                }
                return status;
            }
            message.adopt();

            this._writeStream.swap(message.stream);
            this._sendStreams.push(message);
            this.scheduleTimeout(SocketOperation.Write, this._endpoint.timeout());
            return AsyncStatus.Queued;
        };

        var MessageInfo = function(instance)
        {
            this.stream = new BasicStream(instance, Protocol.currentProtocolEncoding, false);

            this.invokeNum = 0;
            this.requestId = 0;
            this.compress = false;
            this.servantManager = null;
            this.adapter = null;
            this.outAsync = null;
        };

        ConnectionI.prototype.parseMessage = function()
        {
            Debug.assert(this._state > StateNotValidated && this._state < StateClosed);

            var info = new MessageInfo(this._instance);

            this._readStream.swap(info.stream);
            this._readStream.resize(Protocol.headerSize);
            this._readStream.pos = 0;
            this._readHeader = true;

            //
            // Connection is validated on first message. This is only used by
            // setState() to check wether or not we can print a connection
            // warning (a client might close the connection forcefully if the
            // connection isn't validated).
            //
            this._validated = true;

            Debug.assert(info.stream.pos === info.stream.size);

            try
            {
                //
                // We don't need to check magic and version here. This has already
                // been done by the caller.
                //
                info.stream.pos = 8;
                var messageType = info.stream.readByte();
                info.compress = info.stream.readByte();
                if(info.compress === 2)
                {
                    var ex = new Ice.FeatureNotSupportedException();
                    ex.unsupportedFeature = "Cannot uncompress compressed message";
                    throw ex;
                }
                info.stream.pos = Protocol.headerSize;

                switch(messageType)
                {
                    case Protocol.closeConnectionMsg:
                    {
                        TraceUtil.traceRecv(info.stream, this._logger, this._traceLevels);
                        if(this._endpoint.datagram())
                        {
                            if(this._warn)
                            {
                                this._logger.warning("ignoring close connection message for datagram connection:\n" +
                                                    this._desc);
                            }
                        }
                        else
                        {
                            this.setStateEx(StateClosed, new Ice.CloseConnectionException());
                        }
                        break;
                    }

                    case Protocol.requestMsg:
                    {
                        if(this._state === StateClosing)
                        {
                            TraceUtil.trace("received request during closing\n" +
                                            "(ignored by server, client will retry)",
                                            info.stream, this._logger, this._traceLevels);
                        }
                        else
                        {
                            TraceUtil.traceRecv(info.stream, this._logger, this._traceLevels);
                            info.requestId = info.stream.readInt();
                            info.invokeNum = 1;
                            info.servantManager = this._servantManager;
                            info.adapter = this._adapter;
                            ++this._dispatchCount;
                        }
                        break;
                    }

                    case Protocol.requestBatchMsg:
                    {
                        if(this._state === StateClosing)
                        {
                            TraceUtil.trace("received batch request during closing\n" +
                                            "(ignored by server, client will retry)",
                                            info.stream, this._logger, this._traceLevels);
                        }
                        else
                        {
                            TraceUtil.traceRecv(info.stream, this._logger, this._traceLevels);
                            info.invokeNum = info.stream.readInt();
                            if(info.invokeNum < 0)
                            {
                                info.invokeNum = 0;
                                throw new Ice.UnmarshalOutOfBoundsException();
                            }
                            info.servantManager = this._servantManager;
                            info.adapter = this._adapter;
                            this._dispatchCount += info.invokeNum;
                        }
                        break;
                    }

                    case Protocol.replyMsg:
                    {
                        TraceUtil.traceRecv(info.stream, this._logger, this._traceLevels);
                        info.requestId = info.stream.readInt();
                        info.outAsync = this._asyncRequests.get(info.requestId);
                        this._asyncRequests.delete(info.requestId);
                        if(info.outAsync === undefined)
                        {
                            throw new Ice.UnknownRequestIdException();
                        }
                        this.checkClose();
                        break;
                    }

                    case Protocol.validateConnectionMsg:
                    {
                        TraceUtil.traceRecv(info.stream, this._logger, this._traceLevels);
                        if(this._warn)
                        {
                            this._logger.warning("ignoring unexpected validate connection message:\n" + this._desc);
                        }
                        break;
                    }

                    default:
                    {
                        TraceUtil.trace("received unknown message\n(invalid, closing connection)",
                                        info.stream, this._logger, this._traceLevels);
                        throw new Ice.UnknownMessageException();
                    }
                }
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    if(this._endpoint.datagram())
                    {
                        if(this._warn)
                        {
                            this._logger.warning("datagram connection exception:\n" + ex + '\n' + this._desc);
                        }
                    }
                    else
                    {
                        this.setStateEx(StateClosed, ex);
                    }
                }
                else
                {
                    throw ex;
                }
            }

            return info;
        };

        ConnectionI.prototype.invokeAll = function(stream, invokeNum, requestId, compress, servantManager, adapter)
        {
            var inc = null;
            try
            {
                while(invokeNum > 0)
                {
                    //
                    // Prepare the invocation.
                    //
                    var response = !this._endpoint.datagram() && requestId !== 0;
                    inc = new Incoming(this._instance, this, adapter, response, compress, requestId);

                    //
                    // Dispatch the invocation.
                    //
                    inc.invoke(servantManager);

                    --invokeNum;
                    inc = null;
                }

                stream.clear();
            }
            catch(ex)
            {
                if(ex instanceof Ice.LocalException)
                {
                    this.invokeException(ex, invokeNum);
                }
                else if(ex instanceof Debug.AssertionFailedException)
                {
                    // Upon assertion, we print the stack trace.
                    var stackTrace = ""; // TODO: Can we obtain a stack trace?
                    var uex = new Ice.UnknownException(stackTrace, ex);
                    this._logger.error(uex.unknown);
                    this.invokeException(uex, invokeNum);
                }
                else
                {
                    throw ex;
                }
            }
        };

        ConnectionI.prototype.scheduleTimeout = function(op, timeout)
        {
            if(timeout < 0)
            {
                return;
            }

            var self = this;
            if((op & SocketOperation.Read) !== 0)
            {
                this._readTimeoutId = this._timer.schedule(timeout, function() { self.timedOut(); });
                this._readTimeoutScheduled = true;
            }
            if((op & (SocketOperation.Write | SocketOperation.Connect)) !== 0)
            {
                this._writeTimeoutId = this._timer.schedule(timeout, function() { self.timedOut(); });
                this._writeTimeoutScheduled = true;
            }
        };

        ConnectionI.prototype.unscheduleTimeout = function(op)
        {
            if((op & SocketOperation.Read) !== 0 && this._readTimeoutScheduled)
            {
                this._timer.cancel(this._readTimeoutId);
                this._readTimeoutScheduled = false;
            }
            if((op & (SocketOperation.Write | SocketOperation.Connect)) !== 0 && this._writeTimeoutScheduled)
            {
                this._timer.cancel(this._writeTimeoutId);
                this._writeTimeoutScheduled = false;
            }
        };

        ConnectionI.prototype.connectTimeout = function()
        {
            var defaultsAndOverrides = this._instance.defaultsAndOverrides();
            if(defaultsAndOverrides.overrideConnectTimeout)
            {
                return defaultsAndOverrides.overrideConnectTimeoutValue;
            }
            else
            {
                return this._endpoint.timeout();
            }
        };

        ConnectionI.prototype.closeTimeout = function()
        {
            var defaultsAndOverrides = this._instance.defaultsAndOverrides();
            if(defaultsAndOverrides.overrideCloseTimeout)
            {
                return defaultsAndOverrides.overrideCloseTimeoutValue;
            }
            else
            {
                return this._endpoint.timeout();
            }
        };

        ConnectionI.prototype.warning = function(msg, ex)
        {
            this._logger.warning(msg + ":\n" + this._desc + "\n" + ExUtil.toString(ex));
        };

        ConnectionI.prototype.checkState = function()
        {
            if(this._state < StateHolding || this._dispatchCount > 0)
            {
                return;
            }

            var i;
            if(this._holdPromises.length > 0)
            {
                for(i = 0; i < this._holdPromises.length; ++i)
                {
                    this._holdPromises[i].succeed();
                }
                this._holdPromises = [];
            }

            //
            // We aren't finished until the state is finished and all
            // outstanding requests are completed. Otherwise we couldn't
            // guarantee that there are no outstanding calls when deactivate()
            // is called on the servant locators.
            //
            if(this._state === StateFinished && this._finishedPromises.length > 0)
            {
                //
                // Clear the OA. See bug 1673 for the details of why this is necessary.
                //
                this._adapter = null;

                for(i = 0; i < this._finishedPromises.length; ++i)
                {
                    this._finishedPromises[i].succeed();
                }
                this._finishedPromises = [];
            }
        };

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.ConnectionI = ConnectionI;

        var OutgoingMessage = function()
        {
            this.stream = null;
            this.outAsync = null;
            this.compress = false;
            this.requestId = 0;
            this.prepared = false;
            this.isSent = false;
        };

        OutgoingMessage.createForStream = function(stream, compress, adopt)
        {
            var m = new OutgoingMessage();
            m.stream = stream;
            m.compress = compress;
            m.adopt = adopt;
            m.isSent = false;
            m.requestId = 0;
            m.outAsync = null;
            return m;
        };

        OutgoingMessage.create = function(out, stream, compress, requestId)
        {
            var m = new OutgoingMessage();
            m.stream = stream;
            m.compress = compress;
            m.outAsync = out;
            m.requestId = requestId;
            m.isSent = false;
            m.adopt = false;
            return m;
        };

        OutgoingMessage.prototype.adopt = function()
        {
            if(this.adopt)
            {
                var stream = new BasicStream(this.stream.instance(), Protocol.currentProtocolEncoding);
                stream.swap(this.stream);
                this.stream = stream;
                this.adopt = false;
            }
        };

        OutgoingMessage.prototype.sent = function(connection)
        {
            this.isSent = true; // The message is sent.

            if(this.outAsync !== null)
            {
                return this.outAsync.__sent(connection);
            }
            else
            {
                return false;
            }
        };

        OutgoingMessage.prototype.finished = function(ex)
        {
            if(this.outAsync !== null)
            {
                this.outAsync.__finishedEx(ex, this.isSent);
            }
        };
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ConnectionI"));
