// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

import Ice.Instrumentation.InvocationObserver;

public class CollocatedRequestHandler implements RequestHandler, ResponseHandler
{
    class InvokeAll extends DispatchWorkItem
    {
        public 
        InvokeAll(OutgoingMessageCallback out, BasicStream os, int requestId, int invokeNum, boolean batch)
        {
            _out = out;
            _os = os;
            _requestId = requestId;
            _invokeNum = invokeNum;
            _batch = batch;
        }

        public void
        run()
        {
            if(sent(_out))
            {
                invokeAll(_os, _requestId, _invokeNum, _batch);
            }
        }
    
        private final OutgoingMessageCallback _out;
        private final BasicStream _os;
        private final int _requestId;
        private final int _invokeNum;
        private final boolean _batch;
    };

    class InvokeAllAsync extends DispatchWorkItem
    {
        public InvokeAllAsync(OutgoingAsyncMessageCallback outAsync, BasicStream os, int requestId, int invokeNum, 
                              boolean batch)
        {
            _outAsync = outAsync;
            _os = os; 
            _requestId = requestId; 
            _invokeNum = invokeNum; 
            _batch = batch;
        }

        public void
        run()
        {
            if(sentAsync(_outAsync))
            {
                invokeAll(_os, _requestId, _invokeNum, _batch);
            }
        }
        
        private final OutgoingAsyncMessageCallback _outAsync;
        private final BasicStream _os;
        private final int _requestId;
        private final int _invokeNum;
        private final boolean _batch;
    };

    private void
    fillInValue(BasicStream os, int pos, int value)
    {
        os.pos(pos);
        os.writeInt(value);
    }
    
    public 
    CollocatedRequestHandler(Reference ref, Ice.ObjectAdapter adapter)
    {
        _reference = ref;
        _response = _reference.getMode() == Reference.ModeTwoway;
        _adapter = (Ice.ObjectAdapterI)adapter;

        _logger = _reference.getInstance().initializationData().logger; // Cached for better performance.
        _traceLevels = _reference.getInstance().traceLevels(); // Cached for better performance.
        _batchAutoFlush = ref.getInstance().initializationData().properties.getPropertyAsIntWithDefault(
            "Ice.BatchAutoFlush", 1) > 0;
        _requestId = 0;
        _batchStreamInUse = false;
        _batchRequestNum = 0;
        _batchStream = new BasicStream(ref.getInstance(), Protocol.currentProtocolEncoding, _batchAutoFlush);
    }

    synchronized public void
    prepareBatchRequest(BasicStream os)
    {
        while(_batchStreamInUse)
        {
            try
            {
                wait();
            }
            catch(java.lang.InterruptedException ex)
            {
            }
        }

        if(_batchStream.isEmpty())
        {
            try
            {
                _batchStream.writeBlob(Protocol.requestBatchHdr);
            }
            catch(Ice.LocalException ex)
            {
                throw ex;
            }
        }

        _batchStreamInUse = true;
        _batchMarker = _batchStream.size();
        _batchStream.swap(os);
    }

