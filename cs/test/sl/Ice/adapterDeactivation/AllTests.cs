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

namespace adapterDeactivation
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
            Write("testing stringToProxy... ");
            string @ref = "test:default -p 12010";
            Ice.ObjectPrx @base = communicator.stringToProxy(@ref);
            test(@base != null);
            WriteLine("ok");

            Write("testing checked cast... ");
            TestIntfPrx obj = TestIntfPrxHelper.checkedCast(@base);
            test(obj != null);
            test(obj.Equals(@base));
            WriteLine("ok");

            {
                Write("creating object adapter with endpotins throws... ");
                try
                {
                    Ice.ObjectAdapter adapter =
                        communicator.createObjectAdapterWithEndpoints("TransientTestAdapter", "default");
                    test(false);
                }
                catch (Ice.FeatureNotSupportedException)
                {
                }
                WriteLine("ok");
            }

            Write("creating/activating/deactivating object adapter in one operation... ");
            obj.transient();
            WriteLine("ok");

            Write("deactivating object adapter in the server... ");
            obj.deactivate();
            WriteLine("ok");

            Write("testing whether server is gone... ");
            try
            {
                obj.ice_ping();
                test(false);
            }
            catch (Ice.LocalException)
            {
                WriteLine("ok");
            }
        }
    }
}
