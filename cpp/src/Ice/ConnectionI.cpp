// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <Ice/ConnectionI.h>
#include <Ice/Instance.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Properties.h>
#include <Ice/TraceUtil.h>
#include <Ice/DefaultsAndOverrides.h>
#include <Ice/Transceiver.h>
#include <Ice/ThreadPool.h>
#include <Ice/ConnectionMonitor.h>
#include <Ice/ObjectAdapterI.h> // For getThreadPool() and getServantManager().
#include <Ice/EndpointI.h>
#include <Ice/Outgoing.h>
#include <Ice/OutgoingAsync.h>
#include <Ice/Incoming.h>
#include <Ice/LocalException.h>
#include <Ice/ReferenceFactory.h> // For createProxy().
#include <Ice/ProxyFactory.h> // For createProxy().

#if !defined(__APPLE__) || TARGET_OS_IPHONE == 0
#include <bzlib.h>
#endif

using namespace std;
using namespace Ice;
using namespace IceInternal;

Ice::LocalObject* IceInternal::upCast(ConnectionI* p) { return p; }

namespace
{

class TimeoutCallback : public IceUtil::TimerTask
{
public:

    TimeoutCallback(Ice::ConnectionI* connection) : _connection(connection)
    {
    }

    void
    runTimerTask()
    {
        _connection->timedOut();
    }
    
private:

    Ice::ConnectionI* _connection;
};

}

void
Ice::ConnectionI::OutgoingMessage::adopt(BasicStream* str)
{
    if(adopted)
    {
        if(str)
        {
            delete stream;
            stream = 0;
            adopted = false;
        }
        else
        {
            return; // Stream is already adopted.
        }
    }
    else if(!str)
    {
        if(out || outAsync)
        {
            return; // Adopting request stream is not necessary.
        }
        else
        {
            str = stream; // Adopt this stream
            stream = 0;
        }
    }

    assert(str);
    stream = new BasicStream(str->instance());
    stream->swap(*str);
    adopted = true;
}

void
Ice::ConnectionI::OutgoingMessage::sent(ConnectionI* connection, bool notify)
{
    if(out)
    {
        out->sent(notify); // true = notify the waiting thread that the request was sent.
    }
    else if(outAsync)
    {
        outAsync->__sent(connection);
    }

    if(adopted)
    {
        delete stream;
        stream = 0;
    }
}

void
Ice::ConnectionI::OutgoingMessage::finished(const Ice::LocalException& ex)
{
    if(!response)
    {
        //
        // Only notify oneway requests. The connection keeps track of twoway
        // requests in the _requests/_asyncRequests maps and will notify them
        // of the connection exceptions.
        //
        if(out)
        {
            out->finished(ex);
        }
        else if(outAsync)
        {
            outAsync->__finished(ex);
        }
    }

    if(adopted)
    {
        delete stream;
        stream = 0;
    }
}

void
Ice::ConnectionI::start(const StartCallbackPtr& callback)
{
    try
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        if(_state >= StateClosed) // The connection might already be closed if the communicator was destroyed.
        {
            assert(_exception.get());
            _exception->ice_throw();
        }

        if(!initialize() || !validate())
        {
            if(callback)
            {
                _startCallback = callback;
                return;
            }

            //
            // Wait for the connection to be validated.
            //
            while(_state <= StateNotValidated)
            {
                wait();
            }

            if(_state >= StateClosing)
            {
                assert(_exception.get());
                _exception->ice_throw();
            }
        }

        //
        // We start out in holding state.
        //
        setState(StateHolding);
    }
    catch(const Ice::LocalException& ex)
    {
        exception(ex);
        if(callback)
        {
            callback->connectionStartFailed(this, *_exception.get());
            return;
        }
        else
        {
            waitUntilFinished();
            throw;
        }
    }

    if(callback)
    {
        callback->connectionStartCompleted(this);
    }
}

void
Ice::ConnectionI::activate()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    if(_state <= StateNotValidated)
    {
        return;
    }

    if(_acmTimeout > 0)
    {
        _acmAbsoluteTimeout = IceUtil::Time::now(IceUtil::Time::Monotonic) + IceUtil::Time::seconds(_acmTimeout);
    }

    setState(StateActive);
}

void
Ice::ConnectionI::hold()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    if(_state <= StateNotValidated)
    {
        return;
    }

    setState(StateHolding);
}

void
Ice::ConnectionI::destroy(DestructionReason reason)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    switch(reason)
    {
        case ObjectAdapterDeactivated:
        {
            setState(StateClosing, ObjectAdapterDeactivatedException(__FILE__, __LINE__));
            break;
        }

        case CommunicatorDestroyed:
        {
            setState(StateClosing, CommunicatorDestroyedException(__FILE__, __LINE__));
            break;
        }
    }
}

void
Ice::ConnectionI::close(bool force)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    if(force)
    {
        setState(StateClosed, ForcedCloseConnectionException(__FILE__, __LINE__));
    }
    else
    {
        //
        // If we do a graceful shutdown, then we wait until all
        // outstanding requests have been completed. Otherwise, the
        // CloseConnectionException will cause all outstanding
        // requests to be retried, regardless of whether the server
        // has processed them or not.
        //
        while(!_requests.empty() || !_asyncRequests.empty())
        {
            wait();
        }

        setState(StateClosing, CloseConnectionException(__FILE__, __LINE__));
    }
}

bool
Ice::ConnectionI::isActiveOrHolding() const
{
    //
    // We can not use trylock here, otherwise the outgoing connection
    // factory might return destroyed (closing or closed) connections,
    // resulting in connection retry exhaustion.
    //
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    return _state > StateNotValidated && _state < StateClosing;
}

bool
Ice::ConnectionI::isFinished() const
{
    //
    // We can use trylock here, because as long as there are still
    // threads operating in this connection object, connection
    // destruction is considered as not yet finished.
    //
    IceUtil::Monitor<IceUtil::Mutex>::TryLock sync(*this);

    if(!sync.acquired())
    {
        return false;
    }

    if(_state != StateFinished || _dispatchCount != 0)
    {
        return false;
    }

    assert(_state == StateFinished);
    return true;
}

void
Ice::ConnectionI::throwException() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    if(_exception.get())
    {
        assert(_state >= StateClosing);
        _exception->ice_throw();
    }
}

void
Ice::ConnectionI::waitUntilHolding() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    while(_state < StateHolding || _dispatchCount > 0)
    {
        wait();
    }
}

void
Ice::ConnectionI::waitUntilFinished()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    //
    // We wait indefinitely until connection closing has been
    // initiated. We also wait indefinitely until all outstanding
    // requests are completed. Otherwise we couldn't guarantee
    // that there are no outstanding calls when deactivate() is
    // called on the servant locators.
    //
    while(_state < StateClosing || _dispatchCount > 0)
    {
        wait();
    }

    //
    // Now we must wait until close() has been called on the
    // transceiver.
    //
    while(_state != StateFinished)
    {
        if(_state < StateClosed && _endpoint->timeout() >= 0)
        {
            IceUtil::Time timeout = IceUtil::Time::milliSeconds(_endpoint->timeout());
            IceUtil::Time waitTime = _stateTime + timeout - IceUtil::Time::now(IceUtil::Time::Monotonic);

            if(waitTime > IceUtil::Time())
            {
                //
                // We must wait a bit longer until we close this
                // connection.
                //
                if(!timedWait(waitTime))
                {
                    setState(StateClosed, CloseTimeoutException(__FILE__, __LINE__));
                }
            }
            else
            {
                //
                // We already waited long enough, so let's close this
                // connection!
                //
                setState(StateClosed, CloseTimeoutException(__FILE__, __LINE__));
            }

            //
            // No return here, we must still wait until close() is
            // called on the _transceiver.
            //
        }
        else
        {
            wait();
        }
    }

    assert(_state == StateFinished);

    //
    // Clear the OA. See bug 1673 for the details of why this is necessary.
    //
    _adapter = 0;
}

