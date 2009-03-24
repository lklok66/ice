// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
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
    internal class App : Ice.Application
    {
        public override int run(string[] args)
        {
            communicator().getProperties().setProperty("TestAdapter.Endpoints", "default -p 12010 -t 2000");
            communicator().getProperties().setProperty("Ice.Warn.Dispatch", "0");

            Ice.ObjectAdapter adapter = communicator().createObjectAdapter("TestAdapter");
            adapter.addServantLocator(new ServantLocatorI("category"), "category");
            adapter.addServantLocator(new ServantLocatorI(""), "");
            adapter.add(new TestI(), communicator().stringToIdentity("asm"));

            adapter.activate();
            adapter.waitForDeactivate();
            return 0;
        }
    }
    
    public static void Main(string[] args)
    {
        Debug.Listeners.Add(new ConsoleTraceListener());

        App app = new App();
        int result = app.main(args);
        if(result != 0)
        {
            System.Environment.Exit(result);
        }
    }
}
