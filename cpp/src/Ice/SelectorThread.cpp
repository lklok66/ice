// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <Ice/SelectorThread.h>
#include <Ice/Network.h>
#include <Ice/Instance.h>
#include <Ice/LoggerUtil.h>

#ifdef ICE_APPLE_CFNETWORK
#if TARGET_OS_IPHONE
#    include <CFNetwork/CFNetwork.h>
#else
#    include <CoreServices/CoreServices.h>
#endif
#include <Ice/ConnectionI.h>
#include <Ice/Transceiver.h>
#endif

using namespace std;
using namespace Ice;
using namespace IceInternal;

#ifdef ICE_APPLE_CFNETWORK

namespace IceInternal
{

class RunLoopThread : public IceUtil::Thread, public IceUtil::Monitor<IceUtil::Mutex>
{
public:
    
    RunLoopThread(const InstancePtr& instance) :
        _instance(instance), _runLoop(0)
    {
    }

    virtual void
    start()
    {
        IceUtil::Thread::start();

        Lock sync(*this);
        while(!_runLoop)
        {
            wait();
        }
    }

    CFRunLoopRef
    getRunLoop()
    {
        return _runLoop;
    }

    virtual void
    run()
    {
        if(_instance->initializationData().threadHook)
        {
            _instance->initializationData().threadHook->start();
        }

        {
            Lock sync(*this);
            _runLoop = CFRunLoopGetCurrent();
            notifyAll();
        }

        CFRunLoopSourceContext ctx;
        memset(&ctx, 0, sizeof(CFRunLoopSourceContext));
        CFRunLoopSourceRef source = CFRunLoopSourceCreate(0, 0, &ctx);
        CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);
        CFRunLoopRun();
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);
        CFRelease(source);

        if(_instance->initializationData().threadHook)
        {
            _instance->initializationData().threadHook->stop();
        }
    }

private:

    const InstancePtr _instance;
    CFRunLoopRef _runLoop;
};

class StreamOpenedCallbackInfo : public IceUtil::Shared
{
public:
    StreamOpenedCallbackInfo(const SelectorThreadPtr& s, const SocketReadyCallbackPtr& c) :
        selectorThread(s), callback(c)
    {
    }
    const SelectorThreadPtr selectorThread;
    const SocketReadyCallbackPtr callback;
};

}

void* streamOpenedCallbackInfoRetain(void* info)
{
    reinterpret_cast<IceInternal::StreamOpenedCallbackInfo*>(info)->__incRef();
    return info;
}

void streamOpenedCallbackInfoRelease(void* info)
{
    reinterpret_cast<IceInternal::StreamOpenedCallbackInfo*>(info)->__decRef();
}

void streamOpenedCallback(CFReadStreamRef stream, CFStreamEventType eventType, void *info)
{
    IceInternal::StreamOpenedCallbackInfo* cbInfo = reinterpret_cast<IceInternal::StreamOpenedCallbackInfo*>(info);
    SocketReadyCallbackPtr cb = cbInfo->callback;
    cbInfo->selectorThread->streamOpened(cb);
}
#endif 

ICE_DECLSPEC_EXPORT IceUtil::Shared* IceInternal::upCast(SelectorThread* p) { return p; }

IceInternal::SelectorThread::SelectorThread(const InstancePtr& instance) :
    _instance(instance),
    _destroyed(false),
    _selector(instance),
    _timer(_instance->timer())
{
    __setNoDelete(true);
    try
    {
#ifdef ICE_APPLE_CFNETWORK
        _runLoopThread = new RunLoopThread(instance);
        _runLoopThread->start();
#endif
        _thread = new HelperThread(this);
        _thread->start();
    }
    catch(const IceUtil::Exception& ex)
    {
        destroy();
        joinWithThread();

        {
            Error out(_instance->initializationData().logger);
            out << "cannot create thread for selector thread:\n" << ex;
        }
        _thread = 0;
        __setNoDelete(false);
        throw;
    }
    catch(...)
    {
        {
            Error out(_instance->initializationData().logger);
            out << "cannot create thread for selector thread";
        }
        _thread = 0;
        __setNoDelete(false);
        throw;
    }
    __setNoDelete(false);
}

IceInternal::SelectorThread::~SelectorThread()
{
    assert(_destroyed);
}

void
IceInternal::SelectorThread::destroy()
{
    Lock sync(*this);
    assert(!_destroyed);
    _destroyed = true;
    if(_thread)
    {
        _selector.setInterrupt();
    }
#ifdef ICE_APPLE_CFNETWORK
    if(_runLoopThread)
    {
        CFRunLoopStop(_runLoopThread->getRunLoop());
    }
#endif
}

void
IceInternal::SelectorThread::incFdsInUse()
{
#ifdef ICE_USE_SELECT
    Lock sync(*this);
    _selector.incFdsInUse();
#endif
}

