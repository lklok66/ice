// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <quickfix/FileStore.h>
#include <quickfix/SocketAcceptor.h>
#include <quickfix/ThreadedSocketAcceptor.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Application.h>
#include <quickfix/Values.h>
#include <quickfix/Session.h>

#include <quickfix/fix42/MessageCracker.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReject.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/Reject.h>

#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace std;

struct Order
{
    Order()
    {
    }

    Order(const string& fsenderCompID, const string& ftargetCompID, const string& fclOrdID, const string& fsymbol,
          FIX::Side fside, double fprice, long forderQty)
    {
        senderCompID = fsenderCompID;
        targetCompID = ftargetCompID;
        clOrdID = fclOrdID;
        symbol = fsymbol;
        side = fside;
        price = fprice;
        orderQty = forderQty;

        openQuantity = orderQty;
        executedQuantity = 0;
        avgExecutedPrice = 0;
        lastExecutedPrice = 0;
        lastExecutedQuantity = 0;
    }

    void execute(double price, long quantity)
    {
        avgExecutedPrice = ((quantity*price)+(avgExecutedPrice*executedQuantity))/(quantity+executedQuantity);
        openQuantity -= quantity;
        executedQuantity += quantity;
        lastExecutedPrice = price;
        lastExecutedQuantity = quantity;
    }
    
    string senderCompID;
    string targetCompID;
    string clOrdID;
    string symbol;
    FIX::Side side;
    double price;
    long orderQty;

    long openQuantity;
    long executedQuantity;
    double avgExecutedPrice;
    double lastExecutedPrice;
    long lastExecutedQuantity;
};

static bool
BidSort(const Order& left, const Order& right)
{
    return left.price > right.price;
}

static bool
AskSort(const Order& left, const Order& right)
{
    return left.price < right.price;
}

// The order book for a given symbol.
class OrderBook
{
public:

    OrderBook(const string& symbol) :
        _symbol(symbol)
    {
    }

    void
    add(const Order& order)
    {
        if(order.side == FIX::Side_BUY)
        {
            _bids.push_back(order);
            sort(_bids.begin(), _bids.end(), BidSort);
        }
        else // FIX::Side_SELL
        {
            _asks.push_back(order);
            sort(_asks.begin(), _asks.end(), AskSort);
        }
    }

    bool
    cancel(const string& clOrdID, FIX::Side side, Order& o)
    {
        vector<Order>& v = (side == FIX::Side_BUY) ? _bids : _asks;
        for(vector<Order>::iterator p = v.begin(); p != v.end(); ++p)
        {
            if(p->clOrdID == clOrdID)
            {
                o = *p;
                v.erase(p);
                return true;
            }
        }
        return false;
    }

    bool
    find(const string& clOrdID, FIX::Side side, Order& o) const
    {
        const vector<Order>& v = (side == FIX::Side_BUY) ? _bids : _asks;
        for(vector<Order>::const_iterator p = v.begin(); p != v.end(); ++p)
        {
            if(p->clOrdID == clOrdID)
            {
                o = *p;
                return true;
            }
        }
        return false;
    }

    void match(vector<Order>& orders)
    {
        for(;;)
        {
            if(_bids.empty() || _asks.empty())
            {
                return;
            }
            Order& bid = _bids.front();
            Order& ask = _asks.front();

            if(bid.price < ask.price)
            {
                break;
            }

            double price = ask.price;
            long quantity = (bid.openQuantity > ask.openQuantity) ? ask.openQuantity : bid.openQuantity;

            ask.execute(price, quantity);
            bid.execute(price, quantity);

            orders.push_back(ask);
            orders.push_back(bid);

            if(ask.openQuantity == 0)
            {
                _asks.erase(_asks.begin());
            }
            if(bid.openQuantity == 0)
            {
                _bids.erase(_bids.begin());
            }
        }
    }

