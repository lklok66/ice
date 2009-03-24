// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;

namespace IceInternal
{
    public class ConnectRequestHandler : RequestHandler, Reference.GetConnectionCallback, RouterInfo.AddProxyCallback
    {
        private class Request
        {
            internal Request(BasicStream os)
            {
                this.os = new BasicStream(os.instance());
                this.os.swap(os);
            }

            internal Request(OutgoingAsync @out)
            {
                this.@out = @out;
            }

            internal Request(BatchOutgoingAsync @out)
            {
                this.batchOut = @out;
            }

            internal OutgoingAsync @out = null;
            internal BatchOutgoingAsync batchOut = null;
            internal BasicStream os = null;
        }

        public RequestHandler connect()
        {
            _reference.getConnection(this);

            lock(this)
            {
                if(initialized())
                {
                    Debug.Assert(_connection != null);
                    return new ConnectionRequestHandler(_reference, _connection, _compress);
                }
                else
                {
                    // The proxy request handler will be updated when the connection is set.
                    _updateRequestHandler = true; 
                    return this;
                }
            }
        }

        public void prepareBatchRequest(BasicStream os)
        {
            lock(this)
            {
                while(_batchRequestInProgress)
                {
                    Monitor.Wait(this);
                }

                if(!initialized())
                {
                    _batchStream.swap(os);
                    _batchRequestInProgress = true;
                    return;
                }
            }
            _connection.prepareBatchRequest(os);
        }

        public void finishBatchRequest(BasicStream os)
        {
            lock(this)
            {
                if(!initialized())
                {
                    Debug.Assert(_batchRequestInProgress);
                    _batchRequestInProgress = false;
                    Monitor.PulseAll(this);

                    _batchStream.swap(os);

                    if(!_batchAutoFlush &&
                       _batchStream.size() + _batchRequestsSize > _reference.getInstance().messageSizeMax())
                    {
                        throw new Ice.MemoryLimitException();
                    }
                    _requests.AddLast(new Request(_batchStream));
                    return;
                }
            }
            _connection.finishBatchRequest(os, _compress);
        }

        public void abortBatchRequest()
        {
            lock(this)
            {
                if(!initialized())
                {
                    Debug.Assert(_batchRequestInProgress);
                    _batchRequestInProgress = false;
                    Monitor.PulseAll(this);

                    BasicStream dummy = new BasicStream(_reference.getInstance(), _batchAutoFlush);
                    _batchStream.swap(dummy);
                    _batchRequestsSize = Protocol.requestBatchHdr.Length;
                    return;
                }
            }
            _connection.abortBatchRequest();
        }

        public Ice.ConnectionI sendRequest(Outgoing @out)
        {
            if(!getConnection(true).sendRequest(@out, _compress, _response) || _response)
            {
                return _connection; // The request has been sent or we're expecting a response.
            }
            else
            {
                return null; // The request hasn't been sent yet.
            }
        }

        public bool sendAsyncRequest(OutgoingAsync @out)
        {
            lock(this)
            {
                if(!initialized())
                {
                    _requests.AddLast(new Request(@out));
                    return false;
                }
            }            
            return _connection.sendAsyncRequest(@out, _compress, _response);
        }

        public bool flushBatchRequests(BatchOutgoing @out)
        {
            return getConnection(true).flushBatchRequests(@out);
        }

        public bool flushAsyncBatchRequests(BatchOutgoingAsync @out)
        {
            lock(this)
            {
                if(!initialized())
                {
                    _requests.AddLast(new Request(@out));
                    return false;
                }
            }
            return _connection.flushAsyncBatchRequests(@out);
        }

        public Outgoing getOutgoing(string operation, Ice.OperationMode mode, Dictionary<string, string> context)
        {
            lock(this)
            {
                if(!initialized())
                {
                    return new IceInternal.Outgoing(this, operation, mode, context);
                }
            }

            return _connection.getOutgoing(this, operation, mode, context);
        }

        public void reclaimOutgoing(Outgoing og)
        {
            lock(this)
            {
                if(_connection == null)
                {
                    return;
                }
            }

            _connection.reclaimOutgoing(og);
        }

