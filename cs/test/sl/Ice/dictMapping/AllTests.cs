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

namespace dictMapping
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

        override
        public void run(Ice.Communicator communicator)
        {
            string rf = "test:default -p 12010";
            Ice.ObjectPrx baseProxy = communicator.stringToProxy(rf);
            Test.MyClassPrx cl = Test.MyClassPrxHelper.checkedCast(baseProxy);

            Write("testing twoway operations... ");
            Twoways.twoways(communicator, cl);
            WriteLine("ok");

            Write("testing twoway operations with AMI... ");
            TwowaysAMI.twowaysAMI(communicator, cl);
            WriteLine("ok");

            Write("testing twoway operations with new AMI mapping... ");
            TwowaysNewAMI.twowaysAMI(communicator, cl);
            WriteLine("ok");

            cl.shutdown();
        }
    }
}
