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
    require("TestAMD");

    var Promise = Ice.Promise;

    require("Twoways");
    require("Oneways");
    require("BatchOneways");
    require("MyDerivedClassI");
    require("AMDMyDerivedClassI");
    var Twoways = global.Twoways;
    var Oneways = global.Oneways;
    var BatchOneways = global.BatchOneways;
    var MyDerivedClassI = global.MyDerivedClassI;
    var AMDMyDerivedClassI = global.AMDMyDerivedClassI;

    var allTests = function(out, communicator, amd)
    {
        var p = new Promise();

        var failCB = function() { test(false); };

        setTimeout(function(){
            try
            {
                out.write("testing twoway operations... ");
                var ref = "test:default -p 12010";
                var base = communicator.stringToProxy(ref);
                var cl, derived;

                var Test = amd ? global.TestAMD : global.Test;

                var adapter;
                communicator.createObjectAdapter("").then(
                    function(o)
                    {
                        adapter = o;

                        if(amd)
                        {
                            adapter.add(new AMDMyDerivedClassI(), communicator.stringToIdentity("test"));
                        }
                        else
                        {
                            adapter.add(new MyDerivedClassI(), communicator.stringToIdentity("test"));
                        }
                        return adapter.activate();
                    }
                ).then(
                    function()
                    {
                        return base.ice_getConnection();
                    }
                ).then(
                    function(conn)
                    {
                        conn.setAdapter(adapter);
                        return Test.MyClassPrx.checkedCast(base);
                    }
                ).then(
                    function(prx)
                    {
                        cl = prx;
                        return Test.MyDerivedClassPrx.checkedCast(cl);
                    }
                ).then(
                    function(prx)
                    {
                        derived = prx;
                        return Twoways.run(communicator, cl, Test);
                    }
                ).then(
                    function()
                    {
                        return Twoways.run(communicator, derived, Test);
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");

                        out.write("testing oneway operations... ");
                        return Oneways.run(communicator, cl);
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");

                        out.write("testing batch oneway operations... ");
                        return BatchOneways.run(communicator, cl);
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");

                        return cl.shutdown();
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
                    //
                    // We must set MessageSizeMax to an explicit value,
                    // because we run tests to check whether
                    // Ice.MemoryLimitException is raised as expected.
                    //
                    id.properties.setProperty("Ice.MessageSizeMax", "100");
                    c = Ice.initialize(id);
                    out.writeLine("testing bidir callbacks with synchronous dispatch...");
                    allTests(out, c, false).then(function(){
                            return c.destroy();
                        }).then(function(){
                            c = Ice.initialize(id);
                            out.writeLine("testing bidir callbacks with asynchronous dispatch...");
                            return allTests(out, c, true);
                        }).then(function(){
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
