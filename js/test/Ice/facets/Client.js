// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var require = typeof(module) !== "undefined" ? module.require : function(){};
    require("Ice/Ice");
    var Ice = global.Ice;
    
    require("Test");
    var Test = global.Test;
    var Promise = Ice.Promise;
    
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
        //
        // re-throw exception so it propagates to final exception
        // handler.
        //
        var exceptionCB = function(ex){ throw ex; };
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
                out.write("testing checked cast... ");
                var ff, gf, hf;
                
                Ice.ObjectPrx.checkedCast(db).then(
                    function(asyncResult, obj)
                    {
                        prx = obj;
                        test(prx.ice_getFacet().length == 0);
                        return Ice.ObjectPrx.checkedCast(db, "facetABCD");
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        prx = obj;
                        test(prx.ice_getFacet() == "facetABCD");
                        return Ice.ObjectPrx.checkedCast(prx);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        prx2 = obj;
                        test(prx2.ice_getFacet() == "facetABCD");
                        return Ice.ObjectPrx.checkedCast(prx, "");
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        prx3 = obj;
                        test(prx3.ice_getFacet().length === 0);
                        return Test.DPrx.checkedCast(db);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        d = obj;
                        test(d.ice_getFacet().length === 0);
                        return Test.DPrx.checkedCast(db, "facetABCD");
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        df = obj;
                        test(df.ice_getFacet() == "facetABCD");
                        return Test.DPrx.checkedCast(df);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        df2 = obj;
                        test(df2.ice_getFacet() == "facetABCD");
                        return Test.DPrx.checkedCast(df, "");
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        df3 = obj;
                        test(df3.ice_getFacet().length === 0);
                        out.writeLine("ok");
                        out.write("testing non-facets A, B, C, and D... ");
                        return Test.DPrx.checkedCast(db);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        d = obj;
                        test(d !== null);
                        test(d.equals(db));
                        
                        return Promise.all(
                            d.callA(),
                            d.callB(),
                            d.callC(),
                            d.callD());
                    },
                    exceptionCB
                ).then(
                    function(r1, r2, r3, r4)
                    {
                        test(r1[1] == "A");
                        test(r2[1] == "B");
                        test(r3[1] == "C");
                        test(r4[1] == "D");
                        out.writeLine("ok");
                        out.write("testing facets A, B, C, and D... ");
                        return Test.DPrx.checkedCast(d, "facetABCD");
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        df = obj;
                        test(df !== null);
                        
                        return Promise.all(
                            df.callA(),
                            df.callB(),
                            df.callC(),
                            df.callD());
                    },
                    exceptionCB
                ).then(
                    function(r1, r2, r3, r4)
                    {
                        test(r1[1] == "A");
                        test(r2[1] == "B");
                        test(r3[1] == "C");
                        test(r4[1] == "D");
                        out.writeLine("ok");
                        out.write("testing facets E and F... ");
                        return Test.FPrx.checkedCast(d, "facetEF");
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        ff = obj;
                        test(ff !== null);
                        
                        return Promise.all(
                            ff.callE(),
                            ff.callF());
                    },
                    exceptionCB
                ).then(
                    function(r1, r2)
                    {
                        test(r1[1] == "E");
                        test(r2[1] == "F");
                        out.writeLine("ok");
                        out.write("testing facet G... ");
                        return Test.GPrx.checkedCast(ff, "facetGH");
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        gf = obj;
                        test(gf !== null);
                        return gf.callG();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, v)
                    {
                        test(v == "G");
                        out.writeLine("ok");
                        out.write("testing whether casting preserves the facet... ");
                        return Test.HPrx.checkedCast(gf);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, obj)
                    {
                        hf = obj;
                        test(hf !== null);
                        
                        return Promise.all(
                            hf.callG(),
                            hf.callH());
                    },
                    exceptionCB
                ).then(
                    function(r1, r2)
                    {
                        test(r1[1] == "G");
                        test(r2[1] == "H");
                        out.writeLine("ok");
                        return gf.shutdown();
                    },
                    exceptionCB
                ).then(
                    function()
                    {
                        p.succeed();
                    },
                    exceptionCB
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
                    allTests(out, c).then(function(){ 
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
    this.__test__ = run;
}());
