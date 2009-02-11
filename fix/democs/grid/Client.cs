// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using QuickFix;
using IceFIX;
using System.Collections.Generic;
using System.Reflection;

[assembly: CLSCompliant(true)]

[assembly: AssemblyTitle("IceFIXClient")]
[assembly: AssemblyDescription("Ice FIX demo client")]
[assembly: AssemblyCompany("ZeroC, Inc.")]

public class Client
{
    public class ReporterI : IceFIX.ReporterDisp_
    {
	public override void message(String data, Ice.Current current)
	{
	    Message message = new Message(data);
	    Console.Out.WriteLine("message: " + message.ToXML());
	}
    };

    public class IceFIXClient : Ice.Application
    {
        private static void menu()
        {
            Console.Write(
                "usage:\n" +
                "o: submit order\n" +
                "c: order cancel\n" +
                "r: submit cancel replace\n" +
                "t: status inquiry\n" +
                "b: submit bad order\n" +
                "s: switch bridges\n" +
                "x: exit\n" +
                "?: help\n");
        }

        private static void usage()
        {
            Console.WriteLine("Usage: " + appName() + " [--filtered true|false] [--id id]");
        }

        public IceFIXClient() :
            base(Ice.SignalPolicy.NoSignalHandling)
        {
        }

        public override int run(string[] args)
        {
	    Ice.Properties properties = communicator().getProperties();
	    String id = properties.getPropertyWithDefault("ClientId", "test");
	    String filtered = properties.getPropertyWithDefault("Filtered", "true");

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
                else if(opt == "--filtered")
                {
                    ++i;
                    if(i >= args.Length)
                    {
                        usage();
                        return 1;
                    }
                    filtered = args[i];
                }
                else
                {
                    usage();
                    return 1;
                }
            }

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

