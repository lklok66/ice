// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Ice;

import IceInternal.AsyncCallback;
import IceInternal.Protocol;
import IceInternal.Timer;

public final class ConnectionI implements Connection, IceInternal.TimerTask
{
    public interface StartCallback
    {
        void connectionStartCompleted(ConnectionI connection);

        void connectionStartFailed(ConnectionI connection, Ice.LocalException ex);
    }

    public void start(StartCallback callback)
    {
        try
        {
            synchronized(this)
            {
                // The connection might already be closed if the
                // communicator was destroyed.
                if(_state == StateClosed)
                {
                    assert (_exception != null);
                    throw _exception;
                }

                //
                // Use asynchronous I/O. We cannot begin an
                // asynchronous I/O request from this thread if a
                // callback is provided, so we queue a work item.
                //
                if(callback == null)
                {
                    initializeAsync(null);
                }
                else
                {
                    _startCallback = callback;
                    initializeAsync(null);
                    return;
                }

                while(_state <= StateNotValidated)
                {
                    try
                    {
                        wait();
                    }
                    catch(InterruptedException ex)
                    {
                    }
                }

                if(_state >= StateClosing)
                {
                    assert _exception != null;
                    throw _exception;
                }
            }
        }
        catch(LocalException ex)
        {
            exception(ex);
            if(callback != null)
            {
                callback.connectionStartFailed(this, _exception);
                return;
            }
            else
            {
                waitUntilFinished();
                throw ex;
            }
        }

        if(callback != null)
        {
            callback.connectionStartCompleted(this);
        }
    }

    public synchronized void activate()
    {
        if(_state <= StateNotValidated)
        {
            return;
        }

        if(_acmTimeout > 0)
        {
            _acmAbsoluteTimeoutMillis = IceInternal.Time.currentMonotonicTimeMillis() + _acmTimeout * 1000;
        }

        setState(StateActive);
    }

    public synchronized void hold()
    {
        if(_state <= StateNotValidated)
        {
            return;
        }

        setState(StateHolding);
    }

    // DestructionReason.
    public final static int ObjectAdapterDeactivated = 0;
    public final static int CommunicatorDestroyed = 1;

    synchronized public void destroy(int reason)
    {
        switch (reason)
        {
        case ObjectAdapterDeactivated:
        {
            setState(StateClosing, new ObjectAdapterDeactivatedException());
            break;
        }

        case CommunicatorDestroyed:
        {
            setState(StateClosing, new CommunicatorDestroyedException());
            break;
        }
        }
    }

    synchronized public void close(boolean force)
    {
        if(force)
        {
            setState(StateClosed, new ForcedCloseConnectionException());
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
            while(!_requests.isEmpty() || !_asyncRequests.isEmpty())
            {
                try
                {
                    wait();
                }
                catch(InterruptedException ex)
                {
                }
            }

            setState(StateClosing, new CloseConnectionException());
        }
    }

    public synchronized boolean isActiveOrHolding()
    {
        return _state > StateNotValidated && _state < StateClosing;
    }

    public synchronized boolean isFinished()
    {
        if(_transceiver != null || _dispatchCount != 0)
        {
            return false;
        }

        assert (_state == StateClosed);
        return true;
    }

    public synchronized void throwException()
    {
        if(_exception != null)
        {
            assert (_state >= StateClosing);
            throw _exception;
        }
    }

    public synchronized void waitUntilHolding()
    {
        while(_state < StateHolding || _dispatchCount > 0)
        {
            try
            {
                wait();
            }
            catch(InterruptedException ex)
            {
            }
        }
    }

    public synchronized void waitUntilFinished()
    {
        //
        // We wait indefinitely until connection closing has been
        // initiated. We also wait indefinitely until all outstanding
        // requests are completed. Otherwise we couldn't guarantee
        // that there are no outstanding calls when deactivate() is
        // called on the servant locators.
        //
        while(_state < StateClosing || _dispatchCount > 0)
        {
            try
            {
                wait();
            }
            catch(InterruptedException ex)
            {
            }
        }

        //
        // Now we must wait until close() has been called on the
        // transceiver.
        //
        while(_transceiver != null)
        {
            try
            {
                if(_state != StateClosed && _endpoint.timeout() >= 0)
                {
                    long absoluteWaitTime = _stateTime + _endpoint.timeout();
                    long waitTime = absoluteWaitTime - IceInternal.Time.currentMonotonicTimeMillis();

                    if(waitTime > 0)
                    {
                        //
                        // We must wait a bit longer until we close
                        // this connection.
                        //
                        wait(waitTime);
                        if(IceInternal.Time.currentMonotonicTimeMillis() >= absoluteWaitTime)
                        {
                            setState(StateClosed, new CloseTimeoutException());
                        }
                    }
                    else
                    {
                        //
                        // We already waited long enough, so let's close this
                        // connection!
                        //
                        setState(StateClosed, new CloseTimeoutException());
                    }

                    //
                    // No return here, we must still wait until
                    // close() is called on the _transceiver.
                    //
                }
                else
                {
                    wait();
                }
            }
            catch(InterruptedException ex)
            {
            }
        }

        assert (_state == StateClosed);

        //
        // Clear the OA. See bug 1673 for the details of why this is necessary.
        //
        _adapter = null;
    }

    synchronized public void monitor()
    {
        if(_state != StateActive)
        {
            return;
        }

        //
        // Active connection management for idle connections.
        //
        if(_acmTimeout <= 0 || !_requests.isEmpty() || !_asyncRequests.isEmpty() || _batchStreamInUse
                || !_batchStream.isEmpty() || _sendInProgress || _dispatchCount > 0)
        {
            return;
        }

        if(IceInternal.Time.currentMonotonicTimeMillis() >= _acmAbsoluteTimeoutMillis)
        {
            setState(StateClosing, new ConnectionTimeoutException());
        }
    }

    synchronized public boolean sendRequest(IceInternal.Outgoing out, boolean compress, boolean response)
        throws IceInternal.LocalExceptionWrapper
    {
        int requestId = 0;
        final IceInternal.BasicStream os = out.os();

        if(_exception != null)
        {
            //
            // If the connection is closed before we even have a
            // chance to send our request, we always try to send the
            // request again.
            //
            throw new IceInternal.LocalExceptionWrapper(_exception, true);
        }

        assert (_state > StateNotValidated);
        assert (_state < StateClosing);

        if(response)
        {
            //
            // Create a new unique request ID.
            //
            requestId = _nextRequestId++;
            if(requestId <= 0)
            {
                _nextRequestId = 1;
                requestId = _nextRequestId++;
            }

            //
            // Fill in the request ID.
            //
            os.pos(IceInternal.Protocol.headerSize);
            os.writeInt(requestId);
        }

        //
        // Send the message. If it can't be sent without blocking the
        // message is added to _sendStreams and it will be sent by the
        // selector thread or by this thread if flush is true.
        //
        boolean sent = false;
        try
        {
            sent = sendMessage(new OutgoingMessage(out, out.os(), compress, response));
        }
        catch(Ice.LocalException ex)
        {
            setState(StateClosed, ex);
            assert (_exception != null);
            throw _exception;
        }

        if(response)
        {
            //
            // Add to the requests map.
            //
            _requests.put(requestId, out);
        }

        return sent; // The request was sent.
    }

