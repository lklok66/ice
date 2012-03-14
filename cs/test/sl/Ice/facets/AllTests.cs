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

namespace facets
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
            Write("testing Ice.Admin.Facets property... ");
            test(communicator.getProperties().getPropertyAsList("Ice.Admin.Facets").Length == 0);
            communicator.getProperties().setProperty("Ice.Admin.Facets", "foobar");
            String[] facetFilter = communicator.getProperties().getPropertyAsList("Ice.Admin.Facets");
            test(facetFilter.Length == 1 && facetFilter[0].Equals("foobar"));
            communicator.getProperties().setProperty("Ice.Admin.Facets", "foo\\'bar");
            facetFilter = communicator.getProperties().getPropertyAsList("Ice.Admin.Facets");
            test(facetFilter.Length == 1 && facetFilter[0].Equals("foo'bar"));
            communicator.getProperties().setProperty("Ice.Admin.Facets", "'foo bar' toto 'titi'");
            facetFilter = communicator.getProperties().getPropertyAsList("Ice.Admin.Facets");
            test(facetFilter.Length == 3 && facetFilter[0].Equals("foo bar") && facetFilter[1].Equals("toto")
                 && facetFilter[2].Equals("titi"));
            communicator.getProperties().setProperty("Ice.Admin.Facets", "'foo bar\\' toto' 'titi'");
            facetFilter = communicator.getProperties().getPropertyAsList("Ice.Admin.Facets");
            test(facetFilter.Length == 2 && facetFilter[0].Equals("foo bar' toto") && facetFilter[1].Equals("titi"));
            //  communicator.getProperties().setProperty("Ice.Admin.Facets", "'foo bar' 'toto titi");
            // facetFilter = communicator.getProperties().getPropertyAsList("Ice.Admin.Facets");
            // test(facetFilter.Length == 0);
            communicator.getProperties().setProperty("Ice.Admin.Facets", "");
            WriteLine("ok");

            Write("testing stringToProxy... ");
            string @ref = "d:default -p 12010";
            Ice.ObjectPrx db = communicator.stringToProxy(@ref);
            test(db != null);
            WriteLine("ok");

            Write("testing unchecked cast... ");
            Ice.ObjectPrx prx = Ice.ObjectPrxHelper.uncheckedCast(db);
            test(prx.ice_getFacet().Length == 0);
            prx = Ice.ObjectPrxHelper.uncheckedCast(db, "facetABCD");
            test(prx.ice_getFacet() == "facetABCD");
            Ice.ObjectPrx prx2 = Ice.ObjectPrxHelper.uncheckedCast(prx);
            test(prx2.ice_getFacet() == "facetABCD");
            Ice.ObjectPrx prx3 = Ice.ObjectPrxHelper.uncheckedCast(prx, "");
            test(prx3.ice_getFacet().Length == 0);
            DPrx d = Test.DPrxHelper.uncheckedCast(db);
            test(d.ice_getFacet().Length == 0);
            DPrx df = Test.DPrxHelper.uncheckedCast(db, "facetABCD");
            test(df.ice_getFacet() == "facetABCD");
            DPrx df2 = Test.DPrxHelper.uncheckedCast(df);
            test(df2.ice_getFacet() == "facetABCD");
            DPrx df3 = Test.DPrxHelper.uncheckedCast(df, "");
            test(df3.ice_getFacet().Length == 0);
            WriteLine("ok");

            Write("testing checked cast... ");
            prx = Ice.ObjectPrxHelper.checkedCast(db);
            test(prx.ice_getFacet().Length == 0);
            prx = Ice.ObjectPrxHelper.checkedCast(db, "facetABCD");
            test(prx.ice_getFacet() == "facetABCD");
            prx2 = Ice.ObjectPrxHelper.checkedCast(prx);
            test(prx2.ice_getFacet() == "facetABCD");
            prx3 = Ice.ObjectPrxHelper.checkedCast(prx, "");
            test(prx3.ice_getFacet().Length == 0);
            d = Test.DPrxHelper.checkedCast(db);
            test(d.ice_getFacet().Length == 0);
            df = Test.DPrxHelper.checkedCast(db, "facetABCD");
            test(df.ice_getFacet() == "facetABCD");
            df2 = Test.DPrxHelper.checkedCast(df);
            test(df2.ice_getFacet() == "facetABCD");
            df3 = Test.DPrxHelper.checkedCast(df, "");
            test(df3.ice_getFacet().Length == 0);
            WriteLine("ok");

            Write("testing non-facets A, B, C, and D... ");
            d = DPrxHelper.checkedCast(db);
            test(d != null);
            test(d.Equals(db));
            test(d.callA().Equals("A"));
            test(d.callB().Equals("B"));
            test(d.callC().Equals("C"));
            test(d.callD().Equals("D"));
            WriteLine("ok");

            Write("testing facets A, B, C, and D... ");
            df = DPrxHelper.checkedCast(d, "facetABCD");
            test(df != null);
            test(df.callA().Equals("A"));
            test(df.callB().Equals("B"));
            test(df.callC().Equals("C"));
            test(df.callD().Equals("D"));
            WriteLine("ok");

            Write("testing facets E and F... ");
            FPrx ff = FPrxHelper.checkedCast(d, "facetEF");
            test(ff != null);
            test(ff.callE().Equals("E"));
            test(ff.callF().Equals("F"));
            WriteLine("ok");

            Write("testing facet G... ");
            GPrx gf = GPrxHelper.checkedCast(ff, "facetGH");
            test(gf != null);
            test(gf.callG().Equals("G"));
            WriteLine("ok");

            Write("testing whether casting preserves the facet... ");
            HPrx hf = HPrxHelper.checkedCast(gf);
            test(hf != null);
            test(hf.callG().Equals("G"));
            test(hf.callH().Equals("H"));
            WriteLine("ok");
            gf.shutdown();
        }
    }
}