	    Ice.ObjectAdapter adapter = communicator().createObjectAdapter("Client");
	    IceFIX.ReporterPrx reporter = IceFIX.ReporterPrxHelper.uncheckedCast(adapter.addWithUUID(new ReporterI()));
            foreach(KeyValuePair<string, IceFIX.BridgePrx> p in bridges)
            {
                Console.Write("connecting with `" + p.Key + "'...");
                Console.Out.Flush();
	        try
	        {
                    IceFIX.ExecutorPrx executor;
                    p.Value.connect(id, reporter, out executor);
                    _executors[p.Key] = executor;
	        }
	        catch(IceFIX.RegistrationException)
	        {
	    	    try
		    {
                        Console.Write(" not registered, registering...");
                        Console.Out.Flush();
                        IceFIX.BridgeAdminPrx admin = p.Value.getAdmin();
                        Dictionary<string, string> qos = new Dictionary<string, string>();
		        qos["filtered"] = filtered;
		        admin.registerWithId(id, qos);

                        IceFIX.ExecutorPrx executor;
                        p.Value.connect(id, reporter, out executor);
                        _executors[p.Key] = executor;
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
            IceFIX.ExecutorPrx currentExecutor = null;
            foreach(KeyValuePair<string, IceFIX.ExecutorPrx> p in _executors)
            {
                bridge = p.Key;
                currentExecutor = p.Value;
                break;
            }

            string line = null;
            do 
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
                    if(line.Equals("o") || line.Equals("b"))
		    {
			String clOrdID = Ice.Util.generateUUID();
			QuickFix42.NewOrderSingle req = new QuickFix42.NewOrderSingle(
                            new ClOrdID( clOrdID ),
                            new HandlInst('1'),
                            new Symbol( "AAPL"),
                            new Side( Side.BUY ),
                            new TransactTime(),
                            new OrdType( OrdType.LIMIT ));
                        
                        req.set( new Price( 100.0) );
                        if(line.Equals("o"))
                        {
                            req.set( new OrderQty( 100 ));
                        }
                        req.set( new TimeInForce( TimeInForce.DAY ));
                        
                        if(send(currentExecutor, req))
                        {
                            Console.Out.WriteLine("submitted order: `" + clOrdID + "'");
                        }
		    }
		    else if(line.Equals("c"))
		    {
			Console.Out.Write("order id: ");
			Console.Out.Flush();
			String origClOrdID = Console.In.ReadLine();
			if(origClOrdID == null)
			{
			    continue;
			}
			if(origClOrdID.Length == 0)
			{
			    Console.Error.WriteLine("invalid");
			    continue;
			}
			String clOrdID = Ice.Util.generateUUID();
			QuickFix42.OrderCancelRequest req = new QuickFix42.OrderCancelRequest(
                            new OrigClOrdID( origClOrdID ),
                            new ClOrdID( clOrdID ),
                            new Symbol( "AAPL"),
                            new Side( Side.BUY ),
                            new TransactTime());

                        if(send(currentExecutor, req))
                        {
                            Console.Out.WriteLine("submitted cancel order: `" + clOrdID + "'");
                        }
		    }
		    else if(line.Equals("r"))
		    {
			Console.Out.Write("order id: ");
			Console.Out.Flush();
			String origClOrdID = Console.In.ReadLine();
			if(origClOrdID == null)
			{
			    continue;
			}
			if(origClOrdID.Length == 0)
			{
			    Console.Error.WriteLine("invalid");
			    continue;
			}

			String ClOrdID = Ice.Util.generateUUID();
			QuickFix42.OrderCancelReplaceRequest req = new QuickFix42.OrderCancelReplaceRequest(
			    new OrigClOrdID( origClOrdID ),
			    new ClOrdID( ClOrdID ),
			    new HandlInst('1'),
			    new Symbol( "AAPL"),
			    new Side( Side.BUY ),
			    new TransactTime(),
			    new OrdType( OrdType.LIMIT ));
                        req.set( new Price( 110.0) );
                        req.set( new OrderQty( 50 ));
                        req.set( new TimeInForce( TimeInForce.DAY ));
                        
                        if(send(currentExecutor, req))
                        {
                            Console.Out.WriteLine("submitted cancel replace order: `" + ClOrdID + "'");
                        }
		    }
		    else if(line.Equals("t"))
		    {
			Console.Out.Write("order id: ");
			Console.Out.Flush();
			String clOrdID = Console.In.ReadLine();
			if(clOrdID == null)
			{
			    continue;
			}
			if(clOrdID.Length == 0)
			{
			    Console.Error.WriteLine("invalid");
			    continue;
			}

			QuickFix42.OrderStatusRequest req = new QuickFix42.OrderStatusRequest(
                            new ClOrdID( clOrdID ),
                            new Symbol( "AAPL"),
                            new Side( Side.BUY ));

                        send(currentExecutor, req);
		    }
                    else if(line.Equals("s"))
                    {
                        Console.Write("bridge:");
                        string newid;
                        newid = Console.In.ReadLine();
                        if(line == null)
                        {
                            break;
                        }
                        if(line.Length == 0)
                        {
                            Console.WriteLine("invalid");
                            continue;
                        }
                        try
                        {
                            currentExecutor = _executors[newid];
                        }
                        catch(KeyNotFoundException)
                        {
                            Console.WriteLine("cannot locate");
                            continue;
                        }
                        bridge = newid;
                    }
                    else if(line.Equals("x"))
                    {
                        // Nothing to do
                    }
                    else if(line.Equals("?"))
                    {
                        menu();
                    }
                    else
                    {
                        Console.WriteLine("unknown command `" + line + "'");
                        menu();
                    }
                }
                catch(System.Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
            }
            while (!line.Equals("x"));

            foreach(KeyValuePair<string, IceFIX.ExecutorPrx> p in _executors)
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

        private bool send(IceFIX.ExecutorPrx executor, Message msg)
        {
            try
            {
                executor.execute(msg.ToString());
            }
            catch(IceFIX.ExecuteException e)
            {
                Console.Error.WriteLine("ExecuteException: " + e.reason);
                return false;
            }
            return true;
        }

        private Dictionary<string, IceFIX.ExecutorPrx> _executors = new Dictionary<string, IceFIX.ExecutorPrx>();
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
