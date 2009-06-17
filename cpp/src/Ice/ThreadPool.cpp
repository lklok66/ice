// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// The following is required for GetThreadIOPendingFlag
//
#if defined(_WIN32) && !defined(_WIN32_WINNT)
#  define _WIN32_WINNT 0x0501
#endif

#include <IceUtil/DisableWarnings.h>
#include <Ice/ThreadPool.h>
#include <Ice/EventHandler.h>
#include <Ice/Network.h>
#include <Ice/LocalException.h>
#include <Ice/Instance.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Functional.h>
#include <Ice/Protocol.h>
#include <Ice/ObjectAdapterFactory.h>
#include <Ice/Properties.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

ICE_DECLSPEC_EXPORT IceUtil::Shared* IceInternal::upCast(ThreadPool* p) { return p; }

namespace
{

class ShutdownWorkItem : public ThreadPoolWorkItem
{
public:

    ShutdownWorkItem(const InstancePtr& instance) : _instance(instance)
    {
    }
    
    virtual void
    execute()
    {
        ObjectAdapterFactoryPtr factory;
        try
        {
            factory = _instance->objectAdapterFactory();
        }
        catch(const CommunicatorDestroyedException&)
        {
            return;
        }
        
        factory->shutdown();
    }
    
private:

    InstancePtr _instance;
};

class FinishedWorkItem : public ThreadPoolWorkItem
{
public:

    FinishedWorkItem(const EventHandlerPtr& handler) : _handler(handler)
    {
    }
    
    virtual void
    execute()
    {
        _handler->finished();
    }
    
private:

    EventHandlerPtr _handler;
};

//
// Exception raised by the thread pool work queue when the thread pool
// is destroyed.
//
class ThreadPoolDestroyedException
{
};

}

namespace IceInternal
{

class ThreadPoolWorkQueue : public EventHandler, public IceUtil::Mutex
{
public:

    ThreadPoolWorkQueue(ThreadPool* threadPool, const InstancePtr& instance, Selector& selector) : 
        _threadPool(threadPool), 
        _instance(instance),
        _selector(selector),
        _destroyed(false)
#ifdef ICE_USE_IOCP
        , _info(SocketOperationRead)
#endif
    {
#ifndef ICE_USE_IOCP
        SOCKET fds[2];
        createPipe(fds);
        _fdIntrRead = fds[0];
        _fdIntrWrite = fds[1];

        _selector.initialize(this);
        _selector.update(this, SocketOperationNone, SocketOperationRead);
#endif
    }

    ~ThreadPoolWorkQueue()
    {
#ifndef ICE_USE_IOCP
        try
        {
            closeSocket(_fdIntrRead);
        }
        catch(const LocalException& ex)
        {
            Error out(_instance->initializationData().logger);
            out << "exception in selector while calling closeSocket():\n" << ex;
        }
        
        try
        {
            closeSocket(_fdIntrWrite);
        }
        catch(const LocalException& ex)
        {
            Error out(_instance->initializationData().logger);
            out << "exception in selector while calling closeSocket():\n" << ex;
        }
#endif
    }

    void destroy()
    {
        Lock sync(*this);
        assert(!_destroyed);
        _destroyed = true;
        postMessage();
    }

    void queue(const ThreadPoolWorkItemPtr& item)
    {
        Lock sync(*this);
        if(_destroyed)
        {
            throw Ice::CommunicatorDestroyedException(__FILE__, __LINE__);
        }
        _workItems.push_back(item);
        postMessage();
    }

#ifdef ICE_USE_IOCP
    bool startAsync(IceInternal::SocketOperation)
    {
        assert(false);
        return false;
    }

    bool finishAsync(IceInternal::SocketOperation)
    {
        assert(false);
        return false;
    }
#endif