        public Reference getReference()
        {
            return _reference;
        }

        public Ice.ConnectionI getConnection(bool wait)
        {
            lock(this)
            {
                if(wait)
                {
                    //
                    // Wait for the connection establishment to complete or fail.
                    //
                    while(!_initialized && _exception == null)
                    {
                        Monitor.Wait(this);
                    }
                }

                if(_exception != null)
                {
                    throw _exception;
                }
                else
                {
                    Debug.Assert(!wait || _initialized);
                    return _connection;
                }
            }
        }

        //
        // Implementation of Reference.GetConnectionCallback
        //

        public void setConnection(Ice.ConnectionI connection, bool compress)
        {
            lock(this)
            {
                Debug.Assert(_connection == null && _exception == null);
                _connection = connection;
                _compress = compress;
            }

            //
            // If this proxy is for a non-local object, and we are using a router, then
            // add this proxy to the router info object.
            //
            RouterInfo ri = _reference.getRouterInfo();
            if(ri != null && !ri.addProxy(_proxy, this))
            {
                return; // The request handler will be initialized once addProxy returns.
            }

            //
            // We can now send the queued requests.
            //
            flushRequests();
        }

        public void setException(Ice.LocalException ex)
        {
            lock(this)
            {
                Debug.Assert(!_initialized && _exception == null);
                Debug.Assert(_updateRequestHandler || _requests.Count == 0);

                _exception = ex;
                _proxy = null; // Break cyclic reference count.
                _delegate = null; // Break cyclic reference count.

                //
                // If some requests were queued, we notify them of the failure. This is done from a thread
                // from the client thread pool since this will result in ice_exception callbacks to be 
                // called.
                //
                if(_requests.Count > 0)
                {
                    _reference.getInstance().clientThreadPool().execute(delegate(bool unused)
                                                                        {
                                                                            flushRequestsWithException(ex);
                                                                        });
                }

                Monitor.PulseAll(this);
            }
        }

        //
        // Implementation of RouterInfo.AddProxyCallback
        //
        public void addedProxy()
        {
            //
            // The proxy was added to the router info, we're now ready to send the 
            // queued requests.
            //
            flushRequests();
        }

        public ConnectRequestHandler(Reference @ref, Ice.ObjectPrx proxy, Ice.ObjectDelM_ del)
        {
            _reference = @ref;
            _response = _reference.getMode() == Reference.Mode.ModeTwoway;
            _proxy = (Ice.ObjectPrxHelperBase)proxy;
            _delegate = del;
            _batchAutoFlush = @ref.getInstance().initializationData().properties.getPropertyAsIntWithDefault(
                "Ice.BatchAutoFlush", 1) > 0 ? true : false;
            _batchStream = new BasicStream(@ref.getInstance(), _batchAutoFlush);
            _batchRequestInProgress = false;
            _batchRequestsSize = Protocol.requestBatchHdr.Length;
            _updateRequestHandler = false;
        }

        private bool initialized()
        {
            if(_initialized)
            {
                Debug.Assert(_connection != null);
                return true;
            }
            else
            {
                while(_flushing && _exception == null)
                {
                    Monitor.Wait(this);
                }

                if(_exception != null)
                {
                    throw _exception;
                }
                else
                {
                    return _initialized;
                }
            }
        }

