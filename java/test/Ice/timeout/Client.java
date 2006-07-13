// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

public class Client
{
    public static int
    run(String[] args, Ice.Communicator communicator, java.io.PrintStream out)
    {
	Test.TimeoutPrx timeout = AllTests.allTests(communicator, out);
	timeout.shutdown();
	return 0;
    }

    public static void
    main(String[] args)
    {
	int status = 0;
	Ice.Communicator communicator = null;

	try
	{
	    Ice.StringSeqHolder argsH = new Ice.StringSeqHolder(args);
	    Ice.InitializationData initData = new Ice.InitializationData();
	    initData.properties = Ice.Util.createProperties(argsH);

	    //
	    // For this test, we want to disable retries.
	    //
	    initData.properties.setProperty("Ice.RetryIntervals", "-1");

	    //
	    // This test kills connections, so we don't want warnings.
	    //
	    initData.properties.setProperty("Ice.Warn.Connections", "0");

	    //
	    // Check for AMI timeouts every second.
	    //
	    initData.properties.setProperty("Ice.MonitorConnections", "1");

	    communicator = Ice.Util.initialize(argsH, initData);
	    status = run(argsH.value, communicator, System.out);
	}
	catch(Ice.LocalException ex)
	{
	    ex.printStackTrace();
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
		ex.printStackTrace();
		status = 1;
	    }
	}

	System.gc();
	System.exit(status);
    }
}