    synchronized public boolean sendAsyncRequest(IceInternal.OutgoingAsync out, boolean compress, boolean response)
        throws IceInternal.LocalExceptionWrapper
    {
        int requestId = 0;
        final IceInternal.BasicStream os = out.__os();

        if(_exception != null)
        {
            //
            // If the connection is closed before we even have a
            // chance to send our request, we always try to send the
            // request again.
            //
            throw new IceInternal.LocalExceptionWrapper(_exception, true);
        }

        assert (_state > StateNotValidated);
        assert (_state < StateClosing);

        if(response)
        {
            //
            // Create a new unique request ID.
            //
            requestId = _nextRequestId++;
            if(requestId <= 0)
            {
                _nextRequestId = 1;
                requestId = _nextRequestId++;
            }

            //
            // Fill in the request ID.
            //
            os.pos(IceInternal.Protocol.headerSize);
            os.writeInt(requestId);
        }

        boolean sent;
        try
        {
            sent = sendMessage(new OutgoingMessage(out, out.__os(), compress, response));
        }
        catch(Ice.LocalException ex)
        {
            setState(StateClosed, ex);
            assert (_exception != null);
            throw _exception;
        }

        if(response)
        {
            //
            // Add to the async requests map.
            //
            _asyncRequests.put(requestId, out);
        }
        return sent;
    }

    public synchronized void prepareBatchRequest(IceInternal.BasicStream os)
    {
        //
        // Wait if flushing is currently in progress.
        //
        while(_batchStreamInUse && _exception == null)
        {
            try
            {
                wait();
            }
            catch(InterruptedException ex)
            {
            }
        }

        if(_exception != null)
        {
            throw _exception;
        }

        assert (_state > StateNotValidated);
        assert (_state < StateClosing);

        if(_batchStream.isEmpty())
        {
            try
            {
                _batchStream.writeBlob(IceInternal.Protocol.requestBatchHdr);
            }
            catch(LocalException ex)
            {
                setState(StateClosed, ex);
                throw ex;
            }
        }

        _batchStreamInUse = true;
        _batchMarker = _batchStream.size();
        _batchStream.swap(os);

        //
        // The batch stream now belongs to the caller, until
        // finishBatchRequest() or abortBatchRequest() is called.
        //
    }