void
Ice::ConnectionI::monitor(const IceUtil::Time& now)
{
    IceUtil::Monitor<IceUtil::Mutex>::TryLock sync(*this);
    if(!sync.acquired())
    {
        return;
    }

    if(_state != StateActive)
    {
        return;
    }

    //
    // Active connection management for idle connections.
    //
    if(_acmTimeout <= 0 ||
       !_requests.empty() || !_asyncRequests.empty() ||
       _batchStreamInUse || !_batchStream.b.empty() ||
       !_sendStreams.empty() || _dispatchCount > 0)
    {
        return;
    }

    if(now >= _acmAbsoluteTimeout)
    {
        setState(StateClosing, ConnectionTimeoutException(__FILE__, __LINE__));
    }
}

bool
Ice::ConnectionI::sendRequest(Outgoing* out, bool compress, bool response)
{
    BasicStream* os = out->os();

    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    if(_exception.get())
    {
        //
        // If the connection is closed before we even have a chance
        // to send our request, we always try to send the request
        // again.
        //
        throw LocalExceptionWrapper(*_exception.get(), true);
    }

    assert(_state > StateNotValidated);
    assert(_state < StateClosing);

    Int requestId;
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
        const Byte* p = reinterpret_cast<const Byte*>(&requestId);
#ifdef ICE_BIG_ENDIAN
        reverse_copy(p, p + sizeof(Int), os->b.begin() + headerSize);
#else
        copy(p, p + sizeof(Int), os->b.begin() + headerSize);
#endif
    }

    //
    // Send the message. If it can't be sent without blocking the message is added
    // to _sendStreams and it will be sent by the selector thread.
    //
    bool sent = false;
    try
    {
        OutgoingMessage message(out, os, compress, response);
        sent = sendMessage(message);
    }
    catch(const LocalException& ex)
    {
        setState(StateClosed, ex);
        assert(_exception.get());
        _exception->ice_throw();
    }

    if(response)
    {
        //
        // Add to the requests map.
        //
        _requestsHint = _requests.insert(_requests.end(), pair<const Int, Outgoing*>(requestId, out));
    }

    return sent;
}

bool
Ice::ConnectionI::sendAsyncRequest(const OutgoingAsyncPtr& out, bool compress, bool response)
{
    BasicStream* os = out->__getOs();

    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    if(_exception.get())
    {
        //
        // If the exception is closed before we even have a chance
        // to send our request, we always try to send the request
        // again.
        //
        throw LocalExceptionWrapper(*_exception.get(), true);
    }

    assert(_state > StateNotValidated);
    assert(_state < StateClosing);

    Int requestId;
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
        const Byte* p = reinterpret_cast<const Byte*>(&requestId);
#ifdef ICE_BIG_ENDIAN
        reverse_copy(p, p + sizeof(Int), os->b.begin() + headerSize);
#else
        copy(p, p + sizeof(Int), os->b.begin() + headerSize);
#endif
    }

    bool sent = false;
    try
    {
        OutgoingMessage message(out, os, compress, response);
        sent = sendMessage(message);
    }
    catch(const LocalException& ex)
    {
        setState(StateClosed, ex);
        assert(_exception.get());
        _exception->ice_throw();
    }

    if(response)
    {
        //
        // Add to the async requests map.
        //
        _asyncRequestsHint = _asyncRequests.insert(_asyncRequests.end(),
                                                   pair<const Int, OutgoingAsyncPtr>(requestId, out));
    }
    return sent;
}

void
Ice::ConnectionI::prepareBatchRequest(BasicStream* os)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    //
    // Wait if flushing is currently in progress.
    //
    while(_batchStreamInUse && !_exception.get())
    {
        wait();
    }

    if(_exception.get())
    {
        _exception->ice_throw();
    }

    assert(_state > StateNotValidated);
    assert(_state < StateClosing);

    if(_batchStream.b.empty())
    {
        try
        {
            _batchStream.writeBlob(requestBatchHdr, sizeof(requestBatchHdr));
        }
        catch(const LocalException& ex)
        {
            setState(StateClosed, ex);
            ex.ice_throw();
        }
    }

    _batchStreamInUse = true;
    _batchMarker = _batchStream.b.size();
    _batchStream.swap(*os);

    //
    // The batch stream now belongs to the caller, until
    // finishBatchRequest() or abortBatchRequest() is called.
    //
}

void
Ice::ConnectionI::finishBatchRequest(BasicStream* os, bool compress)
{
    try
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

        //
        // Get the batch stream back.
        //
        _batchStream.swap(*os);

        if(_exception.get())
        {
            _exception->ice_throw();
        }

        bool flush = false;
        if(_batchAutoFlush)
        {
            //
            // Throw memory limit exception if the first message added causes us to
            // go over limit. Otherwise put aside the marshalled message that caused
            // limit to be exceeded and rollback stream to the marker.
            //
            try
            {
                _transceiver->checkSendSize(_batchStream, _instance->messageSizeMax());
            }
            catch(const Ice::Exception&)
            {
                if(_batchRequestNum > 0)
                {
                    flush = true;
                }
                else
                {
                    throw;
                }
            }
        }

        if(flush)
        {
            //
            // Temporarily save the last request.
            //
            vector<Ice::Byte> lastRequest(_batchStream.b.begin() + _batchMarker, _batchStream.b.end());
            _batchStream.b.resize(_batchMarker);

            //
            // Send the batch stream without the last request.
            //
            try
            {
                //
                // Fill in the number of requests in the batch.
                //
                const Byte* p = reinterpret_cast<const Byte*>(&_batchRequestNum);
#ifdef ICE_BIG_ENDIAN
                reverse_copy(p, p + sizeof(Int), _batchStream.b.begin() + headerSize);
#else
                copy(p, p + sizeof(Int), _batchStream.b.begin() + headerSize);
#endif

                OutgoingMessage message(&_batchStream, _batchRequestCompress);
                sendMessage(message);
            }
            catch(const Ice::LocalException& ex)
            {
                setState(StateClosed, ex);
                assert(_exception.get());
                _exception->ice_throw();
            }

            //
            // Reset the batch.
            //
            BasicStream dummy(_instance.get(), _batchAutoFlush);
            _batchStream.swap(dummy);
            _batchRequestNum = 0;
            _batchRequestCompress = false;
            _batchMarker = 0;

            //
            // Check again if the last request doesn't exceed what we can send with the auto flush
            //
            if(sizeof(requestBatchHdr) + lastRequest.size() >  _instance->messageSizeMax())
            {
                throw MemoryLimitException(__FILE__, __LINE__);
            }

            //
            // Start a new batch with the last message that caused us to go over the limit.
            //
            _batchStream.writeBlob(requestBatchHdr, sizeof(requestBatchHdr));
            _batchStream.writeBlob(&lastRequest[0], lastRequest.size());
        }

        //
        // Increment the number of requests in the batch.
        //
        ++_batchRequestNum;

        //
        // We compress the whole batch if there is at least one compressed
        // message.
        //
        if(compress)
        {
            _batchRequestCompress = true;
        }

        //
        // Notify about the batch stream not being in use anymore.
        //
        assert(_batchStreamInUse);
        _batchStreamInUse = false;
        notifyAll();
    }
    catch(const Ice::LocalException&)
    {
        abortBatchRequest();
        throw;
    }
}

void
Ice::ConnectionI::abortBatchRequest()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    BasicStream dummy(_instance.get(), _batchAutoFlush);
    _batchStream.swap(dummy);
    _batchRequestNum = 0;
    _batchRequestCompress = false;
    _batchMarker = 0;

    assert(_batchStreamInUse);
    _batchStreamInUse = false;
    notifyAll();
}

void
Ice::ConnectionI::flushBatchRequests()
{
    BatchOutgoing out(this, _instance.get());
    out.invoke();
}

