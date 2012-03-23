// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Diagnostics;
using System.Threading;

#if SILVERLIGHT
using System.Windows.Controls;
#endif

public class AllTests : TestCommon.TestApp
{
    private static string testString = "This is a test string";

    private class Cookie
    {
        public string getString()
        {
            return testString;
        }
    }

    private class CallbackBase
    {
        internal CallbackBase()
        {
            _called = false;
        }

        public virtual void check()
        {
            _m.Lock();
            try
            {
                while(!_called)
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

        public virtual void called()
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

    private class Callback
    {
        public Callback(Ice.Communicator communicator, bool useCookie)
        {
            _communicator = communicator;
            _useCookie = useCookie;
        }

        public void opString(Ice.AsyncResult result)
        {
            string cmp = testString;
            if(_useCookie)
            {
                Cookie cookie = (Cookie)result.AsyncState;
                cmp = cookie.getString();
            }

            byte[] outParams;
            if(result.getProxy().end_ice_invoke(out outParams, result))
            {
                Ice.InputStream inS = Ice.Util.createInputStream(_communicator, outParams);
                string s = inS.readString();
                test(s.Equals(cmp));
                s = inS.readString();
                test(s.Equals(cmp));
                callback.called();
            }
            else
            {
                test(false);
            }
        }

        public void opStringNC(bool ok, byte[] outParams)
        {
            if(ok)
            {
                Ice.InputStream inS = Ice.Util.createInputStream(_communicator, outParams);
                string s = inS.readString();
                test(s.Equals(testString));
                s = inS.readString();
                test(s.Equals(testString));
                callback.called();
            }
            else
            {
                test(false);
            }
        }

        public void opException(Ice.AsyncResult result)
        {
            if(_useCookie)
            {
                Cookie cookie = (Cookie)result.AsyncState;
                test(cookie.getString().Equals(testString));
            }

            byte[] outParams;
            if(result.getProxy().end_ice_invoke(out outParams, result))
            {
                test(false);
            }
            else
            {
                Ice.InputStream inS = Ice.Util.createInputStream(_communicator, outParams);
                try
                {
                    inS.throwException();
                }
                catch(Test.MyException)
                {
                    callback.called();
                }
                catch(System.Exception)
                {
                    test(false);
                }
            }
        }

        public void opExceptionNC(bool ok, byte[] outParams)
        {
            if(ok)
            {
                test(false);
            }
            else
            {
                Ice.InputStream inS = Ice.Util.createInputStream(_communicator, outParams);
                try
                {
                    inS.throwException();
                }
                catch(Test.MyException)
                {
                    callback.called();
                }
                catch(System.Exception)
                {
                    test(false);
                }
            }
        }

        public virtual void check()
        {
            callback.check();
        }

        private Ice.Communicator _communicator;
        private bool _useCookie;

        private CallbackBase callback = new CallbackBase();
    }
#if SILVERLIGHT
    public override Ice.InitializationData initData()
    {
        Ice.InitializationData initData = new Ice.InitializationData();
        initData.properties = Ice.Util.createProperties();
        initData.properties.setProperty("Ice.FactoryAssemblies", "invoke,version=1.0.0.0");
        return initData;
    }

    override
    public void run(Ice.Communicator communicator)
#else
    public static Test.MyClassPrx allTests(Ice.Communicator communicator)
#endif
    {
        Ice.ObjectPrx baseProxy = communicator.stringToProxy("test:default -p 12010");
        Test.MyClassPrx cl = Test.MyClassPrxHelper.checkedCast(baseProxy);
        Test.MyClassPrx oneway = Test.MyClassPrxHelper.uncheckedCast(cl.ice_oneway());

        Write("testing ice_invoke... ");
        Flush();

        {
            byte[] inParams, outParams;
            if(!oneway.ice_invoke("opOneway", Ice.OperationMode.Normal, null, out outParams))
            {
                test(false);
            }

            Ice.OutputStream outS = Ice.Util.createOutputStream(communicator);
            outS.writeString(testString);
            inParams = outS.finished();

            if(cl.ice_invoke("opString", Ice.OperationMode.Normal, inParams, out outParams))
            {
                Ice.InputStream inS = Ice.Util.createInputStream(communicator, outParams);
                string s = inS.readString();
                test(s.Equals(testString));
                s = inS.readString();
                test(s.Equals(testString));
            }
            else
            {
                test(false);
            }
        }

        {
            byte[] outParams;
            if(cl.ice_invoke("opException", Ice.OperationMode.Normal, null, out outParams))
            {
                test(false);
            }
            else
            {
                Ice.InputStream inS = Ice.Util.createInputStream(communicator, outParams);
                try
                {
                    inS.throwException();
                }
                catch(Test.MyException)
                {
                }
                catch(System.Exception)
                {
                    test(false);
                }
            }
        }

        WriteLine("ok");

        Write("testing asynchronous ice_invoke... ");
        Flush();

        {
            byte[] inParams, outParams;
            Ice.AsyncResult result = oneway.begin_ice_invoke("opOneway", Ice.OperationMode.Normal, null);
            if(!oneway.end_ice_invoke(out outParams, result))
            {
                test(false);
            }

            Ice.OutputStream outS = Ice.Util.createOutputStream(communicator);
            outS.writeString(testString);
            inParams = outS.finished();

            // begin_ice_invoke with no callback
            result = cl.begin_ice_invoke("opString", Ice.OperationMode.Normal, inParams);
            if(cl.end_ice_invoke(out outParams, result))
            {
                Ice.InputStream inS = Ice.Util.createInputStream(communicator, outParams);
                string s = inS.readString();
                test(s.Equals(testString));
                s = inS.readString();
                test(s.Equals(testString));
            }
            else
            {
                test(false);
            }

            // begin_ice_invoke with Callback
            Callback cb = new Callback(communicator, false);
            cl.begin_ice_invoke("opString", Ice.OperationMode.Normal, inParams, cb.opString, null);
            cb.check();

            // begin_ice_invoke with Callback with cookie
            cb = new Callback(communicator, true);
            cl.begin_ice_invoke("opString", Ice.OperationMode.Normal, inParams, cb.opString, new Cookie());
            cb.check();

            // begin_ice_invoke with Callback_Object_ice_invoke
            cb = new Callback(communicator, true);
            cl.begin_ice_invoke("opString", Ice.OperationMode.Normal, inParams).whenCompleted(cb.opStringNC, null);
            cb.check();
        }

        {
            // begin_ice_invoke with no callback
            Ice.AsyncResult result = cl.begin_ice_invoke("opException", Ice.OperationMode.Normal, null);
            byte[] outParams;
            if(cl.end_ice_invoke(out outParams, result))
            {
                test(false);
            }
            else
            {
                Ice.InputStream inS = Ice.Util.createInputStream(communicator, outParams);
                try
                {
                    inS.throwException();
                }
                catch(Test.MyException)
                {
                }
                catch(System.Exception)
                {
                    test(false);
                }
            }

            // begin_ice_invoke with Callback
            Callback cb = new Callback(communicator, false);
            cl.begin_ice_invoke("opException", Ice.OperationMode.Normal, null, cb.opException, null);
            cb.check();

            // begin_ice_invoke with Callback with cookie
            cb = new Callback(communicator, true);
            cl.begin_ice_invoke("opException", Ice.OperationMode.Normal, null, cb.opException, new Cookie());
            cb.check();

            // begin_ice_invoke with Callback_Object_ice_invoke
            cb = new Callback(communicator, true);
            cl.begin_ice_invoke("opException", Ice.OperationMode.Normal, null).whenCompleted(cb.opExceptionNC, null);
            cb.check();
        }

        WriteLine("ok");

#if SILVERLIGHT
        cl.shutdown();
#else
        return cl;
#endif
    }
}
