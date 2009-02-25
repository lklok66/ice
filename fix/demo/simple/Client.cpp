// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <IceFIX/IceFIX.h>

#include <fstream>

#include <quickfix/Message.h>
#include <quickfix/Values.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include <quickfix/fix42/OrderCancelReject.h>
#include <quickfix/fix42/BusinessMessageReject.h>
#include <quickfix/fix42/Reject.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/MessageCracker.h>

using namespace std;

static bool
splitString(const string& str, const string& delim, vector<string>& result)
{
    string::size_type pos = 0;
    string::size_type length = str.length();
    string elt;

    while(pos < length)
    {
        char quoteChar = '\0';
        if(str[pos] == '"' || str[pos] == '\'')
        {
            quoteChar = str[pos];
            ++pos;
        }
        bool trim = true;
        while(pos < length)
        {
            if(quoteChar != '\0' && str[pos] == '\\' && pos + 1 < length && str[pos + 1] == quoteChar)
            {
                ++pos;
            }
            else if(quoteChar != '\0' && str[pos] == quoteChar)
            {
                trim = false;
                ++pos;
                quoteChar = '\0';
                break;
            }
            else if(delim.find(str[pos]) != string::npos)
            {
                if(quoteChar == '\0')
                {
                    ++pos;
                    break;
                }
            }
            
            if(pos < length)
            {
               elt += str[pos++];
            }
        }
        if(quoteChar != '\0')
        {
            return false; // Unmatched quote.
        }
        if(elt.length() > 0)
        {
            result.push_back(elt);
            elt = "";
        }
    }
    return true;
}

class ReporterI : public IceFIX::Reporter, public FIX42::MessageCracker
{
public:

    virtual void message(const string& data, const Ice::Current&)
    {
        FIX::Message message(data);
        FIX::SessionID id;
        crack(message, id);
    }

    virtual void onMessage(const FIX42::BusinessMessageReject& reject, const FIX::SessionID&) 
    {
        cout << "BusinessMessageReject" << endl;
        try
        {
            FIX::RefSeqNum seqNum;
            reject.get(seqNum);
            cout << "\tRefSeqNum: " << seqNum << endl;
        }
        catch(const FIX::FieldNotFound&)
        {
        }
    }

    virtual void onMessage(const FIX42::Reject& reject, const FIX::SessionID&) 
    {
        cout << "Reject" << endl;
        try
        {
            FIX::RefSeqNum seqNum;
            reject.get(seqNum);
            cout << "\tRefSeqNum: " << seqNum << endl;
        }
        catch(const FIX::FieldNotFound&)
        {
        }
    }

    virtual void onMessage(const FIX42::OrderCancelReject& reject, const FIX::SessionID&) 
    {
        FIX::OrigClOrdID origClOrdID;
        reject.get(origClOrdID);

        cout << "OrderCancelReject" << endl;
        cout << "OrigClOrdID: " << origClOrdID << endl;
    }

