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
#include <InvokeTest.h>

using namespace std;

#if TARGET_OS_IPHONE
#define main runInvokeClient
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
        Test::Invoke::MyClassPrx invokeAllTests(const Ice::CommunicatorPtr&);
        Test::Invoke::MyClassPrx myClass = invokeAllTests(communicator);
        
        myClass->shutdown();
        
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
