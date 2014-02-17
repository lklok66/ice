// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    var require = typeof(module) !== "undefined" ? module.require : function(){};
    require("Ice/Ice");
    var Ice = global.Ice;

    require("Test");
    var Test = global.Test;
    var Promise = Ice.Promise;

    require("TestI");
    var DI = global.DI;
    var FI = global.FI;
    var HI = global.HI;
    var EmptyI = global.EmptyI;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var allTests = function(out, communicator)
    {
        var p = new Promise();

        var failCB = function(){ test(false); };

        setTimeout(function(){
            try
            {
                out.write("testing stringToProxy... ");
                var ref = "d:default -p 12010";
                var db = communicator.stringToProxy(ref);
                test(db !== null);
                out.writeLine("ok");

                out.write("testing unchecked cast... ");
                var prx = Ice.ObjectPrx.uncheckedCast(db);
                test(prx.ice_getFacet().length === 0);
                prx = Ice.ObjectPrx.uncheckedCast(db, "facetABCD");
                test(prx.ice_getFacet() == "facetABCD");
                var prx2 = Ice.ObjectPrx.uncheckedCast(prx);
                test(prx2.ice_getFacet() == "facetABCD");
                var prx3 = Ice.ObjectPrx.uncheckedCast(prx, "");
                test(prx3.ice_getFacet().length == 0);
                var d = Test.DPrx.uncheckedCast(db);
                test(d.ice_getFacet().length == 0);
                var df = Test.DPrx.uncheckedCast(db, "facetABCD");
                test(df.ice_getFacet() == "facetABCD");
                var df2 = Test.DPrx.uncheckedCast(df);
                test(df2.ice_getFacet() == "facetABCD");
                var df3 = Test.DPrx.uncheckedCast(df, "");
                test(df3.ice_getFacet().length == 0);
                out.writeLine("ok");

                var ff, gf, hf;

                var adapter;

                out.write("testing facet registration exceptions... ");
                communicator.createObjectAdapter("").then(
                    function(o)
                    {
                        adapter = o;
                        var obj = new EmptyI();
                        adapter.add(obj, communicator.stringToIdentity("d"));
                        adapter.addFacet(obj, communicator.stringToIdentity("d"), "facetABCD");
                        try
                        {
                            adapter.addFacet(obj, communicator.stringToIdentity("d"), "facetABCD");
                            test(false);
                        }
                        catch(ex)
                        {
                            test(ex instanceof Ice.AlreadyRegisteredException);
                        }
                        adapter.removeFacet(communicator.stringToIdentity("d"), "facetABCD");
                        try
                        {
                            adapter.removeFacet(communicator.stringToIdentity("d"), "facetABCD");
                            test(false);
                        }
                        catch(ex)
                        {
                            test(ex instanceof Ice.NotRegisteredException);
                        }
                        out.writeLine("ok");

                        out.write("testing removeAllFacets... ");
                        var obj1 = new EmptyI();
                        var obj2 = new EmptyI();
                        adapter.addFacet(obj1, communicator.stringToIdentity("id1"), "f1");
                        adapter.addFacet(obj2, communicator.stringToIdentity("id1"), "f2");
                        var obj3 = new EmptyI();
                        adapter.addFacet(obj1, communicator.stringToIdentity("id2"), "f1");
                        adapter.addFacet(obj2, communicator.stringToIdentity("id2"), "f2");
                        adapter.addFacet(obj3, communicator.stringToIdentity("id2"), "");
                        var fm = adapter.removeAllFacets(communicator.stringToIdentity("id1"));
                        test(fm.size === 2);
                        test(fm.get("f1") === obj1);
                        test(fm.get("f2") === obj2);
                        try
                        {
                            adapter.removeAllFacets(communicator.stringToIdentity("id1"));
                            test(false);
                        }
                        catch(ex)
                        {
                            test(ex instanceof Ice.NotRegisteredException);
                        }
                        fm = adapter.removeAllFacets(communicator.stringToIdentity("id2"));
                        test(fm.size == 3);
                        test(fm.get("f1") === obj1);
                        test(fm.get("f2") === obj2);
                        test(fm.get("") === obj3);
                        out.writeLine("ok");

                        return adapter.deactivate();
                    }
                ).then(
                    function(r)
                    {
                        return communicator.createObjectAdapter("");
                    }
                ).then(
                    function(o)
                    {
                        adapter = o;
                        var di = new DI();
                        adapter.add(di, communicator.stringToIdentity("d"));
                        adapter.addFacet(di, communicator.stringToIdentity("d"), "facetABCD");
                        var fi = new FI();
                        adapter.addFacet(fi, communicator.stringToIdentity("d"), "facetEF");
                        var hi = new HI();
                        adapter.addFacet(hi, communicator.stringToIdentity("d"), "facetGH");
                        return adapter.activate();
                    }
                ).then(
                    function()
                    {
                        return prx.ice_getConnection();
                    }
                ).then(
                    function(conn)
                    {
                        conn.setAdapter(adapter);
                        out.write("testing checked cast... ");
                        return Ice.ObjectPrx.checkedCast(db);
                    }
                ).then(
                    function(obj)
                    {
                        prx = obj;
                        test(prx.ice_getFacet().length == 0);
                        return Ice.ObjectPrx.checkedCast(db, "facetABCD");
                    }
                ).then(
                    function(obj)
                    {
                        prx = obj;
                        test(prx.ice_getFacet() == "facetABCD");
                        return Ice.ObjectPrx.checkedCast(prx);
                    }
                ).then(
                    function(obj)
                    {
                        prx2 = obj;
                        test(prx2.ice_getFacet() == "facetABCD");
                        return Ice.ObjectPrx.checkedCast(prx, "");
                    }
                ).then(
                    function(obj)
                    {
                        prx3 = obj;
                        test(prx3.ice_getFacet().length === 0);
                        return Test.DPrx.checkedCast(db);
                    }
                ).then(
                    function(obj)
                    {
                        d = obj;
                        test(d.ice_getFacet().length === 0);
                        return Test.DPrx.checkedCast(db, "facetABCD");
                    }
                ).then(
                    function(obj)
                    {
                        df = obj;
                        test(df.ice_getFacet() == "facetABCD");
                        return Test.DPrx.checkedCast(df);
                    }
                ).then(
                    function(obj)
                    {
                        df2 = obj;
                        test(df2.ice_getFacet() == "facetABCD");
                        return Test.DPrx.checkedCast(df, "");
                    }
                ).then(
                    function(obj)
                    {
                        df3 = obj;
                        test(df3.ice_getFacet().length === 0);
                        out.writeLine("ok");
                        out.write("testing non-facets A, B, C, and D... ");
                        return Test.DPrx.checkedCast(db);
                    }
                ).then(
                    function(obj)
                    {
                        d = obj;
                        test(d !== null);
                        test(d.equals(db));

                        return Promise.all(
                            d.callA(),
                            d.callB(),
                            d.callC(),
                            d.callD());
                    }
                ).then(
                    function(r1, r2, r3, r4)
                    {
                        test(r1[0] == "A");
                        test(r2[0] == "B");
                        test(r3[0] == "C");
                        test(r4[0] == "D");
                        out.writeLine("ok");
                        out.write("testing facets A, B, C, and D... ");
                        return Test.DPrx.checkedCast(d, "facetABCD");
                    }
                ).then(
                    function(obj)
                    {
                        df = obj;
                        test(df !== null);

                        return Promise.all(
                            df.callA(),
                            df.callB(),
                            df.callC(),
                            df.callD());
                    }
                ).then(
                    function(r1, r2, r3, r4)
                    {
                        test(r1[0] == "A");
                        test(r2[0] == "B");
                        test(r3[0] == "C");
                        test(r4[0] == "D");
                        out.writeLine("ok");
                        out.write("testing facets E and F... ");
                        return Test.FPrx.checkedCast(d, "facetEF");
                    }
                ).then(
                    function(obj)
                    {
                        ff = obj;
                        test(ff !== null);

                        return Promise.all(
                            ff.callE(),
                            ff.callF());
                    }
                ).then(
                    function(r1, r2)
                    {
                        test(r1[0] == "E");
                        test(r2[0] == "F");
                        out.writeLine("ok");
                        out.write("testing facet G... ");
                        return Test.GPrx.checkedCast(ff, "facetGH");
                    }
                ).then(
                    function(obj)
                    {
                        gf = obj;
                        test(gf !== null);
                        return gf.callG();
                    }
                ).then(
                    function(v)
                    {
                        test(v == "G");
                        out.writeLine("ok");
                        out.write("testing whether casting preserves the facet... ");
                        return Test.HPrx.checkedCast(gf);
                    }
                ).then(
                    function(obj)
                    {
                        hf = obj;
                        test(hf !== null);

                        return Promise.all(
                            hf.callG(),
                            hf.callH());
                    }
                ).then(
                    function(r1, r2)
                    {
                        test(r1[0] == "G");
                        test(r2[0] == "H");
                        out.writeLine("ok");
                        return gf.shutdown();
                    }
                ).then(
                    function()
                    {
                        p.succeed();
                    }
                ).exception(
                    function(ex)
                    {
                        p.fail(ex);
                    }
                );
            }
            catch(ex)
            {
                p.fail(ex);
            }
        });
        return p;
    };

    var run = function(out, id)
    {
        var p = new Ice.Promise();
        setTimeout(
            function()
            {
                var c = null;
                try
                {
                    c = Ice.initialize(id);
                    out.writeLine("testing bidir callbacks with synchronous dispatch...");
                    allTests(out, c).then(function(){
                            return c.destroy();
                        }).then(function(){
                            c = Ice.initialize(id);
                            var ref = "__echo:default -p 12010";
                            var base = c.stringToProxy(ref);
                            return global.Test.EchoPrx.checkedCast(base);
                        }).then(function(prx){
                            return prx.shutdown();
                        }).then(function(){
                            return c.destroy();
                        }).then(function(){
                            p.succeed();
                        }).exception(function(ex){
                            p.fail(ex);
                        });
                }
                catch(ex)
                {
                    p.fail(ex);
                }
            });
        return p;
    };
    global.__test__ = run;
}(typeof (global) === "undefined" ? window : global));
