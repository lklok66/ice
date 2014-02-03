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
    var Ice = this.Ice;

    require("Test");
    var Test = this.Test;
    var Promise = Ice.Promise;

    require("Twoways");
    require("Oneways");
    require("BatchOneways");
    var Twoways = this.Twoways;
    var Oneways = this.Oneways;
    var BatchOneways = this.BatchOneways;

    var allTests = function(out, communicator)
    {
        var p = new Promise();

        //
        // Re-throw exception so it propagates to final exception
        // handler.
        //
        var exceptionCB = function(ex) { throw ex; };
        var failCB = function() { test(false); };

        setTimeout(function(){
            try
            {
                out.write("testing twoway operations... ");
                var ref = "test:default -p 12010";
                var base = communicator.stringToProxy(ref);
                var cl, derived;
                Test.MyClassPrx.checkedCast(base).then(
                    function(r, prx)
                    {
                        cl = prx;
                        return Test.MyDerivedClassPrx.checkedCast(cl);
                    },
                    exceptionCB
                ).then(
                    function(r, prx)
                    {
                        derived = prx;
                        return Twoways.run(communicator, out, cl);
                    },
                    exceptionCB
                ).then(
                    function()
                    {
                        return Twoways.run(communicator, out, derived);
                    },
                    exceptionCB
                ).then(
                    function()
                    {
                        out.writeLine("ok");

                        out.write("testing oneway operations... ");
                        return Oneways.run(communicator, out, cl);
                    },
                    exceptionCB
                ).then(
                    function()
                    {
                        out.writeLine("ok");

                        out.write("testing batch oneway operations... ");
                        return BatchOneways.run(communicator, out, cl);
                    },
                    exceptionCB
                ).then(
                    function()
                    {
                        out.writeLine("ok");

                        return cl.shutdown();
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
                    //
                    // We must set MessageSizeMax to an explicit value,
                    // because we run tests to check whether
                    // Ice.MemoryLimitException is raised as expected.
                    //
                    id.properties.setProperty("Ice.MessageSizeMax", "100");
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
