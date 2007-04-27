// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;

public class AllTests
{
    private static void test(bool b)
    {
        if (!b)
        {
            throw new Exception();
        }
    }
    
    public static Test.MyClassPrx allTests(Ice.Communicator communicator, bool collocated)
    {
        Console.Out.Flush();
        string rf = "test:default -p 12010 -t 10000";
        Ice.ObjectPrx baseProxy = communicator.stringToProxy(rf);
        test(baseProxy != null);
        Test.MyClassPrx cl = Test.MyClassPrxHelper.checkedCast(baseProxy);
        Test.MyDerivedClassPrx derived = Test.MyDerivedClassPrxHelper.checkedCast(cl);
        
        Console.Out.Write("testing twoway operations... ");
        Console.Out.Flush();
        Twoways.twoways(communicator, cl);
        Twoways.twoways(communicator, derived);
        derived.opDerived();
        Console.Out.WriteLine("ok");
        
        if(!collocated)
        {
            Console.Out.Write("testing twoway operations with AMI... ");
            Console.Out.Flush();
            TwowaysAMI.twowaysAMI(communicator, cl);
            TwowaysAMI.twowaysAMI(communicator, derived);
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing batch oneway operations... ");
            Console.Out.Flush();
            BatchOneways.batchOneways(cl);
            BatchOneways.batchOneways(derived);
            Console.Out.WriteLine("ok");
        }
        
        return cl;
    }
}
