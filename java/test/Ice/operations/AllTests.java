// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.operations;
import java.io.PrintWriter;

import test.Ice.operations.Test.MyClassPrx;
import test.Ice.operations.Test.MyClassPrxHelper;
import test.Ice.operations.Test.MyDerivedClassPrx;
import test.Ice.operations.Test.MyDerivedClassPrxHelper;

public class AllTests
{
    public static MyClassPrx
    allTests(Ice.Communicator communicator, boolean collocated, PrintWriter out)
    {
        String ref = "test:default -p 12010 -t 10000";
        Ice.ObjectPrx base = communicator.stringToProxy(ref);
        MyClassPrx cl = MyClassPrxHelper.checkedCast(base);
        MyDerivedClassPrx derived = MyDerivedClassPrxHelper.checkedCast(cl);

		out.print("testing twoway operations... ");
        out.flush();
        Twoways.twoways(communicator, cl);
        Twoways.twoways(communicator, derived);
        derived.opDerived();
        out.println("ok");

        out.print("testing oneway operations... ");
        out.flush();
        Oneways.oneways(communicator, cl);
        out.println("ok");

        if(!collocated)
        {
            out.print("testing twoway operations with AMI... ");
            out.flush();
            TwowaysAMI.twowaysAMI(communicator, cl);
            TwowaysAMI.twowaysAMI(communicator, derived);
            out.println("ok");

            out.print("testing oneway operations with AMI... ");
            out.flush();
            OnewaysAMI.onewaysAMI(communicator, cl);
            out.println("ok");

            out.print("testing batch oneway operations... ");
            out.flush();
            BatchOneways.batchOneways(cl, out);
            BatchOneways.batchOneways(derived, out);
            out.println("ok");
        }

        return cl;
    }
}
