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
#include <IceGrid/IceGrid.h>

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

    bool send(const IceFIX::ExecutorPrx&, const FIX::Message&);

    map<string, IceFIX::ExecutorPrx> _executors;
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

    Ice::LocatorPrx locator = communicator()->getDefaultLocator();
    if(!locator)
    {
        cerr << argv[0] << ": no locator configured" << endl;
        return EXIT_FAILURE;
    }

    map<string, IceFIX::BridgePrx > bridges;

    IceGrid::LocatorPrx loc = IceGrid::LocatorPrx::uncheckedCast(locator);
    IceGrid::QueryPrx query = loc->getLocalQuery();
    Ice::ObjectProxySeq a = query->findAllObjectsByType(IceFIX::Bridge::ice_staticId());
    for(Ice::ObjectProxySeq::const_iterator p = a.begin(); p != a.end(); ++p)
    {
	bridges.insert(make_pair((*p)->ice_getIdentity().category, IceFIX::BridgePrx::uncheckedCast(*p)));
    }
    if(bridges.empty())
    {
    	cerr << argv[0] << ": cannot locate any bridges or admins" << endl;
	return EXIT_FAILURE;
    }

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Client");
    IceFIX::ReporterPrx reporter = IceFIX::ReporterPrx::uncheckedCast(adapter->addWithUUID(new ReporterI()));
    for(map<string, IceFIX::BridgePrx>::const_iterator p = bridges.begin(); p != bridges.end(); ++p)
    {
    	cout << "connecting with `" << p->first << "'..." << flush;
	try
	{
	    IceFIX::ExecutorPrx executor;
	    p->second->connect(id, reporter, executor);
	    _executors.insert(make_pair(p->first, executor));
	}
	catch(const IceFIX::RegistrationException&)
	{
	    try
	    {
		cout << " not registered, registering..." << flush;
		IceFIX::BridgeAdminPrx admin = p->second->getAdmin();
		IceFIX::QoS qos;
		qos["filtered"] = filtered;
		admin->registerWithId(id, qos);

		IceFIX::ExecutorPrx executor;
		p->second->connect(id, reporter, executor);
		_executors.insert(make_pair(p->first, executor));
	    }
	    catch(const IceFIX::RegistrationException& ex)
	    {
		cerr << argv[0] << ": registration with `" << p->first << "' failed: `" << ex.reason << "'" << endl;
		return EXIT_FAILURE;
	    }
	}
	cout << " ok" << endl;
    }
    adapter->activate();

    menu();

    IceFIX::ExecutorPrx executor = _executors.begin()->second;
    string bridge = _executors.begin()->first;
    char c;
    do
    {
        try
        {
	    cout << bridge << " ==> ";
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
                
                if(send(executor, req))
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

                if(send(executor, req))
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

                if(send(executor, req))
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

                send(executor, req);
            }
            else if(c == 's')
            {
                cout << "bridge: ";
                string id;
                cin >> id;
                if(id.empty())
                {
                    cout << "invalid" << endl;
                    continue;
                }
		map<string, IceFIX::ExecutorPrx>::const_iterator p = _executors.find(id);
		if(p == _executors.end())
		{
		    cout << "cannot locate" << endl;
		    continue;
		}
		bridge = id;
		executor = p->second;
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

    for(map<string, IceFIX::ExecutorPrx>::const_iterator p = _executors.begin(); p != _executors.end(); ++p)
    {
	try
	{
	    p->second->destroy();
	}
	catch(const Ice::Exception& ex)
	{
	    cerr << "error when destroying excecutor `" << p->first << "': " << ex << endl;
	}
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
        "s: switch bridge\n"
        "x: exit\n"
        "?: help\n";
}

bool
IceFIXClient::send(const IceFIX::ExecutorPrx& executor, const FIX::Message& msg)
{
    try
    {
        executor->execute(msg.toString());
    }
    catch(const IceFIX::ExecuteException& e)
    {
        cout << "ExecuteException: " << e.reason << endl;
        return false;
    }
    return true;
}
