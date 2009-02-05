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

    public class App : Ice.Application
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
                "x: exit\n" +
                "?: help\n");
        }

        public override int run(string[] args)
        {
	    Ice.Properties properties = communicator().getProperties();
	    String id = properties.getPropertyWithDefault("ClientId", "test");
	    String filtered = properties.getPropertyWithDefault("Filtered", "true");

            if(args.Length > 0)
            {
                Console.Error.WriteLine(appName() + ": too many arguments");
                return 1;
            }

	    IceFIX.BridgePrx bridge =
            IceFIX.BridgePrxHelper.uncheckedCast(communicator().propertyToProxy("Bridge"));
	    if(bridge == null)
	    {
                Console.Error.WriteLine(appName() + ": invalid proxy");
		return 1;
	    }

	    IceFIX.BridgeAdminPrx admin = IceFIX.BridgeAdminPrxHelper.uncheckedCast(
                communicator().propertyToProxy("BridgeAdmin"));
	    if(admin == null)
	    {
                Console.Error.WriteLine(appName() + ": invalid proxy");
		return 1;
	    }

	    Ice.ObjectAdapter adapter = communicator().createObjectAdapter("Client");
	    IceFIX.ReporterPrx reporter = IceFIX.ReporterPrxHelper.uncheckedCast(adapter.addWithUUID(new ReporterI()));
	    try
	    {
		bridge.connect(id, reporter, out _executor);
	    }
	    catch(IceFIX.RegistrationException)
	    {
		try
		{
                    Dictionary<string, string> qos = new Dictionary<string, string>();
		    qos["filtered"] = filtered;
		    admin.registerWithId(id, qos);
		    bridge.connect(id, reporter, out _executor);
		}
		catch(IceFIX.RegistrationException ex)
		{
		    Console.Error.WriteLine(appName() + ": registration failed: `" + ex.reason + "'");
		    return 1;
		}
	    }
	    adapter.activate();

            menu();

            string line = null;
            do 
            {
                try
                {
                    Console.Out.Write("==> ");
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
                        
                        if(send(req))
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

                        if(send(req))
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
                        
                        if(send(req))
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

                        send(req);
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

	    try
	    {
		_executor.destroy();
	    }
	    catch(Ice.Exception ex)
	    {
		Console.Error.WriteLine("error when destroying excecutor: " + ex);
	    }
            
            return 0;
        }

        private bool send(Message msg)
        {
            try
            {
                _executor.execute(msg.ToString());
            }
            catch(IceFIX.ExecuteException e)
            {
                Console.Error.WriteLine("ExecuteException: " + e.reason);
                return false;
            }
            return true;
        }

        private IceFIX.ExecutorPrx _executor;
    }

    public static void Main(string[] args)
    {
        App app = new App();
        int status = app.main(args, "config.client");
        if(status != 0)
        {
            System.Environment.Exit(status);
        }
    }
}
