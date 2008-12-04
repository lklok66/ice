// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.adapterDeactivation;

public class Client extends test.Util.Application
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
        AllTests.allTests(communicator(), getWriter());
        return 0;
    }

    public static void
    main(String[] args)
    {
    	Client app = new Client();
        int result = app.main("Client", args);
        System.gc();
        System.exit(result);
    }
}
