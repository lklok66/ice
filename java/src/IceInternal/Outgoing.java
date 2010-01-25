// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public final class Outgoing implements OutgoingMessageCallback
{
    public
    Outgoing(RequestHandler handler, String operation, Ice.OperationMode mode, java.util.Map<String, String> context)
        throws LocalExceptionWrapper
    {
        _state = StateUnsent;
        _sent = false;
        _handler = handler;

        Instance instance = _handler.getReference().getInstance();
        _is = new BasicStream(instance);
        _os = new BasicStream(instance);

        writeHeader(operation, mode, context);
    }

    //
    // These functions allow this object to be reused, rather than reallocated.
    //
    public void
    reset(RequestHandler handler, String operation, Ice.OperationMode mode, java.util.Map<String, String> context)
        throws LocalExceptionWrapper
    {
        _state = StateUnsent;
        _exception = null;
        _sent = false;
        _handler = handler;

        writeHeader(operation, mode, context);
    }

    public void
    reclaim()
    {
        _is.reset();
        _os.reset();
    }

    // Returns true if ok, false if user exception.
    public boolean
    invoke()
        throws LocalExceptionWrapper
    {
        assert(_state == StateUnsent);

        _os.endWriteEncaps();

        switch(_handler.getReference().getMode())
        {
            case Reference.ModeTwoway:
            {
                _state = StateInProgress;

                Ice.ConnectionI connection = _handler.sendRequest(this);
                assert(connection != null);

                boolean timedOut = false;

                synchronized(this)
                {

                    //
                    // If the request is being sent in the background we first wait for the
                    // sent notification.
                    //
                    while(_state != StateFailed && !_sent)
                    {
                        try
                        {
                            wait();
                        }
                        catch(java.lang.InterruptedException ex)
                        {
                        }
                    }
            
                    //
                    // Wait until the request has completed, or until the request
                    // times out.
                    //
                    int timeout = connection.timeout();
                    while(_state == StateInProgress && !timedOut)
                    {
                        try
                        {
                            if(timeout >= 0)
                            {
                                wait(timeout);
                        
                                if(_state == StateInProgress)
                                {
                                    timedOut = true;
                                }
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
                }
        
                if(timedOut)
                {
                    //
                    // Must be called outside the synchronization of
                    // this object
                    //
                    connection.exception(new Ice.TimeoutException());
            
                    //
                    // We must wait until the exception set above has
                    // propagated to this Outgoing object.
                    //
                    synchronized(this)
                    {
                        while(_state == StateInProgress)
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
                }
        
                if(_exception != null)
                {
                    _exception.fillInStackTrace();
            
                    //      
                    // A CloseConnectionException indicates graceful
                    // server shutdown, and is therefore always repeatable
                    // without violating "at-most-once". That's because by
                    // sending a close connection message, the server
                    // guarantees that all outstanding requests can safely
                    // be repeated.
                    //
                    // An ObjectNotExistException can always be retried as
                    // well without violating "at-most-once" (see the
                    // implementation of the checkRetryAfterException
                    // method of the ProxyFactory class for the reasons
                    // why it can be useful).
                    //
                    if(!_sent || 
                       _exception instanceof Ice.CloseConnectionException || 
                       _exception instanceof Ice.ObjectNotExistException)
                    {
                        throw _exception;
                    }
            
                    //
                    // Throw the exception wrapped in a LocalExceptionWrapper,
                    // to indicate that the request cannot be resent without
                    // potentially violating the "at-most-once" principle.
                    //
                    throw new LocalExceptionWrapper(_exception, false);
                }
        
                if(_state == StateUserException)
                {
                    return false;
                }
                else
                {
                    assert(_state == StateOK);
                    return true;
                }

            }

            case Reference.ModeOneway:
            case Reference.ModeDatagram:
            {
                _state = StateInProgress;
                if(_handler.sendRequest(this) != null)
                {
                    //
                    // If the handler returns the connection, we must wait for the sent callback.
                    //
                    synchronized(this)
                    {
                        while(_state != StateFailed && !_sent)
                        {
                            try
                            {
                                wait();
                            }
                            catch(java.lang.InterruptedException ex)
                            {
                            }
                        }
                        
                        if(_exception != null)
                        {
                            assert(!_sent);
                            throw _exception;
                        }
                    }
                }
                return true;
            }

            case Reference.ModeBatchOneway:
            case Reference.ModeBatchDatagram:
            {
                //
                // For batch oneways and datagrams, the same rules as for
                // regular oneways and datagrams (see comment above)
                // apply.
                //
                _state = StateInProgress;
                _handler.finishBatchRequest(_os);
                return true;
            }
        }
        
        assert(false);
        return false;
    }

    public void
    abort(Ice.LocalException ex)
        throws LocalExceptionWrapper
    {
        assert(_state == StateUnsent);

        //
        // If we didn't finish a batch oneway or datagram request, we
        // must notify the connection about that we give up ownership
        // of the batch stream.
        //
        int mode = _handler.getReference().getMode();
        if(mode == Reference.ModeBatchOneway || mode == Reference.ModeBatchDatagram)
        {
            _handler.abortBatchRequest();
        }

        throw ex;
    }

    public void
    sent(boolean async)
    {
        if(async)
        {
            synchronized(this)
            {
                _sent = true;
                notify();
            }
        }
        else
        {
            //
            // No synchronization is necessary if called from sendRequest() because the connection
            // send mutex is locked and no other threads can call on Outgoing until it's released.
            //
            _sent = true;
        }
    }
    
    public synchronized void
    finished(BasicStream is)
    {
        assert(_handler.getReference().getMode() == Reference.ModeTwoway); // Only for twoways.
        
        assert(_state <= StateInProgress);
        
        _is.swap(is);
        byte replyStatus = _is.readByte();
        
        switch(replyStatus)
        {
            case ReplyStatus.replyOK:
            {
                _state = StateOK; // The state must be set last, in case there is an exception.
                break;
            }
            
            case ReplyStatus.replyUserException:
            {
                _state = StateUserException; // The state must be set last, in case there is an exception.
                break;
            }
            
            case ReplyStatus.replyObjectNotExist:
            case ReplyStatus.replyFacetNotExist:
            case ReplyStatus.replyOperationNotExist:
            {
                Ice.RequestFailedException ex = null;
                switch(replyStatus)
                {
                    case ReplyStatus.replyObjectNotExist:
                    {
                        ex = new Ice.ObjectNotExistException();
                        break;
                    }
                    
                    case ReplyStatus.replyFacetNotExist:
                    {
                        ex = new Ice.FacetNotExistException();
                        break;
                    }
                    
                    case ReplyStatus.replyOperationNotExist:
                    {
                        ex = new Ice.OperationNotExistException();
                        break;
                    }
                    
                    default:
                    {
                        assert(false);
                        break;
                    }
                }
                
                ex.id = new Ice.Identity();
                ex.id.__read(_is);

                //
                // For compatibility with the old FacetPath.
                //
                String[] facetPath = _is.readStringSeq();
                if(facetPath.length > 0)
                {
                    if(facetPath.length > 1)
                    {
                        throw new Ice.MarshalException();
                    }
                    ex.facet = facetPath[0];
                }
                else
                {
                    ex.facet = "";
                }

                ex.operation = _is.readString();
                _exception = ex;

                _state = StateLocalException; // The state must be set last, in case there is an exception.
                break;
            }
            
            case ReplyStatus.replyUnknownException:
            case ReplyStatus.replyUnknownLocalException:
            case ReplyStatus.replyUnknownUserException:
            {
                Ice.UnknownException ex = null;
                switch(replyStatus)
                {
                    case ReplyStatus.replyUnknownException:
                    {
                        ex = new Ice.UnknownException();
                        break;
                    }
                    
                    case ReplyStatus.replyUnknownLocalException:
                    {
                        ex = new Ice.UnknownLocalException();
                        break;
                    }
                    
                    case ReplyStatus.replyUnknownUserException: 
                    {
                        ex = new Ice.UnknownUserException();
                        break;
                    }
                    
                    default:
                    {
                        assert(false);
                        break;
                    }
                }
                
                ex.unknown = _is.readString();
                _exception = ex;

                _state = StateLocalException; // The state must be set last, in case there is an exception.
                break;
            }
            
            default:
            {
                _exception = new Ice.UnknownReplyStatusException();
                _state = StateLocalException;
                break;
            }
        }

        notify();
    }

    public synchronized void
    finished(Ice.LocalException ex, boolean sent)
    {
        assert(_state <= StateInProgress);
        _state = StateFailed;
        _exception = ex;
        _sent = sent;
        notify();
    }

    public BasicStream
    is()
    {
        return _is;
    }

    public BasicStream
    os()
    {
        return _os;
    }

    public void 
    throwUserException()
        throws Ice.UserException
    {
        try
        {
            _is.startReadEncaps();
            _is.throwException();
        }
        catch(Ice.UserException ex)
        {
            _is.endReadEncaps();
            throw ex;
        }
    }

    private void
    writeHeader(String operation, Ice.OperationMode mode, java.util.Map<String, String> context)
        throws LocalExceptionWrapper
    {
        switch(_handler.getReference().getMode())
        {
            case Reference.ModeTwoway:
            case Reference.ModeOneway:
            case Reference.ModeDatagram:
            {
                _os.writeBlob(IceInternal.Protocol.requestHdr);
                break;
            }

            case Reference.ModeBatchOneway:
            case Reference.ModeBatchDatagram:
            {
                _handler.prepareBatchRequest(_os);
                break;
            }
        }

        try
        {
            _handler.getReference().getIdentity().__write(_os);

            //
            // For compatibility with the old FacetPath.
            //
            String facet = _handler.getReference().getFacet();
            if(facet == null || facet.length() == 0)
            {
                _os.writeStringSeq(null);
            }
            else
            {
                String[] facetPath = { facet };
                _os.writeStringSeq(facetPath);
            }

            _os.writeString(operation);

            _os.writeByte((byte)mode.ordinal());

            if(context != null)
            {
                //
                // Explicit context
                //
                Ice.ContextHelper.write(_os, context);
            }
            else
            {
                //
                // Implicit context
                //
                Ice.ImplicitContextI implicitContext = _handler.getReference().getInstance().getImplicitContext();
                java.util.Map<String, String> prxContext = _handler.getReference().getContext();
                
                if(implicitContext == null)
                {
                    Ice.ContextHelper.write(_os, prxContext);
                }
                else
                {
                    implicitContext.write(prxContext, _os);
                }
            }

            //
            // Input and output parameters are always sent in an
            // encapsulation, which makes it possible to forward requests as
            // blobs.
            //
            _os.startWriteEncaps();
        }
        catch(Ice.LocalException ex)
        {
            abort(ex);
        }
    }

    private RequestHandler _handler;
    private BasicStream _is;
    private BasicStream _os;
    private boolean _sent;

    private Ice.LocalException _exception;

    private static final int StateUnsent = 0;
    private static final int StateInProgress = 1;
    private static final int StateOK = 2;
    private static final int StateUserException = 3;
    private static final int StateLocalException = 4;
    private static final int StateFailed = 5;
    private int _state;

    public Outgoing next; // For use by Ice._ObjectDelM
}
