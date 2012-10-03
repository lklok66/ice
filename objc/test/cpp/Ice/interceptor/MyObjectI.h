// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_INTERCEPTOR_MY_OBJECT_I_H
#define TEST_INTERCEPTOR_MY_OBJECT_I_H

#include <InterceptorTest.h>

namespace Test
{

namespace Interceptor
{
    
class MyObjectI : public Test::Interceptor::MyObject
{
public:
    
    virtual int add(int, int, const Ice::Current&);
    virtual int addWithRetry(int, int, const Ice::Current&);
    virtual int badAdd(int, int, const Ice::Current&);
    virtual int notExistAdd(int, int, const Ice::Current&);
    virtual int badSystemAdd(int, int, const Ice::Current&);
};
    
}
    
}

#endif
