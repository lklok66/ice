// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){
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
            // Re-throw exception so it propagates to final exception
            // handler.
            //
            var exceptionCB = function(ex) { throw ex; };
            var failCB = function() { test(false); };

            setTimeout(function(){
                try
                {
                    out.write("testing stringToProxy... ");
                    var ref = "retry:default -p 12010";
                    var base1 = communicator.stringToProxy(ref);
                    test(base1 !== null);
                    var base2 = communicator.stringToProxy(ref);
                    test(base2 !== null);
                    out.writeLine("ok");

                    var retry1, retry2;
                    out.write("testing checked cast... ");
                    Test.RetryPrx.checkedCast(base1).then(
                        function(asyncResult, obj)
                        {
                            retry1 = obj;
                            test(retry1 !== null);
                            test(retry1.equals(base1));
                            return Test.RetryPrx.checkedCast(base2);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            retry2 = obj;
                            test(retry2 !== null);
                            test(retry2.equals(base2));
                            out.writeLine("ok");
                            out.write("calling regular operation with first proxy... ");
                            return retry1.op(false);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("calling operation to kill connection with second proxy... ");
                            return retry2.op(true);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            test(false);
                        },
                        function(ex)
                        {
                            test(ex instanceof Ice.ConnectionLostException);
                            out.writeLine("ok");
                            out.write("calling regular operation with first proxy again... ");
                            return retry1.op(false);
                        }
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            return retry1.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
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
                        // For this test, we want to disable retries.
                        //
                        id.properties.setProperty("Ice.RetryIntervals", "-1");

                        //
                        // We don't want connection warnings because of the timeout
                        //
                        id.properties.setProperty("Ice.Warn.Connections", "0");

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
        module.exports.run = run;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/retry"));
