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

#if TARGET_OS_IPHONE
#define main runOperationsClient
#endif

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;
    
    try
    {
        //
        // In this test, we need at least two threads in the
        // client side thread pool for nested AMI.
        //
        Ice::InitializationData initData;
        initData.properties = defaultClientProperties(argc, argv);
        initData.properties->setProperty("Ice.ThreadPool.Client.Size", "2");
        initData.properties->setProperty("Ice.ThreadPool.Client.SizeWarn", "0");
        
        //
        // We must set MessageSizeMax to an explicit values, because
        // we run tests to check whether Ice.MemoryLimitException is
        // raised as expected.
        //
        initData.properties->setProperty("Ice.MessageSizeMax", "100");
        
        communicator = Ice::initialize(initData);
        Test::Operations::MyClassPrx operationsAllTests(const Ice::CommunicatorPtr&);
        Test::Operations::MyClassPrx myClass = operationsAllTests(communicator);
        
        tprintf("testing server shutdown... ");
        myClass->shutdown();
        try
        {
            myClass->opVoid();
            test(false);
        }
        catch(const Ice::LocalException&)
        {
            tprintf("ok\n");
        }
        
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
