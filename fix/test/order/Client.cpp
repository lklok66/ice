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

#include <Control.h>

#include <quickfix/Message.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include <quickfix/fix42/BusinessMessageReject.h>
#include <quickfix/fix42/Reject.h>
#include <quickfix/fix42/MessageCracker.h>
#include <quickfix/Values.h>

#include <list>

using namespace std;

void testFailed(const char* expr, const char* file, unsigned int line)
{
    cout << "failed!" << endl;
    cout << file << ':' << line << ": assertion `" << expr << "' failed" << endl;
    abort();
}

#define test(ex) ((ex) ? ((void)0) : testFailed(#ex, __FILE__, __LINE__))

class IceFIXClient : public Ice::Application
{
public:

    IceFIXClient();

    virtual int run(int, char*[]);

private:

    string nextid();

    int send(const IceFIX::ExecutorPrx&, const FIX::Message&);

    int _nextClOrdID;
};

int
main(int argc, char* argv[])
{
    IceFIXClient app;
    return app.main(argc, argv);
}

IceFIXClient::IceFIXClient() :
    _nextClOrdID(0)
{
}

struct Order
{

    Order()
    {
        ordStatus = FIX::OrdStatus_NEW;
        leavesQty = 0;
        orderQty = 0;
        trades = 0;
        reports = 0;
        status = 0;
    }

    Ice::Identity id;
    FIX::ClOrdID clOrdID;
    FIX::OrderQty origOrderQty;

    FIX::OrdStatus ordStatus;
    FIX::LeavesQty leavesQty;
    FIX::CumQty orderQty;

    int trades;
    int reports;
    int status;
};

static Order
createOrder(const Ice::Identity& id, const FIX42::NewOrderSingle& order)
{
    Order o;
    o.id = id;
    order.get(o.clOrdID);
    try
    {
        order.get(o.origOrderQty);
    }
    catch(const FIX::FieldNotFound&)
    {
        // Expected for some orders.
    }

    return o;
}

static Order
createOrder(const Ice::Identity& id, const FIX42::OrderCancelReplaceRequest& order)
{
    Order o;
    o.id = id;
    order.get(o.clOrdID);
    order.get(o.origOrderQty);

    return o;
}

static FIX::ClOrdID
getClOrdID(const FIX42::NewOrderSingle& msg)
{
    FIX::ClOrdID clOrdID;
    msg.get(clOrdID);
    return clOrdID;
}

struct Rejection
{
    Ice::Identity id;
    FIX::RefMsgType refMsgType;
    FIX::RefSeqNum refSeqNum;
};

class ReporterI : public IceFIX::Reporter, public FIX42::MessageCracker
{
public:

    ReporterI(const Ice::CommunicatorPtr& communicator) :
        _communicator(communicator)
    {
    }

    virtual void message(const string& data, const Ice::Current& current)
    {
        FIX::Message message(data);
        FIX::SessionID id("", _communicator->identityToString(current.id), "");
        crack(message, id);
    }

