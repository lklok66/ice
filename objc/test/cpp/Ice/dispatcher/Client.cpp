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
#include <DispatcherTest.h>
#include <dispatcher/Dispatcher.h>

using namespace std;
using namespace Test::Dispatcher;

#if TARGET_OS_IPHONE
#define main runDispatcherClient
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
        initData.dispatcher = new Test::Dispatcher::ClientDispatcher();
        communicator = Ice::initialize(initData);
        void dispatcherAllTests(const Ice::CommunicatorPtr&);
        dispatcherAllTests(communicator);
        status = EXIT_SUCCESS;
    }
    catch(const Ice::Exception& ex)
    {
        ostringstream os;
        os << ex << endl;
        tprintf(os.str().c_str());
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
    Test::Dispatcher::ClientDispatcher::terminate();
    return status;
}
