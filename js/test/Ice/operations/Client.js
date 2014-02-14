// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
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

    require("Twoways");
    require("Oneways");
    require("BatchOneways");
    var Twoways = global.Twoways;
    var Oneways = global.Oneways;
    var BatchOneways = global.BatchOneways;

    var allTests = function(out, communicator)
    {
        var ref, base, cl, derived;
        
        return Promise.try(
            function()
            {
                out.write("testing twoway operations... ");
                ref = "test:default -p 12010";
                base = communicator.stringToProxy(ref);
                cl, derived;
                return Test.MyClassPrx.checkedCast(base);
            }
        ).then(
            function(r, prx)
            {
                cl = prx;
                return Test.MyDerivedClassPrx.checkedCast(cl);
            }
        ).then(
            function(r, prx)
            {
                derived = prx;
                return Twoways.run(communicator, cl);
            }
        ).then(
            function()
            {
                return Twoways.run(communicator, derived);
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
            });
    };

    var run = function(out, id)
    {
        return Promise.try(
            function()
            {
                //
                // We must set MessageSizeMax to an explicit value,
                // because we run tests to check whether
                // Ice.MemoryLimitException is raised as expected.
                //
                id.properties.setProperty("Ice.MessageSizeMax", "100");
                var c = Ice.initialize(id);
                return allTests(out, c).finally(
                    function()
                    {
                        if(c)
                        {
                            return c.destroy();
                        }
                    });
            });
    };
    global.__test__ = run;
}(typeof (global) === "undefined" ? window : global));
