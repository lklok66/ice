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
#include <objects/TestI.h>

using namespace std;
using namespace Test::Objects;

class MyObjectFactory : public Ice::ObjectFactory
{
public:
    
    virtual Ice::ObjectPtr create(const string& type)
    {
        if(type == "::Test::Objects::B")
        {
            return new BI;
        }
        else if(type == "::Test::Objects::C")
        {
            return new CI;
        }
        else if(type == "::Test::Objects::D")
        {
            return new DI;
        }
        else if(type == "::Test::Objects::E")
        {
            return new EI;
        }
        else if(type == "::Test::Objects::F")
        {
            return new FI;
        }
        else if(type == "::Test::Objects::I")
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
#define main runObjectsClient
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
        Ice::ObjectFactoryPtr factory = new MyObjectFactory;
        communicator->addObjectFactory(factory, "::Test::Objects::B");
        communicator->addObjectFactory(factory, "::Test::Objects::C");
        communicator->addObjectFactory(factory, "::Test::Objects::D");
        communicator->addObjectFactory(factory, "::Test::Objects::E");
        communicator->addObjectFactory(factory, "::Test::Objects::F");
        communicator->addObjectFactory(factory, "::Test::Objects::I");
        communicator->addObjectFactory(factory, "::Test::Objects::J");
        communicator->addObjectFactory(factory, "::Test::Objects::H");
        
        InitialPrx objectsAllTests(const Ice::CommunicatorPtr&, bool);
        InitialPrx initial = objectsAllTests(communicator, false);
        initial->shutdown();
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