    public void
    finishBatchRequest(BasicStream os)
    {
        try
        {
            synchronized(this)
            {
                _batchStream.swap(os);
            
                if(_batchAutoFlush & (_batchStream.size() > _reference.getInstance().messageSizeMax()))
                {
                    //
                    // Temporarily save the last request.
                    //
                    byte[] lastRequest = new byte[_batchStream.size() - _batchMarker];
                    Buffer buffer = _batchStream.getBuffer();
                    buffer.b.position(_batchMarker);
                    buffer.b.get(lastRequest);
                    _batchStream.resize(_batchMarker, false);

                    final int invokeNum = _batchRequestNum;
                    final BasicStream stream = new BasicStream(_reference.getInstance(),
                                                               Protocol.currentProtocolEncoding,
                                                               _batchAutoFlush);
                    stream.swap(_batchStream);

                    _adapter.getThreadPool().execute(
                        new DispatchWorkItem()
                        {
                            public void
                            run()
                            {
                                CollocatedRequestHandler.this.invokeAll(stream, 0, invokeNum, true);
                            }
                        });

                    //
                    // Reset the batch.
                    //
                    _batchRequestNum = 0;
                    _batchMarker = 0;
            
                    //
                    // Check again if the last request doesn't exceed what we can send with the auto flush
                    //
                    if(Protocol.requestBatchHdr.length + lastRequest.length > _reference.getInstance().messageSizeMax())
                    {
                        Ex.throwMemoryLimitException(Protocol.requestBatchHdr.length + lastRequest.length,
                                                     _reference.getInstance().messageSizeMax());
                    }

                    //
                    // Start a new batch with the last message that caused us to go over the limit.
                    //
                    _batchStream.writeBlob(Protocol.requestBatchHdr);
                    _batchStream.writeBlob(lastRequest);
                }
        
                //
                // Increment the number of requests in the batch.
                //
                assert(_batchStreamInUse);
                ++_batchRequestNum;
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

    synchronized public void
    abortBatchRequest()
    {
        BasicStream dummy = new BasicStream(_reference.getInstance(), Protocol.currentProtocolEncoding,
                                            _batchAutoFlush);
        _batchStream.swap(dummy);
        _batchRequestNum = 0;
        _batchMarker = 0;

        assert(_batchStreamInUse);
        _batchStreamInUse = false;
        notifyAll();
    }

    public boolean
    sendRequest(OutgoingMessageCallback out)
    {
        out.invokeCollocated(this);
        return !_response && _reference.getInvocationTimeout() == 0;
    }
    
    public int
    sendAsyncRequest(OutgoingAsyncMessageCallback outAsync)
    {
        return outAsync.__invokeCollocated(this);
    }
    
    synchronized public void 
    requestTimedOut(OutgoingMessageCallback out)
    {
        Integer requestId = _sendRequests.get(out);
        if(requestId != null)
        {
            if(requestId > 0)
            {
                _requests.remove(requestId);
            }
            out.finished(new Ice.InvocationTimeoutException(), false);
            _sendRequests.remove(out);
        }
        else if(out instanceof Outgoing)
        {
            Outgoing o = (Outgoing)out;
            assert(o != null);
            for(java.util.Map.Entry<Integer, Outgoing> e : _requests.entrySet())
            {
                if(e.getValue() == o)
                {
                    o.finished(new Ice.InvocationTimeoutException(), true);
                    _requests.remove(e.getKey());
                    return; // We're done.
                }
            }
        }
    }

    public void 
    asyncRequestTimedOut(OutgoingAsyncMessageCallback outAsync)
    {
        OutgoingAsyncMessageCallback out = null;
        boolean sent = false;
        synchronized(this)
        {
            Integer requestId = _sendAsyncRequests.get(outAsync);
            if(requestId != null)
            {
                if(requestId > 0)
                {
                    _asyncRequests.remove(requestId);
                }
                out = outAsync;
                sent = false;
                _sendAsyncRequests.remove(outAsync);
            }
            else if(outAsync instanceof OutgoingAsync)
            {
                OutgoingAsync o = (OutgoingAsync)outAsync;
                assert(o != null);
                for(java.util.Map.Entry<Integer, OutgoingAsync> e : _asyncRequests.entrySet())
                {
                    if(e.getValue() == o)
                    {
                        out = o;
                        sent = true;
                        _asyncRequests.remove(e.getKey());
                        break;
                    }
                }
            }
        }

        if(out != null)
        {
            out.__finished(new Ice.InvocationTimeoutException(), sent);
        }
    }

    public void
    invokeRequest(Outgoing out)
    {
        int requestId = 0;
        if(_reference.getInvocationTimeout() > 0 || _response)
        {
            synchronized(this)
            {
                if(_response)
                {
                    requestId = ++_requestId;
                    _requests.put(requestId, out);
                }
                if(_reference.getInvocationTimeout() > 0)
                {
                    _sendRequests.put(out, requestId);
                }
            }
        }

        out.attachCollocatedObserver(requestId);

        if(_reference.getInvocationTimeout() > 0)
        {
            _adapter.getThreadPool().execute(new InvokeAll(out, out.os(), requestId, 1, false));
        }
        else
        {
            out.sent();
            invokeAll(out.os(), requestId, 1, false);
        }
    }

    public int
    invokeAsyncRequest(OutgoingAsync outAsync)
    {
        int requestId = 0;
        if(_reference.getInvocationTimeout() > 0 || _response)
        {
            synchronized(this)
            {
                if(_response)
                {
                    requestId = ++_requestId;
                    _asyncRequests.put(requestId, outAsync);
                }
                if(_reference.getInvocationTimeout() > 0)
                {
                    _sendAsyncRequests.put(outAsync, requestId);
                }
            }
        }

        outAsync.__attachCollocatedObserver(requestId);

        _adapter.getThreadPool().execute(new InvokeAllAsync(outAsync, outAsync.__getOs(), requestId, 1, false));

        return AsyncStatus.Queued;
    }

    public void
    invokeBatchRequests(BatchOutgoing out)
    {
        int invokeNum;
        synchronized(this)
        {
            while(_batchStreamInUse)
            {
                try
                {
                    wait();
                }
                catch(java.lang.InterruptedException ex)
                {
                }
            }
                
            invokeNum = _batchRequestNum;
                
            if(_batchRequestNum > 0)
            {
                if(_reference.getInvocationTimeout() > 0)
                {
                    _sendRequests.put(out, 0);
                }
                
                assert(!_batchStream.isEmpty());
                _batchStream.swap(out.os());
            
                //
                // Reset the batch stream.
                //
                BasicStream dummy = new BasicStream(_reference.getInstance(), Protocol.currentProtocolEncoding,
                                                    _batchAutoFlush);
                _batchStream.swap(dummy);
                _batchRequestNum = 0;
                _batchMarker = 0;
            }
        }
        
        out.attachCollocatedObserver(0);

        if(invokeNum > 0)
        {
            if(_reference.getInvocationTimeout() > 0)
            {
                _adapter.getThreadPool().execute(new InvokeAll(out, out.os(), 0, invokeNum, true));
            }
            else
            {
                out.sent();
                invokeAll(out.os(), 0, invokeNum, true);
            }
        }
        else
        {
            out.sent();
        }
    }

    public int
    invokeAsyncBatchRequests(BatchOutgoingAsync outAsync)
    {
        int invokeNum;
        synchronized(this)
        {
            while(_batchStreamInUse)
            {
                try
                {
                    wait();
                }
                catch(java.lang.InterruptedException ex)
                {
                }
            }
            
            invokeNum = _batchRequestNum;
            if(_batchRequestNum > 0)
            {
                if(_reference.getInvocationTimeout() > 0)
                {
                    _sendAsyncRequests.put(outAsync, 0);
                }

                assert(!_batchStream.isEmpty());
                _batchStream.swap(outAsync.__getOs());
            
                //
                // Reset the batch stream.
                //
                BasicStream dummy = new BasicStream(_reference.getInstance(), Protocol.currentProtocolEncoding,
                                                    _batchAutoFlush);
                _batchStream.swap(dummy);
                _batchRequestNum = 0;
                _batchMarker = 0;
            }
        }

        outAsync.__attachCollocatedObserver(0);
    
        if(invokeNum > 0)
        {
            _adapter.getThreadPool().execute(new InvokeAllAsync(outAsync, outAsync.__getOs(), 0, invokeNum, true));
            return AsyncStatus.Queued;
        }
        else if(outAsync.__sent())
        {
            return AsyncStatus.Sent | AsyncStatus.InvokeSentCallback;
        }
        else
        {
            return AsyncStatus.Sent;
        }
    }

    public void 
    sendResponse(int requestId, BasicStream os, byte status)
    {
        OutgoingAsync outAsync = null;
        synchronized(this)
        {
            assert(_response);

            os.pos(Protocol.replyHdr.length + 4);

            if(_traceLevels.protocol >= 1)
            {
                fillInValue(os, 10, os.size());
                TraceUtil.traceRecv(os, _logger, _traceLevels);
            }

            Outgoing out = _requests.get(requestId);
            if(out != null)
            {
                out.finished(os);
                _requests.remove(requestId);
            }
            else
            {
                outAsync = _asyncRequests.get(requestId);
                if(outAsync != null)
                {
                    _asyncRequests.remove(requestId);
                }
            }
        }

        if(outAsync != null)
        {
            outAsync.__finished(os);
        }
        _adapter.decDirectCount();
    }

    public void
    sendNoResponse()
    {
        _adapter.decDirectCount();
    }
    
    public void 
    invokeException(int requestId, Ice.LocalException ex, int invokeNum)
    {
        if(requestId > 0)
        {
            OutgoingAsync outAsync = null;
            synchronized(this)
            {
                Outgoing out = _requests.remove(requestId);
                if(out != null)
                {
                    out.finished(ex, true);
                }
                else
                {
                    outAsync = _asyncRequests.remove(requestId);
                }
            }
            if(outAsync != null)
            {
                outAsync.__finished(ex, true);
            }
        }
        _adapter.decDirectCount();
    }

    public Reference
    getReference()
    {
        return _reference;
    }

    public Ice.ConnectionI
    getConnection(boolean wait)
    {
        return null;
    }
    
    boolean
    sent(OutgoingMessageCallback out)
    {
        if(_reference.getInvocationTimeout() > 0)
        {
            synchronized(this)
            {
                if(_sendRequests.remove(out) == null)
                {
                    return false; // The request timed-out.
                }
                out.sent();
            }
        }
        else
        {
            out.sent();
        }
        return true;
    }
    
    boolean
    sentAsync(OutgoingAsyncMessageCallback outAsync)
    {
        if(_reference.getInvocationTimeout() > 0)
        {
            synchronized(this)
            {
                if(_sendAsyncRequests.remove(outAsync) == null)
                {
                    return false; // The request timed-out.
                }
            }
        }
        if(outAsync.__sent())
        {
            outAsync.__invokeSent();
        }
        return true;
    }
    
    void
    invokeAll(BasicStream os, int requestId, int invokeNum, boolean batch)
    {
        if(batch)
        {
            os.pos(Protocol.requestBatchHdr.length);
        }
        else
        {
            os.pos(Protocol.requestHdr.length);
        }
        
        if(_traceLevels.protocol >= 1)
        {
            fillInValue(os, 10, os.size());
            if(requestId > 0)
            {
                fillInValue(os, Protocol.headerSize, requestId);
            }
            else if(batch)
            {
                fillInValue(os, Protocol.headerSize, invokeNum);
            }
            TraceUtil.traceSend(os, _logger, _traceLevels);
        }

        ServantManager servantManager = _adapter.getServantManager();
        try
        {
            while(invokeNum > 0)
            {        
                try
                {
                    _adapter.incDirectCount();
                }
                catch(Ice.ObjectAdapterDeactivatedException ex)
                {
                    handleException(requestId, ex, false);
                    return;
                }

                Incoming in = new Incoming(_reference.getInstance(), this, null, _adapter, _response, (byte)0, 
                                           requestId);
                try
                {
                    in.invoke(servantManager, os);
                }
                catch(Ice.SystemException ex)
                {
                    handleException(requestId, ex, true);
                    _adapter.decDirectCount();
                }
                --invokeNum;
            }
        }
        catch(Ice.LocalException ex)
        {
            invokeException(requestId, ex, invokeNum); // Fatal invocation exception
        }
        catch(java.lang.AssertionError ex) // Upon assertion, we print the stack trace.
        {
            Ice.UnknownException uex = new Ice.UnknownException(ex);
            java.io.StringWriter sw = new java.io.StringWriter();
            java.io.PrintWriter pw = new java.io.PrintWriter(sw);
            ex.printStackTrace(pw);
            pw.flush();
            uex.unknown = sw.toString();
            _logger.error(uex.unknown);
            invokeException(requestId, uex, invokeNum);
        }
        catch(java.lang.OutOfMemoryError ex)
        {
            Ice.UnknownException uex = new Ice.UnknownException(ex);
            java.io.StringWriter sw = new java.io.StringWriter();
            java.io.PrintWriter pw = new java.io.PrintWriter(sw);
            ex.printStackTrace(pw);
            pw.flush();
            uex.unknown = sw.toString();
            _logger.error(uex.unknown);
            invokeException(requestId, uex, invokeNum);
        }
    }

    void
    handleException(int requestId, Ice.Exception ex, boolean sent)
    {
        if(requestId == 0)
        {
            return; // Ignore exception for oneway messages.
        }

        OutgoingAsync outAsync = null;
        synchronized(this)
        {
            Outgoing out = _requests.get(requestId);
            if(out != null)
            {
                out.finished(ex, sent);
                _requests.remove(requestId);
            }
            else
            {
                outAsync = _asyncRequests.get(requestId);
                if(outAsync != null)
                {
                    _asyncRequests.remove(requestId);
                }
            }
        }

        if(outAsync != null)
        {
            outAsync.__finished(ex, sent);
        }
    }

    private final Reference _reference;
    private final boolean _response;
    private final Ice.ObjectAdapterI _adapter;
    private final Ice.Logger _logger;
    private final TraceLevels _traceLevels;
    private boolean _batchAutoFlush;

    private int _requestId;
    
    private java.util.Map<OutgoingMessageCallback, Integer> _sendRequests = 
        new java.util.HashMap<OutgoingMessageCallback, Integer>();
    private java.util.Map<OutgoingAsyncMessageCallback, Integer> _sendAsyncRequests = 
        new java.util.HashMap<OutgoingAsyncMessageCallback, Integer>();

    private java.util.Map<Integer, Outgoing> _requests = new java.util.HashMap<Integer, Outgoing>();
    private java.util.Map<Integer, OutgoingAsync> _asyncRequests = new java.util.HashMap<Integer, OutgoingAsync>();

    private BasicStream _batchStream;
    private boolean _batchStreamInUse;
    private int _batchRequestNum;
    private int _batchMarker;
}