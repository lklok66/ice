// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#include <adapterDeactivation/ServantLocatorI.h>
#include <TestCommon.h>
#include <Ice/Ice.h>
#include <sstream>

using namespace std;
using namespace Ice;
using namespace Test::AdapterDeactivation;

#if TARGET_OS_IPHONE
#define main runAdapterDeactivationServer
#endif

int
main(int argc, char* argv[])
{
    Ice::CommunicatorPtr communicator;
    int status = 0;
    try 
    {
        Ice::InitializationData initData;
        initData.properties = defaultServerProperties(argc, argv);
        initData.properties->setProperty("Ice.ThreadPool.Server.SizeMax", "3");
        communicator = Ice::initialize(initData);
#if TARGET_OS_IPHONE
        PrivateServerImplementation::communicator = communicator;
#endif
        communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010:udp");
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        ServantLocatorPtr locator = new ServantLocatorI;
    
        adapter->addServantLocator(locator, "");
        adapter->activate();
        serverReady();
        adapter->waitForDeactivate();
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