    void
    list() const
    {
        cout << "Symbol: " << _symbol << endl;
        cout << "Order Book" << endl;
        cout << "      Bid                 Ask" << endl;
        cout << "Price     Size      Price     Size" << endl;
        //       0123456789012345678901234567890123456789
        for(unsigned int index = 0; index < _bids.size() || index < _asks.size(); ++index)
        {
            if(index < _bids.size())
            {
                Order bid = _bids[index];
                printf("%-10.2f%-10ld", (float)bid.price, bid.openQuantity);
            }
            else
            {
                printf("%20s", "");
            }
            if(index < _asks.size())
            {
                Order ask = _asks[index];
                printf("%-10.2f%-10ld", (float)ask.price, ask.openQuantity);
            }
            else
            {
                printf("%20s", "");
            }
            cout << endl;
        }
    }
        
private:

    const string _symbol;
    vector<Order> _bids;
    vector<Order> _asks;
};

class FixApplication : public FIX::Application, public FIX42::MessageCracker
{
public:

    FixApplication() :
        _nextExecId(0),
        _autoMatch(true)
    {
    }

    void list()
    {
        for(map<string, OrderBook>::const_iterator p = _symbols.begin(); p != _symbols.end(); ++p)
        {
            p->second.list();
        }
    }

    void
    match()
    {
        for(map<string, OrderBook>::iterator p = _symbols.begin(); p != _symbols.end(); ++p)
        {
            match(p->second);
        }
    }

    void
    setAutoMatch(bool v)
    {
        _autoMatch = v;
    }

private:

    void onCreate(const FIX::SessionID&) { }
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

    void onMessage(const FIX42::NewOrderSingle& order, const FIX::SessionID&)
    {
        FIX::OrdType ordType;
        order.get(ordType);
        if(ordType != FIX::OrdType_LIMIT)
        {
            throw logic_error("LIMIT orders only please");
        }

        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;
        FIX::ClOrdID clOrdID;
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::Price price;
        FIX::OrderQty orderQty;

        order.getHeader().get(senderCompID);
        order.getHeader().get(targetCompID);
        order.get(clOrdID);
        order.get(symbol);
        order.get(side);
        order.get(price);
        order.get(orderQty);

        FIX42::ExecutionReport report(
            FIX::OrderID(clOrdID),
            FIX::ExecID(nextExecId()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_NEW),
            FIX::OrdStatus(FIX::OrdStatus_NEW),
            FIX::Symbol(symbol),
            FIX::Side(side),
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
            cerr << "SessionNotFound: `" << targetCompID << "'" << endl;
        }
        addOrder(Order(senderCompID, targetCompID, clOrdID, symbol, side, price, static_cast<long>(orderQty)));
    }

    void onMessage(const FIX42::OrderCancelRequest& req, const FIX::SessionID&)
    {
        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;

        FIX::ClOrdID clOrdID;
        FIX::OrigClOrdID origClOrdID;
        FIX::Symbol symbol;
        FIX::Side side;

        req.getHeader().get(senderCompID);
        req.getHeader().get(targetCompID);

        req.get(clOrdID);
        req.get(origClOrdID);
        req.get(symbol);
        req.get(side);

        map<string, OrderBook>::iterator p = _symbols.find(symbol);
        if(p != _symbols.end())
        {
            Order order;
            if(p->second.cancel(origClOrdID, side, order))
            {
                FIX42::ExecutionReport report(
                    FIX::OrderID(order.clOrdID),
                    FIX::ExecID(nextExecId()),
                    FIX::ExecTransType(FIX::ExecTransType_NEW),
                    FIX::ExecType(FIX::OrdStatus_CANCELED),
                    FIX::OrdStatus(FIX::OrdStatus_CANCELED),
                    FIX::Symbol(order.symbol),
                    FIX::Side(order.side),
                    FIX::LeavesQty(order.openQuantity),
                    FIX::CumQty(order.executedQuantity),
                    FIX::AvgPx(order.avgExecutedPrice));

                report.set(clOrdID);
                report.set(origClOrdID);
                report.set(FIX::OrderQty(order.orderQty));

                report.set(FIX::LastShares(order.lastExecutedQuantity));
                report.set(FIX::LastPx(order.lastExecutedPrice));

                try
                {
                    FIX::Session::sendToTarget(report, order.targetCompID, order.senderCompID);
                }
                catch(const FIX::SessionNotFound&)
                {
                    cerr << "SessionNotFound: `" << order.targetCompID << "'" << endl;
                }
                return;
            }
        }

        FIX42::OrderCancelReject report = FIX42::OrderCancelReject(
            FIX::OrderID("NONE"),
            FIX::ClOrdID(clOrdID),
            FIX::OrigClOrdID(origClOrdID),
            FIX::OrdStatus(FIX::OrdStatus_REJECTED),
            FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST));
        try
        {
            FIX::Session::sendToTarget(report, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            cerr << "SessionNotFound: `" << targetCompID << "'" << endl;
        }
    }

