// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_SELECTOR_THREAD_H
#define ICE_SELECTOR_THREAD_H

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include <IceUtil/Timer.h>

#include <Ice/Config.h>
#include <Ice/SelectorThreadF.h>
#include <Ice/SocketReadyCallback.h>
#include <Ice/InstanceF.h>
#include <Ice/Selector.h>

#include <deque>

namespace IceInternal
{

#ifdef ICE_APPLE_CFNETWORK
class RunLoopThread;
typedef IceUtil::Handle<RunLoopThread> RunLoopThreadPtr;
#endif

class SelectorThread : public IceUtil::Shared, IceUtil::Mutex
{
public:

    SelectorThread(const InstancePtr&);
    virtual ~SelectorThread();

    void destroy();

    void incFdsInUse();
    void decFdsInUse();

    void _register(const SocketReadyCallbackPtr&, SocketStatus status, int timeout);
    void unregister(const SocketReadyCallbackPtr&);
    void finish(const SocketReadyCallbackPtr&);

    void joinWithThread();

#ifdef ICE_APPLE_CFNETWORK    
    void streamOpened(const SocketReadyCallbackPtr&);
#endif

private:

    void run();

    class HelperThread : public IceUtil::Thread
    {
    public:
        
        HelperThread(const SelectorThreadPtr&);
        virtual void run();

    private:

        SelectorThreadPtr _selectorThread;
    };
    friend class HelperThread;

    InstancePtr _instance;
    bool _destroyed;

    Selector<SocketReadyCallback> _selector;

    std::deque<SocketReadyCallbackPtr> _finished;

    IceUtil::ThreadPtr _thread;
    IceUtil::TimerPtr _timer;
    
#ifdef ICE_APPLE_CFNETWORK    
    RunLoopThreadPtr _runLoopThread;
#endif
};

}

#endif
