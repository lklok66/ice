// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.hold;

public class Server extends test.Util.Application
{
    public int run(String[] args)
    {
        Ice.Communicator communicator = communicator();
        java.util.Timer timer = new java.util.Timer();

        communicator.getProperties().setProperty("TestAdapter1.Endpoints", "default -p 12010 -t 10000:udp");
        communicator.getProperties().setProperty("TestAdapter1.ThreadPool.Size", "5");
        communicator.getProperties().setProperty("TestAdapter1.ThreadPool.SizeMax", "5");
        communicator.getProperties().setProperty("TestAdapter1.ThreadPool.SizeWarn", "0");
        communicator.getProperties().setProperty("TestAdapter1.ThreadPool.Serialize", "0");
        Ice.ObjectAdapter adapter1 = communicator.createObjectAdapter("TestAdapter1");
        adapter1.add(new HoldI(timer, adapter1), communicator.stringToIdentity("hold"));

        communicator.getProperties().setProperty("TestAdapter2.Endpoints", "default -p 12011 -t 10000:udp");
        communicator.getProperties().setProperty("TestAdapter2.ThreadPool.Size", "5");
        communicator.getProperties().setProperty("TestAdapter2.ThreadPool.SizeMax", "5");
        communicator.getProperties().setProperty("TestAdapter2.ThreadPool.SizeWarn", "0");
        communicator.getProperties().setProperty("TestAdapter2.ThreadPool.Serialize", "1");
        Ice.ObjectAdapter adapter2 = communicator.createObjectAdapter("TestAdapter2");
        adapter2.add(new HoldI(timer, adapter2), communicator.stringToIdentity("hold"));

        adapter1.activate();
        adapter2.activate();

        communicator.waitForShutdown();

        timer.cancel();

        return 0;
    }

    protected Ice.InitializationData getInitData(Ice.StringSeqHolder argsH)
    {
        Ice.InitializationData initData = new Ice.InitializationData();
        initData.properties = Ice.Util.createProperties(argsH);
        initData.properties.setProperty("Ice.Package.Test", "test.Ice.hold");
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
