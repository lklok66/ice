// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_SELECTOR_H
#define ICE_SELECTOR_H

#include <IceUtil/StringUtil.h>

#include <Ice/Config.h>
#include <Ice/Network.h>
#include <Ice/SelectorF.h>
#include <Ice/InstanceF.h>
#include <Ice/EventHandlerF.h>
#include <Ice/LoggerUtil.h>
#include <Ice/LocalException.h>
#include <Ice/Instance.h>

#if defined(ICE_USE_EPOLL)
#   include <sys/epoll.h>
#elif defined(ICE_USE_KQUEUE)
#   include <sys/event.h>
#elif defined(ICE_USE_IOCP)
// Nothing to include
#elif !defined(ICE_USE_SELECT)
#   include <sys/poll.h>
#endif

#ifdef ICE_USE_CFSTREAM
struct __CFRunLoop;
typedef struct __CFRunLoop * CFRunLoopRef;

struct __CFRunLoopSource;
typedef struct __CFRunLoopSource * CFRunLoopSourceRef;

struct __CFSocket;
typedef struct __CFSocket * CFSocketRef;
#endif

namespace IceInternal
{

#ifdef ICE_USE_IOCP

class Selector
{
public:

    Selector(const InstancePtr&, int = 0);
    ~Selector();

    void destroy() { }

    void setup(int);

    void initialize(EventHandler*);
    void update(EventHandler*, SocketOperation, SocketOperation);
    void finish(EventHandler*);

    EventHandler* getNextHandler(SocketOperation&);

    HANDLE getIOCPHandle() { return _handle; } 
    
private:

    InstancePtr _instance;
    int _timeout;
    HANDLE _handle;
};

#elif defined(ICE_USE_KQUEUE) || defined(ICE_USE_EPOLL)

class Selector
{
public:

    Selector(const InstancePtr&, int = 0);
    ~Selector();

    void destroy() { }

    void initialize(EventHandler*)
    {
        // Nothing to do
    }
    void update(EventHandler*, SocketOperation, SocketOperation);
    void enable(EventHandler*, SocketOperation);
    void disable(EventHandler*, SocketOperation);
    void finish(EventHandler*);

#if defined(ICE_USE_KQUEUE)
    void updateSelector();
#endif    

    void
    startSelect()
    {
        _selecting = true;
#ifdef ICE_USE_KQUEUE
        if(!_changes.empty())
        {
            updateSelector();
        }
#endif
    }

    void
    finishSelect()
    {
        _selecting = false;
    }

    void select(std::vector<std::pair<EventHandler*, SocketOperation> >&);

private:

    InstancePtr _instance;
    int _timeout;
#if defined(ICE_USE_EPOLL)
    std::vector<struct epoll_event> _events;
#else
    std::vector<struct kevent> _events;
    std::vector<struct kevent> _changes;
    bool _selecting;
#endif
    int _queueFd;
};

#elif defined(ICE_USE_CFSTREAM)

class EventHandlerWrapper : public IceUtil::Shared
{
public:

    EventHandlerWrapper(const EventHandlerPtr&, Selector&);
    ~EventHandlerWrapper();

    void updateRunLoop();

    void ready(SocketOperation, int);
    void checkReady();
    SocketOperation readyOp();

    bool update(SocketOperation, SocketOperation);
    bool finish();

    Selector& selector() { return _selector; }

private:

    friend class Selector;

    EventHandlerPtr _handler;
    Selector& _selector;
    SocketOperation _ready;
    CFSocketRef _socket;
    CFRunLoopSourceRef _source;
};
typedef IceUtil::Handle<EventHandlerWrapper> EventHandlerWrapperPtr;

class Selector : IceUtil::Monitor<IceUtil::Mutex>
{

public:

    Selector(const InstancePtr&, int);
    virtual ~Selector();

    void destroy();

    void initialize(EventHandler*);
    void update(EventHandler*, SocketOperation, SocketOperation);
    void enable(EventHandler*, SocketOperation);
    void disable(EventHandler*, SocketOperation);
    void finish(EventHandler*);

    void startSelect() { }
    void finishSelect() { }
    void select(std::vector<std::pair<EventHandler*, SocketOperation> >&); 

    void processInterrupt();
    void ready(EventHandlerWrapper*, SocketOperation, int = 0);
    void addReadyHandler(EventHandlerWrapper*);
    void run();

    CFRunLoopRef runLoop() { return _runLoop; }

private:

    struct EventHandlerCI
    {
        bool operator()(const EventHandlerPtr& lhs, const EventHandlerPtr& rhs)
        {
            return lhs.get() < rhs.get();
        }
    };

    InstancePtr _instance;
    IceUtil::ThreadPtr _thread;
    int _timeout;
    CFRunLoopRef _runLoop;
    CFRunLoopSourceRef _source;

    std::vector<EventHandlerWrapperPtr> _changes;
    std::vector<EventHandlerWrapperPtr> _readyHandlers;
    std::vector<std::pair<EventHandlerWrapperPtr, SocketOperation> > _selectedHandlers;
    std::map<EventHandlerPtr, EventHandlerWrapperPtr, EventHandlerCI> _wrappers;
    int _nSelected;
    int _nSelectedReturned;
};

#endif

}

#endif
