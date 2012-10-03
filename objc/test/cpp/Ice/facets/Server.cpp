// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <facets/TestI.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Facets;

#if TARGET_OS_IPHONE
#define main runFacetsServer
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
        communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010");
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        Ice::ObjectPtr d = new DI;
        adapter->add(d, communicator->stringToIdentity("d"));
        adapter->addFacet(d, communicator->stringToIdentity("d"), "facetABCD");
        Ice::ObjectPtr f = new FI;
        adapter->addFacet(f, communicator->stringToIdentity("d"), "facetEF");
        Ice::ObjectPtr h = new HI(communicator);
        adapter->addFacet(h, communicator->stringToIdentity("d"), "facetGH");
        
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
            PrivateServerImplementation::communicator = communicator;
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