void
IceInternal::SelectorThread::decFdsInUse()
{
#ifdef ICE_USE_SELECT
    Lock sync(*this);
    _selector.decFdsInUse();
#endif
}

void
IceInternal::SelectorThread::_register(const SocketReadyCallbackPtr& cb, SocketStatus status, int timeout)
{
    Lock sync(*this);
    assert(!_destroyed); // The selector thread is destroyed after the incoming/outgoing connection factories.
    assert(status != Finished);
    assert(cb->_status == Finished);

    cb->_status = status;
    cb->_timeout = timeout;
    if(cb->_timeout >= 0)
    {
        _timer->schedule(cb, IceUtil::Time::milliSeconds(cb->_timeout));
    }

#ifdef ICE_APPLE_CFNETWORK
    //
    // When using CFNetwork transports, NeedConnect indicates that the
    // network stream need to be opened. We then register a callback
    // with the stream to be notified when the stream is opened to
    // notify the socket ready callback.
    //
    if(status == NeedConnect)
    {
        CFReadStreamRef stream = reinterpret_cast<CFReadStreamRef>(cb->stream());
        if(stream)
        {
            CFStreamClientContext ctx = { 0,
                                          new IceInternal::StreamOpenedCallbackInfo(this, cb),
                                          streamOpenedCallbackInfoRetain, 
                                          streamOpenedCallbackInfoRelease, 
                                          0 
            };
            CFOptionFlags events = kCFStreamEventOpenCompleted | kCFStreamEventErrorOccurred;
#ifdef NDEBUG
            CFReadStreamSetClient(stream, events, streamOpenedCallback, &ctx);
#else
            bool rc = CFReadStreamSetClient(stream, events, streamOpenedCallback, &ctx);
            assert(rc);
#endif
            CFReadStreamScheduleWithRunLoop(stream, _runLoopThread->getRunLoop(), kCFRunLoopDefaultMode);
            CFReadStreamOpen(stream);
            return;
        }
    }
#endif
    _selector.add(cb.get(), cb->_status);
}

void
IceInternal::SelectorThread::unregister(const SocketReadyCallbackPtr& cb)
{
    // Note: unregister should only be called from the socketReady() call-back.
    Lock sync(*this);
    assert(!_destroyed); // The selector thread is destroyed after the incoming/outgoing connection factories.
    assert(cb->fd() != INVALID_SOCKET);
    assert(cb->_status != Finished);

#ifdef ICE_APPLE_CFNETWORK
    if(cb->_status != NeedConnect)
    {
        _selector.remove(cb.get(), cb->_status, true);
    }
#else
    _selector.remove(cb.get(), cb->_status, true); // No interrupt needed, it's always called from the selector thread.
#endif
    cb->_status = Finished;
}

void
IceInternal::SelectorThread::finish(const SocketReadyCallbackPtr& cb)
{
    Lock sync(*this);
    assert(!_destroyed); // The selector thread is destroyed after the incoming/outgoing connection factories.
    assert(cb->_status != Finished);

#ifdef ICE_APPLE_CFNETWORK
    if(cb->_status == NeedConnect)
    {
        CFReadStreamRef stream = reinterpret_cast<CFReadStreamRef>(cb->stream());
        if(stream)
        {
            CFReadStreamUnscheduleFromRunLoop(stream, _runLoopThread->getRunLoop(), kCFRunLoopDefaultMode);
            CFReadStreamSetClient(stream, kCFStreamEventNone, 0, 0);
        }
    }
    else
    {
        _selector.remove(cb.get(), cb->_status);
    }
#else
    _selector.remove(cb.get(), cb->_status);
#endif
    cb->_status = Finished;
    _finished.push_back(cb);
    _selector.setInterrupt();
}

void
IceInternal::SelectorThread::joinWithThread()
{
    assert(_destroyed);
    if(_thread)
    {
        _thread->getThreadControl().join();
    }
#ifdef ICE_APPLE_CFNETWORK
    if(_runLoopThread)
    {
        _runLoopThread->getThreadControl().join();
    }
#endif
}

