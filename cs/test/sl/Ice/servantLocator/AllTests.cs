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
using Ice;

namespace servantLocator
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

        public static void testExceptions(TestIntfPrx obj)
        {
            try
            {
                obj.requestFailedException();
                test(false);
            }
            catch (ObjectNotExistException ex)
            {
                test(ex.id.Equals(obj.ice_getIdentity()));
                test(ex.facet.Equals(obj.ice_getFacet()));
                test(ex.operation.Equals("requestFailedException"));
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.unknownUserException();
                test(false);
            }
            catch (UnknownUserException ex)
            {
                test(ex.unknown.Equals("reason"));
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.unknownLocalException();
                test(false);
            }
            catch (UnknownLocalException ex)
            {
                test(ex.unknown.Equals("reason"));
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.unknownException();
                test(false);
            }
            catch (UnknownException ex)
            {
                test(ex.unknown.Equals("reason"));
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.userException();
                test(false);
            }
            catch (UnknownUserException ex)
            {
                //Console.Error.WriteLine(ex.unknown);
                test(ex.unknown.IndexOf("Test::TestIntfUserException") >= 0);
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.localException();
                test(false);
            }
            catch (UnknownLocalException ex)
            {
                test(ex.unknown.IndexOf("Ice::SocketException") >= 0);
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.csException();
                test(false);
            }
            catch (UnknownException ex)
            {
                test(ex.unknown.IndexOf("System.Exception") >= 0);
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.impossibleException(false);
                test(false);
            }
            catch (UnknownUserException)
            {
                // Operation doesn't throw, but locate() and finished() throw TestIntfUserException.
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.impossibleException(true);
                test(false);
            }
            catch (UnknownUserException)
            {
                // Operation throws TestImpossibleException, but locate() and finished() throw TestIntfUserException.
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                obj.intfUserException(false);
                test(false);
            }
            catch (TestImpossibleException)
            {
                // Operation doesn't throw, but locate() and finished() throw TestImpossibleException.
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex);
                test(false);
            }

            try
            {
                obj.intfUserException(true);
                test(false);
            }
            catch (TestImpossibleException)
            {
                // Operation throws TestIntfUserException, but locate() and finished() throw TestImpossibleException.
            }
            catch (System.Exception)
            {
                test(false);
            }
        }

        public AllTests(TextBox output, Button btnRun)
            : base(output, btnRun)
        {
        }

        public override Ice.InitializationData initData()
        {
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.setProperty("Ice.FactoryAssemblies", "servantLocator,version=1.0.0.0");
            return initData;
        }

        override
        public void run(Ice.Communicator communicator)
        {
            Write("testing stringToProxy... ");
            string @ref = "asm:default -p 12010";
            Ice.ObjectPrx @base = communicator.stringToProxy(@ref);
            test(@base != null);
            WriteLine("ok");

            Write("testing checked cast... ");
            TestIntfPrx obj = TestIntfPrxHelper.checkedCast(@base);
            test(obj != null);
            test(obj.Equals(@base));
            WriteLine("ok");

            Write("testing ice_ids... ");
            try
            {
                Ice.ObjectPrx o = communicator.stringToProxy("category/locate:default -p 12010");
                o.ice_ids();
                test(false);
            }
            catch (UnknownUserException ex)
            {
                test(ex.unknown.Equals("Test::TestIntfUserException"));
            }
            catch (System.Exception)
            {
                test(false);
            }

            try
            {
                Ice.ObjectPrx o = communicator.stringToProxy("category/finished:default -p 12010");
                o.ice_ids();
                test(false);
            }
            catch (UnknownUserException ex)
            {
                test(ex.unknown.Equals("Test::TestIntfUserException"));
            }
            catch (System.Exception)
            {
                test(false);
            }
            WriteLine("ok");

            Write("testing servant locator...");
            @base = communicator.stringToProxy("category/locate:default -p 12010");
            obj = TestIntfPrxHelper.checkedCast(@base);
            try
            {
                TestIntfPrxHelper.checkedCast(communicator.stringToProxy("category/unknown:default -p 12010"));
            }
            catch (ObjectNotExistException)
            {
            }
            WriteLine("ok");

            Write("testing default servant locator...");
            @base = communicator.stringToProxy("anothercat/locate:default -p 12010");
            obj = TestIntfPrxHelper.checkedCast(@base);
            @base = communicator.stringToProxy("locate:default -p 12010");
            obj = TestIntfPrxHelper.checkedCast(@base);
            try
            {
                TestIntfPrxHelper.checkedCast(communicator.stringToProxy("anothercat/unknown:default -p 12010"));
            }
            catch (ObjectNotExistException)
            {
            }
            try
            {
                TestIntfPrxHelper.checkedCast(communicator.stringToProxy("unknown:default -p 12010"));
            }
            catch (ObjectNotExistException)
            {
            }
            WriteLine("ok");

            Write("testing locate exceptions... ");
            @base = communicator.stringToProxy("category/locate:default -p 12010");
            obj = TestIntfPrxHelper.checkedCast(@base);
            testExceptions(obj);
            WriteLine("ok");

            Write("testing finished exceptions... ");
            @base = communicator.stringToProxy("category/finished:default -p 12010");
            obj = TestIntfPrxHelper.checkedCast(@base);
            testExceptions(obj);

            //
            // Only call these for category/finished.
            //
            try
            {
                obj.asyncResponse();
            }
            catch (TestIntfUserException)
            {
                test(false);
            }
            catch (TestImpossibleException)
            {
                //
                // Called by finished().
                //
            }

            //
            // Only call these for category/finished.
            //
            try
            {
                obj.asyncException();
            }
            catch (TestIntfUserException)
            {
                test(false);
            }
            catch (TestImpossibleException)
            {
                //
                // Called by finished().
                //
            }

            WriteLine("ok");

            Write("testing servant locator removal... ");
            @base = communicator.stringToProxy("test/activation:default -p 12010");
            TestActivationPrx activation = TestActivationPrxHelper.checkedCast(@base);
            activation.activateServantLocator(false);
            try
            {
                obj.ice_ping();
                test(false);
            }
            catch (ObjectNotExistException)
            {
                WriteLine("ok");
            }
            Write("testing servant locator addition... ");
            activation.activateServantLocator(true);
            try
            {
                obj.ice_ping();
                WriteLine("ok");
            }
            catch (System.Exception)
            {
                test(false);
            }
            obj.shutdown();
        }
    }
}
