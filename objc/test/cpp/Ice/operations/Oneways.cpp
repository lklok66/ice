// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <OperationsTest.h>

using namespace std;

void
oneways(const Ice::CommunicatorPtr& communicator, const Test::Operations::MyClassPrx& proxy)
{
    Test::Operations::MyClassPrx p = Test::Operations::MyClassPrx::uncheckedCast(proxy->ice_oneway());
    
    {
        p->ice_ping();
    }
    
    {
        try
        {
            p->ice_isA("dummy");
            test(false);
        }
        catch(const Ice::TwowayOnlyException&)
        {
        }
    }
    
    {
        try
        {
            p->ice_id();
            test(false);
        }
        catch(const Ice::TwowayOnlyException&)
        {
        }
    }
    
    {
        try
        {
            p->ice_ids();
            test(false);
        }
        catch(const Ice::TwowayOnlyException&)
        {
        }
    }    
    
    {
        p->opVoid();
    }
    
    {
        p->opIdempotent();
    }
    
    {
        p->opNonmutating();
    }
    
    {
        Ice::Byte b;
        
        try
        {
            p->opByte(Ice::Byte(0xff), Ice::Byte(0x0f), b);
            test(false);
        }
        catch(const Ice::TwowayOnlyException&)
        {
        }
    }
    
}
