// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
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
    public class App : Ice.Application
    {
        public override int run(string[] args)
        {
            args = communicator().getProperties().parseCommandLineOptions("TestAdapter", args);
            Ice.ObjectAdapter adapter = communicator().createObjectAdapter("TestAdapter");
            adapter.add(new TestI(), communicator().stringToIdentity("test"));
            shutdownOnInterrupt();
            try
            {
                adapter.activate();
            }
            catch(Ice.ObjectAdapterDeactivatedException)
            {
            }
            communicator().waitForShutdown();
            return 0;
        }
    }

    public static int Main(string[] args)
    {
#if !COMPACT
        Debug.Listeners.Add(new ConsoleTraceListener());
#endif

        App server = new App();
        return server.main(args);
    }
}
