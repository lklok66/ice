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

namespace inheritance
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

        public override Ice.InitializationData initData()
        {
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.setProperty("Ice.FactoryAssemblies", "inheritance,version=1.0.0.0");
            return initData;
        }

        public AllTests(TextBox output, Button btnRun)
            : base(output, btnRun)
        {
        }

        override
        public void run(Ice.Communicator communicator)
        {
            Write("testing stringToProxy... ");
            string ref_Renamed = "initial:default -p 12010";
            Ice.ObjectPrx @base = communicator.stringToProxy(ref_Renamed);
            test(@base != null);
            WriteLine("ok");

            Write("testing checked cast... ");
            InitialPrx initial = InitialPrxHelper.checkedCast(@base);
            test(initial != null);
            test(initial.Equals(@base));
            WriteLine("ok");

            Write("getting proxies for class hierarchy... ");
            Test.MA.CAPrx ca = initial.caop();
            Test.MB.CBPrx cb = initial.cbop();
            Test.MA.CCPrx cc = initial.ccop();
            Test.MA.CDPrx cd = initial.cdop();
            test(ca != cb);
            test(ca != cc);
            test(ca != cd);
            test(cb != cc);
            test(cb != cd);
            test(cc != cd);
            WriteLine("ok");

            Write("getting proxies for interface hierarchy... ");
            Test.MA.IAPrx ia = initial.iaop();
            Test.MB.IB1Prx ib1 = initial.ib1op();
            Test.MB.IB2Prx ib2 = initial.ib2op();
            Test.MA.ICPrx ic = initial.icop();
            test(ia != ib1);
            test(ia != ib2);
            test(ia != ic);
            test(ib1 != ic);
            test(ib2 != ic);
            WriteLine("ok");

            Write("invoking proxy operations on class hierarchy... ");
            Test.MA.CAPrx cao;
            Test.MB.CBPrx cbo;
            Test.MA.CCPrx cco;

            cao = ca.caop(ca);
            test(cao.Equals(ca));
            cao = ca.caop(cb);
            test(cao.Equals(cb));
            cao = ca.caop(cc);
            test(cao.Equals(cc));
            cao = cb.caop(ca);
            test(cao.Equals(ca));
            cao = cb.caop(cb);
            test(cao.Equals(cb));
            cao = cb.caop(cc);
            test(cao.Equals(cc));
            cao = cc.caop(ca);
            test(cao.Equals(ca));
            cao = cc.caop(cb);
            test(cao.Equals(cb));
            cao = cc.caop(cc);
            test(cao.Equals(cc));

            cao = cb.cbop(cb);
            test(cao.Equals(cb));
            cbo = cb.cbop(cb);
            test(cbo.Equals(cb));
            cao = cb.cbop(cc);
            test(cao.Equals(cc));
            cbo = cb.cbop(cc);
            test(cbo.Equals(cc));
            cao = cc.cbop(cb);
            test(cao.Equals(cb));
            cbo = cc.cbop(cb);
            test(cbo.Equals(cb));
            cao = cc.cbop(cc);
            test(cao.Equals(cc));
            cbo = cc.cbop(cc);
            test(cbo.Equals(cc));

            cao = cc.ccop(cc);
            test(cao.Equals(cc));
            cbo = cc.ccop(cc);
            test(cbo.Equals(cc));
            cco = cc.ccop(cc);
            test(cco.Equals(cc));
            WriteLine("ok");

            Write("ditto, but for interface hierarchy... ");
            Test.MA.IAPrx iao;
            Test.MB.IB1Prx ib1o;
            Test.MB.IB2Prx ib2o;
            Test.MA.ICPrx ico;

            iao = ia.iaop(ia);
            test(iao.Equals(ia));
            iao = ia.iaop(ib1);
            test(iao.Equals(ib1));
            iao = ia.iaop(ib2);
            test(iao.Equals(ib2));
            iao = ia.iaop(ic);
            test(iao.Equals(ic));
            iao = ib1.iaop(ia);
            test(iao.Equals(ia));
            iao = ib1.iaop(ib1);
            test(iao.Equals(ib1));
            iao = ib1.iaop(ib2);
            test(iao.Equals(ib2));
            iao = ib1.iaop(ic);
            test(iao.Equals(ic));
            iao = ib2.iaop(ia);
            test(iao.Equals(ia));
            iao = ib2.iaop(ib1);
            test(iao.Equals(ib1));
            iao = ib2.iaop(ib2);
            test(iao.Equals(ib2));
            iao = ib2.iaop(ic);
            test(iao.Equals(ic));
            iao = ic.iaop(ia);
            test(iao.Equals(ia));
            iao = ic.iaop(ib1);
            test(iao.Equals(ib1));
            iao = ic.iaop(ib2);
            test(iao.Equals(ib2));
            iao = ic.iaop(ic);
            test(iao.Equals(ic));

            iao = ib1.ib1op(ib1);
            test(iao.Equals(ib1));
            ib1o = ib1.ib1op(ib1);
            test(ib1o.Equals(ib1));
            iao = ib1.ib1op(ic);
            test(iao.Equals(ic));
            ib1o = ib1.ib1op(ic);
            test(ib1o.Equals(ic));
            iao = ic.ib1op(ib1);
            test(iao.Equals(ib1));
            ib1o = ic.ib1op(ib1);
            test(ib1o.Equals(ib1));
            iao = ic.ib1op(ic);
            test(iao.Equals(ic));
            ib1o = ic.ib1op(ic);
            test(ib1o.Equals(ic));

            iao = ib2.ib2op(ib2);
            test(iao.Equals(ib2));
            ib2o = ib2.ib2op(ib2);
            test(ib2o.Equals(ib2));
            iao = ib2.ib2op(ic);
            test(iao.Equals(ic));
            ib2o = ib2.ib2op(ic);
            test(ib2o.Equals(ic));
            iao = ic.ib2op(ib2);
            test(iao.Equals(ib2));
            ib2o = ic.ib2op(ib2);
            test(ib2o.Equals(ib2));
            iao = ic.ib2op(ic);
            test(iao.Equals(ic));
            ib2o = ic.ib2op(ic);
            test(ib2o.Equals(ic));

            iao = ic.icop(ic);
            test(iao.Equals(ic));
            ib1o = ic.icop(ic);
            test(ib1o.Equals(ic));
            ib2o = ic.icop(ic);
            test(ib2o.Equals(ic));
            ico = ic.icop(ic);
            test(ico.Equals(ic));
            WriteLine("ok");

            Write("ditto, but for class implementing interfaces... ");
            cao = cd.caop(cd);
            test(cao.Equals(cd));
            cbo = cd.cbop(cd);
            test(cbo.Equals(cd));
            cco = cd.ccop(cd);
            test(cco.Equals(cd));

            iao = cd.iaop(cd);
            test(iao.Equals(cd));
            ib1o = cd.ib1op(cd);
            test(ib1o.Equals(cd));
            ib2o = cd.ib2op(cd);
            test(ib2o.Equals(cd));

            cao = cd.cdop(cd);
            test(cao.Equals(cd));
            cbo = cd.cdop(cd);
            test(cbo.Equals(cd));
            cco = cd.cdop(cd);
            test(cco.Equals(cd));

            iao = cd.cdop(cd);
            test(iao.Equals(cd));
            ib1o = cd.cdop(cd);
            test(ib1o.Equals(cd));
            ib2o = cd.cdop(cd);
            test(ib2o.Equals(cd));
            WriteLine("ok");
            initial.shutdown();
        }
    }
}
