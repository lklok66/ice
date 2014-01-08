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
            // re-throw exception so it propagates to final exception
            // handler.
            //
            var exceptionCB = function(ex){ throw ex; };
            var failCB = function(){ test(false); };
            
            var supportsUndeclaredExceptions = function(thrower)
            {
                var p = new Promise();
                setTimeout(function(){
                    thrower.supportsUndeclaredExceptions().then(
                        function(asyncResult, v)
                        {
                            if(v)
                            {
                                out.write("catching unknown user exception... ");
                                thrower.throwUndeclaredA(1).then(
                                    failCB,
                                    function(ex)
                                    {
                                        if(!(ex instanceof Ice.UnknownUserException))
                                        {
                                            throw ex;
                                        }
                                        return thrower.throwUndeclaredB(1, 2);
                                    }
                                ).then(
                                    failCB,
                                    function(ex)
                                    {
                                        if(!(ex instanceof Ice.UnknownUserException))
                                        {
                                            throw ex;
                                        }
                                        return thrower.throwUndeclaredC(1, 2, 3);
                                    }
                                ).then(
                                    failCB,
                                    function(ex)
                                    {
                                        if(!(ex instanceof Ice.UnknownUserException))
                                        {
                                            throw ex;
                                        }
                                        out.writeLine("ok");
                                        p.succeed();
                                    }
                                ).exception(
                                    function(ex)
                                    {
                                        p.fail(ex)
                                    }
                                );
                            }
                            else
                            {
                                p.succeed();
                            }
                        });
                });
                return p;
            };
            
            var supportsAssertException = function(thrower)
            {
                var p = new Promise();
                setTimeout(function(){
                    thrower.supportsAssertException().then(
                        function(asyncResult, v)
                        {
                            if(v)
                            {
                                out.write("testing assert in the server... ");
                                thrower.throwAssertException().then(
                                    failCB,
                                    function(ex)
                                    {
                                        if(!(ex instanceof Ice.ConnectionLostException))
                                        {
                                            throw ex;
                                        }
                                        out.writeLine("ok");
                                        p.succeed();
                                    }
                                ).exception(
                                    function(ex)
                                    {
                                        p.fail(ex)
                                    }
                                );
                            }
                            else
                            {
                                p.succeed();
                            }
                        });
                });
                return p;
            };
            
            setTimeout(function(){
                try
                {
                    out.write("testing stringToProxy... ");
                    var ref = "thrower:default -p 12010";
                    var base = communicator.stringToProxy(ref);
                    test(base !== null);
                    out.writeLine("ok");
                    
                    var thrower;
                    out.write("testing checked cast... ");
                    Test.ThrowerPrx.checkedCast(base).then(
                        function(asyncResult, obj)
                        {
                            thrower = obj;
                            test(thrower !== null);
                            test(thrower.equals(base));
                            out.writeLine("ok");
                            out.write("catching exact types... ");
                            return thrower.throwAasA(1);
                        },
                        exceptionCB
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.A)
                            {
                                test(ex.aMem === 1);
                            }
                            else
                            {
                                throw ex;
                            }
                            return thrower.throwAorDasAorD(1);
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.A)
                            {
                                test(ex.aMem === 1);
                            }
                            else
                            {
                                throw ex;
                            }
                            return thrower.throwAorDasAorD(-1);
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.D)
                            {
                                test(ex.dMem === -1);
                            }
                            else
                            {
                                throw ex;
                            }
                            return thrower.throwBasB(1, 2);
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.B)
                            {
                                test(ex.aMem == 1);
                                test(ex.bMem == 2);
                            }
                            else
                            {
                                throw ex;
                            }
                            return thrower.throwCasC(1, 2, 3);
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.C)
                            {
                                test(ex.aMem == 1);
                                test(ex.bMem == 2);
                                test(ex.cMem == 3);
                            }
                            else
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("catching base types... ");
                            return thrower.throwBasB(1, 2);
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.A)
                            {
                                test(ex.aMem == 1);
                            }
                            else
                            {
                                throw ex;
                            }
                            return thrower.throwCasC(1, 2, 3);
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.B)
                            {
                                test(ex.aMem == 1);
                                test(ex.bMem == 2);
                            }
                            else
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("catching derived types... ");
                            return thrower.throwBasA(1, 2);
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.B)
                            {
                                test(ex.aMem == 1);
                                test(ex.bMem == 2);
                                return thrower.throwCasA(1, 2, 3);
                            }
                            else
                            {
                                throw ex;
                            }
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.C)
                            {
                                test(ex.aMem == 1);
                                test(ex.bMem == 2);
                                test(ex.cMem == 3);
                                return thrower.throwCasB(1, 2, 3);
                            }
                            else
                            {
                                throw ex;
                            }
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Test.C)
                            {
                                test(ex.aMem == 1);
                                test(ex.bMem == 2);
                                test(ex.cMem == 3);
                            }
                            else
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            return supportsUndeclaredExceptions(thrower);
                        }
                    ).then(
                        function()
                        {
                            return supportsAssertException(thrower);
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            out.write("testing memory limit marshal exception...");
                            return thrower.throwMemoryLimitException(null);
                        },
                        exceptionCB
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.UnknownLocalException))
                            {
                                throw ex;
                            }
                            var data = new Array(20 * 1024);
                            return thrower.throwMemoryLimitException(data); 
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.MemoryLimitException))
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("catching object not exist exception... ");
                            var id = communicator.stringToIdentity("does not exist");
                            var thrower2 = Test.ThrowerPrx.uncheckedCast(thrower.ice_identity(id));
                            return thrower2.ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Ice.ObjectNotExistException)
                            {
                                test(ex.id.equals(communicator.stringToIdentity("does not exist")));
                            }
                            else
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("catching facet not exist exception... ");
                            var thrower2 = Test.ThrowerPrx.uncheckedCast(thrower, "no such facet");
                            return thrower2.ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Ice.FacetNotExistException)
                            {
                                test(ex.facet == "no such facet");
                            }
                            else
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("catching operation not exist exception... ");
                            var thrower2 = Test.WrongOperationPrx.uncheckedCast(thrower);
                            return thrower2.noSuchOperation();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(ex instanceof Ice.OperationNotExistException)
                            {
                                test(ex.operation == "noSuchOperation");
                            }
                            else
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("catching unknown local exception... ");
                            return thrower.throwLocalException();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.UnknownLocalException))
                            {
                                throw ex;
                            }
                            return thrower.throwLocalExceptionIdempotent();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.UnknownLocalException) &&
                               !(ex instanceof Ice.OperationNotExistException))
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("catching unknown non-Ice exception... ");
                            return thrower.throwNonIceException();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.UnknownException))
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("testing asynchronous exceptions... ");
                            return thrower.throwAfterResponse();
                        }
                    ).then(
                        function(asyncResult)
                        {
                            return thrower.throwAfterException();
                        },
                        exceptionCB
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Test.A))
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            return thrower.shutdown();
                        }
                    ).then(
                        function()
                        {
                            p.succeed();
                        },
                        exceptionCB
                    ).exception(
                        function(ex)
                        {
                            console.log(ex);
                            if(ex && ex._asyncResult)
                            {
                                out.writeLine("\nexception occurred in call to " + ex._asyncResult.operation);
                            }
                            
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
        module.exports.test = module.exports.test || {};
        module.exports.test.Common = module.exports.test.Common || {};
        
        if(module.exports.test.Common.TestSuite !== undefined)
        {
            module.exports.test.Common.TestSuite.add("Ice/exceptions", run);
        }
        
        module.exports.test.Ice = module.exports.test.Ice || {};
        module.exports.test.Ice.exceptions = {run: run};
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/exceptions"));