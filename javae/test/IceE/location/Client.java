// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

public class Client
{
    public static int
    run(String[] args, Ice.Communicator communicator, Ice.InitializationData data, java.io.PrintStream out)
    {
        AllTests.allTests(communicator, out);
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
            initData.properties.setProperty("Ice.Default.Locator", "locator:default -p 12010");

            if(initData.properties.getPropertyAsInt("Ice.Blocking") > 0)
            {
                initData.properties.setProperty("Ice.RetryIntervals", "0 0");
                initData.properties.setProperty("Ice.Warn.Connections", "0");
            }

            communicator = Ice.Util.initialize(argsH, initData);
            status = run(argsH.value, communicator, initData, System.out);
        }
        catch(Exception ex)
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
            catch (Ice.LocalException ex)
            {
                ex.printStackTrace();
                status = 1;
            }
        }

        System.gc();
        System.exit(status);
    }
}
