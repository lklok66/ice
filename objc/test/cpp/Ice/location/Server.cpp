// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <location/ServerLocator.h>
#include <location/TestI.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Location;

#if TARGET_OS_IPHONE
#define main runLocationServer
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
#if TARGET_OS_IPHONE
        PrivateServerImplementation::communicator = communicator;
#endif
        //
        // Register the server manager. The server manager creates a new
        // 'server' (a server isn't a different process, it's just a new
        // communicator and object adapter).
        //
        Ice::PropertiesPtr properties = communicator->getProperties();
        properties->setProperty("Ice.ThreadPool.Server.Size", "2");
        properties->setProperty("ServerManager.Endpoints", "default -p 12010:udp");
        
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("ServerManager");
        
        //
        // We also register a sample server locator which implements the
        // locator interface, this locator is used by the clients and the
        // 'servers' created with the server manager interface.
        //
        ServerLocatorRegistryPtr registry = new ServerLocatorRegistry();
        registry->addObject(adapter->createProxy(communicator->stringToIdentity("ServerManager")));
        Ice::ObjectPtr object = new ServerManagerI(registry, initData);
        adapter->add(object, communicator->stringToIdentity("ServerManager"));
        
        Ice::LocatorRegistryPrx registryPrx = 
        Ice::LocatorRegistryPrx::uncheckedCast(adapter->add(registry, communicator->stringToIdentity("registry")));
        
        Ice::LocatorPtr locator = new ServerLocator(registry, registryPrx);
        adapter->add(locator, communicator->stringToIdentity("locator"));
        
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
