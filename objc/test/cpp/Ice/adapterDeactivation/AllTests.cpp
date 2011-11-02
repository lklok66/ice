// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <AdapterDeactivationTest.h>

using namespace std;
using namespace Ice;
using namespace Test::AdapterDeactivation;

TestIntfPrx
adapterDeactivationAllTests(const Ice::CommunicatorPtr& communicator)
{
    tprintf("testing stringToProxy... ");
    ObjectPrx base = communicator->stringToProxy("test:default -p 12010");
    test(base);
    tprintf("ok\n");
    
    tprintf("testing checked cast... ");
    TestIntfPrx obj = TestIntfPrx::checkedCast(base);
    test(obj);
    test(obj == base);
    tprintf("ok\n");
    
    {
        tprintf("creating/destroying/recreating object adapter... ");
        ObjectAdapterPtr adapter = 
        communicator->createObjectAdapterWithEndpoints("TransientTestAdapter", "default");
        try
        {
            communicator->createObjectAdapterWithEndpoints("TransientTestAdapter", "default");
            test(false);
        }
        catch(const AlreadyRegisteredException&)
        {
        }
        adapter->destroy();
        
        //
        // Use a different port than the first adapter to avoid an "address already in use" error.
        //
        adapter = communicator->createObjectAdapterWithEndpoints("TransientTestAdapter", "default");
        adapter->destroy();
        tprintf("ok\n");
    }
    
    tprintf("creating/activating/deactivating object adapter in one operation... ");
    obj->transient();
    tprintf("ok\n");
    
    tprintf("deactivating object adapter in the server... ");
    obj->deactivate();
    tprintf("ok\n");
    
    tprintf("testing whether server is gone... ");
    try
    {
        obj->ice_ping();
        test(false);
    }
    catch(const LocalException&)
    {
        tprintf("ok\n");
    }
    
    return obj;
}