bool
Ice::ConnectionI::flushBatchRequests(BatchOutgoing* out)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    while(_batchStreamInUse && !_exception.get())
    {
        wait();
    }

    if(_exception.get())
    {
        _exception->ice_throw();
    }

    if(_batchRequestNum == 0)
    {
        out->sent(false);
        return true;
    }

    //
    // Fill in the number of requests in the batch.
    //
    const Byte* p = reinterpret_cast<const Byte*>(&_batchRequestNum);
#ifdef ICE_BIG_ENDIAN
    reverse_copy(p, p + sizeof(Int), _batchStream.b.begin() + headerSize);
#else
    copy(p, p + sizeof(Int), _batchStream.b.begin() + headerSize);
#endif
    _batchStream.swap(*out->os());

    //
    // Send the batch stream.
    //
    bool sent = false;
    try
    {
        OutgoingMessage message(out, out->os(), _batchRequestCompress, false);
        sent = sendMessage(message);
    }
    catch(const Ice::LocalException& ex)
    {
        setState(StateClosed, ex);
        assert(_exception.get());
        _exception->ice_throw();
    }

    //
    // Reset the batch stream.
    //
    BasicStream dummy(_instance.get(), _batchAutoFlush);
    _batchStream.swap(dummy);
    _batchRequestNum = 0;
    _batchRequestCompress = false;
    _batchMarker = 0;
    return sent;
}

bool
Ice::ConnectionI::flushAsyncBatchRequests(const BatchOutgoingAsyncPtr& outAsync)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    while(_batchStreamInUse && !_exception.get())
    {
        wait();
    }

    if(_exception.get())
    {
        _exception->ice_throw();
    }

    if(_batchRequestNum == 0)
    {
        outAsync->__sent(this);
        return true;
    }

    //
    // Fill in the number of requests in the batch.
    //
    const Byte* p = reinterpret_cast<const Byte*>(&_batchRequestNum);
#ifdef ICE_BIG_ENDIAN
    reverse_copy(p, p + sizeof(Int), _batchStream.b.begin() + headerSize);
#else
    copy(p, p + sizeof(Int), _batchStream.b.begin() + headerSize);
#endif
    _batchStream.swap(*outAsync->__getOs());

    //
    // Send the batch stream.
    //
    bool sent = false;
    try
    {
        OutgoingMessage message(outAsync, outAsync->__getOs(), _batchRequestCompress, false);
        sent = sendMessage(message);
    }
    catch(const Ice::LocalException& ex)
    {
        setState(StateClosed, ex);
        assert(_exception.get());
        _exception->ice_throw();
    }

    //
    // Reset the batch stream.
    //
    BasicStream dummy(_instance.get(), _batchAutoFlush);
    _batchStream.swap(dummy);
    _batchRequestNum = 0;
    _batchRequestCompress = false;
    _batchMarker = 0;
    return sent;
}

void
Ice::ConnectionI::sendResponse(BasicStream* os, Byte compressFlag)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    assert(_state > StateNotValidated);

    try
    {
        if(--_dispatchCount == 0)
        {
            notifyAll();
        }

        if(_state >= StateClosed)
        {
            assert(_exception.get());
            _exception->ice_throw();
        }

        OutgoingMessage message(os, compressFlag > 0);
        sendMessage(message);

        if(_state == StateClosing && _dispatchCount == 0)
        {
            initiateShutdown();
        }

        if(_acmTimeout > 0)
        {
            _acmAbsoluteTimeout = IceUtil::Time::now(IceUtil::Time::Monotonic) +
                IceUtil::Time::seconds(_acmTimeout);
        }
    }
    catch(const LocalException& ex)
    {
        setState(StateClosed, ex);
    }
}

void
Ice::ConnectionI::sendNoResponse()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    assert(_state > StateNotValidated);

    try
    {
        if(--_dispatchCount == 0)
        {
            notifyAll();
        }

        if(_state >= StateClosed)
        {
            assert(_exception.get());
            _exception->ice_throw();
        }

        if(_state == StateClosing && _dispatchCount == 0)
        {
            initiateShutdown();
        }

        if(_acmTimeout > 0)
        {
            _acmAbsoluteTimeout = IceUtil::Time::now(IceUtil::Time::Monotonic) +
                IceUtil::Time::seconds(_acmTimeout);
        }
    }
    catch(const LocalException& ex)
    {
        setState(StateClosed, ex);
    }
}

EndpointIPtr
Ice::ConnectionI::endpoint() const
{
    return _endpoint; // No mutex protection necessary, _endpoint is immutable.
}

void
Ice::ConnectionI::setAdapter(const ObjectAdapterPtr& adapter)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    if(_state <= StateNotValidated || _state >= StateClosing)
    {
        return;
    }

    _adapter = adapter;

    if(_adapter)
    {
        _servantManager = dynamic_cast<ObjectAdapterI*>(_adapter.get())->getServantManager();
        if(!_servantManager)
        {
            _adapter = 0;
        }
    }
    else
    {
        _servantManager = 0;
    }

    //
    // We never change the thread pool with which we were initially
    // registered, even if we add or remove an object adapter.
    //
}

ObjectAdapterPtr
Ice::ConnectionI::getAdapter() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    return _adapter;
}

ObjectPrx
Ice::ConnectionI::createProxy(const Identity& ident) const
{
    //
    // Create a reference and return a reverse proxy for this
    // reference.
    //
    ConnectionIPtr self = const_cast<ConnectionI*>(this);
    return _instance->proxyFactory()->referenceToProxy(_instance->referenceFactory()->create(ident, self));
}

void
Ice::ConnectionI::finished()
{
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        assert(_state == StateClosed);
        unscheduleTimeout(static_cast<SocketOperation>(SocketOperationRead | SocketOperationWrite));
    }

    if(_startCallback)
    {
        _startCallback->connectionStartFailed(this, *_exception.get());
        _startCallback = 0;
    }

    if(!_sendStreams.empty())
    {
        assert(!_writeStream.b.empty());
        _writeStream.swap(*_sendStreams.front().stream);
        for(deque<OutgoingMessage>::iterator o = _sendStreams.begin(); o != _sendStreams.end(); ++o)
        {
            o->finished(*_exception.get());
        }
        _sendStreams.clear(); // Must be cleared before _requests because of Outgoing* references in OutgoingMessage
    }

    for(map<Int, Outgoing*>::iterator p = _requests.begin(); p != _requests.end(); ++p)
    {
        p->second->finished(*_exception.get());
    }
    _requests.clear();

    for(map<Int, OutgoingAsyncPtr>::iterator q = _asyncRequests.begin(); q != _asyncRequests.end(); ++q)
    {
        q->second->__finished(*_exception.get());
    }
    _asyncRequests.clear();

    //
    // This must be done last as this will cause waitUntilFinished() to return (and communicator
    // objects such as the timer might be destroyed too).
    //
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        setState(StateFinished);
    }
}

void
Ice::ConnectionI::exception(const LocalException& ex)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    setState(StateClosed, ex);
}

void
Ice::ConnectionI::invokeException(const LocalException& ex, int invokeNum)
{
    //
    // Fatal exception while invoking a request. Since sendResponse/sendNoResponse isn't
    // called in case of a fatal exception we decrement _dispatchCount here.
    //

    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    setState(StateClosed, ex);

    if(invokeNum > 0)
    {
        assert(_dispatchCount > 0);
        _dispatchCount -= invokeNum;
        assert(_dispatchCount >= 0);
        if(_dispatchCount == 0)
        {
            notifyAll();
        }
    }
}

string
Ice::ConnectionI::type() const
{
    return _type; // No mutex lock, _type is immutable.
}

Ice::Int
Ice::ConnectionI::timeout() const
{
    return _endpoint->timeout(); // No mutex lock, _endpoint is immutable.
}

string
Ice::ConnectionI::toString() const
{
    return _desc; // No mutex lock, _desc is immutable.
}

NativeInfoPtr
Ice::ConnectionI::getNativeInfo()
{
    return _transceiver->getNativeInfo();
}

