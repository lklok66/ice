// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestI.h>

using namespace std;

int
run(int argc, char* argv[], const Ice::CommunicatorPtr& communicator)
{
    Ice::PropertiesPtr properties = communicator->getProperties();
    if(argc == 2 && string(argv[1]) == "1")
    {
        properties->setProperty("TestAdapter.Endpoints", "udp -p 12010");
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        adapter->add(new TestIntfI, communicator->stringToIdentity("test"));
        adapter->activate();
    }

    string host;
    if(properties->getProperty("Ice.IPv6") == "1")
    {
        host = "\"ff01::1:1\"";
    }
    else
    {
        host = "239.255.1.1";
    }
    properties->setProperty("McastTestAdapter.Endpoints", "udp -h " + host + " -p 12020");
    Ice::ObjectAdapterPtr mcastAdapter = communicator->createObjectAdapter("McastTestAdapter");
    mcastAdapter->add(new TestIntfI, communicator->stringToIdentity("test"));
    mcastAdapter->activate();

    communicator->waitForShutdown();
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
        cerr << ex << endl;
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
            cerr << ex << endl;
            status = EXIT_FAILURE;
        }
    }

    return status;
}
