// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <TestCommon.h>
#include <AdapterDeactivationTest.h>
#include <iostream>

using namespace std;
using namespace Ice;
using namespace Test::AdapterDeactivation;

#if TARGET_OS_IPHONE
#define main runAdapterDeactivationClient
#endif

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;
    try
    {
        Ice::InitializationData initData;
        initData.properties = defaultClientProperties(argc, argv);
        communicator = Ice::initialize(initData);

        ObjectAdapterPtr adapter = 
            communicator->createObjectAdapterWithEndpoints("TransientTestAdapter", "default -p 9999");
        adapter->activate();
        adapter->destroy();

        TestIntfPrx adapterDeactivationAllTests(const CommunicatorPtr&);
        TestIntfPrx obj = adapterDeactivationAllTests(communicator);
        status = EXIT_SUCCESS;
    }
    catch(const Ice::Exception& ex)
    {
        ostringstream os;
        os << ex << endl;
        tprintf(os.str().c_str());
        status = EXIT_FAILURE;
    }
    catch(const TestFailedException&)
    {
        status = EXIT_FAILURE;
    }
    
    if(communicator)
    {
        try
        {
            communicator->destroy();
        }
        catch(const Ice::Exception& ex)
        {
            ostringstream os;
            os << ex << endl;
            tprintf(os.str().c_str());
            status = EXIT_FAILURE;
        }
    }
    
    return status;
}
