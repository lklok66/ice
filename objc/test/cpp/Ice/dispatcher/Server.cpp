// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <dispatcher/TestI.h>
#include <dispatcher/Dispatcher.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Dispatcher;

#if TARGET_OS_IPHONE
#define main runDispatcherServer
#endif

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;
    
    try
    {
        Ice::InitializationData initData;
        initData.properties = defaultServerProperties(argc, argv);
        communicator = Ice::initialize(initData);
        initData.dispatcher = new Test::Dispatcher::ServerDispatcher();
        communicator = Ice::initialize(initData);
#if TARGET_OS_IPHONE
        PrivateServerImplementation::communicator = communicator;
#endif
        communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010");
        communicator->getProperties()->setProperty("ControllerAdapter.Endpoints", "tcp -p 12011");
        communicator->getProperties()->setProperty("ControllerAdapter.ThreadPool.Size", "1");
        
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        Ice::ObjectAdapterPtr adapter2 = communicator->createObjectAdapter("ControllerAdapter");
        
        TestIntfControllerIPtr testController = new TestIntfControllerI(adapter);
        
        adapter->add(new TestIntfI(), communicator->stringToIdentity("test"));
        adapter->activate();
        
        adapter2->add(testController, communicator->stringToIdentity("testController"));
        adapter2->activate();
        serverReady();
        communicator->waitForShutdown();
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
#if TARGET_OS_IPHONE
            PrivateServerImplementation::communicator = 0;
#endif
        }
        catch(const Ice::Exception& ex)
        {
            ostringstream os;
            os << ex << endl;
            tprintf(os.str().c_str());
            status = EXIT_FAILURE;
        }
    }
    
    Test::Dispatcher::ServerDispatcher::terminate();
    return status;
}
