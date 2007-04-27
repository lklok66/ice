// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestAMDI.h>
#include <TestCommon.h>
#include <functional>
#ifdef __BCPLUSPLUS__
#  include <iterator>
#endif

MyDerivedClassI::MyDerivedClassI()
{
}

void
MyDerivedClassI::shutdown_async(const Test::AMD_MyClass_shutdownPtr& cb, const Ice::Current& c)
{
    c.adapter->getCommunicator()->shutdown();
    cb->ice_response();
}

void
MyDerivedClassI::opSleep_async(const Test::AMD_MyClass_opSleepPtr& cb, 
                               int duration, 
                               const Ice::Current&)
{
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(duration));
    cb->ice_response();
}

void
MyDerivedClassI::getContext_async(const Test::AMD_MyClass_getContextPtr& cb, const Ice::Current&)
{
    cb->ice_response(_ctx);
}

bool
MyDerivedClassI::ice_isA(const std::string& s, const Ice::Current& current) const
{
    _ctx = current.ctx;
#ifdef __BCPLUSPLUS__
    return Test::MyDerivedClass::ice_isA(s, current);
#else
    return MyDerivedClass::ice_isA(s, current);
#endif
}
