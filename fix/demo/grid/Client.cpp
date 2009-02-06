// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <IceFIX/IceFIX.h>

#include <quickfix/Message.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include <quickfix/Values.h>

using namespace std;

class IceFIXClient : public Ice::Application
{
public:

    IceFIXClient();

    virtual int run(int, char*[]);

private:

    void menu();

    bool send(const FIX::Message&);

    IceFIX::ExecutorPrx _executor;
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
          << " [--filtered true|false] [--id id]" << endl;
}

IceFIXClient::IceFIXClient() :
    //
    // Since this is an interactive demo we don't want any signal
    // handling.
    //
    Ice::Application(Ice::NoSignalHandling)
{
}

class ReporterI : public IceFIX::Reporter
{
public:

    virtual void message(const string& data, const Ice::Current&)
    {
        FIX::Message message(data);
        cout << "message: " << message.toXML() << endl;
    }
};

int
IceFIXClient::run(int argc, char* argv[])
{
    Ice::PropertiesPtr properties = communicator()->getProperties();
    string id = properties->getPropertyWithDefault("ClientId", "test");
    string filtered = properties->getPropertyWithDefault("Filtered", "true");

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
        else if(opt == "--filtered")
        {
            ++i;
            if(i >= argc)
            {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            filtered = argv[i];
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

    IceFIX::BridgeAdminPrx admin =
    IceFIX::BridgeAdminPrx::uncheckedCast(communicator()->propertyToProxy("BridgeAdmin"));
    if(!admin)
    {
        cerr << argv[0] << ": invalid proxy" << endl;
        return EXIT_FAILURE;
    }

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Client");
    IceFIX::ReporterPrx reporter = IceFIX::ReporterPrx::uncheckedCast(adapter->addWithUUID(new ReporterI()));
    try
    {
        bridge->connect(id, reporter, _executor);
    }
    catch(const IceFIX::RegistrationException&)
    {
        try
        {
            IceFIX::QoS qos;
            qos["filtered"] = filtered;
            admin->registerWithId(id, qos);
            bridge->connect(id, reporter, _executor);
        }
        catch(const IceFIX::RegistrationException& ex)
        {
            cerr << argv[0] << ": registration failed: `" << ex.reason << "'" << endl;
            return EXIT_FAILURE;
        }
    }
    adapter->activate();

    menu();

    char c;
    do
    {
        try
        {
            cout << "==> ";
            cin >> c;

            if(c == 'o' || c == 'b')
            {
                string clOrdID = IceUtil::generateUUID();
                FIX42::NewOrderSingle req(
                    FIX::ClOrdID( clOrdID ),
                    FIX::HandlInst('1'),
                    FIX::Symbol( "AAPL"),
                    FIX::Side( FIX::Side_BUY ),
                    FIX::TransactTime(),
                    FIX::OrdType( FIX::OrdType_LIMIT ));

                req.set( FIX::Price( 100.0) );
                if(c == 'o')
                {
                    req.set( FIX::OrderQty( 100 ));
                }
                req.set( FIX::TimeInForce( FIX::TimeInForce_DAY ));
                
                if(send(req))
                {
                    cout << "submitted order: `" << clOrdID << "'" << endl;
                }
            }
            else if(c == 'c')
            {
                cout << "order id: ";
                string id;
                cin >> id;
                if(id.empty())
                {
                    cout << "invalid" << endl;
                    continue;
                }
                string clOrdID = IceUtil::generateUUID();
                FIX42::OrderCancelRequest req(
                    FIX::OrigClOrdID( id ),
                    FIX::ClOrdID( clOrdID ),
                    FIX::Symbol( "AAPL"),
                    FIX::Side( FIX::Side_BUY ),
                    FIX::TransactTime());

                if(send(req))
                {
                    cout << "submitted cancel order: `" << clOrdID << "'" << endl;
                }
            }
            else if(c == 'r')
            {
                cout << "order id: ";
                string id;
                cin >> id;
                if(id.empty())
                {
                    cout << "invalid" << endl;
                    continue;
                }
                string clOrdID = IceUtil::generateUUID();
                FIX42::OrderCancelReplaceRequest req(
                    FIX::OrigClOrdID( id ),
                    FIX::ClOrdID( clOrdID ),
                    FIX::HandlInst('1'),
                    FIX::Symbol( "AAPL"),
                    FIX::Side( FIX::Side_BUY ),
                    FIX::TransactTime(),
                    FIX::OrdType( FIX::OrdType_LIMIT ));
                req.set( FIX::Price( 110.0) );
                req.set( FIX::OrderQty( 50 ));
                req.set( FIX::TimeInForce( FIX::TimeInForce_DAY ));

                if(send(req))
                {
                    cout << "submitted cancel replace order: `" << clOrdID << "'" << endl;
                }
            }
            else if(c == 't')
            {
                cout << "order id: ";
                string id;
                cin >> id;
                if(id.empty())
                {
                    cout << "invalid" << endl;
                    continue;
                }
                FIX42::OrderStatusRequest req(
                    FIX::ClOrdID( id ),
                    FIX::Symbol( "AAPL"),
                    FIX::Side( FIX::Side_BUY ));

                send(req);
            }
            else if(c == 'x')
            {
                // Nothing to do
            }
            else if(c == '?')
            {
                menu();
            }
            else
            {
                cout << "unknown command `" << c << "'" << endl;
                menu();
            }
        }
        catch(const Ice::Exception& ex)
        {
            cerr << ex << endl;
        }
    }
    while(cin.good() && c != 'x');

    try
    {
        _executor->destroy();
    }
    catch(const Ice::Exception& ex)
    {
        cerr << "error when destroying excecutor: " << ex << endl;
    }
    return EXIT_SUCCESS;
}

void
IceFIXClient::menu()
{
    cout <<
        "usage:\n"
        "o: submit order\n"
        "c: order cancel\n"
        "r: order cancel replace\n"
        "t: status inquiry\n"
        "b: submit bad order\n"
        "x: exit\n"
        "?: help\n";
}

bool
IceFIXClient::send(const FIX::Message& msg)
{
    try
    {
        _executor->execute(msg.toString());
    }
    catch(const IceFIX::ExecuteException& e)
    {
        cout << "ExecuteException: " << e.reason << endl;
        return false;
    }
    return true;
}
