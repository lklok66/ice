// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <TestI.h>
#include <Ice/Ice.h>

using namespace std;
using namespace Ice;

test::Message
MyClassI::opMessage(const test::Message& i, test::Message& o, const Ice::Current&)
{
    o = i;
    return i;
}

void
MyClassI::opMessageAMD_async(const Test::AMD_MyClass_opMessageAMDPtr& cb, const test::Message& i, const Ice::Current&)
{
    cb->ice_response(i, i);
}

void
MyClassI::shutdown(const Ice::Current& current)
{
    current.adapter->getCommunicator()->shutdown();
}