    virtual void onMessage(const FIX42::BusinessMessageReject& reject, const FIX::SessionID& sessid) 
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_ordersMutex);
        Rejection r;
        r.id = _communicator->stringToIdentity(sessid.getSenderCompID());
        reject.get(r.refSeqNum);
        reject.get(r.refMsgType);
        _rejects.push_back(r);
        _ordersMutex.notifyAll();
    }

    virtual void onMessage(const FIX42::Reject& reject, const FIX::SessionID& sessid) 
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_ordersMutex);
        Rejection r;
        r.id = _communicator->stringToIdentity(sessid.getSenderCompID());
        reject.get(r.refSeqNum);
        try
        {
            reject.get(r.refMsgType);
        }
        catch(const FIX::FieldNotFound&)
        {
        }
        _rejects.push_back(r);
        _ordersMutex.notifyAll();
    }

    virtual void onMessage(const FIX42::ExecutionReport& report, const FIX::SessionID& sessid) 
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_ordersMutex);
        Ice::Identity id = _communicator->stringToIdentity(sessid.getSenderCompID());

        FIX::ClOrdID clOrdID;
        report.get(clOrdID);

        FIX::OrdStatus ordStatus;
        report.get(ordStatus);
        // For cancel, pending replace swap the orig clOrdID and the
        // clOrdID.
        if(ordStatus == FIX::OrdStatus_CANCELED || 
           ordStatus == FIX::OrdStatus_PENDING_CANCEL || 
           ordStatus == FIX::OrdStatus_PENDING_REPLACE)
        {
            FIX::OrigClOrdID origClOrdID;
            report.get(origClOrdID);
            clOrdID = string(origClOrdID);
        }

        FIX::ExecType execType;
        report.get(execType);
        for(list<Order>::iterator p = _orders.begin(); p != _orders.end(); ++p)
        {
            if(p->id == id && p->clOrdID == clOrdID)
            {
                report.get(p->ordStatus);
                report.get(p->leavesQty);
                report.get(p->orderQty);

/*
                cout << "(report: ClOrdID: " << clOrdID << " execType: ";

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
                cout << ")" << flush;
*/
                
                if(execType == FIX::ExecType_TRADE)
                {
                    ++p->trades;
                }
                else if(execType == FIX::ExecType_ORDER_STATUS)
                {
                    ++p->status;
                }
                ++p->reports;

                _ordersMutex.notifyAll();
                return;
            }
        }

        // Received a fill for a non-existent order.
        test(false);
    }

    void addOrder(const Order& o)
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_ordersMutex);
        _orders.push_back(o);
    }

    Order waitFill(const string& clOrdID, const IceUtil::Time& timeout = IceUtil::Time::seconds(10))
    {
        IceUtil::Time start = IceUtil::Time::now();
        IceUtil::Time end = start + timeout;

        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_ordersMutex);

        list<Order>::const_iterator p;
        for(p = _orders.begin(); p != _orders.end(); ++p)
        {
            if(p->clOrdID == clOrdID)
            {
                break;
            }
        }
        test(p != _orders.end());

        while(IceUtil::Time::now() < end)
        {
            if(p->ordStatus == FIX::OrdStatus_FILLED ||
               p->ordStatus == FIX::OrdStatus_CANCELED ||
               p->ordStatus == FIX::OrdStatus_PENDING_REPLACE)
            {
                break;
            }
            IceUtil::Time interval = end - IceUtil::Time::now();
            if(interval > IceUtil::Time::seconds(0))
            {
                _ordersMutex.timedWait(interval);
            }
        }
        return *p;
    }

    bool waitReject(const Ice::Identity& id, int refSeqNum, const IceUtil::Time& timeout = IceUtil::Time::seconds(10))
    {
        IceUtil::Time start = IceUtil::Time::now();
        IceUtil::Time end = start + timeout;

        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_ordersMutex);
        while(IceUtil::Time::now() < end && _rejects.empty())
        {
            IceUtil::Time interval = end - IceUtil::Time::now();
            if(interval > IceUtil::Time::seconds(0))
            {
                _ordersMutex.timedWait(interval);
            }
        }
        if(_rejects.empty())
        {
            return false;
        }
        Rejection r = _rejects.front();
        _rejects.pop_front();
        return r.id == id && r.refSeqNum == refSeqNum;
    }

private:

    const Ice::CommunicatorPtr _communicator;
    IceUtil::Monitor<IceUtil::Mutex> _ordersMutex;
    list<Order> _orders;

    list<Rejection> _rejects;
};
typedef IceUtil::Handle<ReporterI> ReporterIPtr;

