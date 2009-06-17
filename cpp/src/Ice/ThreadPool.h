// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_THREAD_POOL_H
#define ICE_THREAD_POOL_H

#include <IceUtil/Shared.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Thread.h>

#include <Ice/Config.h>
#include <Ice/ThreadPoolF.h>
#include <Ice/InstanceF.h>
#include <Ice/LoggerF.h>
#include <Ice/PropertiesF.h>
#include <Ice/EventHandlerF.h>
#include <Ice/Selector.h>
#include <list>

namespace IceInternal
{

class BasicStream;

#ifdef ICE_USE_IOCP
struct ThreadPoolCurrent
{
    SocketOperation operation;
};    
#else
struct ThreadPoolCurrent
{
    SocketOperation operation;
    bool leader;
};
#endif

class ThreadPoolWorkQueue;
typedef IceUtil::Handle<ThreadPoolWorkQueue> ThreadPoolWorkQueuePtr;

class ThreadPool : public IceUtil::Shared, public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    ThreadPool(const InstancePtr&, const std::string&, int);
    virtual ~ThreadPool();

    void destroy();

    void initialize(const EventHandlerPtr&);
    void _register(const EventHandlerPtr& handler, SocketOperation status)
    {
        update(handler, SocketOperationNone, status);
    }
    void update(const EventHandlerPtr&, SocketOperation, SocketOperation);
    void unregister(const EventHandlerPtr& handler, SocketOperation status)
    {
        update(handler, status, SocketOperationNone);
    }
    void finish(const EventHandlerPtr&);
    void execute(const ThreadPoolWorkItemPtr&);

#ifdef ICE_USE_IOCP
    bool startMessage(EventHandler*, ThreadPoolCurrent&);
    void finishMessage(EventHandler*, ThreadPoolCurrent&);
#endif

    bool ioCompleted(EventHandler*, ThreadPoolCurrent&, bool);
    bool ioCompleted(EventHandler* handler, ThreadPoolCurrent& current)
    {
        return ioCompleted(handler, current, _serialize);
    }

    void incThreadInUse();
    bool decThreadInUse();
    void joinWithAllThreads();

    std::string prefix() const;
    
private:

    void run();
#ifndef ICE_USE_IOCP
    void promoteFollower();
#endif

    InstancePtr _instance;
    ThreadPoolWorkQueuePtr _workQueue;
    bool _destroyed;
    const std::string _prefix;

    Selector _selector;

    std::list<EventHandlerPtr> _finished;

    class EventHandlerThread : public IceUtil::Thread
    {
    public:
        
        EventHandlerThread(const ThreadPoolPtr&);
        virtual void run();

    private:

        ThreadPoolPtr _pool;
    };
    friend class EventHandlerThread;

    const int _size; // Number of threads that are pre-created.
    const int _sizeIO; // Maximum number of threads that can concurrently perform IO.
    const int _sizeMax; // Maximum number of threads.
    const int _sizeWarn; // If _inUse reaches _sizeWarn, a "low on threads" warning will be printed.
    const bool _serialize; // True if requests need to be serialized over the connection.
 
    const size_t _stackSize;

    std::vector<IceUtil::ThreadPtr> _threads; // All threads, running or not.
    int _running; // Number of running threads.
    int _inUse; // Number of threads that are currently in use.
#ifndef ICE_USE_IOCP
    int _inUseIO; // Number of threads that are currently performing IO.
    std::vector<std::pair<EventHandler*, SocketOperation> > _handlers;
    std::vector<std::pair<EventHandler*, SocketOperation> >::const_iterator _nextHandler;
#endif
    double _load; // Current load in number of threads.

    bool _promote;
};

//
// The ThreadPoolMessage class below hides the IOCP implementation details from
//  the event handler implementations. Only event handler implementation that
//  require IO need to use this class.
//
// An instance of the IOScope subclass must be created within the synchronization
// of the event handler. It takes care of calling startMessage/finishMessage for
// the IOCP implementation and ensures that finishMessage isn't called multiple 
// times.
//
#ifndef ICE_USE_IOCP
template<class T> class ThreadPoolMessage
{
public:

    class IOScope
    {
    public:

        IOScope(ThreadPoolMessage<T>& message) : _message(message)
        {
            // Nothing to do.
        }

        ~IOScope()
        {
            // Nothing to do.
        }

        operator bool()
        {
            return true;
        }

        void completed()
        {
            _message._threadPool->ioCompleted(_message._handler, _message._current);
        }

    private:
        
        ThreadPoolMessage<T>& _message;
    };
    friend class IOScope;

    ThreadPoolMessage(ThreadPool* threadPool, T* handler, ThreadPoolCurrent& current) :
        _threadPool(threadPool), _handler(handler), _current(current)
    {
        // Nothing to do.
    }

    ~ThreadPoolMessage()
    {
        // Nothing to do.
    }

private:
    ThreadPool* _threadPool;
    T* _handler;
    ThreadPoolCurrent& _current;
};

#else 

template<class T> class ThreadPoolMessage
{
public:

    class IOScope
    {
    public:

        IOScope(ThreadPoolMessage& message) : _message(message)
        {
            // This must be called with the handler locked. 
            _finish = _message._threadPool->startMessage(_message._handler, _message._current);
        }

        ~IOScope()
        {
            if(_finish)
            {
                // This must be called with the handler locked. 
                _message._threadPool->finishMessage(_message._handler, _message._current);
            }
        }

        operator bool()
        {
            return _finish;
        }

        void
        completed()
        {
            //
            // Call finishMessage once IO is completed only if serialization is not enabled.
            // Otherwise, finishMessage will be called when the event handler is done with 
            // the message (it will be called from ~ThreadPoolMessage below).
            //
            assert(_finish);
            if(_message._threadPool->ioCompleted(_message._handler, _message._current))
            {
                _finish = false;
                _message._finish = true;
            }
        }

    private:

        ThreadPoolMessage& _message;
        bool _finish;        
    };
    friend class IOScope;
    
    ThreadPoolMessage(ThreadPool* threadPool, T* handler, ThreadPoolCurrent& current) : 
        _threadPool(threadPool), _handler(handler), _current(current), _finish(false)
    {
    }
            
    ~ThreadPoolMessage()
    {
        if(_finish)
        {
            //
            // A ThreadPoolMessage instance must be created outside the synchronization
            // of the event handler. We need to lock the event handler here to call 
            // finishMessage.
            //
            IceUtil::LockT<typename T> sync(*_handler);
            _threadPool->finishMessage(_handler, _current);
        }
    }

private:
    
    ThreadPool* _threadPool;
    T* _handler;
    ThreadPoolCurrent& _current;
    bool _finish;
};
#endif

};


#endif
