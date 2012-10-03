// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <retry/TestI.h>

using namespace Test;

void
RetryI::op(bool kill, const Ice::Current& current)
{
    if(kill)
    {
        current.con->close(true);
    }
}

void
RetryI::shutdown(const Ice::Current& current)
{
    current.adapter->getCommunicator()->shutdown();
}
