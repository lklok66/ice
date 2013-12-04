// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Promise = require("../../src/Ice/Promise");
var Debug = require("../../src/Ice/Debug");

    
var PromiseTest = {}

PromiseTest.run = function()
{
    function succeedPromise()
    {
       var r = new Promise();
       r.succeed(1024);
       return r;
    }
    
    function succeedPromiseMultipleArgs()
    {
       var r = new Promise();
       r.succeed(1024, "Hello World!");
       return r;
    }
    
    function failPromise()
    {
        var r = new Promise();
        r.fail("promise.fail");
        return r;
    }

    Promise.deferred(
        function(promise)
        {
            process.stdout.write("Creating a promise object that is resolved and succeed... ");
            var promise1 = succeedPromise();
            promise1.then(
                function (i)
                {
                    Debug.assert(i === 1024);
                    Debug.assert(promise1._state === Promise.StateSuccess);
                    console.log("ok");
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
                    process.stdout.write("Creating a promise object that is resolved and failed... ");
                    var promise1 = failPromise();
                    promise1.then(
                        function (i)
                        {
                            promise.fail();
                            Debug.assert(false, e);
                        },
                        function(e)
                        {
                            Debug.assert(e === "promise.fail");
                            Debug.assert(promise1._state === Promise.StateFailed);
                            console.log("ok");
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
                    process.stdout.write("Creating a promise object that is resolved and succeed with multiple arguments... ");
                    var promise1 = succeedPromiseMultipleArgs();
                    promise1.then(
                        function (i, msg)
                        {
                            Debug.assert(i === 1024);
                            Debug.assert(msg === "Hello World!");
                            Debug.assert(promise1._state === Promise.StateSuccess);
                            console.log("ok");
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
                    process.stdout.write("Creating a promise with a callback that returns a new value... ");
                    var promise1 = succeedPromise();
                    promise1.then(
                        function (i)
                        {
                            Debug.assert(i === 1024);
                            Debug.assert(promise1._state === Promise.StateSuccess);
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
                            console.log("ok");
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
                    process.stdout.write("Creating a promise object that recovers from a failure... ");
                    var promise1 = failPromise();
                    promise1.then(
                        function (i)
                        {
                            promise.fail();
                            Debug.assert(false, "Succeed called.failed expected");
                        },
                        function(e)
                        {
                            Debug.assert(e === "promise.fail");
                            Debug.assert(promise1._state === Promise.StateFailed);
                            return "Hello World!";
                        })
                    .then(
                        function(msg)
                        {
                            Debug.assert(msg === "Hello World!");
                            console.log("ok");
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
                    process.stdout.write("Creating a promise object that rethrow a.failure... ");
                    var promise1 = failPromise();
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
                            Debug.assert(promise1._state === Promise.StateFailed);
                            console.log("ok");
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
                    process.stdout.write("A second call to then should produce the same results... ");
                    var promise1 = succeedPromise();
                    promise1.then(
                        function (i)
                        {
                            Debug.assert(i === 1024);
                            Debug.assert(promise1._state === Promise.StateSuccess);
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
                            Debug.assert(promise1._state === Promise.StateSuccess);
                        },
                        function(e)
                        {
                            promise.fail();
                            Debug.assert(false, e);
                        });
                    
                    promise1 = failPromise();
                    promise1.then(
                        function (i)
                        {
                            promise.fail();
                            Debug.assert(false, e);
                        },
                        function(e)
                        {
                            Debug.assert(e === "promise.fail");
                            Debug.assert(promise1._state === Promise.StateFailed);
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
                            Debug.assert(promise1._state === Promise.StateFailed);
                            console.log("ok");
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
                    process.stdout.write("Create a promise that is not yet resolved, but will succeed... ");
                    var promise1 = new Promise();
                    Debug.assert(promise1._state === Promise.StatePending);
                    promise1.then(
                        function(i)
                        {
                            Debug.assert(i === 1024);
                            console.log("ok");
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
                    process.stdout.write("Create a promise that is not yet resolved, but will.fail... ");
                    var promise1 = new Promise();
                    Debug.assert(promise1._state === Promise.StatePending);
                    promise1.then(
                        function(i)
                        {
                            promise.fail();
                            Debug.assert(false, "Succeed called.failed expected");
                        },
                        function(e)
                        {
                            Debug.assert(e === "promise.fail");
                            Debug.assert(promise1._state === Promise.StateFailed);
                            console.log("ok");
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
                    process.stdout.write("Create a promise chain that is not yet resolved, but will succeed... ");
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
                            Debug.assert(promise1._state === Promise.StateSuccess &&
                                         promise2._state === Promise.StateSuccess &&
                                         promise3._state === Promise.StateSuccess);
                            Debug.assert(msg === "Hello World!");
                            console.log("ok");
                            promise.succeed();
                        },
                        function(e)
                        {
                            promise.fail();
                            Debug.assert(false, e);
                        }
                    );
                    Debug.assert(promise1._state === Promise.StatePending &&
                                 promise2._state === Promise.StatePending &&
                                 promise3._state === Promise.StatePending);
                    
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
                    process.stdout.write("Use exception method on a Promise that will.fail... ");
                    var promise1 = failPromise();
                    promise1.exception(
                        function(e)
                        {
                            Debug.assert(e === "promise.fail");
                            console.log("ok");
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
                    process.stdout.write("Promise exception propagation... ");
                    var promise1 = failPromise();
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
                            console.log("ok");
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
                    process.stdout.write("Use Promise.all to wait for several promises and all succeed... ");
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
                            
                            console.log("ok");
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
                    process.stdout.write("Use Promise.all to wait for several promises and one fails... ");
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
                            console.log("ok");
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
                    process.stdout.write("Test Promise progress callback... ");
                    
                    var promise1  = new Promise();
                    var p1, p2, p3;
                    
                    promise1.then(
                        function()
                        {
                            Debug.assert(p1 === 33);
                            Debug.assert(p2 === 66);
                            Debug.assert(p3 === 99);
                            console.log("ok");
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
    );
}

PromiseTest.run();
