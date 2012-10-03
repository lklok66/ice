// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <dispatcher/TestI.h>
#include <Ice/Ice.h>
#include <dispatcher/Dispatcher.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Dispatcher;

void
TestIntfI::op(const Ice::Current&)
{
    test(ServerDispatcher::isDispatcherThread());
}

void
TestIntfI::opWithPayload(const Ice::ByteSeq&, const Ice::Current&)
{
    test(ServerDispatcher::isDispatcherThread());
}

void
TestIntfI::shutdown(const Ice::Current& current)
{
    test(ServerDispatcher::isDispatcherThread());
    current.adapter->getCommunicator()->shutdown();
}

void
TestIntfControllerI::holdAdapter(const Ice::Current&)
{
    test(ServerDispatcher::isDispatcherThread());
    _adapter->hold();
}

void
TestIntfControllerI::resumeAdapter(const Ice::Current&)
{
    test(ServerDispatcher::isDispatcherThread());
    _adapter->activate();
}

TestIntfControllerI::TestIntfControllerI(const Ice::ObjectAdapterPtr& adapter) : _adapter(adapter)
{
}