#ifdef ICE_USE_IOCP
bool
Ice::ConnectionI::startAsync(SocketOperation operation)
{
    if(_state >= StateClosed)
    {
        return false;
    }

    try
    {
        if(operation & SocketOperationWrite)                
        {
            _transceiver->startWrite(_writeStream);
        }
        else if(operation & SocketOperationRead)
        {
            _transceiver->startRead(_readStream);
        }
    }
    catch(const Ice::LocalException& ex)
    {
        setState(StateClosed, ex);
        return false;
    }
    return true;
}

bool
Ice::ConnectionI::finishAsync(SocketOperation operation)
{
    if(_state >= StateClosed)
    {
        return false;
    }

    try
    {
        if(operation & SocketOperationWrite)
        {
            _transceiver->finishWrite(_writeStream);
        }
        else if(operation & SocketOperationRead)
        {
            _transceiver->finishRead(_readStream);
        }
    }
    catch(const Ice::LocalException& ex)
    {
        setState(StateClosed, ex);
        return false;
    }
    return true;
}
#endif

bool
Ice::ConnectionI::message(ThreadPoolCurrent& current)
{
    StartCallbackPtr startCB;
    vector<OutgoingAsyncMessageCallbackPtr> sentCBs;
    BasicStream stream(_instance.get());
    Byte compress = 0;
    Int requestId = 0;
    Int invokeNum = 0;
    ServantManagerPtr servantManager;
    ObjectAdapterPtr adapter;
    OutgoingAsyncPtr outAsync;

    ThreadPoolMessage<ConnectionI> msg(_threadPool.get(), this, current);

    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

        ThreadPoolMessage<ConnectionI>::IOScope io(msg);
        if(!io)
        {
            return false;
        }

        if(_state >= StateClosed)
        {
            return false;
        }

        try
        {
            unscheduleTimeout(current.operation);
            if(current.operation & SocketOperationWrite && !_writeStream.b.empty())
            {
                if(_writeStream.i != _writeStream.b.end() && !_transceiver->write(_writeStream))
                {
                    assert(!_writeStream.b.empty());
                    scheduleTimeout(SocketOperationWrite, _endpoint->timeout());
                    return false;
                }
                assert(_writeStream.i == _writeStream.b.end());
            }
            if(current.operation & SocketOperationRead && !_readStream.b.empty())
            {
                if(static_cast<Int>(_readStream.b.size()) == headerSize) // Read header.
                {
                    if(_readStream.i != _readStream.b.end() && !_transceiver->read(_readStream))
                    {
                        return false;
                    }   
                    assert(_readStream.i == _readStream.b.end());
                
                    ptrdiff_t pos = _readStream.i - _readStream.b.begin();
                    if(pos < headerSize)
                    {
                        //
                        // This situation is possible for small UDP packets.
                        //
                        throw IllegalMessageSizeException(__FILE__, __LINE__);
                    }
                
                    _readStream.i = _readStream.b.begin();
                    const Byte* m;
                    _readStream.readBlob(m, static_cast<Int>(sizeof(magic)));
                    if(m[0] != magic[0] || m[1] != magic[1] || m[2] != magic[2] || m[3] != magic[3])
                    {
                        BadMagicException ex(__FILE__, __LINE__);
                        ex.badMagic = Ice::ByteSeq(&m[0], &m[0] + sizeof(magic));
                        throw ex;
                    }
                    Byte pMajor;
                    Byte pMinor;
                    _readStream.read(pMajor);
                    _readStream.read(pMinor);
                    if(pMajor != protocolMajor
                       || static_cast<unsigned char>(pMinor) > static_cast<unsigned char>(protocolMinor))
                    {
                        UnsupportedProtocolException ex(__FILE__, __LINE__);
                        ex.badMajor = static_cast<unsigned char>(pMajor);
                        ex.badMinor = static_cast<unsigned char>(pMinor);
                        ex.major = static_cast<unsigned char>(protocolMajor);
                        ex.minor = static_cast<unsigned char>(protocolMinor);
                        throw ex;
                    }
                    Byte eMajor;
                    Byte eMinor;
                    _readStream.read(eMajor);
                    _readStream.read(eMinor);
                    if(eMajor != encodingMajor
                       || static_cast<unsigned char>(eMinor) > static_cast<unsigned char>(encodingMinor))
                    {
                        UnsupportedEncodingException ex(__FILE__, __LINE__);
                        ex.badMajor = static_cast<unsigned char>(eMajor);
                        ex.badMinor = static_cast<unsigned char>(eMinor);
                        ex.major = static_cast<unsigned char>(encodingMajor);
                        ex.minor = static_cast<unsigned char>(encodingMinor);
                        throw ex;
                    }
                    Byte messageType;
                    _readStream.read(messageType);
                    Byte compress;
                    _readStream.read(compress);
                    Int size;
                    _readStream.read(size);
                    if(size < headerSize)
                    {
                        throw IllegalMessageSizeException(__FILE__, __LINE__);
                    }
                    if(size > static_cast<Int>(_instance->messageSizeMax()))
                    {
                        throw MemoryLimitException(__FILE__, __LINE__);
                    }
                    if(size > static_cast<Int>(_readStream.b.size()))
                    {
                        _readStream.b.resize(size);
                    }
                    _readStream.i = _readStream.b.begin() + pos;
                }
            
                if(_readStream.i != _readStream.b.end())
                {
                    if(_endpoint->datagram())
                    {
                        if(_instance->initializationData().properties->getPropertyAsInt("Ice.Warn.Datagrams") > 0)
                        {
                            Warning out(_instance->initializationData().logger);
                            out << "DatagramLimitException: maximum size of " << _readStream.i - _readStream.b.begin() 
                                << " exceeded";
                        }
                        throw DatagramLimitException(__FILE__, __LINE__);
                    }
                    else
                    {
                        if(!_transceiver->read(_readStream))
                        {
                            assert(!_readStream.b.empty());
                            scheduleTimeout(SocketOperationRead, _endpoint->timeout());
                            return false;
                        }
                        assert(_readStream.i == _readStream.b.end());
                    }
                }
            }
        
            if(_state <= StateNotValidated)
            {
                if(_state == StateNotInitialized && !initialize(current.operation))
                {
                    return false;
                }

                if(_state <= StateNotValidated && !validate(current.operation))
                {
                    return false;
                }

                _threadPool->unregister(this, current.operation);

                //
                // We start out in holding state.
                //
                setState(StateHolding);
                swap(_startCallback, startCB);
            }
            else
            {
                assert(_state <= StateClosing);

                if(current.operation & SocketOperationWrite)
                {
                    send(sentCBs);
                }

                if(current.operation & SocketOperationRead)
                {
                    parseMessage(stream, invokeNum, requestId, compress, servantManager, adapter, outAsync);
                }
            }
        }
        catch(const DatagramLimitException&) // Expected.
        {
            _readStream.resize(headerSize);
            _readStream.i = _readStream.b.begin();
            return false;
        }
        catch(const SocketException& ex)
        {
            setState(StateClosed, ex);
            return false;
        }
        catch(const LocalException& ex)
        {
            if(_endpoint->datagram())
            {
                if(_instance->initializationData().properties->getPropertyAsInt("Ice.Warn.Connections") > 0)
                {
                    Warning out(_instance->initializationData().logger);
                    out << "datagram connection exception:\n" << ex << '\n' << _desc;
                }
                _readStream.resize(headerSize);
                _readStream.i = _readStream.b.begin();
            }
            else
            {
                setState(StateClosed, ex);
            }
            return false;
        }

        if(_acmTimeout > 0)
        {
            _acmAbsoluteTimeout = IceUtil::Time::now(IceUtil::Time::Monotonic) + IceUtil::Time::seconds(_acmTimeout);
        }

        io.completed();
    }

    //
    // Notify the factory that the connection establishment and
    // validation has completed.
    //
    if(startCB)
    {
        startCB->connectionStartCompleted(this);
    }

    //
    // Notify AMI calls that the message was sent.
    //
    for(vector<OutgoingAsyncMessageCallbackPtr>::const_iterator p = sentCBs.begin(); p != sentCBs.end(); ++p)
    {
        (*p)->__sentCallback(_instance);
    }

    //
    // Asynchronous replies must be handled outside the thread
    // synchronization, so that nested calls are possible.
    //
    if(outAsync)
    {
        outAsync->__finished(stream);
    }

    //
    // Method invocation (or multiple invocations for batch messages)
    // must be done outside the thread synchronization, so that nested
    // calls are possible.
    //
    if(invokeNum)
    {
        invokeAll(stream, invokeNum, requestId, compress, servantManager, adapter);
    }
    return true;
}

