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
                var ref = "timeout:default -p 12010";
                var obj = communicator.stringToProxy(ref);
                test(obj !== null);

                var mult = 1;
                if(communicator.getProperties().getPropertyWithDefault("Ice.Default.Protocol", "tcp") === "ssl")
                {
                    mult = 4;
                }

                var timeout, to;
                var connection;
                var comm;
                var now;

                Test.TimeoutPrx.checkedCast(obj).then(
                    function(asyncResult, obj)
                    {
                        timeout = obj;
                        test(timeout !== null);
                        out.write("testing connect timeout... ");
                        to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(500 * mult));
                        return to.holdAdapter(2000 * mult);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        return to.ice_getConnection();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, con)
                    {
                        return con.close(true); // Force a reconnect.
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        //
                        // Expect ConnectTimeoutException.
                        //
                        return to.op();
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        test(ex instanceof Ice.ConnectTimeoutException);
                        return timeout.op(); // Ensure adapter is active.
                    }
                ).then(
                    function(asyncResult)
                    {
                        to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(2000 * mult));
                        return to.holdAdapter(500 * mult);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        return to.ice_getConnection();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, con)
                    {
                        return con.close(true); // Force a reconnect.
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        //
                        // Expect success.
                        //
                        return to.op();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        out.writeLine("ok");
                        out.write("testing read timeout... ");
                        to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(500 * mult));
                        //
                        // Expect TimeoutException.
                        //
                        return to.sleep(750 * mult);
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        test(ex instanceof Ice.TimeoutException);
                        return timeout.op(); // Ensure adapter is active.
                    }
                ).then(
                    function(asyncResult)
                    {
                        to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(1500 * mult));
                        return to.sleep(500 * mult);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        out.writeLine("ok");
                        out.write("testing write timeout... ");
                        to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(500 * mult));
                        return to.holdAdapter(2000 * mult);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        var seq = Ice.Buffer.createNative(new Array(100000));
                        for(var i = 0; i < seq.length; ++i)
                        {
                            seq[i] = 0;
                        }
                        //
                        // Expect TimeoutException.
                        //
                        return to.sendData(seq);
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        test(ex instanceof Ice.TimeoutException);
                        return timeout.op(); // Ensure adapter is active.
                    }
                ).then(
                    function(asyncResult)
                    {
                        to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(1500 * mult));
                        return to.holdAdapter(500 * mult);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        var seq;
                        if(mult === 1)
                        {
                            seq = Ice.Buffer.createNative(new Array(512 * 1024));
                        }
                        else
                        {
                            seq = Ice.Buffer.createNative(new Array(5 * 1024));
                        }
                        for(var i = 0; i < seq.length; ++i)
                        {
                            seq[i] = 0;
                        }
                        //
                        // Expect success.
                        //
                        return to.sendData(seq);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        out.writeLine("ok");
                        out.write("testing close timeout... ");
                        to = Test.TimeoutPrx.uncheckedCast(obj.ice_timeout(250));
                        return to.ice_getConnection();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, con)
                    {
                        connection = con;
                        return timeout.holdAdapter(750);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        return connection.close(false);
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        try
                        {
                            connection.getInfo(); // getInfo() doesn't throw in the closing state
                        }
                        catch(ex)
                        {
                            test(false);
                        }

                        setTimeout(function()
                            {
                                try
                                {
                                    connection.getInfo();
                                    test(false);
                                }
                                catch(ex)
                                {
                                    test(ex instanceof Ice.CloseConnectionException); // Expected
                                }
                                timeout.op().then(
                                    function(asyncResult)
                                    {
                                        out.writeLine("ok");
                                        out.write("testing timeout overrides... ");
                                        //
                                        // Test Ice.Override.Timeout. This property overrides all
                                        // endpoint timeouts.
                                        //
                                        var initData = new Ice.InitializationData();
                                        initData.properties = communicator.getProperties().clone();
                                        if(mult === 1)
                                        {
                                            initData.properties.setProperty("Ice.Override.Timeout", "500");
                                        }
                                        else
                                        {
                                            initData.properties.setProperty("Ice.Override.Timeout", "2000");
                                        }
                                        comm = Ice.initialize(initData);
                                        return Test.TimeoutPrx.checkedCast(comm.stringToProxy(ref));
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult, obj)
                                    {
                                        to = obj;
                                        return to.sleep(750 * mult);
                                    },
                                    exceptionCB
                                ).then(
                                    failCB,
                                    function(ex)
                                    {
                                        test(ex instanceof Ice.TimeoutException);
                                        return timeout.op(); // Ensure adapter is active.
                                    }
                                ).then(
                                    function(asyncResult)
                                    {
                                        //
                                        // Calling ice_timeout() should have no effect.
                                        //
                                        return Test.TimeoutPrx.checkedCast(to.ice_timeout(1000 * mult));
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult, obj)
                                    {
                                        to = obj;
                                        return to.sleep(750 * mult);
                                    },
                                    exceptionCB
                                ).then(
                                    failCB,
                                    function(ex)
                                    {
                                        test(ex instanceof Ice.TimeoutException);
                                        return comm.destroy();
                                    }
                                ).then(
                                    function(asyncResult)
                                    {
                                        //
                                        // Test Ice.Override.ConnectTimeout.
                                        //
                                        var initData = new Ice.InitializationData();
                                        initData.properties = communicator.getProperties().clone();
                                        if(mult === 1)
                                        {
                                            initData.properties.setProperty("Ice.Override.ConnectTimeout", "1000");
                                        }
                                        else
                                        {
                                            initData.properties.setProperty("Ice.Override.ConnectTimeout", "4000");
                                        }
                                        comm = Ice.initialize(initData);
                                        to = Test.TimeoutPrx.uncheckedCast(comm.stringToProxy(ref));
                                        return timeout.holdAdapter(3000 * mult);
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult)
                                    {
                                        return to.op();
                                    },
                                    exceptionCB
                                ).then(
                                    failCB,
                                    function(ex)
                                    {
                                        test(ex instanceof Ice.ConnectTimeoutException);
                                        return timeout.op(); // Ensure adapter is active.
                                    }
                                ).then(
                                    function(asyncResult)
                                    {
                                        return timeout.holdAdapter(3000 * mult);
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult)
                                    {
                                        //
                                        // Calling ice_timeout() should have no effect on the connect timeout.
                                        //
                                        to = Test.TimeoutPrx.uncheckedCast(to.ice_timeout(3500 * mult));
                                        return to.op();
                                    },
                                    exceptionCB
                                ).then(
                                    failCB,
                                    function(ex)
                                    {
                                        test(ex instanceof Ice.ConnectTimeoutException);
                                        return timeout.op(); // Ensure adapter is active.
                                    }
                                ).then(
                                    function(asyncResult)
                                    {
                                        return to.op(); // Force connection.
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult)
                                    {
                                        return to.sleep(4000 * mult);
                                    },
                                    exceptionCB
                                ).then(
                                    failCB,
                                    function(ex)
                                    {
                                        test(ex instanceof Ice.TimeoutException);
                                        return comm.destroy();
                                    }
                                ).then(
                                    function(asyncResult)
                                    {
                                        //
                                        // Test Ice.Override.CloseTimeout.
                                        //
                                        var initData = new Ice.InitializationData();
                                        initData.properties = communicator.getProperties().clone();
                                        initData.properties.setProperty("Ice.Override.CloseTimeout", "200");
                                        comm = Ice.initialize(initData);
                                        return comm.stringToProxy(ref).ice_getConnection();
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult, con)
                                    {
                                        return timeout.holdAdapter(750);
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult)
                                    {
                                        now = Date.now();
                                        return comm.destroy();
                                    },
                                    exceptionCB
                                ).then(
                                    function(asyncResult)
                                    {
                                        var t = Date.now();
                                        test(t - now < 500);
                                        out.writeLine("ok");
                                        return timeout.shutdown();
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
                            }, 500);
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
    global.__test__ = run;
}(typeof (global) === "undefined" ? window : global));