    void onMessage(const FIX42::OrderCancelReplaceRequest& req, const FIX::SessionID&)
    {
        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;

        FIX::ClOrdID clOrdID;
        FIX::OrigClOrdID origClOrdID;
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::Price price;
        FIX::OrderQty orderQty;

        req.getHeader().get(senderCompID);
        req.getHeader().get(targetCompID);

        req.get(clOrdID);
        req.get(origClOrdID);
        req.get(symbol);
        req.get(side);
        req.get(price);
        req.get(orderQty);

        map<string, OrderBook>::iterator p = _symbols.find(symbol);
        if(p != _symbols.end())
        {
            Order order;
            if(p->second.cancel(origClOrdID, side, order))
            {
                FIX42::ExecutionReport pendingReplace(
                    FIX::OrderID(order.clOrdID),
                    FIX::ExecID(nextExecId()),
                    FIX::ExecTransType(FIX::ExecTransType_NEW),
                    FIX::ExecType(FIX::ExecType_PENDING_REPLACE),
                    FIX::OrdStatus(FIX::OrdStatus_PENDING_REPLACE),
                    FIX::Symbol(order.symbol),
                    FIX::Side(order.side),
                    FIX::LeavesQty(order.openQuantity),
                    FIX::CumQty(order.executedQuantity),
                    FIX::AvgPx(order.avgExecutedPrice));

                pendingReplace.set(clOrdID);
                pendingReplace.set(origClOrdID);
                pendingReplace.set(FIX::OrderQty(order.orderQty));

                pendingReplace.set(FIX::LastShares(order.lastExecutedQuantity));
                pendingReplace.set(FIX::LastPx(order.lastExecutedPrice));

                try
                {
                    FIX::Session::sendToTarget(pendingReplace, targetCompID, senderCompID);
                }
                catch(const FIX::SessionNotFound&)
                {
                    cerr << "SessionNotFound: `" << targetCompID << "'" << endl;
                }

                FIX42::ExecutionReport report(
                    FIX::OrderID(clOrdID),
                    FIX::ExecID(nextExecId()),
                    FIX::ExecTransType(FIX::ExecTransType_NEW),
                    FIX::ExecType(FIX::ExecType_REPLACE),
                    FIX::OrdStatus(FIX::OrdStatus_NEW),
                    FIX::Symbol(symbol),
                    FIX::Side(side),
                    FIX::LeavesQty(orderQty),
                    FIX::CumQty(0),
                    FIX::AvgPx(0));

                report.set(origClOrdID);
                report.set(clOrdID);
                report.set(FIX::OrderQty(orderQty));

                try
                {
                    FIX::Session::sendToTarget(report, targetCompID, senderCompID);
                }
                catch(const FIX::SessionNotFound&)
                {
                    cerr << "SessionNotFound: `" << targetCompID << "'" << endl;
                }

                addOrder(Order(senderCompID, targetCompID, clOrdID, symbol, side, price, static_cast<long>(orderQty)));
            }
            return;
        }

        FIX42::OrderCancelReject report = FIX42::OrderCancelReject(
            FIX::OrderID("NONE"),
            FIX::ClOrdID(clOrdID),
            FIX::OrigClOrdID(origClOrdID),
            FIX::OrdStatus(FIX::OrdStatus_REJECTED),
            FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST));
        try
        {
            FIX::Session::sendToTarget(report, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            cerr << "SessionNotFound: `" << targetCompID << "'" << endl;
        }
    }

