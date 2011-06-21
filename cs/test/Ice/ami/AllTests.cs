// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using Test;

public class AllTests
{
    private static void test(bool b)
    {
        if(!b)
        {
            throw new System.Exception();
        }
    }

    private class Cookie
    {
        public Cookie(int i)
        {
            val = i;
        }

        public int val;
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

    private class AsyncCallback : CallbackBase
    {
        public AsyncCallback()
        {
        }

        public AsyncCallback(Cookie cookie)
        {
            _cookie = cookie;
        }

        public void
        isA(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            test(result.getProxy().end_ice_isA(result));
            called();
        }

        public void
        ping(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            result.getProxy().end_ice_ping(result);
            called();
        }

        public void
        id(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            test(result.getProxy().end_ice_id(result).Equals("::Test::TestIntf"));
            called();
        }

        public void
        ids(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            test(result.getProxy().end_ice_ids(result).Length == 2);
            called();
        }

        public void
        op(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            TestIntfPrxHelper.uncheckedCast(result.getProxy()).end_op(result);
            called();
        }

        public void
        opWithResult(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            test(TestIntfPrxHelper.uncheckedCast(result.getProxy()).end_opWithResult(result) == 15);
            called();
        }

        public void
        opWithUE(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            try
            {
                TestIntfPrxHelper.uncheckedCast(result.getProxy()).end_opWithUE(result);
                test(false);
            }
            catch(TestIntfException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

        public void
        isAEx(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            try
            {
                result.getProxy().end_ice_isA(result);
                test(false);
            }
            catch(Ice.NoEndpointException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

        public void
        pingEx(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            try
            {
                result.getProxy().end_ice_ping(result);
                test(false);
            }
            catch(Ice.NoEndpointException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

        public void
        idEx(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            try
            {
                result.getProxy().end_ice_id(result);
                test(false);
            }
            catch(Ice.NoEndpointException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

        public void
        idsEx(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            try
            {
                result.getProxy().end_ice_ids(result);
                test(false);
            }
            catch(Ice.NoEndpointException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

        public void
        opEx(Ice.AsyncResult result)
        {
            test(result.AsyncState == _cookie);
            try
            {
                TestIntfPrxHelper.uncheckedCast(result.getProxy()).end_op(result);
                test(false);
            }
            catch(Ice.NoEndpointException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

        private Cookie _cookie;
    }

    private class ResponseCallback : CallbackBase
    {
        public ResponseCallback()
        {
        }

        public void
        isA(bool r)
        {
            test(r);
            called();
        }

        public void
        ping()
        {
            called();
        }

        public void
        id(string id)
        {
            test(id.Equals("::Test::TestIntf"));
            called();
        }

        public void
        ids(string[] ids)
        {
            test(ids.Length == 2);
            called();
        }

        public void
        op()
        {
            called();
        }

        public void
        opWithResult(int r)
        {
            test(r == 15);
            called();
        }

        public void
        opWithUE(Ice.Exception e)
        {
            try
            {
                throw e;
            }
            catch(TestIntfException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

    }

    private class ExceptionCallback : CallbackBase
    {
        public ExceptionCallback()
        {
        }

        public void
        isA(bool r)
        {
            test(false);
        }

        public void
        ping()
        {
            test(false);
        }

        public void
        id(string id)
        {
            test(false);
        }

        public void
        ids(string[] ids)
        {
            test(false);
        }

        public void
        op()
        {
            test(false);
        }

        public void
        opWithUE(Ice.Exception e)
        {
            try
            {
                throw e;
            }
            catch(TestIntfException)
            {
                called();
            }
            catch(Ice.Exception)
            {
                test(false);
            }
        }

        public void
        ex(Ice.Exception ex)
        {
            test(ex is Ice.NoEndpointException);
            called();
        }

        public void
        noEx(Ice.Exception ex)
        {
            test(false);
        }
    }

    private class SentCallback : CallbackBase
    {
        public SentCallback()
        {
            _thread = Thread.CurrentThread;
        }

        public void
        isA(bool r)
        {
        }

        public void
        ping()
        {
        }

        public void
        id(string s)
        {
        }

        public void
        ids(string[] s)
        {
        }

        public void
        opAsync(Ice.AsyncResult r)
        {
        }

        public void
        op()
        {
        }

        public void
        ex(Ice.Exception ex)
        {
        }

        public void
        sentAsync(Ice.AsyncResult r)
        {
            test(r.sentSynchronously() && _thread == Thread.CurrentThread ||
                 !r.sentSynchronously() && _thread != Thread.CurrentThread);
            called();
        }

        public void
        sent(bool ss)
        {
            test(ss && _thread == Thread.CurrentThread || !ss && _thread != Thread.CurrentThread);

            called();
        }

        Thread _thread;
    }

    private class FlushCallback : CallbackBase
    {
        public FlushCallback()
        {
            _thread = Thread.CurrentThread;
        }

        public FlushCallback(Cookie cookie)
        {
            _thread = Thread.CurrentThread;
            _cookie = cookie;
        }

        public void
        completedAsync(Ice.AsyncResult r)
        {
            test(false);
        }

        public void
        exception(Ice.Exception ex)
        {
            test(false);
        }

        public void
        sentAsync(Ice.AsyncResult r)
        {
            test(r.AsyncState == _cookie);
            test(r.sentSynchronously() && _thread == Thread.CurrentThread ||
                 !r.sentSynchronously() && _thread != Thread.CurrentThread);
            called();
        }

        public void
        sent(bool sentSynchronously)
        {
            test(sentSynchronously && _thread == Thread.CurrentThread ||
                 !sentSynchronously && _thread != Thread.CurrentThread);
            called();
        }

        Cookie _cookie;
        Thread _thread;
    }

    private class FlushExCallback : CallbackBase
    {
        public FlushExCallback()
        {
        }

        public FlushExCallback(Cookie cookie)
        {
            _cookie = cookie;
        }

        public void
        completedAsync(Ice.AsyncResult r)
        {
            test(r.AsyncState == _cookie);
            try
            {
                if(r.getConnection() != null)
                {
                    r.getConnection().end_flushBatchRequests(r);
                }
                else
                {
                    r.getProxy().end_ice_flushBatchRequests(r);
                }
                test(false);
            }
            catch(Ice.LocalException)
            {
                called();
            }
        }

        public void
        exception(Ice.Exception ex)
        {
            called();
        }

        public void
        sentAsync(Ice.AsyncResult r)
        {
            test(false);
        }

        public void
        sent(bool sentSynchronously)
        {
            test(false);
        }

        Cookie _cookie;
    }

    enum ThrowType { LocalException, UserException, OtherException };

    private class Thrower : CallbackBase
    {
        public Thrower(ThrowType t)
        {
            _t = t;
        }

        public void
        opAsync(Ice.AsyncResult r)
        {
            called();
            throwEx();
        }

        public void
        op()
        {
            called();
            throwEx();
        }

        public void
        noOp()
        {
        }

        public void
        ex(Ice.Exception ex)
        {
            called();
            throwEx();
        }

        public void
        sent(bool ss)
        {
            called();
            throwEx();
        }

        private void
        throwEx()
        {
            switch(_t)
            {
            case ThrowType.LocalException:
            {
                throw new Ice.ObjectNotExistException();
            }
            case ThrowType.UserException:
            {
                throw new TestIntfException();
            }
            case ThrowType.OtherException:
            {
                throw new Exception();
            }
            default:
            {
                Debug.Assert(false);
                break;
            }
            }
        }

        ThrowType _t;
    }

    public static void allTests(Ice.Communicator communicator)
    {
        string sref = "test:default -p 12010";
        Ice.ObjectPrx obj = communicator.stringToProxy(sref);
        test(obj != null);

        Test.TestIntfPrx p = Test.TestIntfPrxHelper.uncheckedCast(obj);

        sref = "testController:tcp -p 12011";
        obj = communicator.stringToProxy(sref);
        test(obj != null);

        Test.TestIntfControllerPrx testController = Test.TestIntfControllerPrxHelper.uncheckedCast(obj);

        Console.Out.Write("testing begin/end invocation... ");
        Console.Out.Flush();
        {
            Ice.AsyncResult result;
            Dictionary<string, string> ctx = new Dictionary<string, string>();

            result = p.begin_ice_isA("::Test::TestIntf");
            test(p.end_ice_isA(result));
            result = p.begin_ice_isA("::Test::TestIntf", ctx);
            test(p.end_ice_isA(result));

            result = p.begin_ice_ping();
            p.end_ice_ping(result);
            result = p.begin_ice_ping(ctx);
            p.end_ice_ping(result);

            result = p.begin_ice_id();
            test(p.end_ice_id(result).Equals("::Test::TestIntf"));
            result = p.begin_ice_id(ctx);
            test(p.end_ice_id(result).Equals("::Test::TestIntf"));

            result = p.begin_ice_ids();
            test(p.end_ice_ids(result).Length == 2);
            result = p.begin_ice_ids(ctx);
            test(p.end_ice_ids(result).Length == 2);

            result = p.begin_op();
            p.end_op(result);
            result = p.begin_op(ctx);
            p.end_op(result);

            result = p.begin_opWithResult();
            test(p.end_opWithResult(result) == 15);
            result = p.begin_opWithResult(ctx);
            test(p.end_opWithResult(result) == 15);

            result = p.begin_opWithUE();
            try
            {
                p.end_opWithUE(result);
                test(false);
            }
            catch(Test.TestIntfException)
            {
            }
            result = p.begin_opWithUE(ctx);
            try
            {
                p.end_opWithUE(result);
                test(false);
            }
            catch(Test.TestIntfException)
            {
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing async callback... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            Dictionary<string, string> ctx = new Dictionary<string, string>();
            Cookie cookie = new Cookie(5);
            AsyncCallback cbWC = new AsyncCallback(cookie);

            p.begin_ice_isA("::Test::TestIntf", cb.isA, null);
            cb.check();
            p.begin_ice_isA("::Test::TestIntf", cbWC.isA, cookie);
            cbWC.check();
            p.begin_ice_isA("::Test::TestIntf", ctx, cb.isA, null);
            cb.check();
            p.begin_ice_isA("::Test::TestIntf", ctx, cbWC.isA, cookie);
            cbWC.check();

            p.begin_ice_ping(cb.ping, null);
            cb.check();
            p.begin_ice_ping(cbWC.ping, cookie);
            cbWC.check();
            p.begin_ice_ping(ctx, cb.ping, null);
            cb.check();
            p.begin_ice_ping(ctx, cbWC.ping, cookie);
            cbWC.check();

            p.begin_ice_id(cb.id, null);
            cb.check();
            p.begin_ice_id(cbWC.id, cookie);
            cbWC.check();
            p.begin_ice_id(ctx, cb.id, null);
            cb.check();
            p.begin_ice_id(ctx, cbWC.id, cookie);
            cbWC.check();

            p.begin_ice_ids(cb.ids, null);
            cb.check();
            p.begin_ice_ids(cbWC.ids, cookie);
            cbWC.check();
            p.begin_ice_ids(ctx, cb.ids, null);
            cb.check();
            p.begin_ice_ids(ctx, cbWC.ids, cookie);
            cbWC.check();

            p.begin_op(cb.op, null);
            cb.check();
            p.begin_op(cbWC.op, cookie);
            cbWC.check();
            p.begin_op(ctx, cb.op, null);
            cb.check();
            p.begin_op(ctx, cbWC.op, cookie);
            cbWC.check();

            p.begin_opWithResult(cb.opWithResult, null);
            cb.check();
            p.begin_opWithResult(cbWC.opWithResult, cookie);
            cbWC.check();
            p.begin_opWithResult(ctx, cb.opWithResult, null);
            cb.check();
            p.begin_opWithResult(ctx, cbWC.opWithResult, cookie);
            cbWC.check();

            p.begin_opWithUE(cb.opWithUE, null);
            cb.check();
            p.begin_opWithUE(cbWC.opWithUE, cookie);
            cbWC.check();
            p.begin_opWithUE(ctx, cb.opWithUE, null);
            cb.check();
            p.begin_opWithUE(ctx, cbWC.opWithUE, cookie);
            cbWC.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing response callback... ");
        Console.Out.Flush();
        {
            ResponseCallback cb = new ResponseCallback();
            Dictionary<string, string> ctx = new Dictionary<string, string>();

            p.begin_ice_isA("::Test::TestIntf").whenCompleted(cb.isA, null);
            cb.check();
            p.begin_ice_isA("::Test::TestIntf", ctx).whenCompleted(cb.isA, null);
            cb.check();

            p.begin_ice_ping().whenCompleted(cb.ping, null);
            cb.check();
            p.begin_ice_ping(ctx).whenCompleted(cb.ping, null);
            cb.check();

            p.begin_ice_id().whenCompleted(cb.id, null);
            cb.check();
            p.begin_ice_id(ctx).whenCompleted(cb.id, null);
            cb.check();

            p.begin_ice_ids().whenCompleted(cb.ids, null);
            cb.check();
            p.begin_ice_ids(ctx).whenCompleted(cb.ids, null);
            cb.check();

            p.begin_op().whenCompleted(cb.op, null);
            cb.check();
            p.begin_op(ctx).whenCompleted(cb.op, null);
            cb.check();

            p.begin_opWithResult().whenCompleted(cb.opWithResult, null);
            cb.check();
            p.begin_opWithResult(ctx).whenCompleted(cb.opWithResult, null);
            cb.check();

            p.begin_opWithUE().whenCompleted(cb.op, cb.opWithUE);
            cb.check();
            p.begin_opWithUE(ctx).whenCompleted(cb.op, cb.opWithUE);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing local exceptions... ");
        Console.Out.Flush();
        {
            Test.TestIntfPrx indirect = Test.TestIntfPrxHelper.uncheckedCast(p.ice_adapterId("dummy"));
            Ice.AsyncResult r;

            r = indirect.begin_op();
            try
            {
                indirect.end_op(r);
                test(false);
            }
            catch(Ice.NoEndpointException)
            {
            }


            try
            {
                r = ((Test.TestIntfPrx)p.ice_oneway()).begin_opWithResult();
                test(false);
            }
            catch(System.ArgumentException)
            {
            }

            //
            // Check that CommunicatorDestroyedException is raised directly.
            //
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = communicator.getProperties().ice_clone_();
            Ice.Communicator ic = Ice.Util.initialize(initData);
            Ice.ObjectPrx o = ic.stringToProxy(p.ToString());
            Test.TestIntfPrx p2 = Test.TestIntfPrxHelper.checkedCast(o);
            ic.destroy();

            try
            {
                p2.begin_op();
                test(false);
            }
            catch(Ice.CommunicatorDestroyedException)
            {
                // Expected.
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing local exceptions with async callback... ");
        Console.Out.Flush();
        {
            Test.TestIntfPrx i = Test.TestIntfPrxHelper.uncheckedCast(p.ice_adapterId("dummy"));
            AsyncCallback cb = new AsyncCallback();
            Cookie cookie = new Cookie(5);
            AsyncCallback cbWC = new AsyncCallback(cookie);

            i.begin_ice_isA("::Test::TestIntf", cb.isAEx, null);
            cb.check();
            i.begin_ice_isA("::Test::TestIntf", cbWC.isAEx, cookie);
            cbWC.check();

            i.begin_ice_ping(cb.pingEx, null);
            cb.check();
            i.begin_ice_ping(cbWC.pingEx, cookie);
            cbWC.check();

            i.begin_ice_id(cb.idEx, null);
            cb.check();
            i.begin_ice_id(cbWC.idEx, cookie);
            cbWC.check();

            i.begin_ice_ids(cb.idsEx, null);
            cb.check();
            i.begin_ice_ids(cbWC.idsEx, cookie);
            cbWC.check();

            i.begin_op(cb.opEx, null);
            cb.check();
            i.begin_op(cbWC.opEx, cookie);
            cbWC.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing local exceptions with response callback... ");
        Console.Out.Flush();
        {
            Test.TestIntfPrx i = Test.TestIntfPrxHelper.uncheckedCast(p.ice_adapterId("dummy"));
            ExceptionCallback cb = new ExceptionCallback();

            i.begin_ice_isA("::Test::TestIntf").whenCompleted(cb.isA, cb.ex);
            cb.check();

            i.begin_ice_ping().whenCompleted(cb.ping, cb.ex);
            cb.check();

            i.begin_ice_id().whenCompleted(cb.id, cb.ex);
            cb.check();

            i.begin_ice_ids().whenCompleted(cb.ids, cb.ex);
            cb.check();

            i.begin_op().whenCompleted(cb.op, cb.ex);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing exception callback... ");
        Console.Out.Flush();
        {
            Test.TestIntfPrx i = Test.TestIntfPrxHelper.uncheckedCast(p.ice_adapterId("dummy"));
            ExceptionCallback cb = new ExceptionCallback();

            i.begin_ice_isA("::Test::TestIntf").whenCompleted(cb.ex);
            cb.check();

            i.begin_op().whenCompleted(cb.ex);
            cb.check();

            i.begin_opWithResult().whenCompleted(cb.ex);
            cb.check();

            i.begin_opWithUE().whenCompleted(cb.ex);
            cb.check();

            // Ensures no exception is called when response is received
            p.begin_ice_isA("::Test::TestIntf").whenCompleted(cb.noEx);
            p.begin_op().whenCompleted(cb.noEx);
            p.begin_opWithResult().whenCompleted(cb.noEx);

            // If response is a user exception, it should be received.
            p.begin_opWithUE().whenCompleted(cb.opWithUE);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing sent callback... ");
        Console.Out.Flush();
        {
            SentCallback cb = new SentCallback();

            p.begin_ice_isA("").whenCompleted(cb.isA, cb.ex).whenSent(cb.sent);
            cb.check();

            p.begin_ice_ping().whenCompleted(cb.ping, cb.ex).whenSent(cb.sent);
            cb.check();

            p.begin_ice_id().whenCompleted(cb.id, cb.ex).whenSent(cb.sent);
            cb.check();

            p.begin_ice_ids().whenCompleted(cb.ids, cb.ex).whenSent(cb.sent);
            cb.check();

            p.begin_op().whenCompleted(cb.op, cb.ex).whenSent(cb.sent);
            cb.check();

            p.begin_op(cb.opAsync, null).whenSent((Ice.AsyncCallback)cb.sentAsync);
            cb.check();

            p.begin_op().whenCompleted(cb.ex).whenSent(cb.sent);
            cb.check();

            List<SentCallback> cbs = new List<SentCallback>();
            byte[] seq = new byte[10024];
            (new System.Random()).NextBytes(seq);
            testController.holdAdapter();
            try
            {
                Ice.AsyncResult r;
                do
                {
                    SentCallback cb2 = new SentCallback();
                    r = p.begin_opWithPayload(seq).whenCompleted(cb2.ex).whenSent(cb2.sent);
                    cbs.Add(cb2);
                }
                while(r.sentSynchronously());
            }
            finally
            {
                testController.resumeAdapter();
            }
            foreach(SentCallback cb3 in cbs)
            {
                cb3.check();
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing illegal arguments... ");
        Console.Out.Flush();
        {
            Ice.AsyncResult result;

            result = p.begin_op();
            p.end_op(result);
            try
            {
                p.end_op(result);
                test(false);
            }
            catch(System.ArgumentException)
            {
            }

            result = p.begin_op();
            try
            {
                p.end_opWithResult(result);
                test(false);
            }
            catch(System.ArgumentException)
            {
            }

            try
            {
                p.end_op(null);
                test(false);
            }
            catch(System.ArgumentException)
            {
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing unexpected exceptions from callback... ");
        Console.Out.Flush();
        {
            Test.TestIntfPrx q = Test.TestIntfPrxHelper.uncheckedCast(p.ice_adapterId("dummy"));
            ThrowType[] throwEx = new ThrowType[]{ ThrowType.LocalException, ThrowType.UserException,
                                                   ThrowType.OtherException };

            for(int i = 0; i < 3; ++i)
            {
                Thrower cb = new Thrower(throwEx[i]);

                p.begin_op(cb.opAsync, null);
                cb.check();

                p.begin_op().whenCompleted(cb.op, null);
                cb.check();

                q.begin_op().whenCompleted(cb.op, cb.ex);
                cb.check();

                p.begin_op().whenCompleted(cb.noOp, cb.ex).whenSent(cb.sent);
                cb.check();

                q.begin_op().whenCompleted(cb.ex);
                cb.check();
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing batch requests with proxy... ");
        Console.Out.Flush();
        {
            Cookie cookie = new Cookie(5);

            {
                //
                // AsyncResult.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.opBatch();
                FlushCallback cb = new FlushCallback(cookie);
                Ice.AsyncResult r = b1.begin_ice_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(2));
            }

            {
                //
                // AsyncResult exception.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.ice_getConnection().close(false);
                FlushExCallback cb = new FlushExCallback(cookie);
                Ice.AsyncResult r = b1.begin_ice_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(!r.isSent());
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }

            {
                //
                // Type-safe.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.opBatch();
                FlushCallback cb = new FlushCallback();
                Ice.AsyncResult r = b1.begin_ice_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(2));
            }

            {
                //
                // Type-safe exception.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.ice_getConnection().close(false);
                FlushExCallback cb = new FlushExCallback();
                Ice.AsyncResult r = b1.begin_ice_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(!r.isSent());
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing batch requests with connection... ");
        Console.Out.Flush();
        {
            Cookie cookie = new Cookie(5);

            {
                //
                // AsyncResult.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.opBatch();
                FlushCallback cb = new FlushCallback(cookie);
                Ice.AsyncResult r = b1.ice_getConnection().begin_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(2));
            }

            {
                //
                // AsyncResult exception.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.ice_getConnection().close(false);
                FlushExCallback cb = new FlushExCallback(cookie);
                Ice.AsyncResult r = b1.ice_getConnection().begin_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(!r.isSent());
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }

            {
                //
                // Type-safe.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.opBatch();
                FlushCallback cb = new FlushCallback();
                Ice.AsyncResult r = b1.ice_getConnection().begin_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(2));
            }

            {
                //
                // Type-safe exception.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.ice_getConnection().close(false);
                FlushExCallback cb = new FlushExCallback();
                Ice.AsyncResult r = b1.ice_getConnection().begin_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(!r.isSent());
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing batch requests with communicator... ");
        Console.Out.Flush();
        {
            Cookie cookie = new Cookie(5);

            {
                //
                // AsyncResult - 1 connection.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.opBatch();
                FlushCallback cb = new FlushCallback(cookie);
                Ice.AsyncResult r = communicator.begin_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(2));
            }

            {
                //
                // AsyncResult exception - 1 connection.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.ice_getConnection().close(false);
                FlushCallback cb = new FlushCallback(cookie);
                Ice.AsyncResult r = communicator.begin_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(r.isSent()); // Exceptions are ignored!
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }

            {
                //
                // AsyncResult - 2 connections.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                TestIntfPrx b2 = (TestIntfPrx)p.ice_connectionId("2").ice_batchOneway();
                b2.ice_getConnection(); // Ensure connection is established.
                b1.opBatch();
                b1.opBatch();
                b2.opBatch();
                b2.opBatch();
                FlushCallback cb = new FlushCallback(cookie);
                Ice.AsyncResult r = communicator.begin_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(4));
            }

            {
                //
                // AsyncResult exception - 2 connections - 1 failure.
                //
                // All connections should be flushed even if there are failures on some connections.
                // Exceptions should not be reported.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                TestIntfPrx b2 = (TestIntfPrx)p.ice_connectionId("2").ice_batchOneway();
                b2.ice_getConnection(); // Ensure connection is established.
                b1.opBatch();
                b2.opBatch();
                b1.ice_getConnection().close(false);
                FlushCallback cb = new FlushCallback(cookie);
                Ice.AsyncResult r = communicator.begin_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(r.isSent()); // Exceptions are ignored!
                test(r.IsCompleted);
                test(p.waitForBatch(1));
            }

            {
                //
                // AsyncResult exception - 2 connections - 2 failures.
                //
                // The sent callback should be invoked even if all connections fail.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                TestIntfPrx b2 = (TestIntfPrx)p.ice_connectionId("2").ice_batchOneway();
                b2.ice_getConnection(); // Ensure connection is established.
                b1.opBatch();
                b2.opBatch();
                b1.ice_getConnection().close(false);
                b2.ice_getConnection().close(false);
                FlushCallback cb = new FlushCallback(cookie);
                Ice.AsyncResult r = communicator.begin_flushBatchRequests(cb.completedAsync, cookie);
                r.whenSent((Ice.AsyncCallback)cb.sentAsync);
                cb.check();
                test(r.isSent()); // Exceptions are ignored!
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }

            {
                //
                // Type-safe - 1 connection.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.opBatch();
                FlushCallback cb = new FlushCallback();
                Ice.AsyncResult r = communicator.begin_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(2));
            }

            {
                //
                // Type-safe exception - 1 connection.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                b1.opBatch();
                b1.ice_getConnection().close(false);
                FlushCallback cb = new FlushCallback();
                Ice.AsyncResult r = communicator.begin_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(r.isSent()); // Exceptions are ignored!
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }

            {
                //
                // 2 connections.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                TestIntfPrx b2 = (TestIntfPrx)p.ice_connectionId("2").ice_batchOneway();
                b2.ice_getConnection(); // Ensure connection is established.
                b1.opBatch();
                b1.opBatch();
                b2.opBatch();
                b2.opBatch();
                FlushCallback cb = new FlushCallback();
                Ice.AsyncResult r = communicator.begin_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(r.isSent());
                test(r.IsCompleted);
                test(p.waitForBatch(4));
            }

            {
                //
                // Exception - 2 connections - 1 failure.
                //
                // All connections should be flushed even if there are failures on some connections.
                // Exceptions should not be reported.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                TestIntfPrx b2 = (TestIntfPrx)p.ice_connectionId("2").ice_batchOneway();
                b2.ice_getConnection(); // Ensure connection is established.
                b1.opBatch();
                b2.opBatch();
                b1.ice_getConnection().close(false);
                FlushCallback cb = new FlushCallback();
                Ice.AsyncResult r = communicator.begin_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(r.isSent()); // Exceptions are ignored!
                test(r.IsCompleted);
                test(p.waitForBatch(1));
            }

            {
                //
                // Exception - 2 connections - 2 failures.
                //
                // The sent callback should be invoked even if all connections fail.
                //
                test(p.opBatchCount() == 0);
                TestIntfPrx b1 = (TestIntfPrx)p.ice_batchOneway();
                TestIntfPrx b2 = (TestIntfPrx)p.ice_connectionId("2").ice_batchOneway();
                b2.ice_getConnection(); // Ensure connection is established.
                b1.opBatch();
                b2.opBatch();
                b1.ice_getConnection().close(false);
                b2.ice_getConnection().close(false);
                FlushCallback cb = new FlushCallback();
                Ice.AsyncResult r = communicator.begin_flushBatchRequests();
                r.whenCompleted(cb.exception);
                r.whenSent((Ice.SentCallback)cb.sent);
                cb.check();
                test(r.isSent()); // Exceptions are ignored!
                test(r.IsCompleted);
                test(p.opBatchCount() == 0);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing AsyncResult operations... ");
        Console.Out.Flush();
        {
            testController.holdAdapter();
            Ice.AsyncResult r1;
            Ice.AsyncResult r2;
            try
            {
                r1 = p.begin_op();
                byte[] seq = new byte[10024];
                (new System.Random()).NextBytes(seq);
                while((r2 = p.begin_opWithPayload(seq)).sentSynchronously());
                
                test(r1.sentSynchronously() && r1.isSent() && !r1.isCompleted_() ||
                     !r1.sentSynchronously() && !r1.isCompleted_());
                
                test(!r2.sentSynchronously() && !r2.isCompleted_());
                
                test(!r1.IsCompleted && !r1.CompletedSynchronously);
                test(!r2.IsCompleted && !r2.CompletedSynchronously);
            }
            finally
            {
                testController.resumeAdapter();
            }

            WaitHandle w1 = r1.AsyncWaitHandle;
            WaitHandle w2 = r2.AsyncWaitHandle;

            r1.waitForSent();
            test(r1.isSent());

            r2.waitForSent();
            test(r2.isSent());

            r1.waitForCompleted();
            test(r1.isCompleted_());
            w1.WaitOne();

            r2.waitForCompleted();
            test(r2.isCompleted_());
            w2.WaitOne();

            test(r1.getOperation().Equals("op"));
            test(r2.getOperation().Equals("opWithPayload"));
        }
        Console.Out.WriteLine("ok");

        p.shutdown();
    }
}
