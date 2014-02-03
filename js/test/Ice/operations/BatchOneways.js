// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var Ice = this.Ice;
    var Test = this.Test;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var run = function(communicator, log, p)
    {
        var promise = new Ice.Promise();

        //
        // Re-throw exception so it propagates to final exception
        // handler.
        //
        var exceptionCB = function(ex) { throw ex; };
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

                p.opByteSOneway(bs1).then(
                    function(r)
                    {
                        return p.opByteSOneway(bs2);
                    },
                    exceptionCB
                ).then(
                    function(r)
                    {
                        return p.opByteSOneway(bs3);
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        test(ex instanceof Ice.MemoryLimitException);

                        batch = p.ice_batchOneway();

                        for(var i = 0; i < 30; ++i)
                        {
                            batch.opByteSOneway(bs1).exception(failCB);
                        }

                        return batch.ice_getConnection();
                    }
                ).then(
                    function(r, con)
                    {
                        return con.flushBatchRequests();
                    },
                    exceptionCB
                ).then(
                    function(r)
                    {
                        batch2 = p.ice_batchOneway();

                        batch.ice_ping().exception(failCB);
                        batch2.ice_ping().exception(failCB);

                        return batch.ice_flushBatchRequests();
                    },
                    exceptionCB
                ).then(
                    function(r)
                    {
                        return batch.ice_getConnection();
                    },
                    exceptionCB
                ).then(
                    function(r, con)
                    {
                        return con.close(false);
                    },
                    exceptionCB
                ).then(
                    function(r)
                    {
                        batch.ice_ping().exception(failCB);
                        batch2.ice_ping().exception(failCB);

                        var identity = communicator.stringToIdentity("invalid");
                        batch3 = batch.ice_identity(identity);
                        batch3.ice_ping().exception(failCB);

                        return batch3.ice_flushBatchRequests();
                    },
                    exceptionCB
                ).then(
                    function(r)
                    {
                        // Make sure that a bogus batch request doesn't cause troubles to other ones.
                        batch3.ice_ping().exception(failCB);
                        batch.ice_ping().exception(failCB);

                        return batch.ice_flushBatchRequests();
                    },
                    exceptionCB
                ).then(
                    function(r)
                    {
                        promise.succeed();
                    },
                    exceptionCB
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

    this.BatchOneways = { run: run };
}());