        private void flushRequests()
        {
            lock(this)
            {
                Debug.Assert(_connection != null && !_initialized);

                if(_batchRequestInProgress)
                {
                    Monitor.Wait(this);
                }

                //
                // We set the _flushing flag to true to prevent any additional queuing. Callers
                // might block for a little while as the queued requests are being sent but this
                // shouldn't be an issue as the request sends are non-blocking.
                //
                _flushing = true;
            }

            LinkedList<OutgoingAsyncMessageCallback> sentCallbacks = 
                new LinkedList<OutgoingAsyncMessageCallback>();
            try
            {
                LinkedListNode<Request> p = _requests.First; // _requests is immutable when _flushing = true
                while(p != null)
                {
                    Request request = p.Value;
                    if(request.@out != null)
                    {
                        if(_connection.sendAsyncRequest(request.@out, _compress, _response))
                        {
                            if(request.@out is Ice.AMISentCallback)
                            {
                                sentCallbacks.AddLast(request.@out);
                            }
                        }
                    }
                    else if(request.batchOut != null)
                    {
                        if(_connection.flushAsyncBatchRequests(request.batchOut))
                        {
                            if(request.batchOut is Ice.AMISentCallback)
                            {
                                sentCallbacks.AddLast(request.batchOut);
                            }
                        }
                    }
                    else
                    {
                        BasicStream os = new BasicStream(request.os.instance());
                        _connection.prepareBatchRequest(os);
                        try
                        {
                            request.os.pos(0);
                            os.writeBlob(request.os.readBlob(request.os.size()));
                            _connection.finishBatchRequest(os, _compress);
                        }
                        catch(Ice.LocalException)
                        {
                            _connection.abortBatchRequest();
                            throw;
                        }
                    }
                    LinkedListNode<Request> tmp = p;
                    p = p.Next;
                    _requests.Remove(tmp);
                }
            }
            catch(LocalExceptionWrapper ex)
            {
                lock(this)
                {
                    Debug.Assert(_exception == null && _requests.Count > 0);
                    _exception = ex.get();
                    _reference.getInstance().clientThreadPool().execute(delegate(bool unused)
                                                                        {
                                                                            flushRequestsWithException(ex);
                                                                        });
                }
            }
            catch(Ice.LocalException ex)
            {
                lock(this)
                {
                    Debug.Assert(_exception == null && _requests.Count > 0);
                    _exception = ex;
                    _reference.getInstance().clientThreadPool().execute(delegate(bool unused)
                                                                        {
                                                                            flushRequestsWithException(ex);
                                                                        });
                }
            }

            if(sentCallbacks.Count > 0)
            {
                Instance instance = _reference.getInstance();
                instance.clientThreadPool().execute(delegate(bool unsused)
                                                    {
                                                        foreach(OutgoingAsyncMessageCallback callback in sentCallbacks)
                                                        {
                                                            callback.sent__(instance);
                                                        }
                                                    });
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
            if(_updateRequestHandler && _exception == null)
            {
                _proxy.setRequestHandler__(_delegate, new ConnectionRequestHandler(_reference, _connection, _compress));
            }

            lock(this)
            {
                Debug.Assert(!_initialized);
                if(_exception == null)
                {
                    _initialized = true;
                    _flushing = false;
                }
                _proxy = null; // Break cyclic reference count.
                _delegate = null; // Break cyclic reference count.
                Monitor.PulseAll(this);
            }
        }

        private void
        flushRequestsWithException(Ice.LocalException ex)
        {
            foreach(Request request in _requests)
            {
                if(request.@out != null)
                {
                    request.@out.finished__(ex);
                }
                else if(request.batchOut != null)
                {
                    request.batchOut.finished__(ex);
                }
            }
            _requests.Clear();
        }

        private void
        flushRequestsWithException(LocalExceptionWrapper ex)
        {
            foreach(Request request in _requests)
            {
                if(request.@out != null)
                {
                    request.@out.finished__(ex);
                }
                else if(request.batchOut != null)
                {
                    request.batchOut.finished__(ex.get());
                }
            }
            _requests.Clear();
        }

        private Reference _reference;
        private bool _batchAutoFlush;
        private Ice.ObjectPrxHelperBase _proxy;
        private Ice.ObjectDelM_ _delegate;
        private bool _initialized = false;
        private bool _flushing = false;
        private Ice.ConnectionI _connection = null;
        private bool _compress = false;
        private bool _response;
        private Ice.LocalException _exception = null;

        private LinkedList<Request> _requests = new LinkedList<Request>();
        private bool _batchRequestInProgress;
        private int _batchRequestsSize;
        private BasicStream _batchStream;
        private bool _updateRequestHandler;
    }
}
