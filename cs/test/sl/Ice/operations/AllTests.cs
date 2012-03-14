// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Test;

namespace operations
{
    public class AllTests : TestCommon.TestApp
    {
        private static void test(bool b)
        {
            if (!b)
            {
                throw new System.Exception();
            }
        }

        public AllTests(TextBox output, Button btnRun)
            : base(output, btnRun)
        {
        }

        public override Ice.InitializationData initData()
        {
            //
            // In this test, we need at least two threads in the
            // client side thread pool for nested AMI.
            //
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.setProperty("Ice.ThreadPool.Client.Size", "2");
            initData.properties.setProperty("Ice.ThreadPool.Client.SizeWarn", "0");

            //
            // We must set MessageSizeMax to an explicit values,
            // because we run tests to check whether
            // Ice.MemoryLimitException is raised as expected.
            //
            initData.properties.setProperty("Ice.MessageSizeMax", "100");
            return initData;
        }

        override
        public void run(Ice.Communicator communicator)
        {
            string rf = "test:default -p 12010";
            Ice.ObjectPrx baseProxy = communicator.stringToProxy(rf);
            Test.MyClassPrx cl = Test.MyClassPrxHelper.checkedCast(baseProxy);
            Test.MyDerivedClassPrx derivedProxy = Test.MyDerivedClassPrxHelper.checkedCast(cl);

            Write("testing twoway operations... ");
            Twoways.twoways(communicator, cl);
            Twoways.twoways(communicator, derivedProxy);
            derivedProxy.opDerived();
            WriteLine("ok");

            Write("testing oneway operations... ");
            Oneways.oneways(communicator, cl);
            WriteLine("ok");

            Write("testing twoway operations with AMI... ");
            TwowaysAMI.twowaysAMI(communicator, cl);
            TwowaysAMI.twowaysAMI(communicator, derivedProxy);
            WriteLine("ok");

            Write("testing twoway operations with new AMI mapping... ");
            TwowaysNewAMI.twowaysNewAMI(communicator, cl);
            TwowaysNewAMI.twowaysNewAMI(communicator, derivedProxy);
            WriteLine("ok");

            Write("testing oneway operations with AMI... ");
            OnewaysAMI.onewaysAMI(communicator, cl);
            WriteLine("ok");

            Write("testing oneway operations with new AMI mapping... ");
            OnewaysNewAMI.onewaysNewAMI(communicator, cl);
            WriteLine("ok");

            Write("testing batch oneway operations... ");
            BatchOneways.batchOneways(cl);
            BatchOneways.batchOneways(derivedProxy);
            WriteLine("ok");

            Write("testing server shutdown... ");
            cl.shutdown();
            try
            {
                cl.opVoid();
                test(false);
            }
            catch (Ice.LocalException)
            {
                WriteLine("ok");
            }
        }
    }
}