    virtual void onMessage(const FIX42::ExecutionReport& report, const FIX::SessionID&) 
    {
        FIX::ClOrdID clOrdID;
        report.get(clOrdID);

        FIX::ExecType execType;
        report.get(execType);

        FIX::OrdStatus ordStatus;
        report.get(ordStatus);

        FIX::LeavesQty leavesQty;
        report.get(leavesQty);

        FIX::CumQty orderQty;
        report.get(orderQty);

        FIX::Symbol symbol;
        report.get(symbol);
        FIX::Side side;
        report.get(side);
        FIX::CumQty cumQty;
        report.get(cumQty);
        FIX::AvgPx avgPx;
        report.get(avgPx);

        cout << "ExecutionReport" << endl;
        cout << "\tClOrdID: " << clOrdID << endl;

        cout << "\tExecType: ";

        switch(execType)
        {
        case FIX::ExecType_NEW: cout << "new"; break;
        case FIX::ExecType_PARTIAL_FILL: cout << "partial fill"; break;
        case FIX::ExecType_FILL: cout << "fill"; break;
        case FIX::ExecType_DONE_FOR_DAY: cout << "done for day"; break;
        case FIX::ExecType_CANCELED: cout << "canceled"; break;
        case FIX::ExecType_REPLACE: cout << "replace"; break;
        case FIX::ExecType_PENDING_CANCEL: cout << "pending cancel"; break;
        case FIX::ExecType_STOPPED: cout << "stopped"; break;
        case FIX::ExecType_REJECTED: cout << "rejected"; break;
        case FIX::ExecType_SUSPENDED: cout << "suspended"; break;
        case FIX::ExecType_PENDING_NEW: cout << "pending new"; break;
        case FIX::ExecType_CALCULATED: cout << "calculated"; break;
        case FIX::ExecType_EXPIRED: cout << "expired"; break;
        case FIX::ExecType_RESTATED: cout << "restated"; break;
        case FIX::ExecType_PENDING_REPLACE: cout << "pending replace"; break;
        case FIX::ExecType_TRADE: cout << "trade"; break;
        case FIX::ExecType_TRADE_CORRECT: cout << "trade correct"; break;
        case FIX::ExecType_TRADE_CANCEL: cout << "trade cancel"; break;
        case FIX::ExecType_ORDER_STATUS: cout << "order status"; break;
        }
        cout << endl;

        cout << "\tOrdStatus: ";
        switch(ordStatus)
        {
        case FIX::OrdStatus_NEW: cout << "new"; break;
        case FIX::OrdStatus_PARTIALLY_FILLED: cout << "partially filled"; break;
        case FIX::OrdStatus_FILLED: cout << "filled"; break;
        case FIX::OrdStatus_DONE_FOR_DAY: cout << "done for day"; break;
        case FIX::OrdStatus_CANCELED: cout << "canceled"; break;
        case FIX::OrdStatus_REPLACED: cout << "replaced"; break;
        case FIX::OrdStatus_PENDING_CANCEL: cout << "pending cancel"; break;
        case FIX::OrdStatus_STOPPED: cout << "stopped"; break;
        case FIX::OrdStatus_REJECTED: cout << "rejected"; break;
        case FIX::OrdStatus_SUSPENDED: cout << "suspended"; break;
        case FIX::OrdStatus_PENDING_NEW: cout << "pending new"; break;
        case FIX::OrdStatus_CALCULATED: cout << "calculated"; break;
        case FIX::OrdStatus_EXPIRED: cout << "expired"; break;
        case FIX::OrdStatus_ACCEPTED_FOR_BIDDING: cout << "accepted for bidding"; break;
        case FIX::OrdStatus_PENDING_REPLACE: cout << "pending replace"; break;
        }
        cout << endl;

        cout << "\tSymbol: " << symbol << endl;
        cout << "\tSide: " << ((side == FIX::Side_BUY) ? "Buy" : "Sell") << endl;
        cout << "\tLeavesQty: " << leavesQty << endl;
        cout << "\tCumQty: " << cumQty << endl;
        cout << "\tAvgPx: " << avgPx << endl;
    }
};

class ClOrdIDGenerator
{
public:

    ClOrdIDGenerator(const string& name) :
        _name(name),
        _dbname(name + "-clordid"),
        _os(_dbname.c_str(), fstream::in|fstream::out)
    {
        if(!_os.is_open())
        {
            _os.open(_dbname.c_str(), fstream::out);
            if(!_os.is_open())
            {
                throw "failed to open: `" + _dbname + "'";
            }
            _os.clear();
        }
        
        // Give me exceptions from now on.
        _os.exceptions(fstream::eofbit|fstream::failbit|fstream::badbit);
        try
        {
            _os >> _nextId;
        }
        catch(const fstream::failure&)
        {
            _os.clear();
            _nextId = 0;
        }
    }

    string next()
    {
        ostringstream clOrdID;
        clOrdID << _name << "-" << _nextId;
        
        ++_nextId;
        _os.seekp(0, fstream::beg);
        _os << _nextId << "\n" << flush;

        return clOrdID.str();
    }
    
private:

    const string _name;
    const string _dbname;
    fstream _os;
    long _nextId;
};

class IceFIXClient : public Ice::Application
{
public:

    IceFIXClient();

    virtual int run(int, char*[]);

private:

