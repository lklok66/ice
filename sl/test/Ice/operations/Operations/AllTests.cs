// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Collections.Generic;

public class AllTests
{
    public static Test.MyClassPrx allTests(Ice.Communicator communicator)
    {
        Console.Out.Flush();
        string rf = "test:default -p 12010 -t 10000";
        Ice.ObjectPrx baseProxy = communicator.stringToProxy(rf);
        Test.MyClassPrx cl = Test.MyClassPrxHelper.checkedCast(baseProxy);
        Test.MyDerivedClassPrx derivedProxy = Test.MyDerivedClassPrxHelper.checkedCast(cl);
        
        Twoways.twoways(communicator, cl);
        Twoways.twoways(communicator, derivedProxy);
        derivedProxy.opDerived();
        
        TwowaysAMI.twowaysAMI(communicator, cl);
        TwowaysAMI.twowaysAMI(communicator, derivedProxy);
        
        return cl;
    }
}
