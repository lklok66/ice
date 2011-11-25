// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice FIX is licensed to you under the terms described in the
// ICE_FIX_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <IceFIX/IceFIX.h>

#include <quickfix/Message.h>

using namespace std;

class ReporterI : public IceFIX::Reporter
{
public:

    virtual void message(const string& data, const Ice::Current&)
    {
        FIX::Message message(data);
        cout << "message: " << message.toXML() << endl;
    }
};

class IceFIXClient : public Ice::Application
{
public:

    IceFIXClient();

    virtual int run(int, char*[]);
};

int
main(int argc, char* argv[])
{
    IceFIXClient app;
    return app.main(argc, argv, "config.client");
}

void
usage(const string& n)
{
     cerr << "Usage: " << n
          << " [--id id]" << endl;
}

IceFIXClient::IceFIXClient()
{
}

int
IceFIXClient::run(int argc, char* argv[])
{
    shutdownOnInterrupt();

    Ice::PropertiesPtr properties = communicator()->getProperties();
    string id = properties->getPropertyWithDefault("ClientId", "logger");

    int i;
    for(i = 1; i < argc; ++i)
    {
        string opt = argv[i];
        if(opt == "--id")
        {
            ++i;
            if(i >= argc)
            {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            id = argv[i];
        }
        else
        {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if(i != argc)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    IceFIX::BridgePrx bridge =
    IceFIX::BridgePrx::uncheckedCast(communicator()->propertyToProxy("Bridge"));
    if(!bridge)
    {
        cerr << argv[0] << ": invalid proxy" << endl;
        return EXIT_FAILURE;
    }

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Client");
    IceFIX::ReporterPrx reporter = IceFIX::ReporterPrx::uncheckedCast(adapter->addWithUUID(new ReporterI()));
    IceFIX::ExecutorPrx executor;
    try
    {
        executor = bridge->connect(id, reporter);
    }
    catch(const IceFIX::RegistrationException&)
    {
        try
        {
            IceFIX::BridgeAdminPrx admin = bridge->getAdmin();
            IceFIX::QoS qos;
            qos["filtered"] = "false";
            admin->registerWithId(id, qos);
            executor = bridge->connect(id, reporter);
        }
        catch(const IceFIX::RegistrationException& ex)
        {
            cerr << argv[0] << ": registration failed: `" << ex.reason << "'" << endl;
            return EXIT_FAILURE;
        }
    }
    adapter->activate();

    communicator()->waitForShutdown();

    try
    {
        executor->destroy();
    }
    catch(const Ice::Exception& ex)
    {
        cerr << "error when destroying excecutor: " << ex << endl;
    }
    return EXIT_SUCCESS;
}
