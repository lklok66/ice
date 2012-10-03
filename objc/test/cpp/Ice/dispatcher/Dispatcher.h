// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef DISPATCHER_I_H
#define DISPATCHER_I_H

#include <IceUtil/Thread.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <Ice/Dispatcher.h>

#include <deque>

namespace Test
{
    
namespace Dispatcher
{

class Dispatcher : public Ice::Dispatcher, public IceUtil::Thread, public IceUtil::Monitor<IceUtil::Mutex>
{
public:
    
    virtual void dispatch(const Ice::DispatcherCallPtr&, const Ice::ConnectionPtr&);
    
    friend class ServerDispatcher;
    friend class ClientDispatcher;
    
protected:
    
    void run();
    
    std::deque<Ice::DispatcherCallPtr> _calls;
    bool _terminated;
};
    
    class ServerDispatcher : public Test::Dispatcher::Dispatcher
    {
    public:
        
        ServerDispatcher();
        virtual ~ServerDispatcher();
        
        static void terminate();
        static bool isDispatcherThread();
        
    private:
        
        static Dispatcher* _instance;
    };
    
    class ClientDispatcher : public Test::Dispatcher::Dispatcher
    {
    public:
        
        ClientDispatcher();
        virtual ~ClientDispatcher();
        
        static void terminate();
        static bool isDispatcherThread();
        
    private:
        
        static Dispatcher* _instance;
    };
    
}
    
}

#endif