#ifdef ICE_APPLE_CFNETWORK
void
IceInternal::SelectorThread::streamOpened(const SocketReadyCallbackPtr& cb)
{
    {
        Lock sync(*this);
        assert(!_destroyed); // The selector thread is destroyed after the incoming/outgoing connection factories.
        assert(cb->_status == NeedConnect || cb->_status == Finished);
        if(cb->_status == Finished)
        {
            return;
        }
        
        Ice::ConnectionI* connection = dynamic_cast<Ice::ConnectionI*>(cb.get());
        assert(connection);
        
        CFReadStreamRef stream = reinterpret_cast<CFReadStreamRef>(cb->stream());
        assert(stream);
        
        CFReadStreamUnscheduleFromRunLoop(stream, _runLoopThread->getRunLoop(), kCFRunLoopDefaultMode);
        CFReadStreamSetClient(stream, kCFStreamEventNone, 0, 0);
    }

    SocketStatus status = Finished;
    try
    {
        if(cb->_timeout >= 0)
        {
            _timer->cancel(cb);
        }
        status = cb->socketReady();
    }
    catch(const std::exception& ex)
    {
        Error out(_instance->initializationData().logger);
        out << "exception in selector thread while calling socketReady():\n" << ex.what();
        status = Finished;
    }
    catch(...)
    {
        Error out(_instance->initializationData().logger);
        out << "unknown exception in selector thread while calling socketReady()";
        status = Finished;
    }

    if(status != Finished)
    {
        Lock sync(*this);
        if(cb->_status != Finished) // The callback might have been finished concurrently.
        {
            _selector.add(cb.get(), status);
            cb->_status = status;

            if(cb->_timeout >= 0)
            {
                _timer->schedule(cb, IceUtil::Time::milliSeconds(cb->_timeout));
            }
        }
    }
}
#endif

void
IceInternal::SelectorThread::run()
{
    if(_instance->initializationData().threadHook)
    {
        _instance->initializationData().threadHook->start();
    }

    while(true)
    {
        try
        {
            _selector.select();
        }
        catch(const Ice::LocalException& ex)
        {
            Error out(_instance->initializationData().logger);
            out << "exception in selector thread:\n" << ex;
            continue;
        }

        vector<pair<SocketReadyCallbackPtr, SocketStatus> > readyList;
        bool finished = false;
    
        {
            Lock sync(*this);
            if(_selector.isInterrupted())
            {
                if(_selector.processInterrupt())
                {
                    continue;
                }

                //
                // There are two possiblities for an interrupt:
                //
                // 1. The selector thread has been destroyed.
                // 2. A callback is being finished (closed).
                //
                
                //
                // Thread destroyed?
                //
                if(_destroyed)
                {
                    break;
                }
                
                do
                {
                    SocketReadyCallbackPtr cb = _finished.front();
                    _finished.pop_front();
                    readyList.push_back(make_pair(cb, Finished));
                }
                while(_selector.clearInterrupt()); // As long as there are interrupts.
                finished = true;
            }
            else
            {
                //
                // Examine the selection key set.
                //
                SocketReadyCallbackPtr cb;
                while(cb = _selector.getNextSelected())
                {
                    readyList.push_back(make_pair(cb, cb->_status));
                }
            }
        }

        vector<pair<SocketReadyCallbackPtr, SocketStatus> >::const_iterator p;
        for(p = readyList.begin(); p != readyList.end(); ++p)
        {
            const SocketReadyCallbackPtr cb = p->first;
            const SocketStatus previousStatus = p->second;
            SocketStatus status = Finished;
            try
            {
                if(cb->_timeout >= 0)
                {
                    _timer->cancel(cb);
                }

                if(finished)
                {
                    cb->socketFinished();
                }
                else
                {
                    status = cb->socketReady();
                }
            }
            catch(const std::exception& ex)
            {
                Error out(_instance->initializationData().logger);
                out << "exception in selector thread while calling socketReady():\n" << ex.what();
                status = Finished;
            }
            catch(...)
            {
                Error out(_instance->initializationData().logger);
                out << "unknown exception in selector thread while calling socketReady()";
                status = Finished;
            }

            if(status != Finished)
            {
                if(status != previousStatus)
                {
                    Lock sync(*this);
                    if(cb->_status != Finished) // The callback might have been finished concurrently.
                    {
                        _selector.update(cb.get(), cb->_status, status);
                        cb->_status = status;
                    }
                }

                if(cb->_timeout >= 0)
                {
                    _timer->schedule(cb, IceUtil::Time::milliSeconds(cb->_timeout));
                }
            }
        }
    }

    if(_instance->initializationData().threadHook)
    {
        _instance->initializationData().threadHook->stop();
    }

    assert(_destroyed);
}

IceInternal::SelectorThread::HelperThread::HelperThread(const SelectorThreadPtr& selectorThread) :
    _selectorThread(selectorThread)
{
}

void
IceInternal::SelectorThread::HelperThread::run()
{
    try
    {
        _selectorThread->run();
    }
    catch(const std::exception& ex)
    {
        Error out(_selectorThread->_instance->initializationData().logger);
        out << "exception in selector thread:\n" << ex.what();
    }
    catch(...)
    {
        Error out(_selectorThread->_instance->initializationData().logger);
        out << "unknown exception in selector thread";
    }

    _selectorThread = 0; // Break cyclic dependency.
}