void
Ice::ConnectionI::timedOut()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    if(_state <= StateNotValidated)
    {
        setState(StateClosed, ConnectTimeoutException(__FILE__, __LINE__));
    }
    else if(_state <= StateClosing)
    {
        setState(StateClosed, TimeoutException(__FILE__, __LINE__));
    }
}

int
Ice::ConnectionI::connectTimeout()
{
    DefaultsAndOverridesPtr defaultsAndOverrides = _instance->defaultsAndOverrides();
    if(defaultsAndOverrides->overrideConnectTimeout)
    {
        return defaultsAndOverrides->overrideConnectTimeoutValue;
    }
    else
    {
        return _endpoint->timeout();
    }
}

//
// Only used by the SSL plug-in.
//
// The external party has to synchronize the connection, since the
// connection is the object that protects the transceiver.
//
IceInternal::TransceiverPtr
Ice::ConnectionI::getTransceiver() const
{
    return _transceiver;
}

Ice::ConnectionI::ConnectionI(const InstancePtr& instance,
                              const TransceiverPtr& transceiver,
                              const EndpointIPtr& endpoint,
                              const ObjectAdapterPtr& adapter) :
    _transceiver(transceiver),
    _instance(instance),
    _desc(transceiver->toString()),
    _type(transceiver->type()),
    _endpoint(endpoint),
    _adapter(adapter),
    _logger(_instance->initializationData().logger), // Cached for better performance.
    _traceLevels(_instance->traceLevels()), // Cached for better performance.
    _timer(_instance->timer()), // Cached for better performance.
    _writeTimeout(new TimeoutCallback(this)),
    _writeTimeoutScheduled(false),
    _readTimeout(new TimeoutCallback(this)),
    _readTimeoutScheduled(false),
    _warn(_instance->initializationData().properties->getPropertyAsInt("Ice.Warn.Connections") > 0),
    _acmTimeout(0),
    _compressionLevel(1),
    _nextRequestId(1),
    _requestsHint(_requests.end()),
    _asyncRequestsHint(_asyncRequests.end()),
    _batchAutoFlush(
        _instance->initializationData().properties->getPropertyAsIntWithDefault("Ice.BatchAutoFlush", 1) > 0),
    _batchStream(_instance.get(), _batchAutoFlush),
    _batchStreamInUse(false),
    _batchRequestNum(0),
    _batchRequestCompress(false),
    _batchMarker(0),
    _readStream(_instance.get()),
    _writeStream(_instance.get()),
    _dispatchCount(0),
    _state(StateNotInitialized),
    _stateTime(IceUtil::Time::now(IceUtil::Time::Monotonic))
{
    Int& acmTimeout = const_cast<Int&>(_acmTimeout);
    if(_endpoint->datagram())
    {
        acmTimeout = 0;
    }
    else
    {
        if(_adapter)
        {
            acmTimeout = _instance->serverACM();
        }
        else
        {
            acmTimeout = _instance->clientACM();
        }
    }

    int& compressionLevel = const_cast<int&>(_compressionLevel);
    compressionLevel = _instance->initializationData().properties->getPropertyAsIntWithDefault(
        "Ice.Compression.Level", 1);
    if(compressionLevel < 1)
    {
        compressionLevel = 1;
    }
    else if(compressionLevel > 9)
    {
        compressionLevel = 9;
    }

    ObjectAdapterI* adapterImpl = _adapter ? dynamic_cast<ObjectAdapterI*>(_adapter.get()) : 0;
    if(adapterImpl)
    {
        _servantManager = adapterImpl->getServantManager();
    }

    __setNoDelete(true);
    try
    {
        if(adapterImpl)
        {
            const_cast<ThreadPoolPtr&>(_threadPool) = adapterImpl->getThreadPool();
        }
        else
        {
            const_cast<ThreadPoolPtr&>(_threadPool) = _instance->clientThreadPool();
        }
        _threadPool->initialize(this);
    }
    catch(const IceUtil::Exception&)
    {
        __setNoDelete(false);
        throw;
    }
    __setNoDelete(false);
}

Ice::ConnectionI::~ConnectionI()
{
    assert(!_startCallback);
    assert(_state == StateFinished);
    assert(_dispatchCount == 0);
    assert(_requests.empty());
    assert(_asyncRequests.empty());
}

void
Ice::ConnectionI::setState(State state, const LocalException& ex)
{
    //
    // If setState() is called with an exception, then only closed and
    // closing states are permissible.
    //
    assert(state >= StateClosing);

    if(_state == state) // Don't switch twice.
    {
        return;
    }

    if(!_exception.get())
    {
        //
        // If we are in closed state, an exception must be set.
        //
        assert(_state != StateClosed);

        _exception.reset(dynamic_cast<LocalException*>(ex.ice_clone()));

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
                if(!(dynamic_cast<const CloseConnectionException*>(_exception.get()) ||
                     dynamic_cast<const ForcedCloseConnectionException*>(_exception.get()) ||
                     dynamic_cast<const ConnectionTimeoutException*>(_exception.get()) ||
                     dynamic_cast<const CommunicatorDestroyedException*>(_exception.get()) ||
                     dynamic_cast<const ObjectAdapterDeactivatedException*>(_exception.get()) ||
                     (dynamic_cast<const ConnectionLostException*>(_exception.get()) && _state == StateClosing)))
                {
                    Warning out(_logger);
                    out << "connection exception:\n" << *_exception.get() << '\n' << _desc;
                }
            }
        }
    }

    //
    // We must set the new state before we notify requests of any
    // exceptions. Otherwise new requests may retry on a connection
    // that is not yet marked as closed or closing.
    //
    setState(state);
}

