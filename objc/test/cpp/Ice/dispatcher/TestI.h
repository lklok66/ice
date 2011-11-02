// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef DISPATCHER_TEST_I_H
#define DISPATCHER_TEST_I_H

#include <DispatcherTest.h>

namespace Test
{
    
namespace Dispatcher
{

class TestIntfControllerI;
typedef IceUtil::Handle<TestIntfControllerI> TestIntfControllerIPtr;

class TestIntfI : virtual public Test::Dispatcher::TestIntf
{
public:
    
    virtual void op(const Ice::Current&);
    virtual void opWithPayload(const Ice::ByteSeq&, const Ice::Current&);
    virtual void shutdown(const Ice::Current&);
};

class TestIntfControllerI : public Test::Dispatcher::TestIntfController, IceUtil::Monitor<IceUtil::Mutex>
{
public:
    
    
    virtual void holdAdapter(const Ice::Current&);
    virtual void resumeAdapter(const Ice::Current&);
    
    TestIntfControllerI(const Ice::ObjectAdapterPtr&);
    
private:
    
    Ice::ObjectAdapterPtr _adapter;
};
    
}
    
}

#endif
