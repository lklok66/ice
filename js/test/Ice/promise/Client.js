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
        require("Ice/Debug");
        
        var Ice = global.Ice || {};
        
        var Debug = Ice.Debug;
        var Promise = Ice.Promise;
        
        var run = function(out)
        {
            var p = new Promise();
            
            Promise.deferred(
                function(promise)
                {
                    out.write("Creating a promise object that is resolved and succeed... ");
                    var promise1 = Promise.succeed(1024);
                    promise1.then(
                        function (i)
                        {
                            Debug.assert(i === 1024);
                            Debug.assert(promise1.succeeded());
                            out.writeLine("ok");
                            promise.succeed();
                        },
                        function(e)
                        {
                            promise.fail();
                            Debug.assert(false, e);
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Creating a promise object that is resolved and failed... ");
                            var promise1 = Promise.fail("promise.fail");
                            promise1.then(
                                function (i)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                },
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    Debug.assert(promise1.failed());
                                    out.writeLine("ok");
                                    promise.succeed();
                                });
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Creating a promise object that is resolved and succeed with multiple arguments... ");
                            var promise1 = Promise.succeed(1024, "Hello World!");
                            promise1.then(
                                function (i, msg)
                                {
                                    Debug.assert(i === 1024);
                                    Debug.assert(msg === "Hello World!");
                                    Debug.assert(promise1.succeeded());
                                    out.writeLine("ok");
                                    promise.succeed();
                                },
                                function(e)
                                {
                                    promise.fail(e);
                                    Debug.assert(false, e);
                                });
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Creating a promise with a callback that returns a new value... ");
                            var promise1 = Promise.succeed(1024);
                            promise1.then(
                                function (i)
                                {
                                    Debug.assert(i === 1024);
                                    Debug.assert(promise1.succeeded());
                                    return "Hello World!";
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                })
                            .then(
                                function(msg)
                                {
                                    Debug.assert(msg === "Hello World!");
                                    out.writeLine("ok");
                                    promise.succeed();
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                });
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Creating a promise object that recovers from a failure... ");
                            var promise1 = Promise.fail("promise.fail");
                            promise1.then(
                                function (i)
                                {
                                    promise.fail();
                                    Debug.assert(false, "Succeed called.failed expected");
                                },
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    Debug.assert(promise1.failed());
                                    return "Hello World!";
                                })
                            .then(
                                function(msg)
                                {
                                    Debug.assert(msg === "Hello World!");
                                    out.writeLine("ok");
                                    promise.succeed();
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                });
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Creating a promise object that rethrow a.failure... ");
                            var promise1 = Promise.fail("promise.fail");
                            promise1.then(
                                function (i)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                },
                                function(e)
                                {
                                    throw e;
                                })
                            .then(
                                function(msg)
                                {
                                    promise.fail();
                                    Debug.assert(false, "Succeed called.failed expected");
                                    
                                },
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    Debug.assert(promise1.failed());
                                    out.writeLine("ok");
                                    promise.succeed();
                                });
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("A second call to then should produce the same results... ");
                            var promise1 = Promise.succeed(1024);
                            promise1.then(
                                function (i)
                                {
                                    Debug.assert(i === 1024);
                                    Debug.assert(promise1.succeeded());
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                });
                            promise1.then(
                                function (i)
                                {
                                    Debug.assert(i === 1024);
                                    Debug.assert(promise1.succeeded());
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                });
                            
                            promise1 = Promise.fail("promise.fail");
                            promise1.then(
                                function (i)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                },
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    Debug.assert(promise1.failed());
                                });
                            promise1.then(
                                function (i)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                },
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    Debug.assert(promise1.failed());
                                    out.writeLine("ok");
                                    promise.succeed();
                                });
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Create a promise that is not yet resolved, but will succeed... ");
                            var promise1 = new Promise();
                            Debug.assert(!promise1.completed());
                            promise1.then(
                                function(i)
                                {
                                    Debug.assert(i === 1024);
                                    out.writeLine("ok");
                                    promise.succeed();
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                }
                            );
                            promise1.succeed(1024);
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Create a promise that is not yet resolved, but will.fail... ");
                            var promise1 = new Promise();
                            Debug.assert(!promise1.completed());
                            promise1.then(
                                function(i)
                                {
                                    promise.fail();
                                    Debug.assert(false, "Succeed called.failed expected");
                                },
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    Debug.assert(promise1.failed());
                                    out.writeLine("ok");
                                    promise.succeed();
                                }
                            );
                            promise1.fail("promise.fail");
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Create a promise chain that is not yet resolved, but will succeed... ");
                            var promise1 = new Promise();
                            var promise2 = new Promise();
                            var promise3 = new Promise();
                            promise1.then(
                                function(i)
                                {
                                    Debug.assert(i === 1);
                                    return promise2;
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                }
                            ).then(
                                function(i)
                                {
                                    Debug.assert(i === 2);
                                    return promise3;
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                }
                            ).then(
                                function(i)
                                {
                                    Debug.assert(i === 3);
                                    return "Hello World!";
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                }
                            ).then(
                                function(msg)
                                {
                                    Debug.assert(promise1.succeeded() && promise2.succeeded() && promise3.succeeded());
                                    Debug.assert(msg === "Hello World!");
                                    out.writeLine("ok");
                                    promise.succeed();
                                },
                                function(e)
                                {
                                    promise.fail();
                                    Debug.assert(false, e);
                                }
                            );
                            Debug.assert(!promise1.completed() && !promise2.completed() && !promise3.completed());
                            
                            promise1.succeed(1);
                            promise2.succeed(2);
                            promise3.succeed(3);
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Use exception method on a Promise that will.fail... ");
                            var promise1 = Promise.fail("promise.fail");
                            promise1.exception(
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    out.writeLine("ok");
                                    promise.succeed();
                                }
                            );
                        });
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Promise exception propagation... ");
                            var promise1 = Promise.fail("promise.fail");
                            promise1.then(
                                function()
                                {
                                    promise.fail();
                                    Debug.assert(false, "response callback called but exception expected");
                                }
                            ).exception(
                                function(e)
                                {
                                    //
                                    // since no exception handler was passed to the first `.then`, the error propagates.
                                    //
                                    Debug.assert(e === "promise.fail");
                                    out.writeLine("ok");
                                    promise.succeed();
                                });
                        }
                    );
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Use Promise.all to wait for several promises and all succeed... ");
                            var promise1 = new Promise();
                            var promise2 = new Promise();
                            var promise3 = new Promise();
                            
                            Promise.all(promise1, promise2, promise3).then(
                                function(r1, r2, r3)
                                {
                                    Debug.assert(r1.length === 1);
                                    Debug.assert(r1[0] === 1024);
                                    
                                    Debug.assert(r2.length === 2);
                                    Debug.assert(r2[0] === 1024);
                                    Debug.assert(r2[1] === 2048);
                                    
                                    Debug.assert(r3.length === 3);
                                    Debug.assert(r3[0] === 1024);
                                    Debug.assert(r3[1] === 2048);
                                    Debug.assert(r3[2] === 4096);
                                    
                                    out.writeLine("ok");
                                    promise.succeed();
                                },
                                function()
                                {
                                    promise.fail();
                                    Debug.assert(false);
                                }
                            );
                            
                            //
                            // Now resolve the promise in the reverse order, all succeed callback
                            // will get the result in the right order.
                            //
                            promise3.succeed(1024, 2048, 4096);
                            promise2.succeed(1024, 2048);
                            promise1.succeed(1024);
                        }
                    );
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Use Promise.all to wait for several promises and one fails... ");
                            var promise1 = new Promise();
                            var promise2 = new Promise();
                            var promise3 = new Promise();
                            
                            Promise.all(promise1, promise2, promise3).then(
                                function(r1, r2, r3)
                                {
                                    promise.fail();
                                },
                                function(e)
                                {
                                    Debug.assert(e === "promise.fail");
                                    out.writeLine("ok");
                                    promise.succeed();
                                }
                            );
                            
                            //
                            // Now resolve the promise in the reverse order.
                            //
                            promise3.succeed(1024, 2048, 4096);
                            promise2.succeed(1024, 2048);
                            promise1.fail("promise.fail");
                        }
                    );
                })
            .then(
                function()
                {
                    return Promise.deferred(
                        function(promise)
                        {
                            out.write("Test Promise progress callback... ");
                            
                            var promise1  = new Promise();
                            var p1, p2, p3;
                            
                            promise1.then(
                                function()
                                {
                                    Debug.assert(p1 === 33);
                                    Debug.assert(p2 === 66);
                                    Debug.assert(p3 === 99);
                                    out.writeLine("ok");
                                    promise.succeed();
                                },
                                function()
                                {
                                    promise.fail();
                                    Debug.assert(false);
                                },
                                function(p)
                                {
                                    // Porgress callback.
                                    if(!p1)
                                    {
                                        p1 = p;
                                        Debug.assert(p1 === 33);
                                        Debug.assert(!p2);
                                        Debug.assert(!p3);
                                    }
                                    else if (!p2)
                                    {
                                        p2 = p;
                                        Debug.assert(p2 === 66);
                                        Debug.assert(!p3);
                                    }
                                    else if(!p3)
                                    {
                                        p3 = p;
                                        Debug.assert(p3 === 99);
                                        promise1.succeed();
                                    }
                                });
                            promise1.progress(33);
                            promise1.progress(66);
                            promise1.progress(99);
                        }
                    );
                }
            ).then(
                function(){
                    p.succeed();
                },
                function(ex){
                    p.fail(ex);
                }
            );
            return p;
        };
        
        global.test = global.test || {};
        global.test.Common = global.test.Common || {};
        
        if(global.test.Common.TestSuite !== undefined)
        {
            global.test.Common.TestSuite.add("Ice/promise", run);
        }
        
        global.test.Ice = global.test.Ice || {};
        global.test.Ice.promise = {run: run};
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/promise"));
