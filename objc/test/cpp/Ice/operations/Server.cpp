// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <operations/TestI.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Operations;

#if TARGET_OS_IPHONE
#define main runOperationsServer
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
        //
        // Its possible to have batch oneway requests dispatched after
        // the adapter is deactivated due to thread scheduling so we
        // supress this warning.
        //
        initData.properties->setProperty("Ice.Warn.Dispatch", "0");
        
        communicator = Ice::initialize(initData);
#if TARGET_OS_IPHONE
        PrivateServerImplementation::communicator = communicator;
#endif
        communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010:udp");
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        adapter->add(new MyDerivedClassI, communicator->stringToIdentity("test"));
        adapter->activate();
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
    
    return status;
}
