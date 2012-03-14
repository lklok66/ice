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
using System.Diagnostics;
using Test;

namespace retry
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

        private class Callback
        {
            internal Callback()
            {
                _called = false;
            }

            public void check()
            {
                _m.Lock();
                try
                {
                    while (!_called)
                    {
                        _m.Wait();
                    }

                    _called = false;
                }
                finally
                {
                    _m.Unlock();
                }
            }

            public void called()
            {
                _m.Lock();
                try
                {
                    Debug.Assert(!_called);
                    _called = true;
                    _m.Notify();
                }
                finally
                {
                    _m.Unlock();
                }
            }

            private bool _called;
            private readonly IceUtilInternal.Monitor _m = new IceUtilInternal.Monitor();
        }

        private class AMIRegular
        {
            public void response()
            {
                callback.called();
            }

            public void exception(Ice.Exception ex)
            {
                test(false);
            }

            public void check()
            {
                callback.check();
            }

            private Callback callback = new Callback();
        }

        private class AMIException
        {
            public void response()
            {
                test(false);
            }

            public void exception(Ice.Exception ex)
            {
                test(ex is Ice.ConnectionLostException);
                callback.called();
            }

            public void check()
            {
                callback.check();
            }

            private Callback callback = new Callback();
        }

        public AllTests(TextBox output, Button btnRun)
            : base(output, btnRun)
        {
        }

        public override Ice.InitializationData initData()
        {
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            //
            // For this test, we want to disable retries.
            //
            initData.properties.setProperty("Ice.RetryIntervals", "-1");

            //
            // This test kills connections, so we don't want warnings.
            //
            initData.properties.setProperty("Ice.Warn.Connections", "0");
            return initData;
        }

        override
        public void run(Ice.Communicator communicator)
        {
            Write("testing stringToProxy... ");
            string rf = "retry:default -p 12010";
            Ice.ObjectPrx base1 = communicator.stringToProxy(rf);
            test(base1 != null);
            Ice.ObjectPrx base2 = communicator.stringToProxy(rf);
            test(base2 != null);
            WriteLine("ok");

            Write("testing checked cast... ");
            Test.RetryPrx retry1 = Test.RetryPrxHelper.checkedCast(base1);
            test(retry1 != null);
            test(retry1.Equals(base1));
            Test.RetryPrx retry2 = Test.RetryPrxHelper.checkedCast(base2);
            test(retry2 != null);
            test(retry2.Equals(base2));
            WriteLine("ok");

            Write("calling regular operation with first proxy... ");
            retry1.op(false);
            WriteLine("ok");

            Write("calling operation to kill connection with second proxy... ");
            try
            {
                retry2.op(true);
                test(false);
            }
            catch (Ice.ConnectionLostException)
            {
                WriteLine("ok");
            }

            Write("calling regular operation with first proxy again... ");
            retry1.op(false);
            WriteLine("ok");

            AMIRegular cb1 = new AMIRegular();
            AMIException cb2 = new AMIException();

            Write("calling regular AMI operation with first proxy... ");
            retry1.begin_op(false).whenCompleted(cb1.response, cb1.exception);
            cb1.check();
            WriteLine("ok");

            Write("calling AMI operation to kill connection with second proxy... ");
            retry2.begin_op(true).whenCompleted(cb2.response, cb2.exception);
            cb2.check();
            WriteLine("ok");

            Write("calling regular AMI operation with first proxy again... ");
            retry1.begin_op(false).whenCompleted(cb1.response, cb1.exception);
            cb1.check();
            WriteLine("ok");

            retry1.shutdown();
        }
    }
}
