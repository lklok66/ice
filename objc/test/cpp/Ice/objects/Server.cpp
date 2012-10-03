// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <objects/TestI.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Objects;

class MyObjectFactory : public Ice::ObjectFactory
{
public:
    
    virtual Ice::ObjectPtr create(const string& type)
    {
        if(type == "::Test::Objects::I")
        {
            return new II;
        }
        else if(type == "::Test::Objects::J")
        {
            return new JI;
        }
        else if(type == "::Test::Objects::H")
        {
            return new HI;
        }
        
        assert(false); // Should never be reached
        return 0;
    }
    
    virtual void destroy()
    {
        // Nothing to do
    }
};

#if TARGET_OS_IPHONE
#define main runObjectsServer
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
        Ice::ObjectFactoryPtr factory = new MyObjectFactory;
        communicator->addObjectFactory(factory, "::Test::Objects::I");
        communicator->addObjectFactory(factory, "::Test::Objects::J");
        communicator->addObjectFactory(factory, "::Test::Objects::H");
        
        communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010");
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        InitialPtr initial = new Test::Objects::InitialI(adapter);
        adapter->add(initial, communicator->stringToIdentity("initial"));
        UnexpectedObjectExceptionTestIPtr uoet = new UnexpectedObjectExceptionTestI;
        adapter->add(uoet, communicator->stringToIdentity("uoet"));
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
