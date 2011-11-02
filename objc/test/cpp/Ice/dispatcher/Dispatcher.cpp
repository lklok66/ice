// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <dispatcher/Dispatcher.h>
#include <TestCommon.h>

Test::Dispatcher::Dispatcher* Test::Dispatcher::ServerDispatcher::_instance = 0;
Test::Dispatcher::Dispatcher* Test::Dispatcher::ClientDispatcher::_instance = 0;

Test::Dispatcher::ServerDispatcher::ServerDispatcher()
{
    _instance = this;
    _terminated = false;
    __setNoDelete(true);
    start();
    __setNoDelete(false);
}

Test::Dispatcher::ServerDispatcher::~ServerDispatcher()
{
    _instance = 0;
}

Test::Dispatcher::ClientDispatcher::ClientDispatcher()
{
    _instance = this;
    _terminated = false;
    __setNoDelete(true);
    start();
    __setNoDelete(false);
}

Test::Dispatcher::ClientDispatcher::~ClientDispatcher()
{
    _instance = 0;
}

void
Test::Dispatcher::Dispatcher::dispatch(const Ice::DispatcherCallPtr& call, const Ice::ConnectionPtr&)
{
    Lock sync(*this);
    _calls.push_back(call);
    if(_calls.size() == 1)
    {
        notify();
    }
}

void
Test::Dispatcher::ServerDispatcher::terminate()
{
    {
        Lock sync(*_instance);
        _instance->_terminated = true;
        _instance->notify();
    }
    
    _instance->getThreadControl().join();
}

bool
Test::Dispatcher::ServerDispatcher::isDispatcherThread()
{
    return IceUtil::ThreadControl() == _instance->getThreadControl();
}

void
Test::Dispatcher::ClientDispatcher::terminate()
{
    {
        Lock sync(*_instance);
        _instance->_terminated = true;
        _instance->notify();
    }
    
    _instance->getThreadControl().join();
}

bool
Test::Dispatcher::ClientDispatcher::isDispatcherThread()
{
    return IceUtil::ThreadControl() == _instance->getThreadControl();
}

void
Test::Dispatcher::Dispatcher::run()
{
    while(true)
    {
        Ice::DispatcherCallPtr call;
        {
            Lock sync(*this);
            
            while(!_terminated && _calls.empty())
            {               
                wait();
            }
            
            if(!_calls.empty())
            {
                call = _calls.front();
                _calls.pop_front();
            }
            else if(_terminated)
            {
                // Terminate only once all calls are dispatched.
                return;
            }
        }
        
        
        if(call)
        {
            try
            {
                call->run();
            }
            catch(...)
            {
                // Exceptions should never propagate here.
                test(false);
            }
        }
    }
}
