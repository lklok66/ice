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
        var Ice = global.Ice;
        var Test = global.Test;

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
                    p = p.ice_oneway();

                    p.ice_ping().then(
                        function(r)
                        {
                            try
                            {
                                p.ice_isA(Test.MyClass.ice_staticId());
                                test(false);
                            }
                            catch(ex)
                            {
                                // Expected: twoway proxy required
                            }
                            try
                            {
                                p.ice_id();
                                test(false);
                            }
                            catch(ex)
                            {
                                // Expected: twoway proxy required
                            }
                            try
                            {
                                p.ice_ids();
                                test(false);
                            }
                            catch(ex)
                            {
                                // Expected: twoway proxy required
                            }

                            return p.opVoid();
                        },
                        exceptionCB
                    ).then(
                        function(r)
                        {
                            return p.opIdempotent();
                        },
                        exceptionCB
                    ).then(
                        function(r)
                        {
                            return p.opNonmutating();
                        },
                        exceptionCB
                    ).then(
                        function(r)
                        {
                            try
                            {
                                p.opByte(0xff, 0x0f);
                                test(false);
                            }
                            catch(ex)
                            {
                                // Expected: twoway proxy required
                            }

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

        global.Oneways = { run: run };
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/operations"));