void
Ice::ConnectionI::setState(State state)
{
    //
    // We don't want to send close connection messages if the endpoint
    // only supports oneway transmission from client to server.
    //
    if(_endpoint->datagram() && state == StateClosing)
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

    switch(state)
    {
    case StateNotInitialized:
    {
        assert(false);
        break;
    }

    case StateNotValidated:
    {
        if(_state != StateNotInitialized)
        {
            assert(_state == StateClosed);
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
        _threadPool->_register(this, SocketOperationRead);
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
        if(_state == StateActive)
        {
            _threadPool->unregister(this, SocketOperationRead);
        }
        break;
    }

    case StateClosing:
    {
        //
        // Can't change back from closed.
        //
        if(_state >= StateClosed)
        {
            return;
        }
        if(_state == StateHolding)
        {
            _threadPool->_register(this, SocketOperationRead); // We need to continue to read in closing state.
        }
        break;
    }

    case StateClosed:
    {
        if(_state == StateFinished)
        {
            return;
        }
        _threadPool->finish(this);
        break;
    }

    case StateFinished:
    {
        assert(_state == StateClosed);
        try
        {
            _transceiver->close();
        }
        catch(const Ice::LocalException&)
        {
            throw;
        }
        break;
    }
    }

    //
    // We only register with the connection monitor if our new state
    // is StateActive. Otherwise we unregister with the connection
    // monitor, but only if we were registered before, i.e., if our
    // old state was StateActive.
    //
    ConnectionMonitorPtr connectionMonitor = _instance->connectionMonitor();
    if(connectionMonitor)
    {
        if(state == StateActive)
        {
            connectionMonitor->add(this);
        }
        else if(_state == StateActive)
        {
            connectionMonitor->remove(this);
        }
    }

    _state = state;
    _stateTime = IceUtil::Time::now(IceUtil::Time::Monotonic);

    notifyAll();

    if(_state == StateClosing && _dispatchCount == 0)
    {
        try
        {
            initiateShutdown();
        }
        catch(const LocalException& ex)
        {
            setState(StateClosed, ex);
        }
    }
}

void
Ice::ConnectionI::initiateShutdown()
{
    assert(_state == StateClosing);
    assert(_dispatchCount == 0);

    if(!_endpoint->datagram())
    {
        //
        // Before we shut down, we send a close connection message.
        //
        BasicStream os(_instance.get());
        os.write(magic[0]);
        os.write(magic[1]);
        os.write(magic[2]);
        os.write(magic[3]);
        os.write(protocolMajor);
        os.write(protocolMinor);
        os.write(encodingMajor);
        os.write(encodingMinor);
        os.write(closeConnectionMsg);
        os.write((Byte)1); // Compression operation: compression supported but not used.
        os.write(headerSize); // Message size.

        OutgoingMessage message(&os, false);
        sendMessage(message);

        //
        // The CloseConnection message should be sufficient. Closing the write
        // end of the socket is probably an artifact of how things were done
        // in IIOP. In fact, shutting down the write end of the socket causes
        // problems on Windows by preventing the peer from using the socket.
        // For example, the peer is no longer able to continue writing a large
        // message after the socket is shutdown.
        //
        //_transceiver->shutdownWrite();
    }
}

bool
Ice::ConnectionI::initialize(SocketOperation operation)
{
    SocketOperation s = _transceiver->initialize();
    if(s != SocketOperationNone)
    {
        scheduleTimeout(s, connectTimeout());
        _threadPool->update(this, operation, s);
        return false;
    }

    //
    // Update the connection description once the transceiver is initialized.
    //
    const_cast<string&>(_desc) = _transceiver->toString();
    setState(StateNotValidated);
    return true;
}

bool
Ice::ConnectionI::validate(SocketOperation operation)
{
    if(!_endpoint->datagram()) // Datagram connections are always implicitly validated.
    {
        if(_adapter) // The server side has the active role for connection validation.
        {
            if(_writeStream.b.empty())
            {
                _writeStream.write(magic[0]);
                _writeStream.write(magic[1]);
                _writeStream.write(magic[2]);
                _writeStream.write(magic[3]);
                _writeStream.write(protocolMajor);
                _writeStream.write(protocolMinor);
                _writeStream.write(encodingMajor);
                _writeStream.write(encodingMinor);
                _writeStream.write(validateConnectionMsg);
                _writeStream.write(static_cast<Byte>(0)); // Compression status (always zero for validate connection).
                _writeStream.write(headerSize); // Message size.
                _writeStream.i = _writeStream.b.begin();
                traceSend(_writeStream, _logger, _traceLevels);
            }

            if(_writeStream.i != _writeStream.b.end() && !_transceiver->write(_writeStream))
            {
                scheduleTimeout(SocketOperationWrite, connectTimeout());
                _threadPool->update(this, operation, SocketOperationWrite);
                return false;
            }
        }
        else // The client side has the passive role for connection validation.
        {
            if(_readStream.b.empty())
            {
                _readStream.b.resize(headerSize);
                _readStream.i = _readStream.b.begin();
            }

            if(_readStream.i != _readStream.b.end() && !_transceiver->read(_readStream))
            {
                scheduleTimeout(SocketOperationRead, connectTimeout());
                _threadPool->update(this, operation, SocketOperationRead);
                return false;
            }

            assert(_readStream.i == _readStream.b.end());
            _readStream.i = _readStream.b.begin();
            Byte m[4];
            _readStream.read(m[0]);
            _readStream.read(m[1]);
            _readStream.read(m[2]);
            _readStream.read(m[3]);
            if(m[0] != magic[0] || m[1] != magic[1] || m[2] != magic[2] || m[3] != magic[3])
            {
                BadMagicException ex(__FILE__, __LINE__);
                ex.badMagic = Ice::ByteSeq(&m[0], &m[0] + sizeof(magic));
                throw ex;
            }
            Byte pMajor;
            Byte pMinor;
            _readStream.read(pMajor);
            _readStream.read(pMinor);
            if(pMajor != protocolMajor)
            {
                UnsupportedProtocolException ex(__FILE__, __LINE__);
                ex.badMajor = static_cast<unsigned char>(pMajor);
                ex.badMinor = static_cast<unsigned char>(pMinor);
                ex.major = static_cast<unsigned char>(protocolMajor);
                ex.minor = static_cast<unsigned char>(protocolMinor);
                throw ex;
            }
            Byte eMajor;
            Byte eMinor;
            _readStream.read(eMajor);
            _readStream.read(eMinor);
            if(eMajor != encodingMajor)
            {
                UnsupportedEncodingException ex(__FILE__, __LINE__);
                ex.badMajor = static_cast<unsigned char>(eMajor);
                ex.badMinor = static_cast<unsigned char>(eMinor);
                ex.major = static_cast<unsigned char>(encodingMajor);
                ex.minor = static_cast<unsigned char>(encodingMinor);
                throw ex;
            }
            Byte messageType;
            _readStream.read(messageType);
            if(messageType != validateConnectionMsg)
            {
                throw ConnectionNotValidatedException(__FILE__, __LINE__);
            }
            Byte compress;
            _readStream.read(compress); // Ignore compression status for validate connection.
            Int size;
            _readStream.read(size);
            if(size != headerSize)
            {
                throw IllegalMessageSizeException(__FILE__, __LINE__);
            }
            traceRecv(_readStream, _logger, _traceLevels);
        }
    }

    _writeStream.resize(0);
    _writeStream.i = _writeStream.b.begin();

    _readStream.resize(headerSize);
    _readStream.i = _readStream.b.begin();
    return true;
}

void
Ice::ConnectionI::send(vector<OutgoingAsyncMessageCallbackPtr>& callbacks)
{
    assert(!_sendStreams.empty());
    
    assert(!_writeStream.b.empty() && _writeStream.i == _writeStream.b.end());
    _writeStream.swap(*_sendStreams.front().stream);
    
    try
    {
        while(!_sendStreams.empty())
        {
            OutgoingMessage* message = &_sendStreams.front();

            //
            // Prepare the message stream for writing if necessary.
            //
            if(!message->stream->i)
            {
                message->stream->i = message->stream->b.begin();
#if !defined(__APPLE__) || TARGET_OS_IPHONE == 0
                if(message->compress && message->stream->b.size() >= 100) // Only compress messages > 100 bytes.
                {
                    //
                    // Message compressed. Request compressed response, if any.
                    //
                    message->stream->b[9] = 2;

                    //
                    // Do compression.
                    //
                    BasicStream stream(_instance.get());
                    doCompress(*message->stream, stream);

                    if(message->outAsync)
                    {
                        trace("sending asynchronous request", *message->stream, _logger, _traceLevels);
                    }
                    else
                    {
                        traceSend(*message->stream, _logger, _traceLevels);
                    }

                    message->adopt(&stream); // Adopt the compressed stream.
                    message->stream->i = message->stream->b.begin();
                }
                else
                {
                    if(message->compress)
                    {
                        //
                        // Message not compressed. Request compressed response, if any.
                        //
                        message->stream->b[9] = 1;
                    }
#endif

                    //
                    // No compression, just fill in the message size.
                    //
                    Int sz = static_cast<Int>(message->stream->b.size());
                    const Byte* p = reinterpret_cast<const Byte*>(&sz);
#ifdef ICE_BIG_ENDIAN
                    reverse_copy(p, p + sizeof(Int), message->stream->b.begin() + 10);
#else
                    copy(p, p + sizeof(Int), message->stream->b.begin() + 10);
#endif
                    message->stream->i = message->stream->b.begin();

                    if(message->outAsync)
                    {
                        trace("sending asynchronous request", *message->stream, _logger, _traceLevels);
                    }
                    else
                    {
                        traceSend(*message->stream, _logger, _traceLevels);
                    }
                }
#if !defined(__APPLE__) || TARGET_OS_IPHONE == 0
            }
#endif
            //
            // Send the first message.
            //
            assert(message->stream->i);
            if(message->stream->i != message->stream->b.end() && !_transceiver->write(*message->stream))
            {
                _writeStream.swap(*message->stream);
                assert(!_writeStream.b.empty());
                scheduleTimeout(SocketOperationWrite, _endpoint->timeout());
                return;
            }

            //
            // Notify the message that it was sent.
            //
            message->sent(this, true);
            if(dynamic_cast<Ice::AMISentCallback*>(message->outAsync.get()))
            {
                callbacks.push_back(message->outAsync);
            }
            _sendStreams.pop_front();
        }
    }
    catch(const Ice::LocalException& ex)
    {
        setState(StateClosed, ex);
        return;
    }

    _threadPool->unregister(this, SocketOperationWrite);
}

bool
Ice::ConnectionI::sendMessage(OutgoingMessage& message)
{
    assert(_state < StateClosed);

    message.stream->i = 0; // Reset the message stream iterator before starting sending the message.

    if(!_sendStreams.empty())
    {
        _sendStreams.push_back(message);
        _sendStreams.back().adopt(0);
        return false;
    }

    //
    // Attempt to send the message without blocking. If the send blocks, we register
    // the connection with the selector thread.
    //

    message.stream->i = message.stream->b.begin();

#if !defined(__APPLE__) || TARGET_OS_IPHONE == 0
    if(message.compress && message.stream->b.size() >= 100) // Only compress messages larger than 100 bytes.
    {
        //
        // Message compressed. Request compressed response, if any.
        //
        message.stream->b[9] = 2;

        //
        // Do compression.
        //
        BasicStream stream(_instance.get());
        doCompress(*message.stream, stream);
        stream.i = stream.b.begin();

        if(message.outAsync)
        {
            trace("sending asynchronous request", *message.stream, _logger, _traceLevels);
        }
        else
        {
            traceSend(*message.stream, _logger, _traceLevels);
        }

        //
        // Send the message without blocking.
        //
        if(_transceiver->write(stream))
        {
            message.sent(this, false);
            if(_acmTimeout > 0)
            {
                _acmAbsoluteTimeout =
                    IceUtil::Time::now(IceUtil::Time::Monotonic) + IceUtil::Time::seconds(_acmTimeout);
            }
            return true;
        }

        _sendStreams.push_back(message);
        _sendStreams.back().adopt(&stream);
    }
    else
    {
        if(message.compress)
        {
            //
            // Message not compressed. Request compressed response, if any.
            //
            message.stream->b[9] = 1;
        }
#endif

        //
        // No compression, just fill in the message size.
        //
        Int sz = static_cast<Int>(message.stream->b.size());
        const Byte* p = reinterpret_cast<const Byte*>(&sz);
#ifdef ICE_BIG_ENDIAN
        reverse_copy(p, p + sizeof(Int), message.stream->b.begin() + 10);
#else
        copy(p, p + sizeof(Int), message.stream->b.begin() + 10);
#endif
        message.stream->i = message.stream->b.begin();

        if(message.outAsync)
        {
            trace("sending asynchronous request", *message.stream, _logger, _traceLevels);
        }
        else
        {
            traceSend(*message.stream, _logger, _traceLevels);
        }

        //
        // Send the message without blocking.
        //
        if(_transceiver->write(*message.stream))
        {
            message.sent(this, false);
            if(_acmTimeout > 0)
            {
                _acmAbsoluteTimeout =
                    IceUtil::Time::now(IceUtil::Time::Monotonic) + IceUtil::Time::seconds(_acmTimeout);
            }
            return true;
        }

        _sendStreams.push_back(message);
        _sendStreams.back().adopt(0); // Adopt the stream.
#if !defined(__APPLE__) || TARGET_OS_IPHONE == 0
    }
#endif
    _writeStream.swap(*_sendStreams.back().stream);
    scheduleTimeout(SocketOperationWrite, _endpoint->timeout());
    _threadPool->_register(this, SocketOperationWrite);
    return false;
}

#if !defined(__APPLE__) || TARGET_OS_IPHONE == 0
static string
getBZ2Error(int bzError)
{
    if(bzError == BZ_RUN_OK)
    {
        return ": BZ_RUN_OK";
    }
    else if(bzError == BZ_FLUSH_OK)
    {
        return ": BZ_FLUSH_OK";
    }
    else if(bzError == BZ_FINISH_OK)
    {
        return ": BZ_FINISH_OK";
    }
    else if(bzError == BZ_STREAM_END)
    {
        return ": BZ_STREAM_END";
    }
    else if(bzError == BZ_CONFIG_ERROR)
    {
        return ": BZ_CONFIG_ERROR";
    }
    else if(bzError == BZ_SEQUENCE_ERROR)
    {
        return ": BZ_SEQUENCE_ERROR";
    }
    else if(bzError == BZ_PARAM_ERROR)
    {
        return ": BZ_PARAM_ERROR";
    }
    else if(bzError == BZ_MEM_ERROR)
    {
        return ": BZ_MEM_ERROR";
    }
    else if(bzError == BZ_DATA_ERROR)
    {
        return ": BZ_DATA_ERROR";
    }
    else if(bzError == BZ_DATA_ERROR_MAGIC)
    {
        return ": BZ_DATA_ERROR_MAGIC";
    }
    else if(bzError == BZ_IO_ERROR)
    {
        return ": BZ_IO_ERROR";
    }
    else if(bzError == BZ_UNEXPECTED_EOF)
    {
        return ": BZ_UNEXPECTED_EOF";
    }
    else if(bzError == BZ_OUTBUFF_FULL)
    {
        return ": BZ_OUTBUFF_FULL";
    }
    else
    {
        return "";
    }
}

void
Ice::ConnectionI::doCompress(BasicStream& uncompressed, BasicStream& compressed)
{
    const Byte* p;

    //
    // Compress the message body, but not the header.
    //
    unsigned int uncompressedLen = static_cast<unsigned int>(uncompressed.b.size() - headerSize);
    unsigned int compressedLen = static_cast<unsigned int>(uncompressedLen * 1.01 + 600);
    compressed.b.resize(headerSize + sizeof(Int) + compressedLen);
    int bzError = BZ2_bzBuffToBuffCompress(reinterpret_cast<char*>(&compressed.b[0]) + headerSize + sizeof(Int),
                                           &compressedLen,
                                           reinterpret_cast<char*>(&uncompressed.b[0]) + headerSize,
                                           uncompressedLen,
                                           _compressionLevel, 0, 0);
    if(bzError != BZ_OK)
    {
        CompressionException ex(__FILE__, __LINE__);
        ex.reason = "BZ2_bzBuffToBuffCompress failed" + getBZ2Error(bzError);
        throw ex;
    }
    compressed.b.resize(headerSize + sizeof(Int) + compressedLen);

    //
    // Write the size of the compressed stream into the header of the
    // uncompressed stream. Since the header will be copied, this size
    // will also be in the header of the compressed stream.
    //
    Int compressedSize = static_cast<Int>(compressed.b.size());
    p = reinterpret_cast<const Byte*>(&compressedSize);
#ifdef ICE_BIG_ENDIAN
    reverse_copy(p, p + sizeof(Int), uncompressed.b.begin() + 10);
#else
    copy(p, p + sizeof(Int), uncompressed.b.begin() + 10);
#endif

    //
    // Add the size of the uncompressed stream before the message body
    // of the compressed stream.
    //
    Int uncompressedSize = static_cast<Int>(uncompressed.b.size());
    p = reinterpret_cast<const Byte*>(&uncompressedSize);
#ifdef ICE_BIG_ENDIAN
    reverse_copy(p, p + sizeof(Int), compressed.b.begin() + headerSize);
#else
    copy(p, p + sizeof(Int), compressed.b.begin() + headerSize);
#endif

    //
    // Copy the header from the uncompressed stream to the compressed one.
    //
    copy(uncompressed.b.begin(), uncompressed.b.begin() + headerSize, compressed.b.begin());
}

void
Ice::ConnectionI::doUncompress(BasicStream& compressed, BasicStream& uncompressed)
{
    Int uncompressedSize;
    compressed.i = compressed.b.begin() + headerSize;
    compressed.read(uncompressedSize);
    if(uncompressedSize <= headerSize)
    {
        throw IllegalMessageSizeException(__FILE__, __LINE__);
    }

    uncompressed.resize(uncompressedSize);
    unsigned int uncompressedLen = uncompressedSize - headerSize;
    unsigned int compressedLen = static_cast<unsigned int>(compressed.b.size() - headerSize - sizeof(Int));
    int bzError = BZ2_bzBuffToBuffDecompress(reinterpret_cast<char*>(&uncompressed.b[0]) + headerSize,
                                             &uncompressedLen,
                                             reinterpret_cast<char*>(&compressed.b[0]) + headerSize + sizeof(Int),
                                             compressedLen,
                                             0, 0);
    if(bzError != BZ_OK)
    {
        CompressionException ex(__FILE__, __LINE__);
        ex.reason = "BZ2_bzBuffToBuffCompress failed" + getBZ2Error(bzError);
        throw ex;
    }

    copy(compressed.b.begin(), compressed.b.begin() + headerSize, uncompressed.b.begin());
}
#endif

void
Ice::ConnectionI::parseMessage(BasicStream& stream, Int& invokeNum, Int& requestId, Byte& compress,
                               ServantManagerPtr& servantManager, ObjectAdapterPtr& adapter,
                               OutgoingAsyncPtr& outAsync)
{
    assert(_state > StateNotValidated && _state < StateClosed);

    _readStream.swap(stream);
    _readStream.resize(headerSize);
    _readStream.i = _readStream.b.begin();

    assert(stream.i == stream.b.end());

    try
    {
        //
        // We don't need to check magic and version here. This has
        // already been done by the ThreadPool, which provides us
        // with the stream.
        //
        assert(stream.i == stream.b.end());
        stream.i = stream.b.begin() + 8;
        Byte messageType;
        stream.read(messageType);
        stream.read(compress);
        if(compress == 2)
        {
#if !defined(__APPLE__) || TARGET_OS_IPHONE == 0
            BasicStream ustream(_instance.get());
            doUncompress(stream, ustream);
            stream.b.swap(ustream.b);
#else
            FeatureNotSupportedException ex(__FILE__, __LINE__);
            ex.unsupportedFeature = "Cannot uncompress compressed message";
            throw ex;
#endif
        }
        stream.i = stream.b.begin() + headerSize;

        switch(messageType)
        {
            case closeConnectionMsg:
            {
                traceRecv(stream, _logger, _traceLevels);
                if(_endpoint->datagram())
                {
                    if(_warn)
                    {
                        Warning out(_logger);
                        out << "ignoring close connection message for datagram connection:\n" << _desc;
                    }
                }
                else
                {
                    setState(StateClosed, CloseConnectionException(__FILE__, __LINE__));
                }
                break;
            }

            case requestMsg:
            {
                if(_state == StateClosing)
                {
                    trace("received request during closing\n(ignored by server, client will retry)", stream, _logger,
                          _traceLevels);
                }
                else
                {
                    traceRecv(stream, _logger, _traceLevels);
                    stream.read(requestId);
                    invokeNum = 1;
                    servantManager = _servantManager;
                    adapter = _adapter;
                    ++_dispatchCount;
                }
                break;
            }

            case requestBatchMsg:
            {
                if(_state == StateClosing)
                {
                    trace("received batch request during closing\n(ignored by server, client will retry)", stream,
                          _logger, _traceLevels);
                }
                else
                {
                    traceRecv(stream, _logger, _traceLevels);
                    stream.read(invokeNum);
                    if(invokeNum < 0)
                    {
                        invokeNum = 0;
                        throw NegativeSizeException(__FILE__, __LINE__);
                    }
                    servantManager = _servantManager;
                    adapter = _adapter;
                    _dispatchCount += invokeNum;
                }
                break;
            }

            case replyMsg:
            {
                traceRecv(stream, _logger, _traceLevels);

                stream.read(requestId);

                map<Int, Outgoing*>::iterator p = _requests.end();
                map<Int, OutgoingAsyncPtr>::iterator q = _asyncRequests.end();

                if(_requestsHint != _requests.end())
                {
                    if(_requestsHint->first == requestId)
                    {
                        p = _requestsHint;
                    }
                }

                if(p == _requests.end())
                {
                    if(_asyncRequestsHint != _asyncRequests.end())
                    {
                        if(_asyncRequestsHint->first == requestId)
                        {
                            q = _asyncRequestsHint;
                        }
                    }
                }

                if(p == _requests.end() && q == _asyncRequests.end())
                {
                    p = _requests.find(requestId);
                }

                if(p == _requests.end() && q == _asyncRequests.end())
                {
                    q = _asyncRequests.find(requestId);
                }

                if(p == _requests.end() && q == _asyncRequests.end())
                {
                    throw UnknownRequestIdException(__FILE__, __LINE__);
                }

                if(p != _requests.end())
                {
                    p->second->finished(stream);

                    if(p == _requestsHint)
                    {
                        _requests.erase(p++);
                        _requestsHint = p;
                    }
                    else
                    {
                        _requests.erase(p);
                    }
                }
                else
                {
                    assert(q != _asyncRequests.end());

                    outAsync = q->second;

                    if(q == _asyncRequestsHint)
                    {
                        _asyncRequests.erase(q++);
                        _asyncRequestsHint = q;
                    }
                    else
                    {
                        _asyncRequests.erase(q);
                    }
                }
                notifyAll(); // Notify threads blocked in close(false)
                break;
            }

            case validateConnectionMsg:
            {
                traceRecv(stream, _logger, _traceLevels);
                if(_warn)
                {
                    Warning out(_logger);
                    out << "ignoring unexpected validate connection message:\n" << _desc;
                }
                break;
            }

            default:
            {
                trace("received unknown message\n(invalid, closing connection)", stream, _logger, _traceLevels);
                throw UnknownMessageException(__FILE__, __LINE__);
                break;
            }
        }
    }
    catch(const LocalException& ex)
    {
        if(_endpoint->datagram())
        {
            if(_warn)
            {
                Warning out(_logger);
                out << "datagram connection exception:\n" << ex << '\n' << _desc;
            }
        }
        else
        {
            setState(StateClosed, ex);
        }
    }
}

void
Ice::ConnectionI::invokeAll(BasicStream& stream, Int invokeNum, Int requestId, Byte compress,
                            const ServantManagerPtr& servantManager, const ObjectAdapterPtr& adapter)
{
    //
    // Note: In contrast to other private or protected methods, this
    // operation must be called *without* the mutex locked.
    //

    try
    {
        while(invokeNum > 0)
        {
            //
            // Prepare the invocation.
            //
            bool response = !_endpoint->datagram() && requestId != 0;
            Incoming in(_instance.get(), this, adapter, response, compress, requestId);
            BasicStream* is = in.is();
            stream.swap(*is);
            BasicStream* os = in.os();

            //
            // Prepare the response if necessary.
            //
            if(response)
            {
                assert(invokeNum == 1); // No further invocations if a response is expected.
                os->writeBlob(replyHdr, sizeof(replyHdr));

                //
                // Add the request ID.
                //
                os->write(requestId);
            }

            in.invoke(servantManager);

            //
            // If there are more invocations, we need the stream back.
            //
            if(--invokeNum > 0)
            {
                stream.swap(*is);
            }
        }
    }
    catch(const LocalException& ex)
    {
        invokeException(ex, invokeNum);  // Fatal invocation exception
    }
}

