// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.operations;

public class Server extends test.Util.Application
{
    public int run(String[] args)
    {
        communicator().getProperties().setProperty("TestAdapter.Endpoints", "default -p 12010:udp");
        Ice.ObjectAdapter adapter = communicator().createObjectAdapter("TestAdapter");
        adapter.add(new MyDerivedClassI(), communicator().stringToIdentity("test"));
        adapter.activate();

        return WAIT;
    }

    protected Ice.InitializationData getInitData(Ice.StringSeqHolder argsH)
    {
        Ice.InitializationData initData = new Ice.InitializationData();
        initData.properties = Ice.Util.createProperties(argsH);
        //
        // Its possible to have batch oneway requests dispatched
        // after the adapter is deactivated due to thread
        // scheduling so we supress this warning.
        //
        initData.properties.setProperty("Ice.Warn.Dispatch", "0");
        initData.properties.setProperty("Ice.Package.Test", "test.Ice.operations");
        initData.properties.setProperty("TestAdapter.Endpoints", "default -p 12010:udp");
        initData.properties.setProperty("TestAdapter.PublishedEndpoints", "default -p 12010 -h 127.0.0.1:udp -h 127.0.0.1");
        return initData;
    }

    public static void main(String[] args)
    {
        Server c = new Server();
        int status = c.main("Server", args);

        System.gc();
        System.exit(status);
    }
}
