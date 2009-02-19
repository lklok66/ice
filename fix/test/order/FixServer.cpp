// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <Control.h>

#include <quickfix/FileStore.h>
#include <quickfix/SocketAcceptor.h>
#include <quickfix/ThreadedSocketAcceptor.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Application.h>
#include <quickfix/fix42/MessageCracker.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/Reject.h>
#include <quickfix/Values.h>
#include <quickfix/Session.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void testFailed(const char* expr, const char* file, unsigned int line)
{
    cout << "failed!" << endl;
    cout << file << ':' << line << ": assertion `" << expr << "' failed" << endl;
    abort();
}

#define test(ex) ((ex) ? ((void)0) : testFailed(#ex, __FILE__, __LINE__))

class FixApplication : public FIX::Application, public FIX42::MessageCracker
{
public:

    FixApplication() :
        _nextExecId(0)
    {
    }

private:

    void onCreate(const FIX::SessionID& id) { }
    void onLogon(const FIX::SessionID& sessionID) {}
    void onLogout(const FIX::SessionID& sessionID) {}
    void toAdmin(FIX::Message&, const FIX::SessionID&) {}
    void toApp(FIX::Message&, const FIX::SessionID&)
        throw(FIX::DoNotSend) {}
    void fromAdmin(const FIX::Message&, const FIX::SessionID&)
        throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {}
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
        throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
    {
        crack(message, sessionID);
    }

    void onMessage(const FIX42::NewOrderSingle& order, const FIX::SessionID& id)
    {
        _lastOrder = order;

        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;
        FIX::ClOrdID clOrdID;
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::Price price;
        FIX::OrderQty orderQty;
        FIX::TimeInForce timeInForce(FIX::TimeInForce_DAY);

        order.getHeader().get(senderCompID);
        order.getHeader().get(targetCompID);
        order.get(clOrdID);
        order.get(symbol);
        order.get(side);
        order.get(ordType);
        if(ordType == FIX::OrdType_LIMIT)
        {
            order.get(price);
        }
        order.get(orderQty);
        if(order.isSetField(timeInForce))
        {
            order.get(timeInForce);
        }

        if(symbol == "REJ") // Reject.
        {
            FIX::MsgSeqNum seqNum;
            order.getHeader().get(seqNum);

            FIX::RefSeqNum refSeqNum(seqNum);

            FIX42::Reject rej(refSeqNum);

            FIX::MsgType msgType;
            order.getHeader().get(msgType);

            rej.set(FIX::RefMsgType(msgType));
            rej.set(FIX::Text("bad symbol"));

            try
            {
                FIX::Session::sendToTarget(rej, targetCompID, senderCompID);
            }
            catch(const FIX::SessionNotFound&)
            {
                test(false);
            }
            return;
        }

        newOrder(clOrdID, symbol, orderQty, senderCompID, targetCompID);

        if(symbol == "STATUS") // Don't fill, until order status is
                               // received.
        {
        }
        else if(symbol == "REPL") // Don't fill, it will be replaced.
        {
        }
        else if(symbol == "CANC") // Don't fill, it will be canceled.
        {
        }
        else if(symbol == "MFIL") // Multiple fills please.
        {
            multiFill(clOrdID, symbol, orderQty, price, senderCompID, targetCompID);
        }
        else // Fill in a single report.
        {
            singleFill(clOrdID, symbol, orderQty, price, senderCompID, targetCompID);
        }
    }

    void onMessage(const FIX42::OrderCancelRequest& req, const FIX::SessionID& id)
    {
        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;

        FIX::OrigClOrdID origClOrdID;
        FIX::Symbol symbol;
        FIX::Side side;

        req.getHeader().get(senderCompID);
        req.getHeader().get(targetCompID);

        req.get(origClOrdID);
        req.get(symbol);
        req.get(side);

        cancel(FIX::ClOrdID(origClOrdID), symbol, senderCompID, targetCompID);
    }