    void onMessage(const FIX42::OrderStatusRequest& req, const FIX::SessionID&)
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

        map<string, OrderBook>::const_iterator p = _symbols.find(symbol);
        if(p != _symbols.end())
        {
            Order order;
            if(p->second.find(clOrdID, side, order))
            {
                FIX42::ExecutionReport report(
                    FIX::OrderID(clOrdID),
                    FIX::ExecID("0"), // No exec id for order status.
                    FIX::ExecTransType(FIX::ExecTransType_NEW),
                    FIX::ExecType(FIX::ExecType_ORDER_STATUS),
                    FIX::OrdStatus(FIX::OrdStatus_NEW),
                    FIX::Symbol(symbol),
                    FIX::Side(side),
                    FIX::LeavesQty(order.openQuantity),
                    FIX::CumQty(order.executedQuantity),
                    FIX::AvgPx(order.avgExecutedPrice));

                report.set(FIX::ClOrdID(order.clOrdID));
                report.set(FIX::OrderQty(order.orderQty));

                report.set(FIX::LastShares(order.lastExecutedQuantity));
                report.set(FIX::LastPx(order.lastExecutedPrice));

                try
                {
                    FIX::Session::sendToTarget(report, targetCompID, senderCompID);
                }
                catch(const FIX::SessionNotFound&)
                {
                    cerr << "SessionNotFound: `" << targetCompID << "'" << endl;
                }
                return;
            }
        }

        FIX42::ExecutionReport report(
            FIX::OrderID(clOrdID),
            FIX::ExecID("0"), // No exec id for order status.
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_ORDER_STATUS),
            FIX::OrdStatus(FIX::OrdStatus_REJECTED),
            FIX::Symbol(symbol),
            FIX::Side(side),
            FIX::LeavesQty(0),
            FIX::CumQty(0),
            FIX::AvgPx(0));

        report.set(FIX::ClOrdID(clOrdID));
        report.set(FIX::OrdRejReason(FIX::OrdRejReason_UNKNOWN_ORDER));

        try
        {
            FIX::Session::sendToTarget(report, targetCompID, senderCompID);
        }
        catch(const FIX::SessionNotFound&)
        {
            cerr << "SessionNotFound: `" << targetCompID << "'" << endl;
        }
    }
            
private:

    void
    addOrder(const Order& order)
    {
        map<string, OrderBook>::iterator p = _symbols.find(order.symbol);
        if(p == _symbols.end())
        {
            pair<map<string, OrderBook>::iterator, bool> i =
                _symbols.insert(make_pair(order.symbol, OrderBook(order.symbol)));
            assert(i.second);
            p = i.first;
        }

        p->second.add(order);

        if(_autoMatch)
        {
            match(p->second);
        }
    }

    void
    match(OrderBook& orderBook)
    {
        vector<Order> orders;
        orderBook.match(orders);
        for(vector<Order>::const_iterator q = orders.begin(); q != orders.end(); ++q)
        {
            FIX::OrdStatus status;
            if(q->openQuantity > 0)
            {
                status = FIX::OrdStatus_PARTIALLY_FILLED;
            }
            else
            {
                status = FIX::OrdStatus_FILLED;
            }

            FIX42::ExecutionReport report = FIX42::ExecutionReport(
                FIX::OrderID(q->clOrdID),
                FIX::ExecID(nextExecId()),
                FIX::ExecTransType(FIX::ExecTransType_NEW),
                FIX::ExecType(FIX::ExecType_TRADE),
                FIX::OrdStatus(status),
                FIX::Symbol(q->symbol),
                FIX::Side(q->side),
                FIX::LeavesQty(q->openQuantity),
                FIX::CumQty(q->executedQuantity),
                FIX::AvgPx(q->avgExecutedPrice));

            report.set(FIX::ClOrdID(q->clOrdID));
            report.set(FIX::OrderQty(q->orderQty));

            report.set(FIX::LastShares(q->lastExecutedQuantity));
            report.set(FIX::LastPx(q->lastExecutedPrice));

            try
            {
                FIX::Session::sendToTarget(report, q->targetCompID, q->senderCompID);
            }
            catch(const FIX::SessionNotFound&)
            {
                cerr << "SessionNotFound: `" << q->targetCompID << "'" << endl;
            }
        }
    }
 
    string nextExecId()
    {
        ostringstream os;
        os << _nextExecId++;
        return os.str();
    }
    
    int _nextExecId;
    bool _autoMatch;
    map<string, OrderBook> _symbols;
};

