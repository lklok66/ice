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
    require("Ice/Debug");
    
    var Ice = global.Ice || {};
    
    var Debug = Ice.Debug;
    var Promise = Ice.Promise;
    
    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };
    
    //
    // Create a new promise object and call function fn with
    // the promise as its first argument, then return the new
    // promise.
    //
    var deferred = function(fn)
    {
        var promise = new Promise();
        fn.call(null, promise);
        return promise;
    };
    
    var run = function(out)
    {
        var p = new Promise();
        
        deferred(
            function(promise)
            {
                out.write("Creating a promise object that is resolved and succeed... ");
                var promise1 = Promise.succeed(1024);
                promise1.then(
                    function (i)
                    {
                        test(i === 1024);
                        test(promise1.succeeded());
                        out.writeLine("ok");
                        promise.succeed();
                    },
                    function(e)
                    {
                        promise.fail();
                        test(false, e);
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Creating a promise object that is resolved and failed... ");
                        var promise1 = Promise.fail("promise.fail");
                        promise1.then(
                            function (i)
                            {
                                promise.fail();
                                test(false, e);
                            },
                            function(e)
                            {
                                test(e === "promise.fail");
                                test(promise1.failed());
                                out.writeLine("ok");
                                promise.succeed();
                            });
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Creating a promise object that is resolved and succeed with multiple arguments... ");
                        var promise1 = Promise.succeed(1024, "Hello World!");
                        promise1.then(
                            function (i, msg)
                            {
                                test(i === 1024);
                                test(msg === "Hello World!");
                                test(promise1.succeeded());
                                out.writeLine("ok");
                                promise.succeed();
                            },
                            function(e)
                            {
                                promise.fail(e);
                                test(false, e);
                            });
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Creating a promise with a callback that returns a new value... ");
                        var promise1 = Promise.succeed(1024);
                        promise1.then(
                            function (i)
                            {
                                test(i === 1024);
                                test(promise1.succeeded());
                                return "Hello World!";
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            })
                        .then(
                            function(msg)
                            {
                                test(msg === "Hello World!");
                                out.writeLine("ok");
                                promise.succeed();
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            });
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Creating a promise object that recovers from a failure... ");
                        var promise1 = Promise.fail("promise.fail");
                        promise1.then(
                            function (i)
                            {
                                promise.fail();
                                test(false, "Succeed called.failed expected");
                            },
                            function(e)
                            {
                                test(e === "promise.fail");
                                test(promise1.failed());
                                return "Hello World!";
                            })
                        .then(
                            function(msg)
                            {
                                test(msg === "Hello World!");
                                out.writeLine("ok");
                                promise.succeed();
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            });
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Creating a promise object that rethrow a.failure... ");
                        var promise1 = Promise.fail("promise.fail");
                        promise1.then(
                            function (i)
                            {
                                promise.fail();
                                test(false, e);
                            },
                            function(e)
                            {
                                throw e;
                            })
                        .then(
                            function(msg)
                            {
                                promise.fail();
                                test(false, "Succeed called.failed expected");
                                
                            },
                            function(e)
                            {
                                test(e === "promise.fail");
                                test(promise1.failed());
                                out.writeLine("ok");
                                promise.succeed();
                            });
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("A second call to then should produce the same results... ");
                        var promise1 = Promise.succeed(1024);
                        promise1.then(
                            function (i)
                            {
                                test(i === 1024);
                                test(promise1.succeeded());
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            });
                        promise1.then(
                            function (i)
                            {
                                test(i === 1024);
                                test(promise1.succeeded());
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            });
                        
                        promise1 = Promise.fail("promise.fail");
                        promise1.then(
                            function (i)
                            {
                                promise.fail();
                                test(false, e);
                            },
                            function(e)
                            {
                                test(e === "promise.fail");
                                test(promise1.failed());
                            });
                        promise1.then(
                            function (i)
                            {
                                promise.fail();
                                test(false, e);
                            },
                            function(e)
                            {
                                test(e === "promise.fail");
                                test(promise1.failed());
                                out.writeLine("ok");
                                promise.succeed();
                            });
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Create a promise that is not yet resolved, but will succeed... ");
                        var promise1 = new Promise();
                        test(!promise1.completed());
                        promise1.then(
                            function(i)
                            {
                                test(i === 1024);
                                out.writeLine("ok");
                                promise.succeed();
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            }
                        );
                        promise1.succeed(1024);
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Create a promise that is not yet resolved, but will.fail... ");
                        var promise1 = new Promise();
                        test(!promise1.completed());
                        promise1.then(
                            function(i)
                            {
                                promise.fail();
                                test(false, "Succeed called.failed expected");
                            },
                            function(e)
                            {
                                test(e === "promise.fail");
                                test(promise1.failed());
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
                return deferred(
                    function(promise)
                    {
                        out.write("Create a promise chain that is not yet resolved, but will succeed... ");
                        var promise1 = new Promise();
                        var promise2 = new Promise();
                        var promise3 = new Promise();
                        promise1.then(
                            function(i)
                            {
                                test(i === 1);
                                return promise2;
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            }
                        ).then(
                            function(i)
                            {
                                test(i === 2);
                                return promise3;
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            }
                        ).then(
                            function(i)
                            {
                                test(i === 3);
                                return "Hello World!";
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            }
                        ).then(
                            function(msg)
                            {
                                test(promise1.succeeded() && promise2.succeeded() && promise3.succeeded());
                                test(msg === "Hello World!");
                                out.writeLine("ok");
                                promise.succeed();
                            },
                            function(e)
                            {
                                promise.fail();
                                test(false, e);
                            }
                        );
                        test(!promise1.completed() && !promise2.completed() && !promise3.completed());
                        
                        promise1.succeed(1);
                        promise2.succeed(2);
                        promise3.succeed(3);
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Use exception method on a Promise that will.fail... ");
                        var promise1 = Promise.fail("promise.fail");
                        promise1.exception(
                            function(e)
                            {
                                test(e === "promise.fail");
                                out.writeLine("ok");
                                promise.succeed();
                            }
                        );
                    });
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Promise exception propagation... ");
                        var promise1 = Promise.fail("promise.fail");
                        promise1.then(
                            function()
                            {
                                promise.fail();
                                test(false, "response callback called but exception expected");
                            }
                        ).exception(
                            function(e)
                            {
                                //
                                // since no exception handler was passed to the first `.then`, the error propagates.
                                //
                                test(e === "promise.fail");
                                out.writeLine("ok");
                                promise.succeed();
                            });
                    }
                );
            })
        .then(
            function()
            {
                return deferred(
                    function(promise)
                    {
                        out.write("Use Promise.all to wait for several promises and all succeed... ");
                        var promise1 = new Promise();
                        var promise2 = new Promise();
                        var promise3 = new Promise();
                        
                        Promise.all(promise1, promise2, promise3).then(
                            function(r1, r2, r3)
                            {
                                test(r1.length === 1);
                                test(r1[0] === 1024);
                                
                                test(r2.length === 2);
                                test(r2[0] === 1024);
                                test(r2[1] === 2048);
                                
                                test(r3.length === 3);
                                test(r3[0] === 1024);
                                test(r3[1] === 2048);
                                test(r3[2] === 4096);
                                
                                out.writeLine("ok");
                                promise.succeed();
                            },
                            function()
                            {
                                promise.fail();
                                test(false);
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
                return deferred(
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
                                test(e === "promise.fail");
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
                return deferred(
                    function(promise)
                    {
                        out.write("Test Promise progress callback... ");
                        
                        var promise1  = new Promise();
                        var p1, p2, p3;
                        
                        promise1.then(
                            function()
                            {
                                test(p1 === 33);
                                test(p2 === 66);
                                test(p3 === 99);
                                out.writeLine("ok");
                                promise.succeed();
                            },
                            function()
                            {
                                promise.fail();
                                test(false);
                            },
                            function(p)
                            {
                                // Porgress callback.
                                if(!p1)
                                {
                                    p1 = p;
                                    test(p1 === 33);
                                    test(!p2);
                                    test(!p3);
                                }
                                else if (!p2)
                                {
                                    p2 = p;
                                    test(p2 === 66);
                                    test(!p3);
                                }
                                else if(!p3)
                                {
                                    p3 = p;
                                    test(p3 === 99);
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
    global.__test__ = run;
}(typeof (global) === "undefined" ? window : global));
