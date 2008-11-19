// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Diagnostics;
using System.Reflection;

[assembly: CLSCompliant(true)]

[assembly: AssemblyTitle("IceTest")]
[assembly: AssemblyDescription("Ice test")]
[assembly: AssemblyCompany("ZeroC, Inc.")]

public class Server
{
    private static int run(string[] args, Ice.Communicator communicator)
    {
        Ice.Properties properties = communicator.getProperties();
        if(args.Length == 1 && args[0].Equals("1"))
        {
            properties.setProperty("TestAdapter.Endpoints", "udp -p 12010");
            Ice.ObjectAdapter adapter = communicator.createObjectAdapter("TestAdapter");
            adapter.add(new TestIntfI(), communicator.stringToIdentity("test"));
            adapter.activate();
        }

        string host;
        if(properties.getProperty("Ice.IPv6") == "1")
        {
            host = "\"ff01::1:1\"";
        }
        else
        {
            host = "239.255.1.1";
        }
        properties.setProperty("McastTestAdapter.Endpoints", "udp -h " + host + " -p 12020");
        Ice.ObjectAdapter mcastAdapter = communicator.createObjectAdapter("McastTestAdapter");
        mcastAdapter.add(new TestIntfI(), communicator.stringToIdentity("test"));
        mcastAdapter.activate();

        communicator.waitForShutdown();
        return 0;
    }

    public static void Main(string[] args)
    {
        int status = 0;
        Ice.Communicator communicator = null;

        Debug.Listeners.Add(new ConsoleTraceListener());

        try
        {
            communicator = Ice.Util.initialize(ref args);
            status = run(args, communicator);
        }
        catch(System.Exception ex)
        {
            System.Console.Error.WriteLine(ex);
            status = 1;
        }

        if(communicator != null)
        {
            try
            {
                communicator.destroy();
            }
            catch(Ice.LocalException ex)
            {
                System.Console.Error.WriteLine(ex);
                status = 1;
            }
        }

        if(status != 0)
        {
            System.Environment.Exit(status);
        }
    }
}
