// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <slicing/exceptions/TestI.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Slicing::Exceptions;

#if TARGET_OS_IPHONE
#define main runSlicingExceptionsServer
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
        Ice::PropertiesPtr properties = communicator->getProperties();
        properties->setProperty("Ice.Warn.Dispatch", "0");
        communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010 -t 2000");
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        Ice::ObjectPtr object = new TestI();
        adapter->add(object, communicator->stringToIdentity("Test"));
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