    void onMessage(const FIX42::OrderCancelReplaceRequest& req, const FIX::SessionID& id)
    {
        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;

        FIX::OrigClOrdID origClOrdID;
        FIX::ClOrdID clOrdID;
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::Price price;
        FIX::OrderQty orderQty;

        req.getHeader().get(senderCompID);
        req.getHeader().get(targetCompID);

        req.get(origClOrdID);
        req.get(clOrdID);
        req.get(symbol);
        req.get(side);
        req.get(price);
        req.get(orderQty);

        // Note that the original quantity isn't actually known,
        // however, for the purposes of the test its not a real issue.
        replace(FIX::ClOrdID(origClOrdID), clOrdID, symbol, orderQty, senderCompID, targetCompID);

        multiFill(clOrdID, symbol, orderQty, price, senderCompID, targetCompID);
    }

    void onMessage(const FIX42::OrderStatusRequest& req, const FIX::SessionID& id)
    {
        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;

        FIX::ClOrdID clOrdID;
        FIX::Symbol symbol;
        FIX::Side side;

        req.getHeader().get(senderCompID);
        req.getHeader().get(targetCompID);

        req.get(clOrdID);
        req.get(symbol);
        req.get(side);

        FIX::ClOrdID ordClOrdID;
        _lastOrder.get(ordClOrdID);
        test(ordClOrdID == clOrdID);
        FIX::Symbol ordSymbol;
        _lastOrder.get(ordSymbol);
        test(ordSymbol == symbol);

        FIX42::ExecutionReport report(
            FIX::OrderID(clOrdID),
            FIX::ExecID(nextExecId()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_ORDER_STATUS),
            FIX::OrdStatus(FIX::OrdStatus_NEW),
            FIX::Symbol(symbol),
            FIX::Side(FIX::Side_BUY),
            FIX::LeavesQty(0),
            FIX::CumQty(0),
            FIX::AvgPx(0));

        report.set(clOrdID);

        try
        {
            FIX::Session::sendToTarget(report, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            test(false);
        }

        FIX::Price price;
        FIX::OrderQty orderQty;

        _lastOrder.get(price);
        _lastOrder.get(orderQty);

        singleFill(clOrdID, symbol, orderQty, price, senderCompID, targetCompID);
    }

private:
 
    void
    newOrder(const FIX::ClOrdID& clOrdID, const FIX::Symbol& symbol, const FIX::OrderQty& orderQty,
               const FIX::SenderCompID& senderCompID, const FIX::TargetCompID& targetCompID)
    {
        FIX42::ExecutionReport report(
            FIX::OrderID(clOrdID),
            FIX::ExecID(nextExecId()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_NEW),
            FIX::OrdStatus(FIX::OrdStatus_NEW),
            FIX::Symbol(symbol),
            FIX::Side(FIX::Side_BUY),
            FIX::LeavesQty(orderQty),
            FIX::CumQty(0),
            FIX::AvgPx(0));

        report.set(clOrdID);
        report.set(orderQty);

        try
        {
            FIX::Session::sendToTarget(report, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            test(false);
        }
    }

    void
    singleFill(const FIX::ClOrdID& clOrdID, const FIX::Symbol& symbol, const FIX::OrderQty& orderQty,
               const FIX::Price& price,
               const FIX::SenderCompID& senderCompID, const FIX::TargetCompID& targetCompID)
    {
        FIX42::ExecutionReport report(
            FIX::OrderID(clOrdID),
            FIX::ExecID(nextExecId()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_TRADE),
            FIX::OrdStatus(FIX::OrdStatus_FILLED),
            FIX::Symbol(symbol),
            FIX::Side(FIX::Side_BUY),
            FIX::LeavesQty(0),
            FIX::CumQty(orderQty),
            FIX::AvgPx(price));

        report.set(clOrdID);
        report.set(orderQty);

        report.set(FIX::LastShares(0));
        report.set(FIX::LastPx(price));

        try
        {
            FIX::Session::sendToTarget(report, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            test(false);
        }
    }

    void
    multiFill(const FIX::ClOrdID& clOrdID, const FIX::Symbol& symbol, const FIX::OrderQty& orderQty,
              const FIX::Price& price,
              const FIX::SenderCompID& senderCompID, const FIX::TargetCompID& targetCompID)
    {
        // Fill in a number of 100 share fills.
        double open = orderQty;
        double cum = 0;
        while(open > 0)
        {
            double fill = 100;
            if(fill > open)
            {
                fill = open;
            }
            open -= fill;
            cum += fill;
            FIX::OrdStatus status;
            if(open > 0)
            {
                status = FIX::OrdStatus_PARTIALLY_FILLED;
            }
            else
            {
                status = FIX::OrdStatus_FILLED;
            }
               
            FIX42::ExecutionReport report = FIX42::ExecutionReport(
                FIX::OrderID(clOrdID),
                FIX::ExecID(nextExecId()),
                FIX::ExecTransType(FIX::ExecTransType_NEW),
                FIX::ExecType(FIX::ExecType_TRADE),
                FIX::OrdStatus(status),
                FIX::Symbol(symbol),
                FIX::Side(FIX::Side_BUY),
                FIX::LeavesQty(open),
                FIX::CumQty(cum),
                FIX::AvgPx(price));

            report.set(FIX::ClOrdID(clOrdID));
            report.set(FIX::OrderQty(orderQty));

            report.set(FIX::LastShares(0));
            report.set(FIX::LastPx(price));

            try
            {
                FIX::Session::sendToTarget(report, targetCompID, senderCompID);
            }
            catch(const FIX::SessionNotFound&)
            {
                test(false);
            }
        }
    }

    void
    cancel(const FIX::ClOrdID& clOrdID, const FIX::Symbol& symbol,
           const FIX::SenderCompID& senderCompID, const FIX::TargetCompID& targetCompID)
    {
        FIX42::ExecutionReport report(
            FIX::OrderID(clOrdID),
            FIX::ExecID(nextExecId()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::OrdStatus_CANCELED),
            FIX::OrdStatus(FIX::OrdStatus_CANCELED),
            FIX::Symbol(symbol),
            FIX::Side(FIX::Side_BUY),
            FIX::LeavesQty(0),
            FIX::CumQty(0),
            FIX::AvgPx(0));

        report.set(clOrdID);

        try
        {
            FIX::Session::sendToTarget(report, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            test(false);
        }
    }

    void
    replace(const FIX::ClOrdID& clOrdID, const FIX::ClOrdID& replaceClOrdID, const FIX::Symbol& symbol,
            const FIX::OrderQty& orderQty,
           const FIX::SenderCompID& senderCompID, const FIX::TargetCompID& targetCompID)
    {
        FIX42::ExecutionReport pendingReplace(
            FIX::OrderID(clOrdID),
            FIX::ExecID(nextExecId()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_PENDING_REPLACE),
            FIX::OrdStatus(FIX::OrdStatus_PENDING_REPLACE),
            FIX::Symbol(symbol),
            FIX::Side(FIX::Side_BUY),
            FIX::LeavesQty(orderQty),
            FIX::CumQty(0),
            FIX::AvgPx(0));

        pendingReplace.set(clOrdID);
        pendingReplace.set(orderQty);

        try
        {
            FIX::Session::sendToTarget(pendingReplace, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            test(false);
        }

        FIX42::ExecutionReport replaceReport(
            FIX::OrderID(replaceClOrdID),
            FIX::ExecID(nextExecId()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_REPLACE),
            FIX::OrdStatus(FIX::OrdStatus_NEW),
            FIX::Symbol(symbol),
            FIX::Side(FIX::Side_BUY),
            FIX::LeavesQty(orderQty),
            FIX::CumQty(0),
            FIX::AvgPx(0));

        replaceReport.set(replaceClOrdID);
        replaceReport.set(orderQty);

        try
        {
            FIX::Session::sendToTarget(replaceReport, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            test(false);
        }
    }
    
    string nextExecId()
    {
        ostringstream os;
        os << _nextExecId++;
        return os.str();
    }
    
    int _nextExecId;
    FIX42::NewOrderSingle _lastOrder;
};

class ControlI : public Test::Control
{
public:

    ControlI(FIX::ThreadedSocketAcceptor& acceptor) :
        _acceptor(acceptor)
    {
    }

    virtual void start(const Ice::Current&)
    {
        _acceptor.start();
    }

    virtual void stop(const Ice::Current&)
    {
        _acceptor.stop();
    }

private:

    FIX::ThreadedSocketAcceptor& _acceptor;
};

class FSLog : public FIX::Log
{
public:

    FSLog(const Ice::CommunicatorPtr& communicator) :
        _communicator(communicator),
        _prefix("GLOBAL")
    {
        init();
    }

    FSLog(const Ice::CommunicatorPtr& communicator, const FIX::SessionID& id) :
        _communicator(communicator),
        _prefix(id.toString())
    {
        init();
    }
    
    virtual void clear() 
    {
    }

    virtual void onIncoming(const string& msg)
    {
        if(_traceIncoming > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), _prefix);
            trace << msg;
        }
    }

    virtual void onOutgoing(const string& msg)
    {
        if(_traceOutgoing > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), _prefix);
            trace << msg;
        }
    }

    virtual void onEvent(const string& msg)
    {
        if(_traceEvent)
        {
            Ice::Trace trace(_communicator->getLogger(), _prefix);
            trace << msg;
        }
    }

private:

    void init()
    {
        Ice::PropertiesPtr properties = _communicator->getProperties();
        _traceIncoming = properties->getPropertyAsIntWithDefault("Trace.Incoming", 0);
        _traceOutgoing = properties->getPropertyAsIntWithDefault("Trace.Outgoing", 0);
        _traceEvent = properties->getPropertyAsIntWithDefault("Trace.Event", 0);
    }

    const Ice::CommunicatorPtr _communicator;
    const string _prefix;
    /*const*/ int _traceIncoming;
    /*const*/ int _traceOutgoing;
    /*const*/ int _traceEvent;
};

class FSLogFactory : public FIX::LogFactory
{
public:

    FSLogFactory(const Ice::CommunicatorPtr& communicator) :
        _communicator(communicator)
    {
    }

    virtual FIX::Log* create()
    {
        return new FSLog(_communicator);
    }

    virtual FIX::Log* create(const FIX::SessionID& id)
    {
        return new FSLog(_communicator, id);
    }

    virtual void destroy(FIX::Log* l)
    {
        delete l;
    }

private:

    const Ice::CommunicatorPtr _communicator;
};

class IceFIXServer : public Ice::Application
{
public:

    IceFIXServer()
    {
    }
    
    virtual int run(int argc, char* argv[])
    {
        Ice::PropertiesPtr properties = communicator()->getProperties();
        const string instanceName = properties->getProperty("InstanceName");
        const string file = properties->getProperty("FIXConfig");

        try
        {
            FIX::SessionSettings settings(file);

            FixApplication application;
            FIX::FileStoreFactory storeFactory(settings);
            FSLogFactory logFactory(communicator());
            FIX::ThreadedSocketAcceptor acceptor(application, storeFactory, settings, logFactory);

            Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter(instanceName + ".FixServer");
            adapter->add(new ControlI(acceptor), communicator()->stringToIdentity(instanceName + "/Control"));
            adapter->activate();

            communicator()->waitForShutdown();

            acceptor.stop();

            return 0;
        }
        catch(exception & e)
        {
            cerr << e.what() << endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
};

int
main(int argc, char* argv[])
{
    IceFIXServer app;
    return app.main(argc, argv);
}
