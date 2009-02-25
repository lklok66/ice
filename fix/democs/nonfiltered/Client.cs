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

[assembly: AssemblyTitle("IceFIXNonFilteredClient")]
[assembly: AssemblyDescription("Ice FIX demo nonfiltered client")]
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

	    IceFIX.BridgePrx bridge = IceFIX.BridgePrxHelper.uncheckedCast(
                communicator().propertyToProxy("Bridge"));
	    if(bridge == null)
	    {
                Console.Error.WriteLine(appName() + ": invalid proxy");
		return 1;
	    }

            IceFIX.ExecutorPrx executor;
	    Ice.ObjectAdapter adapter = communicator().createObjectAdapter("Client");
	    IceFIX.ReporterPrx reporter = IceFIX.ReporterPrxHelper.uncheckedCast(adapter.addWithUUID(new ReporterI()));
	    try
	    {
		executor = bridge.connect(id, reporter);
	    }
	    catch(IceFIX.RegistrationException)
	    {
		try
		{
                    IceFIX.BridgeAdminPrx admin = bridge.getAdmin();
                    Dictionary<string, string> qos = new Dictionary<string, string>();
		    qos["filtered"] = "false";
		    admin.registerWithId(id, qos);
                    executor = bridge.connect(id, reporter);
		}
		catch(IceFIX.RegistrationException ex)
		{
		    Console.Error.WriteLine(appName() + ": registration failed: `" + ex.reason + "'");
		    return 1;
		}
	    }
	    adapter.activate();

            communicator().waitForShutdown();

	    try
	    {
		executor.destroy();
	    }
	    catch(Ice.Exception ex)
	    {
		Console.Error.WriteLine("error when destroying excecutor: " + ex);
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
