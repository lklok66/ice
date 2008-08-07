// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceE/IceE.h>
#include <TestI.h>
#include <MyObjectFactory.h>

using namespace std;
using namespace Test;

int
run(int argc, char* argv[], const Ice::CommunicatorPtr& communicator)
{
    Ice::ObjectFactoryPtr factory = new MyObjectFactory;
    communicator->addObjectFactory(factory, "::Test::B");
    communicator->addObjectFactory(factory, "::Test::C");
    communicator->addObjectFactory(factory, "::Test::D");
    communicator->addObjectFactory(factory, "::Test::E");
    communicator->addObjectFactory(factory, "::Test::F");
    communicator->addObjectFactory(factory, "::Test::I");
    communicator->addObjectFactory(factory, "::Test::J");
    communicator->addObjectFactory(factory, "::Test::H");

    communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010 -t 10000");
    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
    InitialPtr initial = new InitialI(adapter);
    adapter->add(initial, communicator->stringToIdentity("initial"));
    adapter->activate();
    InitialPrx allTests(const Ice::CommunicatorPtr&);
    allTests(communicator);
    // We must call shutdown even in the collocated case for cyclic dependency cleanup
    initial->shutdown(Ice::Current());
    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;

    try
    {
        communicator = Ice::initialize(argc, argv);
        status = run(argc, argv, communicator);
    }
    catch(const Ice::Exception& ex)
    {
        fprintf(stderr, "%s\n", ex.toString().c_str());
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
            fprintf(stderr, "%s\n", ex.toString().c_str());
            status = EXIT_FAILURE;
        }
    }

    return status;
}
