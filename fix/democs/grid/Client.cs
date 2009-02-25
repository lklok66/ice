// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.IO;
using QuickFix;
using IceFIX;
using System.Collections.Generic;
using System.Reflection;

// The QuickFix.MessageCracker is not CLS compliant, so don't
// specify that.
//[assembly: CLSCompliant(true)]

[assembly: AssemblyTitle("IceFIXGridClient")]
[assembly: AssemblyDescription("Ice FIX demo grid client")]
[assembly: AssemblyCompany("ZeroC, Inc.")]

public class Client
{
    public class ReporterI : QuickFix42.MessageCracker, IceFIX.ReporterOperations_
    {
        public void message(String data, Ice.Current current)
        {
            Message message = new Message(data);
            String msgType = message.getHeader().getField(MsgType.FIELD);
            QuickFix.MessageFactory factory = new QuickFix42.MessageFactory();
            Message msg = factory.create(data, msgType);
            msg.setString(data);

            SessionID id = new SessionID();
            crack(msg, id);
        }

        public override void onMessage(QuickFix42.BusinessMessageReject reject, SessionID id) 
        {
            Console.WriteLine("BusinessMessageReject");
            try
            {
                RefSeqNum seqNum = new RefSeqNum();
                reject.get(seqNum);
                Console.WriteLine("\tRefSeqNum: " + seqNum);
            }
            catch(FieldNotFound)
            {
            }
        }

        public override void onMessage(QuickFix42.Reject reject, SessionID id) 
        {
            Console.WriteLine("Reject");
            try
            {
                RefSeqNum seqNum = new RefSeqNum();
                reject.get(seqNum);
                Console.WriteLine("\tRefSeqNum: " + seqNum);
            }
            catch(FieldNotFound)
            {
            }
        }

        public override void onMessage(QuickFix42.OrderCancelReject reject, SessionID id) 
        {
            OrigClOrdID origClOrdID = new OrigClOrdID();
            reject.get(origClOrdID);

            Console.WriteLine("OrderCancelReject");
            Console.WriteLine("OrigClOrdID: " + origClOrdID);
        }

        public override void onMessage(QuickFix42.ExecutionReport report, SessionID id)  
        {
            ClOrdID clOrdID = new
            ClOrdID();
            report.get(clOrdID);

            ExecType execType = new ExecType();
            report.get(execType);

            OrdStatus ordStatus = new OrdStatus();
            report.get(ordStatus);

            LeavesQty leavesQty = new LeavesQty();
            report.get(leavesQty);

            CumQty orderQty = new CumQty();
            report.get(orderQty);

            Symbol symbol = new Symbol();
            report.get(symbol);

            Side side = new Side();
            report.get(side);

            CumQty cumQty = new CumQty();
            report.get(cumQty);

            AvgPx avgPx = new AvgPx();
            report.get(avgPx);

            Console.WriteLine("ExecutionReport");
            Console.WriteLine("\tClOrdID: " + clOrdID);

            Console.Write("\tExecType: ");

            switch(execType.getValue())
            {
            case ExecType.NEW: Console.Write("new"); break;
            case ExecType.PARTIAL_FILL: Console.Write("partial fill"); break;
            case ExecType.FILL: Console.Write("fill"); break;
            case ExecType.DONE_FOR_DAY: Console.Write("done for day"); break;
            case ExecType.CANCELED: Console.Write("canceled"); break;
            case ExecType.REPLACE: Console.Write("replace"); break;
            case ExecType.PENDING_CANCEL: Console.Write("pending cancel"); break;
            case ExecType.STOPPED: Console.Write("stopped"); break;
            case ExecType.REJECTED: Console.Write("rejected"); break;
            case ExecType.SUSPENDED: Console.Write("suspended"); break;
            case ExecType.PENDING_NEW: Console.Write("pending new"); break;
            case ExecType.CALCULATED: Console.Write("calculated"); break;
            case ExecType.EXPIRED: Console.Write("expired"); break;
            case ExecType.RESTATED: Console.Write("restated"); break;
            case ExecType.PENDING_REPLACE: Console.Write("pending replace"); break;
            case ExecType.TRADE: Console.Write("trade"); break;
            case ExecType.TRADE_CORRECT: Console.Write("trade correct"); break;
            case ExecType.TRADE_CANCEL: Console.Write("trade cancel"); break;
            case ExecType.ORDER_STATUS: Console.Write("order status"); break;
            }
            Console.WriteLine("");

            Console.Write("\tOrdStatus: ");
            switch(ordStatus.getValue())
            {
            case OrdStatus.NEW: Console.Write("new"); break;
            case OrdStatus.PARTIALLY_FILLED: Console.Write("partially filled"); break;
            case OrdStatus.FILLED: Console.Write("filled"); break;
            case OrdStatus.DONE_FOR_DAY: Console.Write("done for day"); break;
            case OrdStatus.CANCELED: Console.Write("canceled"); break;
            case OrdStatus.REPLACED: Console.Write("replaced"); break;
            case OrdStatus.PENDING_CANCEL: Console.Write("pending cancel"); break;
            case OrdStatus.STOPPED: Console.Write("stopped"); break;
            case OrdStatus.REJECTED: Console.Write("rejected"); break;
            case OrdStatus.SUSPENDED: Console.Write("suspended"); break;
            case OrdStatus.PENDING_NEW: Console.Write("pending new"); break;
            case OrdStatus.CALCULATED: Console.Write("calculated"); break;
            case OrdStatus.EXPIRED: Console.Write("expired"); break;
            case OrdStatus.ACCEPTED_FOR_BIDDING: Console.Write("accepted for bidding"); break;
            case OrdStatus.PENDING_REPLACE: Console.Write("pending replace"); break;
            }
            Console.WriteLine("");

            Console.WriteLine("\tSymbol: " + symbol);
            Console.WriteLine("\tSide: " + ((side.getValue() == Side.BUY) ? "Buy" : "Sell"));
            Console.WriteLine("\tLeavesQty: " + leavesQty);
            Console.WriteLine("\tCumQty: " + cumQty);
            Console.WriteLine("\tAvgPx: " + avgPx);
        }
    };

