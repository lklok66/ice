// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <TimeoutTest.h>

using namespace std;
using namespace Test;

#if TARGET_OS_IPHONE
#define main runTimeoutClient
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
        
        //
        // For this test, we want to disable retries.
        //
        initData.properties->setProperty("Ice.RetryIntervals", "-1");
        
        //
        // This test kills connections, so we don't want warnings.
        //
        initData.properties->setProperty("Ice.Warn.Connections", "0");
        
        communicator = Ice::initialize(initData);
        TimeoutPrx timeoutAllTests(const Ice::CommunicatorPtr&);
        TimeoutPrx timeout = timeoutAllTests(communicator);
        timeout->shutdown();
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
