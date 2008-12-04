// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.faultTolerance;

import java.io.PrintWriter;

public class Server extends test.Util.Application
{
    private static void
    usage()
    {
        System.err.println("Usage: Server port");
    }

    public int
    run(String[] args)
    {
    	Ice.Communicator communicator = communicator();
        int port = 0;
        PrintWriter out = getWriter();
		for(int i = 0; i < args.length; i++)
        {
            if(args[i].charAt(0) == '-')
            {
                out.println("Server: unknown option `" + args[i] + "'");
                usage();
                return 1;
            }

            if(port > 0)
            {
                out.println("Server: only one port can be specified");
                usage();
                return 1;
            }

            try
            {
                port = Integer.parseInt(args[i]);
            }
            catch(NumberFormatException ex)
            {
                out.println("Server: invalid port");
                usage();
                return 1;
            }
        }

        if(port <= 0)
        {
            out.println("Server: no port specified");
            usage();
            return 1;
        }

        communicator.getProperties().setProperty("TestAdapter.Endpoints", "default -p " + port + ":udp");
        Ice.ObjectAdapter adapter = communicator.createObjectAdapter("TestAdapter");
        Ice.Object object = new TestI(adapter, port);
        adapter.add(object, communicator.stringToIdentity("test"));
        adapter.activate();
        communicator.waitForShutdown();
        return 0;
    }

	protected Ice.InitializationData getInitData(Ice.StringSeqHolder argsH)
    {
        Ice.InitializationData initData = new Ice.InitializationData();
        initData.properties = Ice.Util.createProperties(argsH);
        initData.properties.setProperty("Ice.Package.Test", "test.Ice.faultTolerance");
        initData.properties.setProperty("Ice.ServerIdleTime", "120"); // Two
                                                                      // minutes.
        return initData;
    }

    public static void main(String[] args)
    {
        Server app = new Server();
        int result = app.main("Server", args);
        System.gc();
        System.exit(result);
    }
}
