// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_I_H
#define TEST_I_H

#include <AdapterDeactivationTest.h>

namespace Test
{

namespace AdapterDeactivation
{

class TestI : public Test::AdapterDeactivation::TestIntf
{
public:
    
    virtual void transient(const Ice::Current&);
    virtual void deactivate(const Ice::Current&);
};

class CookieI : public Test::AdapterDeactivation::Cookie
{
public:
    
    virtual std::string message() const;
};
    
}

}

#endif