    private class ClOrdIDGenerator
    {
        public ClOrdIDGenerator(String name)
        {
            _name = name;
            _dbname = name + "-clordid";
            _file = new FileStream(_dbname, FileMode.OpenOrCreate);
	    try
	    {
		StreamReader reader = new StreamReader(_file);
		String s = reader.ReadLine();
		_nextId = Convert.ToInt32(s);
	    }
	    catch(ArgumentException)
	    {
		_nextId = 0;
	    }
	    catch(IOException)
	    {
		_nextId = 0;
	    }
        }

	public String next()
	{
	    String clOrdID = _name + "-"+ _nextId;
	    ++_nextId;

	    _file.Seek(0, SeekOrigin.Begin);
	    StreamWriter writer = new StreamWriter(_file);
	    writer.WriteLine(_nextId);
	    writer.Flush();

	    return clOrdID;
	}

	private String _name;
	private String _dbname;
	private int _nextId;
	private FileStream _file;
    };

    public class IceFIXClient : Ice.Application
    {
        private static void menu()
        {
            Console.Write(
                "usage:\n" +
                "buy symbol price quantity                   submit buy order\n" +
                "sell symbol price quantity                  submit sell order\n" +
                "cancel id sell|buy symbol                   order cancel\n" +
                "replace id sell|buy symbol price quantity   order cancel replace\n" +
                "status id sell|buy symbol                   status inquiry\n" +
                "bridge name                                 switch bridge\n" +
                "exit                                        exit\n" +
                "?: help\n");
        }

        private static void usage()
        {
            Console.WriteLine("Usage: " + appName() + " [--id id]");
        }

        public IceFIXClient() :
            base(Ice.SignalPolicy.NoSignalHandling)
        {
        }

