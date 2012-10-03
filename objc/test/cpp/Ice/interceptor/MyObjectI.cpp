// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <interceptor/MyObjectI.h>
#include <TestCommon.h>
#include <IceUtil/IceUtil.h>

using namespace IceUtil; 
using namespace std;
using namespace Test::Interceptor;

int 
MyObjectI::add(int x, int y, const Ice::Current&)
{
    return x + y;
}

int 
MyObjectI::addWithRetry(int x, int y, const Ice::Current& current)
{
    Ice::Context::const_iterator p = current.ctx.find("retry");
    
    if(p == current.ctx.end() || p->second != "no")
    {
        throw Test::Interceptor::RetryException(__FILE__, __LINE__);
    }
    return x + y;
}

int 
MyObjectI::badAdd(int, int, const Ice::Current&)
{
    throw Test::Interceptor::InvalidInputException();
}

int 
MyObjectI::notExistAdd(int, int, const Ice::Current&)
{
    throw Ice::ObjectNotExistException(__FILE__, __LINE__);
}

int 
MyObjectI::badSystemAdd(int, int, const Ice::Current&)
{
    throw Ice::InitializationException(__FILE__, __LINE__, "testing");
}
