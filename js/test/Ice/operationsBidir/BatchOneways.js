// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    var Ice = global.Ice;
    var Test = global.Test;

    var Promise = Ice.Promise;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var run = function(communicator, p)
    {
        var promise = new Promise();

        var failCB = function() { test(false); };

        setTimeout(function(){
            try
            {
                var bs1 = Ice.Buffer.createNative(new Array(10 * 1024));
                for(var i = 0; i < bs1.length; ++i)
                {
                    bs1[i] = 0;
                }
                var bs2 = Ice.Buffer.createNative(new Array(99 * 1024));
                for(var i = 0; i < bs2.length; ++i)
                {
                    bs2[i] = 0;
                }
                var bs3 = Ice.Buffer.createNative(new Array(100 * 1024));
                for(var i = 0; i < bs3.length; ++i)
                {
                    bs3[i] = 0;
                }

                var batch, batch2, batch3;
                var echo;

                var ref = "__echo:default -p 12010";
                var base = communicator.stringToProxy(ref);

                Test.EchoPrx.checkedCast(base).then(
                    function(prx)
                    {
                        echo = prx;
                        return p.opByteSOneway(bs1);
                    }
                ).then(
                    function()
                    {
                        return p.opByteSOneway(bs2);
                    }
                ).then(
                    function()
                    {
                        return p.opByteSOneway(bs3);
                    }
                ).then(
                    failCB,
                    function(ex)
                    {
                        test(ex instanceof Ice.MemoryLimitException);

                        return echo.startBatch();
                    }
                ).then(
                    function()
                    {
                        batch = p.ice_batchOneway();

                        var promise1 = new Promise();

                        var all = [];
                        for(var i = 0; i < 8; ++i)
                        {
                            all[i] = batch.opByteSOneway(bs1);
                        }

                        Promise.all.apply(Promise, all).then(
                            function()
                            {
                                return batch.ice_getConnection();
                            }
                        ).then(
                            function(con)
                            {
                                return con.flushBatchRequests();
                            }
                        ).then(
                            function()
                            {
                                return echo.flushBatch();
                            }
                        ).then(
                            function()
                            {
                                promise1.succeed(p);
                            }
                        ).exception(
                            function(ex)
                            {
                                promise1.fail(ex);
                            }
                        );
                        return promise1;
                    }
                ).then(
                    function()
                    {
                        return echo.startBatch();
                    }
                ).then(
                    function()
                    {
                        batch2 = p.ice_batchOneway();

                        return Promise.all(batch.ice_ping(), batch2.ice_ping());
                    }
                ).then(
                    function()
                    {
                        return batch.ice_flushBatchRequests();
                    }
                ).then(
                    function()
                    {
                        return echo.flushBatch();
                    }
                ).then(
                    function()
                    {
                        return echo.startBatch();
                    }
                ).then(
                    function()
                    {
                        return Promise.all(batch.ice_ping(), batch2.ice_ping());
                    }
                ).then(
                    function()
                    {
                        var identity = communicator.stringToIdentity("invalid");
                        batch3 = batch.ice_identity(identity);
                        return batch3.ice_ping();
                    }
                ).then(
                    function()
                    {
                        return batch3.ice_flushBatchRequests();
                    }
                ).then(
                    function()
                    {
                        return echo.flushBatch();
                    }
                ).then(
                    function()
                    {
                        return echo.startBatch();
                    }
                ).then(
                    function()
                    {
                        // Make sure that a bogus batch request doesn't cause troubles to other ones.
                        return Promise.all(batch3.ice_ping(), batch.ice_ping());
                    }
                ).then(
                    function()
                    {
                        return batch.ice_flushBatchRequests();
                    }
                ).then(
                    function()
                    {
                        return echo.flushBatch();
                    }
                ).then(
                    function()
                    {
                        promise.succeed();
                    }
                ).exception(
                    function(ex)
                    {
                        promise.fail(ex);
                    }
                );
            }
            catch(ex)
            {
                promise.fail(ex);
            }
        });

        return promise;
    };

    global.BatchOneways = { run: run };
}(typeof (global) === "undefined" ? window : global));