        public override int run(string[] args)
        {
	    Ice.Properties properties = communicator().getProperties();
	    String id = properties.getPropertyWithDefault("ClientId", "test");

            int i;
            for(i = 0; i < args.Length; ++i)
            {
                string opt = args[i];
                if(opt == "--id")
                {
                    ++i;
                    if(i >= args.Length)
                    {
                        usage();
                        return 1;
                    }
                    id = args[i];
                }
                else
                {
                    usage();
                    return 1;
                }
            }

	    ClOrdIDGenerator gen = new ClOrdIDGenerator(id);

            Ice.LocatorPrx locator = communicator().getDefaultLocator();
            if(locator == null)
            {
                Console.Error.WriteLine(appName() + ": no locator configured");
		return 1;
            }

            Dictionary<string, IceFIX.BridgePrx> bridges = new Dictionary<string, IceFIX.BridgePrx>();

            IceGrid.LocatorPrx loc = IceGrid.LocatorPrxHelper.uncheckedCast(locator);
            IceGrid.QueryPrx query = loc.getLocalQuery();
            Ice.ObjectPrx[] a = query.findAllObjectsByType(IceFIX.BridgeDisp_.ice_staticId());
            for(i = 0; i < a.Length; ++i)
            {
                bridges[a[i].ice_getIdentity().category] = IceFIX.BridgePrxHelper.uncheckedCast(a[i]);
            }
            if(bridges.Count == 0)
            {
                Console.Error.WriteLine(appName() + ": cannot locate any bridges or admins");
                return 1;
            }

            Dictionary<string, IceFIX.ExecutorPrx> executors = new Dictionary<string, IceFIX.ExecutorPrx>();
	    Ice.ObjectAdapter adapter = communicator().createObjectAdapter("Client");
	    IceFIX.ReporterPrx reporter = IceFIX.ReporterPrxHelper.uncheckedCast(adapter.addWithUUID(new ReporterTie_(new ReporterI())));
            foreach(KeyValuePair<string, IceFIX.BridgePrx> p in bridges)
            {
                Console.Write("connecting with `" + p.Key + "'...");
                Console.Out.Flush();
	        try
	        {
                    IceFIX.ExecutorPrx e = p.Value.connect(id, reporter);
                    executors[p.Key] = e;
	        }
	        catch(IceFIX.RegistrationException)
	        {
	    	    try
		    {
                        Console.Write(" not registered, registering...");
                        Console.Out.Flush();
                        IceFIX.BridgeAdminPrx admin = p.Value.getAdmin();
                        Dictionary<string, string> qos = new Dictionary<string, string>();
		        admin.registerWithId(id, qos);

                        IceFIX.ExecutorPrx e = p.Value.connect(id, reporter);
                        executors[p.Key] = e;
		    }
		    catch(IceFIX.RegistrationException ex)
		    {
		        Console.Error.WriteLine(appName() + ": registration with  `" + p.Key + "' failed: `" +
                                                ex.reason + "'");
		        return 1;
		    }
	        }
                Console.WriteLine(" ok");
            }
	    adapter.activate();

            menu();

            string bridge = null;
            IceFIX.ExecutorPrx executor = null;
            foreach(KeyValuePair<string, IceFIX.ExecutorPrx> p in executors)
            {
                bridge = p.Key;
                executor = p.Value;
                break;
            }

            string line = null;
            while(true)
            {
                try
                {
                    Console.Out.Write(bridge + " ==> ");
                    Console.Out.Flush();
                    line = Console.In.ReadLine();
                    if(line == null)
                    {
                        break;
                    }
                    String[] tok = line.Split(new Char[]{' ', '\t'});
                    if(tok.Length == 0)
                    {
                        menu();
                        continue;
                    }
                    if(tok[0].Equals("buy") || tok[0].Equals("sell"))
                    {
                        if(tok.Length != 4)
                        {
                            menu();
                            continue;
                        }

                        char side = tok[0].Equals("buy") ? Side.BUY : Side.SELL;
                        String symbol = tok[1];
                        double price = Convert.ToDouble(tok[2]);
                        long quantity = Convert.ToInt64(tok[3]);
                        String clOrdID = gen.next();

                        QuickFix42.NewOrderSingle req = new QuickFix42.NewOrderSingle(
                            new ClOrdID( clOrdID ),
                            new HandlInst('1'),
                            new Symbol(symbol),
                            new Side(side),
                            new TransactTime(),
                            new OrdType(OrdType.LIMIT));
                        
                        req.set(new Price(price));
                        req.set(new OrderQty(quantity));
                        req.set(new TimeInForce(TimeInForce.DAY));
                        
                        int seqNum = executor.execute(req.ToString());
                        Console.Out.WriteLine("submitted order: " + seqNum + " `" + clOrdID + "'");
                    }
                    else if(tok[0].Equals("cancel"))
                    {
                        if(tok.Length != 4)
                        {
                            menu();
                            continue;
                        }

                        String origClOrdID = tok[1];
                        char side = tok[2].Equals("buy") ? Side.BUY : Side.SELL;
                        String symbol = tok[3];
                        String clOrdID = gen.next();

                        QuickFix42.OrderCancelRequest req = new QuickFix42.OrderCancelRequest(
                            new OrigClOrdID(origClOrdID),
                            new ClOrdID(clOrdID),
                            new Symbol(symbol),
                            new Side(side),
                            new TransactTime());

                        int seqNum = executor.execute(req.ToString());
                        Console.Out.WriteLine("submitted cancel order: " + seqNum + " `" + clOrdID + "'");
                    }
                    else if(tok[0].Equals("replace"))
                    {
                        if(tok.Length != 6)
                        {
                            menu();
                            continue;
                        }

                        String origClOrdID = tok[1];
                        char side = tok[2].Equals("buy") ? Side.BUY : Side.SELL;
                        String symbol = tok[3];
                        double price = Convert.ToDouble(tok[4]);
                        long quantity = Convert.ToInt64(tok[5]);
                        String clOrdID = gen.next();

                        QuickFix42.OrderCancelReplaceRequest req = new QuickFix42.OrderCancelReplaceRequest(
                            new OrigClOrdID(origClOrdID),
                            new ClOrdID(clOrdID),
                            new HandlInst('1'),
                            new Symbol(symbol),
                            new Side(side),
                            new TransactTime(),
                            new OrdType( OrdType.LIMIT ));
                        req.set(new Price(price));
                        req.set(new OrderQty(quantity));
                        req.set(new TimeInForce(TimeInForce.DAY));
                        
                        int seqNum = executor.execute(req.ToString());
                        Console.Out.WriteLine("submitted cancel replace order: " + seqNum + " `" + clOrdID + "'");
                    }
                    else if(tok[0].Equals("status"))
                    {
                        if(tok.Length != 6)
                        {
                            menu();
                            continue;
                        }

                        String clOrdID = tok[1];
                        char side = tok[2].Equals("buy") ? Side.BUY : Side.SELL;
                        String symbol = tok[3];

                        QuickFix42.OrderStatusRequest req = new QuickFix42.OrderStatusRequest(
                            new ClOrdID(clOrdID),
                            new Symbol(symbol),
                            new Side(side));

                        int seqNum = executor.execute(req.ToString());
                        Console.Out.WriteLine("submitted order status: " + seqNum);
                    }
                    else if(tok[0].Equals("bridge"))
                    {
                        if(tok.Length != 2)
                        {
                            menu();
                            continue;
                        }

                        try
                        {
                            executor = executors[tok[1]];
                        }
                        catch(KeyNotFoundException)
                        {
                            Console.WriteLine("cannot locate");
                            continue;
                        }
                        bridge = tok[1];
                    }
                    else if(tok[0].Equals("exit"))
                    {
                        break;
                    }
                    else if(tok[0].Equals("?"))
                    {
                        menu();
                    }
                    else
                    {
                        Console.WriteLine("unknown command `" + line + "'");
                        menu();
                    }
                }
                catch(IceFIX.ExecuteException e)
                {
                    Console.Error.WriteLine("ExecuteException: " + e.reason);
                }
                catch(System.Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
            }

            foreach(KeyValuePair<string, IceFIX.ExecutorPrx> p in executors)
            {
	        try
	        {
	    	    p.Value.destroy();
	        }
	        catch(Ice.Exception ex)
	        {
		    Console.Error.WriteLine("error when destroying excecutor `" + p.Key + "': " + ex);
	        }
            }
            
            return 0;
        }
    }

    public static void Main(string[] args)
    {
        IceFIXClient app = new IceFIXClient();
        int status = app.main(args, "config.client");
        if(status != 0)
        {
            System.Environment.Exit(status);
        }
    }
}