    virtual bool message(ThreadPoolCurrent& current)
    {
#ifndef ICE_USE_IOCP
        char c;
        while(true)
        {
            ssize_t ret;
#ifdef _WIN32
            ret = ::recv(_fdIntrRead, &c, 1, 0);
#else
            ret = ::read(_fdIntrRead, &c, 1);
#endif
            if(ret == SOCKET_ERROR)
            {
                if(interrupted())
                {
                    continue;
                }
                
                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            break;
        }
#endif

        ThreadPoolWorkItemPtr workItem;
        {        
            Lock sync(*this);
            if(!_workItems.empty())
            {
                workItem = _workItems.front();
                _workItems.pop_front();
            }
            else
            {
                assert(_destroyed);
                postMessage();
            }
        }

        _threadPool->ioCompleted(this, current, false);
        if(workItem)
        {
            workItem->execute();
        }
        else
        {
            throw ThreadPoolDestroyedException();
        }
        return true;
    }

    virtual void finished()
    {
        assert(false);
    }

    virtual std::string toString() const
    {
        return "work queue";
    }

    virtual NativeInfoPtr getNativeInfo()
    {
#ifndef ICE_USE_IOCP
        return new NativeInfo(_fdIntrRead);
#endif
        return 0;
    }

    virtual void postMessage()
    {
#ifndef ICE_USE_IOCP
        char c = 0;
        while(true)
        {
#ifdef _WIN32
            if(::send(_fdIntrWrite, &c, 1, 0) == SOCKET_ERROR)
#else
            if(::write(_fdIntrWrite, &c, 1) == SOCKET_ERROR)
#endif
            {
                if(interrupted())
                {
                    continue;
                }
                
                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            break;
        }
#else
        if(!PostQueuedCompletionStatus(_selector.getIOCPHandle(), 0, reinterpret_cast<ULONG_PTR>(this), &_info))
        {
            SocketException ex(__FILE__, __LINE__);
            ex.error = GetLastError();
            throw ex;
        }
#endif
    }

private:

    ThreadPool* _threadPool;
    InstancePtr _instance;
    Selector& _selector;
    bool _destroyed;
#ifdef ICE_USE_IOCP
    AsyncInfo _info;
#else
    SOCKET _fdIntrRead;
    SOCKET _fdIntrWrite;
#endif
    std::list<ThreadPoolWorkItemPtr> _workItems;
};

}

IceInternal::ThreadPool::ThreadPool(const InstancePtr& instance, const string& prefix, int timeout) :
    _instance(instance),
    _destroyed(false),
    _prefix(prefix),
    _selector(instance, timeout),
    _size(0),
    _sizeIO(0),
    _sizeMax(0),
    _sizeWarn(0),
    _serialize(_instance->initializationData().properties->getPropertyAsInt(_prefix + ".Serialize") > 0),
    _stackSize(0),
    _running(0),
    _inUse(0),
#ifndef ICE_USE_IOCP
    _inUseIO(0),
    _nextHandler(_handlers.end()),
#endif
    _load(1.0),
    _promote(true)
{
    //
    // We use just one thread as the default. This is the fastest
    // possible setting, still allows one level of nesting, and
    // doesn't require to make the servants thread safe.
    //
    int size = _instance->initializationData().properties->getPropertyAsIntWithDefault(_prefix + ".Size", 1);
    if(size < 1)
    {
        Warning out(_instance->initializationData().logger);
        out << _prefix << ".Size < 1; Size adjusted to 1";
        size = 1;
    }
    
    int sizeMax = 
        _instance->initializationData().properties->getPropertyAsIntWithDefault(_prefix + ".SizeMax", size);
    if(sizeMax < size)
    {
        Warning out(_instance->initializationData().logger);
        out << _prefix << ".SizeMax < " << _prefix << ".Size; SizeMax adjusted to Size (" << size << ")";
        sizeMax = size;
    }           
    
    int sizeWarn = _instance->initializationData().properties->
                        getPropertyAsIntWithDefault(_prefix + ".SizeWarn", sizeMax * 80 / 100);
    if(sizeWarn > sizeMax)
    {
        Warning out(_instance->initializationData().logger);
        out << _prefix << ".SizeWarn > " << _prefix << ".SizeMax; adjusted SizeWarn to SizeMax (" << sizeMax << ")";
        sizeWarn = sizeMax;
    }

    int sizeIO = _instance->initializationData().properties->getPropertyAsIntWithDefault(_prefix + ".SizeIO", size);
    if(sizeIO < 1)
    {
        Warning out(_instance->initializationData().logger);
        out << _prefix << ".SizeIO < 1; Size adjusted to 1";
        sizeIO = 1;
    }    

    const_cast<int&>(_size) = size;
    const_cast<int&>(_sizeMax) = sizeMax;
    const_cast<int&>(_sizeWarn) = sizeWarn;
    const_cast<int&>(_sizeIO) = sizeIO;

#ifdef ICE_USE_IOCP
    _selector.setup(_sizeIO);
#endif

    int stackSize = _instance->initializationData().properties->getPropertyAsInt(_prefix + ".StackSize");
    if(stackSize < 0)
    {
        Warning out(_instance->initializationData().logger);
        out << _prefix << ".StackSize < 0; Size adjusted to OS default";
        stackSize = 0;
    }
    const_cast<size_t&>(_stackSize) = static_cast<size_t>(stackSize);

    _workQueue = new ThreadPoolWorkQueue(this, _instance, _selector);

    __setNoDelete(true);
    try
    {
        for(int i = 0 ; i < _size ; ++i)
        {
            IceUtil::ThreadPtr thread = new EventHandlerThread(this);
            thread->start(_stackSize);
            _threads.push_back(thread);
            ++_running;
        }
    }
    catch(const IceUtil::Exception& ex)
    {
        {
            Error out(_instance->initializationData().logger);
            out << "cannot create thread for `" << _prefix << "':\n" << ex;
        }

        destroy();
        joinWithAllThreads();
        __setNoDelete(false);
        throw;
    }
    catch(...)
    {
        __setNoDelete(false);
        throw;
    }
    __setNoDelete(false);
}

IceInternal::ThreadPool::~ThreadPool()
{
    assert(_destroyed);
}

void
IceInternal::ThreadPool::destroy()
{
    {
        Lock sync(*this);
        assert(!_destroyed);
        _destroyed = true;
    }
    _workQueue->destroy();
}

void
IceInternal::ThreadPool::initialize(const EventHandlerPtr& handler)
{
    Lock sync(*this);
    _selector.initialize(handler.get());
}

void
IceInternal::ThreadPool::update(const EventHandlerPtr& handler, SocketOperation remove, SocketOperation add)
{
    Lock sync(*this);
    _selector.update(handler.get(), remove, add);
}

void
IceInternal::ThreadPool::finish(const EventHandlerPtr& handler)
{
    Lock sync(*this);
    _selector.finish(handler.get());
    _workQueue->queue(new FinishedWorkItem(handler));

#ifndef ICE_USE_CFSTREAM
    //
    // Clear the current ready handlers. The handlers from this vector can't be 
    // reference counted and a handler might get destroyed once it's finished.
    //
    _handlers.clear();
    _nextHandler = _handlers.end();
#endif
}

void
IceInternal::ThreadPool::execute(const ThreadPoolWorkItemPtr& workItem)
{
    _workQueue->queue(workItem);
}

#ifdef ICE_USE_IOCP
bool
IceInternal::ThreadPool::startMessage(EventHandler* handler, ThreadPoolCurrent& current)
{
    if(handler->_pending & current.operation)
    {
        assert(!(handler->_ready & current.operation));
        handler->_ready = static_cast<SocketOperation>(handler->_ready | current.operation);

        handler->__decRef();
        handler->_pending = static_cast<SocketOperation>(handler->_pending & ~current.operation);
        if(!handler->finishAsync(current.operation))
        {
            return false;
        }
    }
    else if(!(handler->_ready & current.operation))
    {
        assert(!(handler->_pending & current.operation));
        if(handler->startAsync(current.operation))
        {
            handler->_pending = static_cast<SocketOperation>(handler->_pending | current.operation);
            handler->__incRef();
        }
        return false;
    }

    assert(handler->_ready & current.operation);

    if(handler->_registered & current.operation)
    {
        handler->_ready = static_cast<SocketOperation>(handler->_ready & ~current.operation);
        handler->_pending = static_cast<SocketOperation>(handler->_pending | current.operation);
        return true;
    }
    else
    {
        handler->_pending = static_cast<SocketOperation>(handler->_pending & ~current.operation);
        return false;
    }
}

void
IceInternal::ThreadPool::finishMessage(EventHandler* handler, ThreadPoolCurrent& current)
{
    if(handler->_registered & current.operation)
    {
        assert(!(handler->_ready & current.operation));
        if(handler->startAsync(current.operation))
        {
            assert(handler->_pending & current.operation);
            handler->__incRef();
        }
    }
    else
    {
        handler->_pending = static_cast<SocketOperation>(handler->_pending & ~current.operation);
    }
}
#endif

void
IceInternal::ThreadPool::joinWithAllThreads()
{
    assert(_destroyed);

    //
    // _threads is immutable after destroy() has been called,
    // therefore no synchronization is needed. (Synchronization
    // wouldn't be possible here anyway, because otherwise the other
    // threads would never terminate.)
    //
    for(vector<IceUtil::ThreadPtr>::iterator p = _threads.begin(); p != _threads.end(); ++p)
    {
        (*p)->getThreadControl().join();
    }

    _selector.finish(_workQueue.get());
    _selector.destroy();
}

string
IceInternal::ThreadPool::prefix() const
{
    return _prefix;
}

void
IceInternal::ThreadPool::run()
{
#ifndef ICE_USE_IOCP
    EventHandlerPtr handler;
    ThreadPoolCurrent current;
    current.operation = SocketOperationNone;
    current.leader = false;
    bool select = false;
    vector<pair<EventHandler*, SocketOperation> > handlers;
    while(true)
    {
        bool ioCompleted;
        try
        {
            if(handler)
            {
                ioCompleted = handler->message(current);
            }
            else if(select)
            {
                _selector.select(handlers);
            }
        }
        catch(ThreadPoolDestroyedException&)
        {
            return;
        }
        catch(const LocalException& ex)
        {
            Error out(_instance->initializationData().logger);
            out << "exception in `" << _prefix << "' while calling on event handler:\n"
                << ex << '\n' << handler->toString();
        }

        {
            Lock sync(*this);
            if(!handler)
            {
                if(select)
                {
                    if(handlers.empty())
                    {
                        _workQueue->queue(new ShutdownWorkItem(_instance)); // Select timed-out.
                    }
                    _handlers.swap(handlers);
                    _nextHandler = _handlers.begin();
                    _selector.finishSelect();
                    select = false;
                }
                else
                {
                    //
                    // Wait to be promotted and for all the IO threads to be done.
                    // 
                    assert(!current.leader);
                    while(!_promote || _inUseIO == _sizeIO || _nextHandler == _handlers.end() && _inUseIO > 0)
                    {
                        wait();
                    }
                    _promote = false;
                    current.leader = true;
                }
            }
            else if(_sizeMax > 1)
            {
                if(!ioCompleted)
                {
                    //
                    // The handler didn't call ioCompleted so we take care of decreasing 
                    // the IO thread count now.
                    //
                    --_inUseIO;
                }
                else
                {
                    //
                    // If the handler called ioCompleted(), we re-enable the handler in
                    // case it was disabled and we decrease the number of thread in use.
                    //
                    _selector.enable(handler.get(), current.operation);
                    if(!decThreadInUse())
                    {
                        return;
                    }
                }

                handler = 0; // It's important to clear the handler before waiting to make sure that
                             // resources for the handler are released now if it's finished.
                
                if(!current.leader)
                {
                    //
                    // Wait to be promotted and for all the IO threads to be done.
                    //
                    while(!_promote || _inUseIO == _sizeIO ||  _nextHandler == _handlers.end() && _inUseIO > 0)
                    {
                        wait();
                    }
                    _promote = false;
                    current.leader = true;
                }
            }
            else
            {
                handler = 0;
            }

            //
            // Get the next ready handler.
            //
            if(_nextHandler != _handlers.end())
            {
                handler = _nextHandler->first;
                current.operation = _nextHandler->second;
                ++_nextHandler;
            }

            if(!handler)
            {
                //
                // If there's no more ready handlers and there's still threads busy performing 
                // IO, we give up leader ship and promote another follower (which will perform
                // the select() only once all the IOs are completed). Otherwise, if there's no
                // more threads peforming IOs, it's time to do another select().
                //
                if(_inUseIO > 0)
                {
                    promoteFollower();
                    current.leader = false;
                }
                else
                {
                    _selector.startSelect();
                    select = true;
                }
            }
            else if(_sizeMax > 1)
            {
                //
                // Increment the IO thread count and if there's still threads available
                // to perform IO and more handlers ready, we promote a follower.
                //
                ++_inUseIO;
                if(_nextHandler != _handlers.end() && _inUseIO < _sizeIO)
                {
                    promoteFollower();
                    current.leader = false;
                }
            }
        }
    }
#else
    while(true)
    {
        EventHandlerPtr handler;
        ThreadPoolCurrent current;
        try
        {
            handler = _selector.getNextHandler(current.operation);
        }
        catch(const LocalException& ex)
        {
            Error out(_instance->initializationData().logger);
            out << "exception in `" << _prefix << "':\n" << ex; 
            continue;
        }

        if(!handler) // We initiate a shutdown if there is a selector timeout.
        {
            _workQueue->queue(new ShutdownWorkItem(_instance));
        }
        else
        {
            try
            {
                bool ioCompleted = handler->message(current);
                if(_sizeMax > 1 && ioCompleted)
                {
                    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
                    if(!decThreadInUse())
                    {
                        return;
                    }
                }
            }
            catch(ThreadPoolDestroyedException&)
            {
                return;
            }
            catch(const LocalException& ex)
            {
                Error out(_instance->initializationData().logger);
                out << "exception in `" << _prefix << "' while calling on event handler:\n"
                    << ex << '\n' << handler->toString();
            }
        }
    }
#endif
}

bool
IceInternal::ThreadPool::decThreadInUse()
{
    if(_size < _sizeMax) // Dynamic thread pool
    {
        //
        // First we reap threads that have been destroyed before.
        //
        int sz = static_cast<int>(_threads.size());
        assert(_running <= sz);
        if(_running < sz)
        {
            vector<IceUtil::ThreadPtr>::iterator start =
                partition(_threads.begin(), _threads.end(), 
                          IceUtil::constMemFun(&IceUtil::Thread::isAlive));

            for(vector<IceUtil::ThreadPtr>::iterator p = start; p != _threads.end(); ++p)
            {
                (*p)->getThreadControl().join();
            }

            _threads.erase(start, _threads.end());
        }
                
        //
        // Now we check if this thread can be destroyed, based
        // on a load factor.
        //

        //
        // The load factor jumps immediately to the number of
        // threads that are currently in use, but decays
        // exponentially if the number of threads in use is
        // smaller than the load factor. This reflects that we
        // create threads immediately when they are needed,
        // but want the number of threads to slowly decline to
        // the configured minimum.
        //
        double inUse = static_cast<double>(_inUse);
        if(_load < inUse)
        {
            _load = inUse;
        }
        else
        {
            const double loadFactor = 0.05; // TODO: Configurable?
            const double oneMinusLoadFactor = 1 - loadFactor;
            _load = _load * oneMinusLoadFactor + inUse * loadFactor;
        }
                
        if(_running > _size)
        {
#ifdef ICE_USE_IOCP
            BOOL hasIO = false;
            GetThreadIOPendingFlag(GetCurrentThread(), &hasIO);
#endif

            int load = static_cast<int>(_load + 0.5);

            //
            // We add one to the load factor because on
            // additional thread is needed for select().
            //
#ifdef ICE_USE_IOCP
            if(!hasIO && load + 1 < _running)
#else
            if(load + 1 < _running)
#endif
            {
                assert(_inUse > 0);
                --_inUse;
                        
                assert(_running > 0);
                --_running;
                notify();
                return false;
            }
        }
    }

    assert(_inUse > 0);
    --_inUse;
    return true;
}

void
IceInternal::ThreadPool::incThreadInUse()
{
    if(!_destroyed)
    {
        assert(_inUse >= 0);
        ++_inUse;
    
        if(_inUse == _sizeWarn)
        {
            Warning out(_instance->initializationData().logger);
            out << "thread pool `" << _prefix << "' is running low on threads\n"
                << "Size=" << _size << ", " << "SizeMax=" << _sizeMax << ", " << "SizeWarn=" << _sizeWarn;
        }
    
        assert(_inUse <= _running);
        if(_inUse < _sizeMax && _inUse == _running)
        {
            try
            {
                IceUtil::ThreadPtr thread = new EventHandlerThread(this);
                thread->start(_stackSize);
                _threads.push_back(thread);
                ++_running;
            }
            catch(const IceUtil::Exception& ex)
            {
                Error out(_instance->initializationData().logger);
                out << "cannot create thread for `" << _prefix << "':\n" << ex;
            }
        }
    }
}

bool
IceInternal::ThreadPool::ioCompleted(EventHandler* handler, ThreadPoolCurrent& current, bool serialize)
{
    if(_sizeMax > 1)
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
#ifndef ICE_USE_IOCP
        --_inUseIO;

        if(serialize)
        {
            _selector.disable(handler, current.operation);
        }    

        if(current.leader)
        {
            //
            // If this thread is still the leader, it's time to promote a new leader.
            //
            promoteFollower();
            current.leader = false;
        }
        else if(_promote && (_nextHandler != _handlers.end() || _inUseIO == 0))
        {
            notify();
        }
#endif
        incThreadInUse();
    }
    return _serialize;
}

#ifndef ICE_USE_IOCP
void
IceInternal::ThreadPool::promoteFollower()
{
    assert(!_promote);
    _promote = true;
    if(_inUseIO < _sizeIO && (_nextHandler != _handlers.end() || _inUseIO == 0))
    {
        notify();
    }
}
#endif

IceInternal::ThreadPool::EventHandlerThread::EventHandlerThread(const ThreadPoolPtr& pool) :
    _pool(pool)
{
}

void
IceInternal::ThreadPool::EventHandlerThread::run()
{
    if(_pool->_instance->initializationData().threadHook)
    {
        _pool->_instance->initializationData().threadHook->start();
    }

    try
    {
        _pool->run();
    }
    catch(const std::exception& ex)
    {
        Error out(_pool->_instance->initializationData().logger);
        out << "exception in `" << _pool->_prefix << "':\n" << ex.what();
    }
    catch(...)
    {
        Error out(_pool->_instance->initializationData().logger);
        out << "unknown exception in `" << _pool->_prefix << "'"; 
    }

    if(_pool->_instance->initializationData().threadHook)
    {
        _pool->_instance->initializationData().threadHook->stop();
    }

    _pool = 0; // Break cyclic dependency.
}