    void menu();
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

IceFIXClient::IceFIXClient() :
    //
    // Since this is an interactive demo we don't want any signal
    // handling.
    //
    Ice::Application(Ice::NoSignalHandling)
{
}

int
IceFIXClient::run(int argc, char* argv[])
{
    Ice::PropertiesPtr properties = communicator()->getProperties();
    string id = properties->getPropertyWithDefault("ClientId", "test");

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

    ClOrdIDGenerator gen(id);

    IceFIX::BridgePrx bridge = IceFIX::BridgePrx::uncheckedCast(communicator()->propertyToProxy("Bridge"));
    if(!bridge)
    {
        cerr << argv[0] << ": invalid proxy" << endl;
        return EXIT_FAILURE;
    }

    IceFIX::ExecutorPrx executor;
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Client");
    IceFIX::ReporterPrx reporter = IceFIX::ReporterPrx::uncheckedCast(adapter->addWithUUID(new ReporterI()));
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

    menu();

    for(;;)
    {
        try
        {
            cout << "==> " << flush;
            string s;
            getline(cin, s);
            if(!cin.good())
            {
                break;
            }
            vector<string> tok;
            if(!splitString(s, " ", tok))
            {
                menu();
                continue;
            }
            if(tok.empty())
            {
                menu();
                continue;
            }

            if(tok[0] == "buy" || tok[0] == "sell")
            {
                if(tok.size() != 4)
                {
                    menu();
                    continue;
                }

                char side = (tok[0] == "buy") ? FIX::Side_BUY : FIX::Side_SELL;
                string symbol(tok[1]);
                double price = atof(tok[2].c_str());
                long quantity = atoi(tok[3].c_str());

                string clOrdID = gen.next();
                FIX42::NewOrderSingle req(
                    FIX::ClOrdID(clOrdID),
                    FIX::HandlInst('1'),
                    FIX::Symbol(symbol),
                    FIX::Side(side),
                    FIX::TransactTime(),
                    FIX::OrdType(FIX::OrdType_LIMIT));

                req.set(FIX::Price(price));
                req.set(FIX::OrderQty(quantity));
                req.set(FIX::TimeInForce(FIX::TimeInForce_DAY));
                
                int seqNum = executor->execute(req.toString());
                cout << "submitted order: " << seqNum << " `" << clOrdID << "'" << endl;
            }
            else if(tok[0] == "cancel")
            {
                if(tok.size() != 4)
                {
                    menu();
                    continue;
                }

                string origClOrdID = tok[1];
                char side = (tok[2] == "buy") ? FIX::Side_BUY : FIX::Side_SELL;
                string symbol = tok[3];

                string clOrdID = gen.next();
                FIX42::OrderCancelRequest req = FIX42::OrderCancelRequest(
                    FIX::OrigClOrdID(origClOrdID),
                    FIX::ClOrdID(clOrdID),
                    FIX::Symbol(symbol),
                    FIX::Side(side),
                    FIX::TransactTime());

                int seqNum = executor->execute(req.toString());
                cout << "submitted cancel order: " << seqNum << " clOrdID: `" << clOrdID << "'" << endl;
            }
            else if(tok[0] == "replace")
            {
                if(tok.size() != 6)
                {
                    menu();
                    continue;
                }

                string origClOrdID = tok[1];
                char side = (tok[2] == "buy") ? FIX::Side_BUY : FIX::Side_SELL;
                string symbol(tok[3]);
                double price = atof(tok[4].c_str());
                long quantity = atoi(tok[5].c_str());

                string clOrdID = gen.next();
                FIX42::OrderCancelReplaceRequest req(
                    FIX::OrigClOrdID(origClOrdID),
                    FIX::ClOrdID(clOrdID),
                    FIX::HandlInst('1'),
                    FIX::Symbol(symbol),
                    FIX::Side(side),
                    FIX::TransactTime(),
                    FIX::OrdType(FIX::OrdType_LIMIT));
                req.set(FIX::Price(price));
                req.set(FIX::OrderQty(quantity));
                req.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

                int seqNum = executor->execute(req.toString());
                cout << "submitted cancel replace order: " << seqNum << " `" << clOrdID << "'" << endl;
            }
            else if(tok[0] == "status")
            {
                if(tok.size() != 4)
                {
                    menu();
                    continue;
                }
                string clOrdID = tok[1];
                char side = (tok[2] == "buy") ? FIX::Side_BUY : FIX::Side_SELL;
                string symbol = tok[3];
                FIX42::OrderStatusRequest req = FIX42::OrderStatusRequest(
                    FIX::ClOrdID(clOrdID),
                    FIX::Symbol(symbol),
                    FIX::Side(side));

                int seqNum = executor->execute(req.toString());
                cout << "submitted order status: " << seqNum << endl;
            }
            else if(tok[0] == "exit")
            {
                break;
            }
            else if(tok[0] == "?")
            {
                menu();
            }
            else
            {
                cout << "unknown command `" << tok[0] << "'" << endl;
                menu();
            }
        }
        catch(const IceFIX::ExecuteException& e)
        {
            cout << "ExecuteException: `" << e.reason << "'" << endl;
        }
        catch(const Ice::Exception& ex)
        {
            cout << ex << endl;
        }
    }

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

void
IceFIXClient::menu()
{
    cout <<
        "usage:\n"
        "buy symbol price quantity                   submit buy order\n"
        "sell symbol price quantity                  submit sell order\n"
        "cancel id sell|buy symbol                   order cancel\n"
        "replace id sell|buy symbol price quantity   order cancel replace\n"
        "status id sell|buy symbol                   status inquiry\n"
        "exit                                        exit\n"
        "?: help\n";
}