    public void finishBatchRequest(IceInternal.BasicStream os, boolean compress)
    {
        try
        {
            synchronized(this)
            {
                //
                // Get the batch stream back.
                //
                _batchStream.swap(os);

                if(_exception != null)
                {
                    throw _exception;
                }

                boolean flush = false;
                if(_batchAutoFlush)
                {
                    //
                    // Throw memory limit exception if the first
                    // message added causes us to go over
                    // limit. Otherwise put aside the marshalled
                    // message that caused limit to be exceeded and
                    // rollback stream to the marker.
                    try
                    {
                        _transceiver.checkSendSize(_batchStream.getBuffer(), _instance.messageSizeMax());
                    }
                    catch(Ice.LocalException ex)
                    {
                        if(_batchRequestNum > 0)
                        {
                            flush = true;
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
                    byte[] lastRequest = new byte[_batchStream.size() - _batchMarker];
                    IceInternal.Buffer buffer = _batchStream.getBuffer();
                    buffer.b.position(_batchMarker);
                    buffer.b.get(lastRequest);
                    _batchStream.resize(_batchMarker, false);

                    try
                    {
                        //
                        // Fill in the number of requests in the batch.
                        //
                        _batchStream.pos(IceInternal.Protocol.headerSize);
                        _batchStream.writeInt(_batchRequestNum);

                        OutgoingMessage message = new OutgoingMessage(_batchStream, _batchRequestCompress, true);
                        sendMessage(message);
                    }
                    catch(Ice.LocalException ex)
                    {
                        setState(StateClosed, ex);
                        assert (_exception != null);
                        throw _exception;
                    }

                    //
                    // Reset the batch stream.
                    //
                    _batchStream = new IceInternal.BasicStream(_instance, _batchAutoFlush);
                    _batchRequestNum = 0;
                    _batchRequestCompress = false;
                    _batchMarker = 0;

                    //
                    // Check again if the last request doesn't exceed the
                    // maximum message size.
                    //
                    if(IceInternal.Protocol.requestBatchHdr.length + lastRequest.length > _instance.messageSizeMax())
                    {
                        throw new MemoryLimitException();
                    }

                    //
                    // Start a new batch with the last message that caused us to
                    // go over the limit.
                    //
                    _batchStream.writeBlob(IceInternal.Protocol.requestBatchHdr);
                    _batchStream.writeBlob(lastRequest);
                }

                //
                // Increment the number of requests in the batch.
                //
                ++_batchRequestNum;

                //
                // We compress the whole batch if there is at least
                // one compressed message.
                //
                if(compress)
                {
                    _batchRequestCompress = true;
                }

                //
                // Notify about the batch stream not being in use anymore.
                //
                assert (_batchStreamInUse);
                _batchStreamInUse = false;
                notifyAll();
            }
        }
        catch(Ice.LocalException ex)
        {
            abortBatchRequest();
            throw ex;
        }
    }

    public synchronized void abortBatchRequest()
    {
        _batchStream = new IceInternal.BasicStream(_instance, _batchAutoFlush);
        _batchRequestNum = 0;
        _batchRequestCompress = false;
        _batchMarker = 0;

        assert (_batchStreamInUse);
        _batchStreamInUse = false;
        notifyAll();
    }

    public void flushBatchRequests()
    {
        IceInternal.BatchOutgoing out = new IceInternal.BatchOutgoing(this, _instance);
        out.invoke();
    }

    synchronized public boolean flushBatchRequests(IceInternal.BatchOutgoing out)
    {
        while(_batchStreamInUse && _exception == null)
        {
            try
            {
                wait();
            }
            catch(InterruptedException ex)
            {
            }
        }

        if(_exception != null)
        {
            throw _exception;
        }

        if(_batchRequestNum == 0)
        {
            out.sent(false);
            return true;
        }

        //
        // Fill in the number of requests in the batch.
        //
        _batchStream.pos(IceInternal.Protocol.headerSize);
        _batchStream.writeInt(_batchRequestNum);

        _batchStream.swap(out.os());

        boolean sent = false;
        try
        {
            OutgoingMessage message = new OutgoingMessage(out, out.os(), _batchRequestCompress, false);
            sent = sendMessage(message);
        }
        catch(Ice.LocalException ex)
        {
            setState(StateClosed, ex);
            assert (_exception != null);
            throw _exception;
        }

        //
        // Reset the batch stream.
        //
        _batchStream = new IceInternal.BasicStream(_instance, _batchAutoFlush);
        _batchRequestNum = 0;
        _batchRequestCompress = false;
        _batchMarker = 0;
        return sent;
    }

    synchronized public boolean flushAsyncBatchRequests(IceInternal.BatchOutgoingAsync outAsync)
    {
        while(_batchStreamInUse && _exception == null)
        {
            try
            {
                wait();
            }
            catch(InterruptedException ex)
            {
            }
        }

        if(_exception != null)
        {
            throw _exception;
        }

        if(_batchRequestNum == 0)
        {
            outAsync.__sent(this);
            return true;
        }

        //
        // Fill in the number of requests in the batch.
        //
        _batchStream.pos(IceInternal.Protocol.headerSize);
        _batchStream.writeInt(_batchRequestNum);

        _batchStream.swap(outAsync.__os());

        boolean sent;
        try
        {
            OutgoingMessage message = new OutgoingMessage(outAsync, outAsync.__os(), _batchRequestCompress, false);
            sent = sendMessage(message);
        }
        catch(Ice.LocalException ex)
        {
            setState(StateClosed, ex);
            assert (_exception != null);
            throw _exception;
        }

        //
        // Reset the batch stream.
        //
        _batchStream = new IceInternal.BasicStream(_instance, _batchAutoFlush);
        _batchRequestNum = 0;
        _batchRequestCompress = false;
        _batchMarker = 0;
        return sent;
    }

    synchronized public void sendResponse(IceInternal.BasicStream os, byte compressFlag)
    {
        assert (_state > StateNotValidated);

        try
        {
            if(--_dispatchCount == 0)
            {
                notifyAll();
            }

            if(_state == StateClosed)
            {
                assert (_exception != null);
                throw _exception;
            }

            sendMessage(new OutgoingMessage(os, compressFlag != 0, true));

            if(_state == StateClosing && _dispatchCount == 0)
            {
                initiateShutdown();
            }

            if(_acmTimeout > 0)
            {
                _acmAbsoluteTimeoutMillis = IceInternal.Time.currentMonotonicTimeMillis() + _acmTimeout * 1000;
            }
        }
        catch(LocalException ex)
        {
            setState(StateClosed, ex);
        }
    }

    synchronized public void sendNoResponse()
    {
        assert (_state > StateNotValidated);
        try
        {
            if(--_dispatchCount == 0)
            {
                notifyAll();
            }

            if(_state == StateClosed)
            {
                assert (_exception != null);
                throw _exception;
            }

            if(_state == StateClosing && _dispatchCount == 0)
            {
                initiateShutdown();
            }

            if(_acmTimeout > 0)
            {
                _acmAbsoluteTimeoutMillis = IceInternal.Time.currentMonotonicTimeMillis() + _acmTimeout * 1000;
            }
        }
        catch(LocalException ex)
        {
            setState(StateClosed, ex);
        }
    }

    public IceInternal.EndpointI endpoint()
    {
        return _endpoint; // No mutex protection necessary, _endpoint is
        // immutable.
    }

    public synchronized void setAdapter(ObjectAdapter adapter)
    {
        if(_state == StateClosing || _state == StateClosed)
        {
            assert (_exception != null);
            throw _exception;
        }
        else if(_state <= StateNotValidated)
        {
            return;
        }
        assert (_state < StateClosing);

        _adapter = adapter;

        if(_adapter != null)
        {
            _servantManager = ((ObjectAdapterI) _adapter).getServantManager();
            if(_servantManager == null)
            {
                _adapter = null;
            }
        }
        else
        {
            _servantManager = null;
        }

        //
        // We never change the thread pool with which we were
        // initially registered, even if we add or remove an object
        // adapter.
        //
    }

    public synchronized ObjectAdapter getAdapter()
    {
        return _adapter;
    }

    public ObjectPrx createProxy(Identity ident)
    {
        //
        // Create a reference and return a reverse proxy for this
        // reference.
        //
        return _instance.proxyFactory().referenceToProxy(_instance.referenceFactory().create(ident, this));
    }

    public synchronized void exception(LocalException ex)
    {
        setState(StateClosed, ex);
    }

    public synchronized void invokeException(LocalException ex, int invokeNum)
    {
        //
        // Fatal exception while invoking a request. Since
        // sendResponse/sendNoResponse isn't called in case of a fatal
        // exception we decrement _dispatchCount here.
        //

        setState(StateClosed, ex);

        if(invokeNum > 0)
        {
            assert (_dispatchCount > 0);
            _dispatchCount -= invokeNum;
            assert (_dispatchCount >= 0);
            if(_dispatchCount == 0)
            {
                notifyAll();
            }
        }
    }

    public String type()
    {
        return _type; // No mutex lock, _type is immutable.
    }

    public int timeout()
    {
        return _endpoint.timeout(); // No mutex protection necessary, _endpoint
        // is immutable.
    }

    public String toString()
    {
        return _toString();
    }

    public String _toString()
    {
        return _desc; // No mutex lock, _desc is immutable.
    }

    //
    // Only used by the SSL plug-in.
    //
    // The external party has to synchronize the connection, since the
    // connection is the object that protects the transceiver.
    //
    public IceInternal.Transceiver getTransceiver()
    {
        return _transceiver;
    }

    public ConnectionI(IceInternal.Instance instance, IceInternal.Transceiver transceiver,
            IceInternal.EndpointI endpoint, ObjectAdapter adapter)
    {
        _instance = instance;

        final Ice.InitializationData initData = instance.initializationData();
        _transceiver = transceiver;
        _desc = transceiver.toString();
        _type = transceiver.type();
        _endpoint = endpoint;
        _adapter = adapter;
        _logger = initData.logger; // Cached for better performance.
        _traceLevels = instance.traceLevels(); // Cached for better performance.
        _stream = new IceInternal.BasicStream(instance);
        _initializeAsyncCallback = new AsyncCallback()
        {
            public void complete(RuntimeException ex)
            {
                initializeAsync(ex);
            }
        };
        _validateAsyncCallback = new AsyncCallback()
        {
            public void complete(RuntimeException ex)
            {
                validateAsync(ex);
            }
        };
        _writeAsyncCallback = new AsyncCallback()
        {
            public void complete(RuntimeException ex)
            {
                writeAsync(ex);
            }
        };
        _readAsyncCallback = new AsyncCallback()
        {
            public void complete(RuntimeException ex)
            {
                readAsync(ex);
            }
        };
        _flushSentCallbacks = new IceInternal.ThreadPoolWorkItem()
        {
            public void execute()
            {
                flushSentCallbacks();
            }
        };
        _reading = false;

        _warn = initData.properties.getPropertyAsInt("Ice.Warn.Connections") > 0 ? true : false;
        _warnUdp = initData.properties.getPropertyAsInt("Ice.Warn.Datagrams") > 0;
        _cacheBuffers = initData.properties.getPropertyAsIntWithDefault("Ice.CacheMessageBuffers", 1) == 1;
        _acmAbsoluteTimeoutMillis = 0;
        _nextRequestId = 1;
        _batchAutoFlush = initData.properties.getPropertyAsIntWithDefault("Ice.BatchAutoFlush", 1) > 0 ? true : false;
        _batchStream = new IceInternal.BasicStream(instance, _batchAutoFlush);
        _batchStreamInUse = false;
        _batchRequestNum = 0;
        _batchRequestCompress = false;
        _batchMarker = 0;
        _sendInProgress = false;
        _dispatchCount = 0;
        _state = StateNotInitialized;
        _stateTime = IceInternal.Time.currentMonotonicTimeMillis();

        if(_endpoint.datagram())
        {
            _acmTimeout = 0;
        }
        else
        {
            if(_adapter != null)
            {
                _acmTimeout = _instance.serverACM();
            }
            else
            {
                _acmTimeout = _instance.clientACM();
            }
        }

        int compressionLevel = initData.properties.getPropertyAsIntWithDefault("Ice.Compression.Level", 1);
        if(compressionLevel < 1)
        {
            compressionLevel = 1;
        }
        else if(compressionLevel > 9)
        {
            compressionLevel = 9;
        }
        _compressionLevel = compressionLevel;

        ObjectAdapterI adapterImpl = (ObjectAdapterI) _adapter;
        if(adapterImpl != null)
        {
            _servantManager = ((ObjectAdapterI) _adapter).getServantManager();
        }
        if(adapterImpl != null)
        {
            _threadPool = adapterImpl.getThreadPool();
        }
        else
        {
            _threadPool = instance.clientThreadPool();
        }

        try
        {
            _timer = _instance.timer();
            _overrideCompress = _instance.defaultsAndOverrides().overrideCompress;
            _overrideCompressValue = _instance.defaultsAndOverrides().overrideCompressValue;
        }
        catch(Ice.LocalException ex)
        {
            throw ex;
        }
        catch(java.lang.Exception ex)
        {
            Ice.SyscallException e = new Ice.SyscallException();
            e.initCause(ex);
            throw e;
        }
    }

    private static final int StateNotInitialized = 0;
    private static final int StateNotValidated = 1;
    private static final int StateActive = 2;
    private static final int StateHolding = 3;
    private static final int StateClosing = 4;
    private static final int StateClosed = 5;

    private void setState(int state, LocalException ex)
    {
        //
        // If setState() is called with an exception, then only closed
        // and closing states are permissible.
        //
        assert (state == StateClosing || state == StateClosed);

        if(_state == state) // Don't switch twice.
        {
            return;
        }

        if(_exception == null)
        {
            _exception = ex;

            if(_warn)
            {
                //
                // We don't warn if we are not validated.
                //
                if(_state > StateNotValidated)
                {
                    //
                    // Don't warn about certain expected exceptions.
                    //
                    if(!(_exception instanceof CloseConnectionException
                            || _exception instanceof ForcedCloseConnectionException
                            || _exception instanceof ConnectionTimeoutException
                            || _exception instanceof CommunicatorDestroyedException
                            || _exception instanceof ObjectAdapterDeactivatedException || (_exception instanceof ConnectionLostException && _state == StateClosing)))
                    {
                        warning("connection exception", _exception);
                    }
                }
            }
        }

        //
        // We must set the new state before we notify requests of any
        // exceptions. Otherwise new requests may retry on a
        // connection that is not yet marked as closed or closing.
        //
        setState(state);
    }

    private void setState(int state)
    {
        //
        // We don't want to send close connection messages if the endpoint
        // only supports oneway transmission from client to server.
        //
        if(_endpoint.datagram() && state == StateClosing)
        {
            state = StateClosed;
        }

        //
        // Skip graceful shutdown if we are destroyed before validation.
        //
        if(_state <= StateNotValidated && state == StateClosing)
        {
            state = StateClosed;
        }

        if(_state == state) // Don't switch twice.
        {
            return;
        }

        switch (state)
        {
        case StateNotInitialized:
        {
            assert (false);
            break;
        }

        case StateNotValidated:
        {
            if(_state != StateNotInitialized)
            {
                assert (_state == StateClosed);
                return;
            }
            break;
        }

        case StateActive:
        {
            //
            // Can only switch from holding or not validated to
            // active.
            //
            if(_state != StateHolding && _state != StateNotValidated)
            {
                return;
            }
            if(!_reading)
            {
                //
                // Start reading.
                //
                _reading = true;
                _transceiver.read(_stream.getBuffer(), _readAsyncCallback); // Continue reading.
            }
            break;
        }

        case StateHolding:
        {
            //
            // Can only switch from active or not validated to
            // holding.
            //
            if(_state != StateActive && _state != StateNotValidated)
            {
                return;
            }
            break;
        }

        case StateClosing:
        {
            //
            // Can't change back from closed.
            //
            if(_state == StateClosed)
            {
                return;
            }

            if(!_reading)
            {
                //
                // We need to continue to read in closing state.
                //
                _reading = true;
                _transceiver.read(_stream.getBuffer(), _readAsyncCallback); // Continue reading.
            }
            break;
        }

        case StateClosed:
        {
            if(!_sendInProgress)
            {
                _threadPool.finish(this);
            }
            //
            // We shutdown the transceiver for reading and writing. This will
            // unblock any read/write calls (which will return with an exception).
            //
            _transceiver.shutdownReadWrite();
            break;
        }
        }

        //  
        // We only register with the connection monitor if our new state
        // is StateActive. Otherwise we unregister with the connection
        // monitor, but only if we were registered before, i.e., if our
        // old state was StateActive.
        //
        IceInternal.ConnectionMonitor connectionMonitor = _instance.connectionMonitor();
        if(connectionMonitor != null)
        {
            if(state == StateActive)
            {
                connectionMonitor.add(this);
            }
            else if(_state == StateActive)
            {
                connectionMonitor.remove(this);
            }
        }

        _state = state;
        _stateTime = IceInternal.Time.currentMonotonicTimeMillis();

        notifyAll();

        if(_state == StateClosing && _dispatchCount == 0)
        {
            try
            {
                initiateShutdown();
            }
            catch(LocalException ex)
            {
                setState(StateClosed, ex);
            }
        }
    }

    private void initiateShutdown()
    {
        assert (_state == StateClosing);
        assert (_dispatchCount == 0);

        if(!_endpoint.datagram())
        {
            //
            // Before we shut down, we send a close connection
            // message.
            //
            IceInternal.BasicStream os = new IceInternal.BasicStream(_instance);
            os.writeBlob(IceInternal.Protocol.magic);
            os.writeByte(IceInternal.Protocol.protocolMajor);
            os.writeByte(IceInternal.Protocol.protocolMinor);
            os.writeByte(IceInternal.Protocol.encodingMajor);
            os.writeByte(IceInternal.Protocol.encodingMinor);
            os.writeByte(IceInternal.Protocol.closeConnectionMsg);
            os.writeByte(_compressionSupported ? (byte) 1 : (byte) 0);
            os.writeInt(IceInternal.Protocol.headerSize); // Message size.

            sendMessage(new OutgoingMessage(os, false, false));

            //
            // The CloseConnection message should be
            // sufficient. Closing the write end of the socket is
            // probably an artifact of how things were done in
            // IIOP. In fact, shutting down the write end of the
            // socket causes problems on Windows by preventing the
            // peer from using the socket.  For example, the peer is
            // no longer able to continue writing a large message
            // after the socket is shutdown.
            //
            // _transceiver.shutdownWrite();
        }
    }

    private void initializeAsync(RuntimeException connectEx)
    {
        synchronized(this)
        {
            if(_state == StateClosed)
            {
                // Call one more time to do any pending shutdown.
                _transceiver.initialize(null);
                return;
            }

            //
            // An I/O request has completed, so cancel a pending timeout.
            //
            _timer.cancel(this);

            try
            {
                if(connectEx != null)
                {
                    throw connectEx;
                }

                //
                // Initialize the transceiver.
                //
                // If initialize returns false, the transceiver has begun an
                // asynchronous I/O request and we must await the callback.
                // Otherwise we can progress to validation.
                //
                if(!_transceiver.initialize(_initializeAsyncCallback))
                {
                    scheduleConnectTimeout();
                    return;
                }

                //
                // Update the connection description once the transceiver is
                // initialized.
                //
                _desc = _transceiver.toString();
                setState(StateNotValidated);

                // Datagram connections are always implicitly validated.
                if(!_endpoint.datagram())
                {
                    if(_adapter != null)
                    {
                        // The server side has the active role for connection
                        // validation.
                        IceInternal.BasicStream os = _stream;
                        if(os.size() == 0)
                        {
                            //
                            // We haven't tried to write yet; prepare the
                            // validate connection message.
                            //
                            os.writeBlob(IceInternal.Protocol.magic);
                            os.writeByte(IceInternal.Protocol.protocolMajor);
                            os.writeByte(IceInternal.Protocol.protocolMinor);
                            os.writeByte(IceInternal.Protocol.encodingMajor);
                            os.writeByte(IceInternal.Protocol.encodingMinor);
                            os.writeByte(IceInternal.Protocol.validateConnectionMsg);
                            // Compression status (always zero for
                            // validate connection).
                            os.writeByte((byte) 0);
                            os.writeInt(IceInternal.Protocol.headerSize); // Message
                            // size.
                            IceInternal.TraceUtil.traceSend(os, _logger, _traceLevels);
                            os.prepareWrite();
                        }

                        _transceiver.write(os.getBuffer(), _validateAsyncCallback);
                    }
                    else
                    {
                        // The client side has the passive role for connection
                        // validation.

                        //
                        // We haven't tried to read yet; resize the stream to
                        // hold the header.
                        //
                        _stream.resize(IceInternal.Protocol.headerSize, true);
                        _stream.pos(0);

                        _transceiver.read(_stream.getBuffer(), _validateAsyncCallback);

                        //
                        // Wait until the I/O request completes.
                        //
                        scheduleConnectTimeout();
                    }
                    return;
                }
            }
            catch(LocalException ex)
            {
                setState(StateClosed, ex);
                return;
            }
        }

        // Otherwise it is a datagram transceiver, so complete the validation.
        validateAsync(null);
    }

    private void validateAsync(RuntimeException resultEx)
    {
        StartCallback callback = null;
        synchronized(this)
        {
            if(_state == StateClosed)
            {
                return;
            }

            // Datagram connections are always implicitly validated.
            if(!_endpoint.datagram()) 
            {
                //
                // Cancel a pending timeout (if any).
                //
                _timer.cancel(this);

                try
                {
                    if(resultEx != null)
                    {
                        throw resultEx;
                    }
                    // The server side has the active role for connection
                    // validation.
                    if(_adapter != null)
                    {
                        assert (_stream.pos() == _stream.size());
                    }
                    else
                    {
                        // The client side has the passive role for connection
                        // validation.
                        //
                        // Ensure that we're read the entire stream.
                        //
                        if(_stream.pos() < _stream.size())
                        {
                            _transceiver.read(_stream.getBuffer(), _validateAsyncCallback);
                            scheduleConnectTimeout();
                            return;
                        }

                        assert (_stream.pos() == _stream.size());
                        checkValidationMessage(_stream);
                    }
                }
                catch(LocalException ex)
                {
                    setState(StateClosed, ex);
                    return;
                }
            }

            _stream.reset();
            
            // Setup the stream ready for a new read.
            _stream.resize(IceInternal.Protocol.headerSize, true);
            _stream.pos(0);

            assert _state != StateClosed;

            //
            // We start out in holding state.
            //
            setState(StateHolding);

            callback = _startCallback;
            _startCallback = null;
        }

        if(callback != null)
        {
            callback.connectionStartCompleted(this);
        }
    }

    synchronized private void writeAsync(RuntimeException writeEx)
    {
        //
        // Use a lock to prevent race conditions, such as when an
        // asynchronous callback is scheduled to execute just after
        // beginWrite completes.
        //
        boolean flushSentCallbacks = _sentCallbacks.size() == 0;
        try
        {
            assert _sendInProgress;
            assert _sendStreams.size() > 0;
            assert _transceiver != null;

            //
            // An I/O request has completed, so cancel a pending timeout.
            //
            _timer.cancel(this);

            if(_state == StateClosed)
            {
                OutgoingMessage message = _sendStreams.remove();
                message.sent(this, true);
                if(message.outAsync instanceof Ice.AMISentCallback)
                {
                    _sentCallbacks.add(message);
                }

                _sendInProgress = false;
                _threadPool.finish(this);
                return;
            }

            // If we had an error, raise it here.
            if(writeEx != null)
            {
                throw writeEx;
            }

            //
            // If we have a result, it means we need to complete a
            // pending I/O request.
            //
            OutgoingMessage message = _sendStreams.remove();

            message.sent(this, true); // true indicates that this is
            // called by the async
            // callback.
            if(message.outAsync instanceof Ice.AMISentCallback)
            {
                _sentCallbacks.add(message);
            }

            // Send the next message.
            if(!_sendStreams.isEmpty())
            {
                message = _sendStreams.getFirst();

                //
                // The message may have already been prepared and partially
                // sent.
                //
                if(!message.prepared)
                {
                    prepareMessage(message);
                }

                //
                // Begin an asynchronous write to send the remainder of the
                // message.
                //
                _transceiver.write(message.stream.getBuffer(), _writeAsyncCallback);
                //
                // We will be called again when the write completes.
                // Schedule a timeout if necessary
                // and return now.
                //
                scheduleTimeout();
                return;
            }
        }
        catch(LocalException ex)
        {
            _sendInProgress = false;
            setState(StateClosed, ex);
            return;
        }
        finally
        {
            if(flushSentCallbacks && _sentCallbacks.size() > 0)
            {
                _threadPool.execute(_flushSentCallbacks);
            }
        }

        assert (_sendStreams.size() == 0);
        _sendInProgress = false;
        if(_acmTimeout > 0)
        {
            _acmAbsoluteTimeoutMillis = IceInternal.Time.currentMonotonicTimeMillis() + _acmTimeout * 1000;
        }
    }

    private void readAsync(RuntimeException readEx)
    {
        //
        // This method is responsible for reading all messages once
        // this connection is validated.  It ensures that a read is
        // always pending until the connection is closed or an
        // exception occurs.
        //
        // This method is always invoked as a result of a read
        // completing.

        try
        {
            IceInternal.BasicStream stream = null;
            final IncomingMessage message = new IncomingMessage();
            synchronized(this)
            {
                //
                // We're done if the connection has been closed.
                //
                if(_state == StateClosed)
                {
                    return;
                }

                assert (_transceiver != null);

                if(readEx != null)
                {
                    throw readEx;
                }

                //
                // Check for StateHold *after* completing any pending
                // asynchronous operation.
                //
                if(_state == StateHolding)
                {
                    _reading = false;
                    return;
                }

                assert _stream.size() > 0;

                int pos = _stream.pos();

                //
                // For a datagram connection, each read on the transceiver
                // produces a new message.
                //
                if(_endpoint.datagram())
                {
                    // We must have at least headerSize bytes.
                    if(pos < IceInternal.Protocol.headerSize)
                    {
                        throw new Ice.IllegalMessageSizeException();
                    }

                    validateHeader(_stream);

                    //
                    // Check for a datagram packet that was smaller than
                    // expected, if so restart the read.
                    //
                    if(pos != _stream.size())
                    {
                        if(_warnUdp)
                        {
                            _logger.warning("DatagramLimitException: maximum size of " + _stream.pos() + " exceeded");
                        }
                        throw new Ice.DatagramLimitException();
                    }
                }
                else
                {
                    //
                    // When we've read the header, we need to validate it. The
                    // stream will be enlarged if necessary to
                    // contain the entire message.
                    //
                    if(pos == IceInternal.Protocol.headerSize)
                    {
                        validateHeader(_stream);

                        if(pos < _stream.size())
                        {
                            _transceiver.read(_stream.getBuffer(), _readAsyncCallback);
                            // This method is called again when the read
                            // completes.
                            return;
                        }
                    }
                }

                //
                // We're done when there's nothing left to read.
                //
                assert pos == _stream.size();

                //
                // Preserve the stream before another thread begins to read.
                //
                stream = new IceInternal.BasicStream(_instance);
                stream.swap(_stream);

                // Make room for the next header.
                _stream.resize(IceInternal.Protocol.headerSize, true);
                _stream.pos(0);

                //
                // Parse the message. Note that this may involve decompressing
                // the message.
                //
                parseMessage(stream, message);

                //
                // parseMessage() can close the connection, so we must check for
                // closed state again.
                //
                if(_state == StateClosed)
                {
                    return;
                }

                //
                // At this point we have received and parsed a complete message.
                // We have two responsibilities:
                //
                // 1. Dispatch the message, if necessary (parseMessage may have
                // already handled the message).
                // 2. Start another read.
                //
                if(message.stream != null)
                {
                    _threadPool.execute(new IceInternal.ThreadPoolWorkItem()
                    {
                        public void execute()
                        {
                            message(message);
                        };
                    });
                }
                else
                {
                    // Start the next read.
                    _transceiver.read(_stream.getBuffer(), _readAsyncCallback);
                }
            }
        }
        catch(SocketException ex)
        {
            exception(ex);
        }
        catch(LocalException ex)
        {
            if(_endpoint.datagram())
            {
                if(_instance.initializationData().properties.getPropertyAsInt("Ice.Warn.Connections") > 0)
                {
                    warning("datagram connection exception", ex);
                }
    
                //
                // Restart the read.
                //
                synchronized(this)
                {
                    _stream.resize(IceInternal.Protocol.headerSize, true);
                    _stream.pos(0);
                    _transceiver.read(_stream.getBuffer(), _readAsyncCallback);
                }
            }
            else
            {
                exception(ex);
            }
        }
        catch(Exception e)
        {
            UnknownException ex = new UnknownException();
            ex.initCause(e);
            exception(ex);

            //
            // All relevant exceptions should have been caught. We log
            // it here because the AsyncCallback thread silently ignores
            // exceptions.
            //
            _logger.error("exception in readAsync for connection:\n" + _desc + "\n" + ex);
        }
    }

    private boolean sendMessage(OutgoingMessage message)
    {
        assert (_state != StateClosed);
        if(_sendInProgress)
        {
            message.adopt();
            _sendStreams.add(message);
            return false;
        }

        _sendInProgress = true;
        message.adopt();
        prepareMessage(message);

        // Add the message to the list of sending streams, and start the
        // asynchronous write. Schedule a timeout.
        _sendStreams.add(message);

        _transceiver.write(message.stream.getBuffer(), _writeAsyncCallback);

        scheduleTimeout();

        return false;
    }

    private void prepareMessage(OutgoingMessage message)
    {
        //
        // Attempt to send the message without blocking. If the send
        // blocks, we register the connection with the selector thread
        // or we request the caller to call finishSendMessage()
        // outside the synchronization.
        //

        assert (!message.prepared);

        IceInternal.BasicStream stream = message.stream;

        boolean compress = _overrideCompress ? _overrideCompressValue : message.compress;
        message.stream = doCompress(stream, compress);
        message.stream.prepareWrite();
        message.prepared = true;

        if(message.outAsync != null)
        {
            IceInternal.TraceUtil.trace("sending asynchronous request", stream, _logger, _traceLevels);
        }
        else
        {
            IceInternal.TraceUtil.traceSend(stream, _logger, _traceLevels);
        }
    }

    private IceInternal.BasicStream doCompress(IceInternal.BasicStream uncompressed, boolean compress)
    {
        if(_compressionSupported)
        {
            if(compress && uncompressed.size() >= 100)
            {
                //
                // Do compression.
                //
                IceInternal.BasicStream cstream = uncompressed.compress(IceInternal.Protocol.headerSize,
                        _compressionLevel);
                if(cstream != null)
                {
                    //
                    // Set compression status.
                    //
                    cstream.pos(9);
                    cstream.writeByte((byte) 2);

                    //
                    // Write the size of the compressed stream into the header.
                    //
                    cstream.pos(10);
                    cstream.writeInt(cstream.size());

                    //
                    // Write the compression status and size of the
                    // compressed stream into the header of the
                    // uncompressed stream -- we need this to trace
                    // requests correctly.
                    //
                    uncompressed.pos(9);
                    uncompressed.writeByte((byte) 2);
                    uncompressed.writeInt(cstream.size());

                    return cstream;
                }
            }
        }

        uncompressed.pos(9);
        uncompressed.writeByte((byte) ((_compressionSupported && compress) ? 1 : 0));

        //
        // Not compressed, fill in the message size.
        //
        uncompressed.pos(10);
        uncompressed.writeInt(uncompressed.size());

        return uncompressed;
    }

    private void warning(String msg, Exception ex)
    {
        java.io.StringWriter sw = new java.io.StringWriter();
        java.io.PrintWriter pw = new java.io.PrintWriter(sw);
        ex.printStackTrace(pw);
        pw.flush();
        String s = msg + ":\n" + _desc + "\n" + sw.toString();
        _logger.warning(s);
    }

    private void flushSentCallbacks()
    {
        java.util.List<OutgoingMessage> sentCallbacks;
        synchronized(this)
        {
            assert (_sentCallbacks != null && !_sentCallbacks.isEmpty());
            sentCallbacks = _sentCallbacks;
            _sentCallbacks = new java.util.LinkedList<OutgoingMessage>();
        }
        for(OutgoingMessage message : sentCallbacks)
        {
            message.outAsync.__sent(_instance);
        }
    }

    private void message(IncomingMessage message)
    {
        //
        // NOTE: Unlike C++, the thread pool doesn't parse the message.
        // The message parsed by the readAsync callback. This avoids doing extra
        // context context switches when the message to read can be processed
        // without blocking (this is for example the case of responses to
        // regular twoway requests).
        //

        assert message.stream != null;

        //
        // If we're not required to serialize the dispatching of the connection
        // messages, we can start a new read right now. Otherwise, a new read
        // we be started once the message is dispatched.
        //
        if(!_threadPool.serialize())
        {
            //
            // Don't restart the read if in holding or closed state. Read will
            // be restarted if connection becomes active/closing again.
            //
            synchronized(this)
            {
                assert _reading;
                if(_state == StateHolding || _state == StateClosed)
                {
                    _reading = false;
                }
                else
                {
                    _transceiver.read(_stream.getBuffer(), _readAsyncCallback);
                }
            }
        }

        //
        // Asynchronous replies must be handled outside the thread
        // synchronization, so that nested calls are possible.
        //
        if(message.outAsync != null)
        {
            message.outAsync.__finished(message.stream);
        }

        if(message.invokeNum > 0)
        {
            invokeAll(message);
        }

        //
        // If we're serializing message dispatch over the connection, it's time
        // start a new read since all the messages have been dispatched.
        //
        if(_threadPool.serialize())
        {
            //
            // Don't restart the read if in holding or closed
            // state. Read will be restarted if connection becomes
            // active/closing again.
            //
            synchronized(this)
            {
                assert (_reading);
                if(_state == StateHolding || _state == StateClosed)
                {
                    _reading = false;
                }
                else
                {
                    _transceiver.read(_stream.getBuffer(), _readAsyncCallback);
                }
            }
        }
    }

    private void validateHeader(IceInternal.BasicStream stream)
    {
        //
        // Validate the message header.
        //

        //
        // The stream's current position can be larger than
        // Protocol.headerSize in the case of datagrams.
        //
        int pos = stream.pos();
        assert (pos >= IceInternal.Protocol.headerSize);

        stream.pos(0);
        byte[] m = new byte[4];
        m[0] = stream.readByte();
        m[1] = stream.readByte();
        m[2] = stream.readByte();
        m[3] = stream.readByte();
        if(m[0] != Protocol.magic[0] || m[1] != Protocol.magic[1] || m[2] != Protocol.magic[2]
                || m[3] != Protocol.magic[3])
        {
            Ice.BadMagicException ex = new Ice.BadMagicException();
            ex.badMagic = m;
            throw ex;
        }

        byte pMajor = stream.readByte();
        byte pMinor = stream.readByte();
        if(pMajor != Protocol.protocolMajor || pMinor > Protocol.protocolMinor)
        {
            Ice.UnsupportedProtocolException e = new Ice.UnsupportedProtocolException();
            e.badMajor = pMajor < 0 ? pMajor + 255 : pMajor;
            e.badMinor = pMinor < 0 ? pMinor + 255 : pMinor;
            e.major = Protocol.protocolMajor;
            e.minor = Protocol.protocolMinor;
            throw e;
        }

        byte eMajor = stream.readByte();
        byte eMinor = stream.readByte();
        if(eMajor != Protocol.encodingMajor || eMinor > Protocol.encodingMinor)
        {
            Ice.UnsupportedEncodingException e = new Ice.UnsupportedEncodingException();
            e.badMajor = eMajor < 0 ? eMajor + 255 : eMajor;
            e.badMinor = eMinor < 0 ? eMinor + 255 : eMinor;
            e.major = Protocol.encodingMajor;
            e.minor = Protocol.encodingMinor;
            throw e;
        }

        stream.pos(10);
        int size = stream.readInt();

        if(size < IceInternal.Protocol.headerSize)
        {
            throw new Ice.IllegalMessageSizeException();
        }
        if(size > _instance.messageSizeMax())
        {
            throw new Ice.MemoryLimitException();
        }
        if(size > stream.size())
        {
            stream.resize(size, true);
        }
        stream.pos(pos);
    }

    private void checkValidationMessage(IceInternal.BasicStream stream)
    {
        assert (stream.pos() == IceInternal.Protocol.headerSize);
        validateHeader(stream);

        stream.pos(8);
        byte messageType = stream.readByte();
        if(messageType != IceInternal.Protocol.validateConnectionMsg)
        {
            throw new ConnectionNotValidatedException();
        }

        if(stream.size() != IceInternal.Protocol.headerSize)
        {
            throw new IllegalMessageSizeException();
        }
        IceInternal.TraceUtil.traceRecv(stream, _logger, _traceLevels);
    }

    private class IncomingMessage
    {
        public IceInternal.BasicStream stream;
        public int invokeNum;
        public int requestId;
        public byte compress;
        public IceInternal.ServantManager servantManager;
        public ObjectAdapter adapter;
        public IceInternal.OutgoingAsync outAsync;
    }

    private void parseMessage(IceInternal.BasicStream stream, IncomingMessage message)
    {
        assert (_state > StateNotValidated && _state < StateClosed);

        if(_acmTimeout > 0)
        {
            _acmAbsoluteTimeoutMillis = IceInternal.Time.currentMonotonicTimeMillis() + _acmTimeout * 1000;
        }

        try
        {
            //
            // We don't need to check magic and version here. This has already
            // been done by the ThreadPool which provides us with the stream.
            //

            assert (stream.pos() == stream.size());
            stream.pos(8);
            byte messageType = stream.readByte();
            byte compress = stream.readByte();
            if(compress == (byte) 2)
            {
                if(_compressionSupported)
                {
                    IceInternal.BasicStream ustream = stream.uncompress(IceInternal.Protocol.headerSize);
                    if(ustream != stream)
                    {
                        stream = ustream;
                    }
                }
                else
                {
                    FeatureNotSupportedException ex = new FeatureNotSupportedException();
                    ex.unsupportedFeature = "Cannot uncompress compressed message: "
                            + "org.apache.tools.bzip2.CBZip2OutputStream was not found";
                    throw ex;
                }
            }
            stream.pos(IceInternal.Protocol.headerSize);

            switch (messageType)
            {
            case IceInternal.Protocol.closeConnectionMsg:
            {
                IceInternal.TraceUtil.traceRecv(stream, _logger, _traceLevels);
                if(_endpoint.datagram())
                {
                    if(_warn)
                    {
                        _logger.warning("ignoring close connection message for datagram connection:\n" + _desc);
                    }
                }
                else
                {
                    setState(StateClosed, new CloseConnectionException());
                }
                break;
            }

            case IceInternal.Protocol.requestMsg:
            {
                if(_state == StateClosing)
                {
                    IceInternal.TraceUtil.trace("received request during closing\n"
                            + "(ignored by server, client will retry)", stream, _logger, _traceLevels);
                }
                else
                {
                    IceInternal.TraceUtil.traceRecv(stream, _logger, _traceLevels);
                    message.stream = stream;
                    message.compress = compress;
                    message.requestId = stream.readInt();
                    message.invokeNum = 1;
                    message.servantManager = _servantManager;
                    message.adapter = _adapter;
                    ++_dispatchCount;
                }
                break;
            }

            case IceInternal.Protocol.requestBatchMsg:
            {
                if(_state == StateClosing)
                {
                    IceInternal.TraceUtil.trace("received batch request during closing\n"
                            + "(ignored by server, client will retry)", stream, _logger, _traceLevels);
                }
                else
                {
                    IceInternal.TraceUtil.traceRecv(stream, _logger, _traceLevels);
                    message.stream = stream;
                    message.compress = compress;
                    message.invokeNum = stream.readInt();
                    if(message.invokeNum < 0)
                    {
                        message.invokeNum = 0;
                        throw new NegativeSizeException();
                    }
                    message.servantManager = _servantManager;
                    message.adapter = _adapter;
                    _dispatchCount += message.invokeNum;
                }
                break;
            }

            case IceInternal.Protocol.replyMsg:
            {
                IceInternal.TraceUtil.traceRecv(stream, _logger, _traceLevels);
                int requestId = stream.readInt();
                IceInternal.Outgoing og = _requests.remove(requestId);
                if(og != null)
                {
                    og.finished(stream);
                }
                else
                {
                    message.outAsync = _asyncRequests.remove(requestId);
                    if(message.outAsync == null)
                    {
                        throw new UnknownRequestIdException();
                    }
                    message.stream = stream;
                    message.requestId = requestId;
                    message.compress = compress;
                }
                break;
            }

            case IceInternal.Protocol.validateConnectionMsg:
            {
                IceInternal.TraceUtil.traceRecv(stream, _logger, _traceLevels);
                if(_warn)
                {
                    _logger.warning("ignoring unexpected validate connection message:\n" + _desc);
                }
                break;
            }

            default:
            {
                IceInternal.TraceUtil.trace("received unknown message\n(invalid, closing connection)", stream, _logger,
                        _traceLevels);
                throw new UnknownMessageException();
            }
            }
        }
        catch(LocalException ex)
        {
            if(_endpoint.datagram())
            {
                if(_warn)
                {
                    _logger.warning("udp connection exception:\n" + ex + _desc);
                }
            }
            else
            {
                setState(StateClosed, ex);
            }
        }
    }

    private void invokeAll(IncomingMessage message)
    {
        assert (message.stream != null);

        //
        // Note: In contrast to other private or protected methods, this
        // operation must be called *without* the mutex locked.
        //

        IceInternal.Incoming inc = null;
        try
        {
            while(message.invokeNum > 0)
            {
                //
                // Prepare the invocation.
                //
                boolean response = !_endpoint.datagram() && message.requestId != 0;
                inc = getIncoming(message.adapter, response, message.compress, message.requestId);
                IceInternal.BasicStream ins = inc.is();
                message.stream.swap(ins);
                IceInternal.BasicStream os = inc.os();

                //
                // Prepare the response if necessary.
                //
                if(response)
                {
                    assert (message.invokeNum == 1); // No further invocations
                    // if a response is
                    // expected.
                    os.writeBlob(IceInternal.Protocol.replyHdr);

                    //
                    // Add the request ID.
                    //
                    os.writeInt(message.requestId);
                }

                inc.invoke(message.servantManager);

                //
                // If there are more invocations, we need the stream back.
                //
                if(--message.invokeNum > 0)
                {
                    message.stream.swap(ins);
                }

                reclaimIncoming(inc);
                inc = null;
            }
        }
        catch(LocalException ex)
        {
            invokeException(ex, message.invokeNum);
        }
        catch(java.lang.AssertionError ex) // Upon assertion, we print the stack
        // trace.
        {
            UnknownException uex = new UnknownException();
            java.io.StringWriter sw = new java.io.StringWriter();
            java.io.PrintWriter pw = new java.io.PrintWriter(sw);
            ex.printStackTrace(pw);
            pw.flush();
            uex.unknown = sw.toString();
            _logger.error(uex.unknown);
            invokeException(uex, message.invokeNum);
        }
        finally
        {
            if(inc != null)
            {
                reclaimIncoming(inc);
            }
        }
    }

    public void finished(IceInternal.ThreadPool threadPool)
    {
        synchronized(this)
        {
            assert (threadPool == _threadPool && _state == StateClosed && !_sendInProgress);
        }

        if(_startCallback != null)
        {
            _startCallback.connectionStartFailed(this, _exception);
            _startCallback = null;
        }

        java.util.Iterator<OutgoingMessage> p = _sendStreams.iterator();
        while(p.hasNext())
        {
            p.next().finished(_exception);
        }
        _sendStreams.clear();

        java.util.Iterator<IceInternal.Outgoing> q = _requests.values().iterator();
        while(q.hasNext())
        {
            q.next().finished(_exception);
        }
        _requests.clear();

        java.util.Iterator<IceInternal.OutgoingAsync> r = _asyncRequests.values().iterator();
        while(r.hasNext())
        {
            r.next().__finished(_exception);
        }
        _asyncRequests.clear();

        // Close the transceiver outside of the
        // lock. readAsync/writeAsync won't do anything since _state
        // == StateClosed. Closing inside the lock results in a
        // deadlock, since read/write callback tells the async calls
        // of a result due to exception which would result in a
        // deadlock. Note that this is a unique deadlock situation
        // since close() joins with the read/write threads.
        try
        {
            _transceiver.close();
        }
        catch(Exception e)
        {
        }

        //
        // This must be done last as this will cause
        // waitUntilFinished() to return (and communicator objects
        // such as the timer might be destroyed too).
        synchronized(this)
        {
            _transceiver = null;
            notifyAll();
        }
    }

    private IceInternal.Incoming getIncoming(ObjectAdapter adapter, boolean response, byte compress, int requestId)
    {
        IceInternal.Incoming in = null;

        if(_cacheBuffers)
        {
            synchronized(_incomingCacheMutex)
            {
                if(_incomingCache == null)
                {
                    in = new IceInternal.Incoming(_instance, this, adapter, response, compress, requestId);
                }
                else
                {
                    in = _incomingCache;
                    _incomingCache = _incomingCache.next;
                    in.reset(_instance, this, adapter, response, compress, requestId);
                    in.next = null;
                }
            }
        }
        else
        {
            in = new IceInternal.Incoming(_instance, this, adapter, response, compress, requestId);
        }

        return in;
    }

    private void reclaimIncoming(IceInternal.Incoming in)
    {
        if(_cacheBuffers)
        {
            synchronized(_incomingCacheMutex)
            {
                in.next = _incomingCache;
                _incomingCache = in;
                //
                // Clear references to Ice objects as soon as possible.
                //
                _incomingCache.reclaim();
            }
        }
    }

    public IceInternal.Outgoing getOutgoing(IceInternal.RequestHandler handler, String operation, OperationMode mode,
                                            java.util.Map<String, String> context)
        throws IceInternal.LocalExceptionWrapper
    {
        IceInternal.Outgoing out = null;

        if(_cacheBuffers)
        {
            synchronized(_outgoingCacheMutex)
            {
                if(_outgoingCache == null)
                {
                    out = new IceInternal.Outgoing(handler, operation, mode, context);
                }
                else
                {
                    out = _outgoingCache;
                    _outgoingCache = _outgoingCache.next;
                    out.reset(handler, operation, mode, context);
                    out.next = null;
                }
            }
        }
        else
        {
            out = new IceInternal.Outgoing(handler, operation, mode, context);
        }

        return out;
    }

    public void reclaimOutgoing(IceInternal.Outgoing out)
    {
        if(_cacheBuffers)
        {
            //
            // Clear references to Ice objects as soon as possible.
            //
            out.reclaim();

            synchronized(_outgoingCacheMutex)
            {
                out.next = _outgoingCache;
                _outgoingCache = out;
            }
        }
    }

    private void scheduleConnectTimeout()
    {
        int timeout;
        IceInternal.DefaultsAndOverrides defaultsAndOverrides = _instance.defaultsAndOverrides();
        if(defaultsAndOverrides.overrideConnectTimeout)
        {
            timeout = defaultsAndOverrides.overrideConnectTimeoutValue;
        }
        else
        {
            timeout = _endpoint.timeout();
        }

        if(timeout >= 0)
        {
            _timer.schedule(this, timeout);
        }
    }

    private void scheduleTimeout()
    {
        if(_endpoint.timeout() >= 0)
        {
            _timer.schedule(this, _endpoint.timeout());
        }
    }

    //
    // Timer.TimerTask method.
    //
    synchronized public void runTimerTask()
    {
        if(_state <= StateNotValidated)
        {
            setState(StateClosed, new ConnectTimeoutException());
        }
        else if(_state <= StateClosing)
        {
            setState(StateClosed, new TimeoutException());
        }
    }

    private static class OutgoingMessage
    {
        OutgoingMessage(IceInternal.BasicStream stream, boolean compress, boolean adopt)
        {
            this.stream = stream;
            this.compress = compress;
            this.adopt = adopt;
        }

        OutgoingMessage(IceInternal.OutgoingMessageCallback out, IceInternal.BasicStream stream, boolean compress,
                boolean resp)
        {
            this.stream = stream;
            this.compress = compress;
            this.out = out;
            this.response = resp;
        }

        OutgoingMessage(IceInternal.OutgoingAsyncMessageCallback out, IceInternal.BasicStream stream, boolean compress,
                boolean resp)
        {
            this.stream = stream;
            this.compress = compress;
            this.outAsync = out;
            this.response = resp;
        }

        public void adopt()
        {
            if(adopt)
            {
                IceInternal.BasicStream stream = new IceInternal.BasicStream(this.stream.instance());
                stream.swap(this.stream);
                this.stream = stream;
                adopt = false;
            }
        }

        public void sent(ConnectionI connection, boolean notify)
        {
            if(out != null)
            {
                // true = notify the waiting thread that the request
                // was sent.
                //
                out.sent(notify);
            }
            else if(outAsync != null)
            {
                outAsync.__sent(connection);
            }
        }

        public void finished(Ice.LocalException ex)
        {
            //
            // Only notify oneway requests. The connection keeps track
            // of twoway requests in the _requests/_asyncRequests maps
            // and will notify them of the connection exceptions.
            //
            if(!response)
            {
                if(out != null)
                {
                    out.finished(ex);
                }
                else if(outAsync != null)
                {
                    outAsync.__finished(ex);
                }
            }
        }

        public IceInternal.BasicStream stream;
        public IceInternal.OutgoingMessageCallback out;
        public IceInternal.OutgoingAsyncMessageCallback outAsync;
        public boolean compress;
        public boolean response;
        boolean adopt;
        boolean prepared;
    }

    private IceInternal.Instance _instance;

    private IceInternal.Transceiver _transceiver;

    private String _desc;
    private final String _type;
    private final IceInternal.EndpointI _endpoint;

    private ObjectAdapter _adapter;
    private IceInternal.ServantManager _servantManager = null;

    private final Logger _logger;
    private final IceInternal.TraceLevels _traceLevels;

    private IceInternal.ThreadPool _threadPool;

    private IceInternal.BasicStream _stream;

    private IceInternal.AsyncCallback _initializeAsyncCallback;
    private IceInternal.AsyncCallback _validateAsyncCallback;
    private IceInternal.AsyncCallback _writeAsyncCallback;
    private IceInternal.AsyncCallback _readAsyncCallback;
    private boolean _reading;

    private Timer _timer;

    private StartCallback _startCallback = null;

    private final boolean _warn;
    private final boolean _warnUdp;

    private final int _acmTimeout;
    private long _acmAbsoluteTimeoutMillis;

    private final int _compressionLevel;

    private int _nextRequestId;

    private java.util.Map<Integer, IceInternal.Outgoing> _requests =
        new java.util.HashMap<Integer, IceInternal.Outgoing>();
    private java.util.Map<Integer, IceInternal.OutgoingAsync> _asyncRequests =
        new java.util.HashMap<Integer, IceInternal.OutgoingAsync>();

    private LocalException _exception;

    private boolean _batchAutoFlush;
    private IceInternal.BasicStream _batchStream;
    private boolean _batchStreamInUse;
    private int _batchRequestNum;
    private boolean _batchRequestCompress;
    private int _batchMarker;

    private java.util.LinkedList<OutgoingMessage> _sendStreams = new java.util.LinkedList<OutgoingMessage>();
    private boolean _sendInProgress;

    private java.util.List<OutgoingMessage> _sentCallbacks = new java.util.LinkedList<OutgoingMessage>();
    private IceInternal.ThreadPoolWorkItem _flushSentCallbacks;;

    private int _dispatchCount;

    private int _state; // The current state.
    private long _stateTime; // The last time when the state was changed.

    private IceInternal.Incoming _incomingCache;
    private java.lang.Object _incomingCacheMutex = new java.lang.Object();

    private IceInternal.Outgoing _outgoingCache;
    private java.lang.Object _outgoingCacheMutex = new java.lang.Object();

    private static boolean _compressionSupported = IceInternal.BasicStream.compressible();

    private boolean _overrideCompress;
    private boolean _overrideCompressValue;
    private boolean _cacheBuffers;
}