class FSLog : public FIX::Log
{
public:

    FSLog() :
        _prefix("GLOBAL"),
        _trace(false)
    {
    }

    FSLog(const FIX::SessionID& id) :
        _prefix(id.toString()),
        _trace(false)
    {
    }
    
    virtual void clear() 
    {
    }

    virtual void onIncoming(const string& msg)
    {
        if(_trace)
        {
            cout << _prefix << ":" << msg << endl;
        }
    }

    virtual void onOutgoing(const string& msg)
    {
        if(_trace)
        {
            cout << _prefix << ":" << msg << endl;
        }
    }

    virtual void onEvent(const string& msg)
    {
        if(_trace)
        {
            cout << _prefix << ":" << msg << endl;
        }
    }

    void
    setTrace(bool f)
    {
        _trace = f;
    }

private:

    const string _prefix;
    bool _trace;
};

class FSLogFactory : public FIX::LogFactory
{
public:

    FSLogFactory()
    {
    }

    virtual FIX::Log* create()
    {
        FSLog* l = new FSLog();
        _logs.push_back(l);
        return l;
    }

    virtual FIX::Log* create(const FIX::SessionID& id)
    {
        FSLog* l = new FSLog(id);
        _logs.push_back(l);
        return l;
    }

    void
    setTrace(bool f)
    {
        for(vector<FSLog*>::const_iterator p = _logs.begin(); p != _logs.end(); ++p)
        {
            (*p)->setTrace(f);
        }
    }

    virtual void destroy(FIX::Log* l)
    {
        vector<FSLog*>::iterator p = find(_logs.begin(), _logs.end(), l);
        if(p != _logs.end())
        {
            _logs.erase(p);
        }
        delete l;
    }

private:

    vector<FSLog*> _logs;
};

void
menu()
{
    cout <<
        "usage:\n"
        "match        match all pending orders\n"
        "automatch    enable automatch\n"
        "noautomatch  disable automatch\n"
        "trace        enable FIX tracing\n"
        "notrace      disable FIX tracing\n"
        "list         list the order books\n"
        "exit         exit\n"
        "?            help\n";
}

int
main(int argc, char* argv[])
{
    string file = "config";
    if(argc > 1)
    {
        file = argv[1];
    }
    try
    {
        FIX::SessionSettings settings(file);

        FixApplication application;
        FIX::FileStoreFactory storeFactory(settings);
        FSLogFactory logFactory;
        FIX::ThreadedSocketAcceptor acceptor(application, storeFactory, settings, logFactory);

        acceptor.start();
        menu();
        while(true)
        {
            cout << "==> " << flush;
            string s;
            getline(cin, s);
            if(!cin.good())
            {
                break;
            }
            if(s == "list")
            {
               application.list();
            }
            else if(s == "match")
            {
               application.match();
            }
            else if(s == "automatch")
            {
               application.setAutoMatch(true);
            }
            else if(s == "noautomatch")
            {
               application.setAutoMatch(false);
            }
            else if(s == "trace")
            {
                logFactory.setTrace(true);
            }
            else if(s == "notrace")
            {
                logFactory.setTrace(false);
            }
            else if(s == "exit")
            {
                break;
            }
            else if(s == "?")
            {
                menu();
            }
            else
            {
                cout << "unknown command `" << s << "'" << endl;
                menu();
            }
        }
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
