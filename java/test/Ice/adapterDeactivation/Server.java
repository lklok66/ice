// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.adapterDeactivation;

public class Server extends test.Util.Application
{
	protected Ice.InitializationData getInitData(Ice.StringSeqHolder argsH)
	{
		Ice.InitializationData initData = new Ice.InitializationData();
        initData.properties = Ice.Util.createProperties(argsH);
        initData.properties.setProperty("Ice.Package.Test", "test.Ice.adapterDeactivation");
        return initData;
	}
	
    public int
    run(String[] args)
    {
        communicator().getProperties().setProperty("TestAdapter.Endpoints", "default -p 12010 -t 10000:udp");
        Ice.ObjectAdapter adapter = communicator().createObjectAdapter("TestAdapter");
        Ice.ServantLocator locator = new ServantLocatorI();

        adapter.addServantLocator(locator, "");
        adapter.activate();
        adapter.waitForDeactivate();
        return 0;
    }

    public static void
    main(String[] args)
    {
    	Server app = new Server();
        int result = app.main("Server", args);

        System.gc();
        System.exit(result);
    }
}
