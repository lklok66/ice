// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using Test;
using System;
using System.Diagnostics;
using System.Reflection;

[assembly: CLSCompliant(true)]

[assembly: AssemblyTitle("IceTest")]
[assembly: AssemblyDescription("Ice test")]
[assembly: AssemblyCompany("ZeroC, Inc.")]

public class Client
{
    private class MyObjectFactory : Ice.ObjectFactory
    {
        public Ice.Object create(string type)
        {
            if(type.Equals("::Test::B"))
            {
                return new BI();
            }
            else if(type.Equals("::Test::C"))
            {
                return new CI();
            }
            else if(type.Equals("::Test::D"))
            {
                return new DI();
            }
            else if(type.Equals("::Test::E"))
            {
                return new EI();
            }
            else if(type.Equals("::Test::F"))
            {
                return new FI();
            }
            else if(type.Equals("::Test::I"))
            {
                return new II();
            }
            else if(type.Equals("::Test::J"))
            {
                return new JI();
            }
            else if(type.Equals("::Test::H"))
            {
                return new HI();
            }
            Debug.Assert(false); // Should never be reached
            return null;
        }

        public void
        destroy()
        {
            // Nothing to do
        }
    }

    private static int run(string[] args, Ice.Communicator communicator)
    {
        Ice.ObjectFactory factory = new MyObjectFactory();
        communicator.addObjectFactory(factory, "::Test::B");
        communicator.addObjectFactory(factory, "::Test::C");
        communicator.addObjectFactory(factory, "::Test::D");
        communicator.addObjectFactory(factory, "::Test::E");
        communicator.addObjectFactory(factory, "::Test::F");
        communicator.addObjectFactory(factory, "::Test::I");
        communicator.addObjectFactory(factory, "::Test::J");
        communicator.addObjectFactory(factory, "::Test::H");

        InitialPrx initial = AllTests.allTests(communicator, false);
        initial.shutdown();
        return 0;
    }

    public static int Main(string[] args)
    {
        int status = 0;
        Ice.Communicator communicator = null;

#if !COMPACT
        Debug.Listeners.Add(new ConsoleTraceListener());
#endif

        try
        {
            Ice.InitializationData data = new Ice.InitializationData();
#if COMPACT
            //
            // When using Ice for .NET Compact Framework, we need to specify
            // the assembly so that Ice can locate classes and exceptions.
            //
            data.properties = Ice.Util.createProperties();
            data.properties.setProperty("Ice.FactoryAssemblies", "client");
#endif
            communicator = Ice.Util.initialize(ref args, data);
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

        return status;
    }
}
