// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

public class AllTests
{
    private static void
    test(boolean b)
    {
        if(!b)
        {
            throw new RuntimeException();
        }
    }

    public static Test.MyClassPrx
    allTests(Ice.Communicator communicator, Ice.InitializationData initData, boolean collocated)
    {
        System.out.print("testing stringToProxy... ");
        System.out.flush();
        String ref = "test:default -p 12010 -t 10000";
        Ice.ObjectPrx base = communicator.stringToProxy(ref);
        test(base != null);
        System.out.println("ok");

    	System.out.print("testing ice_getCommunicator... ");
	System.out.flush();
	test(base.ice_getCommunicator() == communicator);
	System.out.println("ok");

	System.out.print("testing proxy methods... ");
	System.out.flush();
	test(communicator.identityToString(
		 base.ice_identity(communicator.stringToIdentity("other")).ice_getIdentity()).equals("other"));
	test(base.ice_facet("facet").ice_getFacet().equals("facet"));
	test(base.ice_adapterId("id").ice_getAdapterId().equals("id"));
	test(base.ice_twoway().ice_isTwoway());
	test(base.ice_oneway().ice_isOneway());
	test(base.ice_batchOneway().ice_isBatchOneway());
	test(base.ice_datagram().ice_isDatagram());
	test(base.ice_batchDatagram().ice_isBatchDatagram());
	test(base.ice_secure(true).ice_isSecure());
	test(!base.ice_secure(false).ice_isSecure());
	test(base.ice_collocationOptimized(true).ice_isCollocationOptimized());
	test(!base.ice_collocationOptimized(false).ice_isCollocationOptimized());
	System.out.println("ok");

        System.out.print("testing checked cast... ");
        System.out.flush();
        Test.MyClassPrx cl = Test.MyClassPrxHelper.checkedCast(base);
        test(cl != null);
        Test.MyDerivedClassPrx derived = Test.MyDerivedClassPrxHelper.checkedCast(cl);
        test(derived != null);
        test(cl.equals(base));
        test(derived.equals(base));
        test(cl.equals(derived));
        System.out.println("ok");

	System.out.print("testing checked cast with context... ");
	System.out.flush();
	String cref = "context:default -p 12010 -t 10000";
	Ice.ObjectPrx cbase = communicator.stringToProxy(cref);
	test(cbase != null);

	Test.TestCheckedCastPrx tccp = Test.TestCheckedCastPrxHelper.checkedCast(cbase);
	java.util.Map c = tccp.getContext();
	test(c == null || c.size() == 0);

	c = new java.util.HashMap();
	c.put("one", "hello");
	c.put("two", "world");
	tccp = Test.TestCheckedCastPrxHelper.checkedCast(cbase, c);
	java.util.Map c2 = tccp.getContext();
	test(c.equals(c2));
	System.out.println("ok");

	if(!collocated)
	{
	    System.out.print("testing timeout... ");
	    System.out.flush();
	    try
	    {
		Test.MyClassPrx clTimeout = Test.MyClassPrxHelper.uncheckedCast(cl.ice_timeout(500));
		clTimeout.opSleep(1000);
		test(false);
	    }
	    catch(Ice.TimeoutException ex)
	    {
	    }
	    System.out.println("ok");
	}

        System.out.print("testing twoway operations... ");
        System.out.flush();
        Twoways.twoways(communicator, initData, cl);
        Twoways.twoways(communicator, initData, derived);
        derived.opDerived();
        System.out.println("ok");

	if(!collocated)
	{
	    System.out.print("testing twoway operations with AMI... ");
	    System.out.flush();
	    TwowaysAMI.twowaysAMI(communicator, initData, cl);
	    TwowaysAMI.twowaysAMI(communicator, initData, derived);
	    System.out.println("ok");

	    System.out.print("testing batch oneway operations... ");
	    System.out.flush();
	    BatchOneways.batchOneways(cl);
	    BatchOneways.batchOneways(derived);
	    System.out.println("ok");
	}

        return cl;
    }
}