int
IceFIXClient::run(int argc, char* argv[])
{
    Ice::PropertiesPtr properties = communicator()->getProperties();
    Ice::LocatorPrx locator = communicator()->getDefaultLocator();
    if(!locator)
    {
        cerr << argv[0] << ": no locator configured" << endl;
        return EXIT_FAILURE;
    }

    IceFIX::BridgePrx tp1Bridge = IceFIX::BridgePrx::uncheckedCast(communicator()->stringToProxy("TP1/Bridge"));
    IceFIX::BridgePrx tp2Bridge = IceFIX::BridgePrx::uncheckedCast(communicator()->stringToProxy("TP2/Bridge"));

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("Client", "default");
    ReporterIPtr reporter = new ReporterI(communicator());

    cout << "testing register/connect... " << flush;
    //
    // TEST: connect with a non-registered id generates the correct
    // exception.
    //
    try
    {
        IceFIX::ReporterPrx testReporterPrx;
        IceFIX::ExecutorPrx testExecutor;
        tp1Bridge->connect("client",  testReporterPrx, testExecutor);
        test(false);
    }
    catch(const IceFIX::RegistrationException&)
    {
        // Expected
    }

    IceFIX::QoS qos;

    IceFIX::BridgeAdminPrx tp1Admin = tp1Bridge->getAdmin();
    string clientId = tp1Admin->_cpp_register(qos);

    //
    // TEST: register with an existing id generates the correct
    // exception.
    //
    try
    {
        tp1Admin->registerWithId(clientId, qos);
        test(false);
    }
    catch(const IceFIX::RegistrationException&)
    {
        // Expected.
    }

    //
    // TEST: unregister.
    //
    try
    {
        tp1Admin->unregister(clientId);
    }
    catch(const IceFIX::RegistrationException&)
    {
        test(false);
    }

    try
    {
        tp1Admin->unregister(clientId);
        test(false);
    }
    catch(const IceFIX::RegistrationException&)
    {
        // Expected.
    }
    tp1Admin->registerWithId(clientId, qos);

    Ice::Identity tp1ReporterId = communicator()->stringToIdentity("TP1Reporter");
    IceFIX::ReporterPrx tp1ReporterPrx = IceFIX::ReporterPrx::uncheckedCast(adapter->add(reporter, tp1ReporterId));
    IceFIX::ExecutorPrx tp1;
    tp1Bridge->connect(clientId, tp1ReporterPrx, tp1);

    //
    // TEST: Ensure the id namespace of the two bridges are separate.
    //
    try
    {
        IceFIX::ReporterPrx testReporterPrx;
        IceFIX::ExecutorPrx testExecutor;
        tp2Bridge->connect(clientId,  testReporterPrx, testExecutor);
        test(false);
    }
    catch(const IceFIX::RegistrationException&)
    {
        // Expected
    }

    IceFIX::BridgeAdminPrx tp2Admin = tp2Bridge->getAdmin();
    tp2Admin->registerWithId(clientId, qos);

    Ice::Identity tp2ReporterId = communicator()->stringToIdentity("TP2Reporter");
    IceFIX::ReporterPrx tp2ReporterPrx = IceFIX::ReporterPrx::uncheckedCast(adapter->add(reporter, tp2ReporterId));
    IceFIX::ExecutorPrx tp2;
    tp2Bridge->connect(clientId, tp2ReporterPrx, tp2);
    cout << "ok" << endl;

    adapter->activate();

    Test::ControlPrx tp1Control = Test::ControlPrx::uncheckedCast(
        communicator()->stringToProxy("FixTP1/Control"));
    Test::ControlPrx tp2Control = Test::ControlPrx::uncheckedCast(
        communicator()->stringToProxy("FixTP2/Control"));

    tp2Control->start();
    tp2Admin->activate();

    // TEST: activate/deactivate. Send a request and wait for the fill
    // which should not arrive.
    cout << "testing activate... " << flush;
    {
        FIX42::NewOrderSingle newOrderSingle1(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle1.set(FIX::Price(100.0));
        newOrderSingle1.set(FIX::OrderQty(100));
        newOrderSingle1.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp1ReporterId, newOrderSingle1));
        send(tp1, newOrderSingle1);
        // There should be no fill received in 5 seconds.
        Order o = reporter->waitFill(getClOrdID(newOrderSingle1), IceUtil::Time::seconds(5));
        test(o.ordStatus != FIX::OrdStatus_FILLED);

        // Now we'll start the FIX servers, and activate the bridge.
        tp1Control->start();

        tp1Admin->activate();

        o = reporter->waitFill(getClOrdID(newOrderSingle1));
        test(o.ordStatus == FIX::OrdStatus_FILLED);
    }
    cout << "ok" << endl;

    // TEST: clean
    cout << "testing clean... " << flush;
    {
        Ice::Long count = tp1Admin->clean(60*60, true);
        test(count == 0);
        count = tp2Admin->clean(60*60, true);
        test(count == 0);

        count = tp1Admin->clean(0, false);
        test(count == 2);
        count = tp2Admin->clean(0, false);
        test(count == 0);
        count = tp1Admin->clean(0, true);
        test(count == 2);
        count = tp1Admin->clean(0, true);
        test(count == 0);
    }
    cout << "ok" << endl;

    // TEST: NewOrderSingle execution reports are correctly reverse
    // routed.
    cout << "testing NewOrderSingle... " << flush;
    {
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(200));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp2ReporterId, newOrderSingle));

        send(tp2, newOrderSingle);

        Order o = reporter->waitFill(getClOrdID(newOrderSingle));
        test(o.ordStatus == FIX::OrdStatus_FILLED);
    }

    cout << "ok" << endl;

    // TEST: NewOrderSingle with multiple trades.
    cout << "testing NewOrderSingle with multiple trades... " << flush;

    {
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("MFIL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(555));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp1ReporterId, newOrderSingle));

        send(tp1, newOrderSingle);

        Order o = reporter->waitFill(getClOrdID(newOrderSingle));
        test(o.ordStatus == FIX::OrdStatus_FILLED && o.trades > 1);
    }

    cout << "ok" << endl;

    // TEST: OrderCancelRequest
    cout << "testing OrderCancelRequest... " << flush;
    {
        FIX::ClOrdID clOrdID = FIX::ClOrdID(nextid());
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(clOrdID),
            FIX::HandlInst('1'),
            FIX::Symbol("CANC"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(555));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp1ReporterId, newOrderSingle));

        send(tp1, newOrderSingle);

        FIX42::OrderCancelRequest cancelReq(
            FIX::OrigClOrdID(clOrdID),
            FIX::ClOrdID(FIX::ClOrdID(nextid())),
            FIX::Symbol("CANC"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime());
        cout << "cancel: " << getClOrdID(cancelReq) << endl;

        send(tp1, cancelReq);

        Order o = reporter->waitFill(clOrdID);
        test(o.ordStatus == FIX::OrdStatus_CANCELED);
    }
    cout << "ok" << endl;

    // TEST: OrderCancelReplaceRequest
    cout << "testing OrderCancelReplaceRequest... " << flush;
    {
        FIX::ClOrdID clOrdID = FIX::ClOrdID(nextid());
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(clOrdID),
            FIX::HandlInst('1'),
            FIX::Symbol("REPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(555));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp1ReporterId, newOrderSingle));
        send(tp1, newOrderSingle);

        FIX::ClOrdID replaceClOrdID = FIX::ClOrdID(nextid());
        FIX42::OrderCancelReplaceRequest req(
            FIX::OrigClOrdID(clOrdID),
            FIX::ClOrdID(replaceClOrdID),
            FIX::HandlInst('1'),
            FIX::Symbol("REPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));
        req.set(FIX::Price(95.0));
        req.set(FIX::OrderQty(550));
        req.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp1ReporterId, req));
        
        send(tp1, req);

        Order o = reporter->waitFill(clOrdID);
        test(o.ordStatus == FIX::OrdStatus_PENDING_REPLACE);

        o = reporter->waitFill(replaceClOrdID);
        test(o.ordStatus == FIX::OrdStatus_FILLED && o.trades > 1);
    }
    cout << "ok" << endl;

    // TEST: OrderStatusRequest
    cout << "testing OrderStatusRequest... " << flush;
    {
        FIX::ClOrdID clOrdID = FIX::ClOrdID(nextid());
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(clOrdID),
            FIX::HandlInst('1'),
            FIX::Symbol("STATUS"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(555));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp1ReporterId, newOrderSingle));

        send(tp1, newOrderSingle);

        FIX42::OrderStatusRequest req(
            FIX::ClOrdID( clOrdID ),
            FIX::Symbol("STATUS"),
            FIX::Side( FIX::Side_BUY ));

        send(tp1, req);

        Order o = reporter->waitFill(clOrdID);
        test(o.ordStatus == FIX::OrdStatus_FILLED && o.status > 0);
    }
    cout << "ok" << endl;

    // TEST: BusinessMessageReject reverse routing.
    cout << "testing BusinessMessageReject... " << flush;
    {
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        // Here OrderQty is not set, causing a business reject.
        //newOrderSingle.set(FIX::OrderQty(200));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp2ReporterId, newOrderSingle));

        int seqNum = send(tp2, newOrderSingle);
        test(reporter->waitReject(tp2ReporterId, seqNum));
    }
    cout << "ok" << endl;

    // TEST: Reject reverse routing.
    cout << "testing Reject... " << flush;
    {
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("REJ"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(1));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp2ReporterId, newOrderSingle));

        int seqNum = send(tp2, newOrderSingle);
        test(reporter->waitReject(tp2ReporterId, seqNum));
    }
    cout << "ok" << endl;

    // TEST: deactivate
    cout << "testing deactivate... " << flush;
    {
        tp1Admin->deactivate();

        FIX42::NewOrderSingle newOrderSingle3(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle3.set(FIX::Price(100.0));
        newOrderSingle3.set(FIX::OrderQty(200));
        newOrderSingle3.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp1ReporterId, newOrderSingle3));

        send(tp1, newOrderSingle3);

        Order o = reporter->waitFill(getClOrdID(newOrderSingle3), IceUtil::Time::seconds(5));
        test(o.ordStatus != FIX::OrdStatus_FILLED);

        FIX42::NewOrderSingle newOrderSingle4(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle4.set(FIX::Price(100.0));
        newOrderSingle4.set(FIX::OrderQty(200));
        newOrderSingle4.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp2ReporterId, newOrderSingle4));

        send(tp2, newOrderSingle4);

        o = reporter->waitFill(getClOrdID(newOrderSingle4));
        test(o.ordStatus == FIX::OrdStatus_FILLED);

        tp1Control->stop();
        tp1Control->start();
        tp1Admin->activate();
        o = reporter->waitFill(getClOrdID(newOrderSingle3));
        test(o.ordStatus == FIX::OrdStatus_FILLED);
    }
    cout << "ok" << endl;

    // TEST: QoS
    cout << "testing QoS... " << flush;

    Ice::Identity nonFilteredReporterId = communicator()->stringToIdentity("FilteredReporter");
    ReporterIPtr nonFilteredReporter = new ReporterI(communicator());
    IceFIX::ExecutorPrx nonFilteredTp;

    IceFIX::QoS nonFilteredQoS;
    nonFilteredQoS["filtered"] = "false";
    string nonFilteredClientId = tp2Admin->_cpp_register(nonFilteredQoS);

    IceFIX::ReporterPrx nonFilteredTp2ReporterPrx = IceFIX::ReporterPrx::uncheckedCast(
        adapter->add(nonFilteredReporter, nonFilteredReporterId));
    tp2Bridge->connect(nonFilteredClientId, nonFilteredTp2ReporterPrx, nonFilteredTp);

    {
        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(200));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp2ReporterId, newOrderSingle));
        nonFilteredReporter->addOrder(createOrder(nonFilteredReporterId, newOrderSingle));

        send(tp2, newOrderSingle);

        Order o = reporter->waitFill(getClOrdID(newOrderSingle));
        test(o.ordStatus == FIX::OrdStatus_FILLED);

        o = nonFilteredReporter->waitFill(getClOrdID(newOrderSingle));
        test(o.ordStatus == FIX::OrdStatus_FILLED);
    }
    cout << "ok" << endl;

    // TEST: message persistence
    cout << "testing message persistence... " << flush;
    {
        // Destroy the non-filtered client, and then send a message.
        nonFilteredTp->destroy();

        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(200));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp2ReporterId, newOrderSingle));
        nonFilteredReporter->addOrder(createOrder(nonFilteredReporterId, newOrderSingle));

        send(tp2, newOrderSingle);

        Order o = reporter->waitFill(getClOrdID(newOrderSingle));
        test(o.ordStatus == FIX::OrdStatus_FILLED);

        // Now the message has been queued for the non-filtered
        // client, so reconnect the client and wait for the message to
        // arrive.
        tp2Bridge->connect(nonFilteredClientId, nonFilteredTp2ReporterPrx, nonFilteredTp);

        o = nonFilteredReporter->waitFill(getClOrdID(newOrderSingle));
        test(o.ordStatus == FIX::OrdStatus_FILLED);
    }
    {
        // For the second test, we want to queue up a message, stop
        // the server and restart and ensure the message is delivered
        // when it is restarted.

        // Destroy the non-filtered client, and then send a message.
        nonFilteredTp->destroy();

        FIX42::NewOrderSingle newOrderSingle(
            FIX::ClOrdID(nextid()),
            FIX::HandlInst('1'),
            FIX::Symbol("AAPL"),
            FIX::Side(FIX::Side_BUY),
            FIX::TransactTime(),
            FIX::OrdType(FIX::OrdType_LIMIT));

        newOrderSingle.set(FIX::Price(100.0));
        newOrderSingle.set(FIX::OrderQty(200));
        newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

        reporter->addOrder(createOrder(tp2ReporterId, newOrderSingle));
        nonFilteredReporter->addOrder(createOrder(nonFilteredReporterId, newOrderSingle));

        send(tp2, newOrderSingle);

        Order o = reporter->waitFill(getClOrdID(newOrderSingle));
        test(o.ordStatus == FIX::OrdStatus_FILLED);

        {
            IceGrid::RegistryPrx registry = IceGrid::RegistryPrx::checkedCast(
                communicator()->stringToProxy("IceGrid/Registry"));
            test(registry);
            IceGrid::AdminSessionPrx session = registry->createAdminSession("foo", "bar");

            // No need for the keep alive thread.
            //SessionKeepAliveThreadPtr keepAlive = new SessionKeepAliveThread(
            //   session, registry->getSessionTimeout()/2);
            //keepAlive->start();

            IceGrid::AdminPrx admin = session->getAdmin();
            test(admin);
            admin->stopServer("TP2");

            // Destroy the admin session.
            session->destroy();
        }

        // Now the message has been queued for the non-filtered
        // client, so reconnect the client and wait for the message to
        // arrive.
        tp2Bridge->connect(nonFilteredClientId, nonFilteredTp2ReporterPrx, nonFilteredTp);

        // We need more timeout here because it takes some time to
        // restart the server.
        o = nonFilteredReporter->waitFill(getClOrdID(newOrderSingle), IceUtil::Time::seconds(30));
        test(o.ordStatus == FIX::OrdStatus_FILLED);

        nonFilteredTp->destroy();
        tp2Admin->unregister(nonFilteredClientId);
        adapter->remove(nonFilteredReporterId);

    }
    cout << "ok" << endl;

    try
    {
        tp1->destroy();
        tp2->destroy();
    }
    catch(const Ice::Exception& ex)
    {
        cerr << "error when destroying excecutor: " << ex << endl;
    }


    return EXIT_SUCCESS;
}

int
IceFIXClient::send(const IceFIX::ExecutorPrx& executor, const FIX::Message& msg)
{
    try
    {
        return executor->execute(msg.toString());
    }
    catch(const IceFIX::ExecuteException& e)
    {
        cout << "ExecuteException: " << e.reason << endl;
        return false;
    }
    return -1;
}

string
IceFIXClient::nextid()
{
    ostringstream os;
    os << _nextClOrdID++;
    return os.str();
}

