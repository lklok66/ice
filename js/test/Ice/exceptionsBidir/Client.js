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
    var Promise = Ice.Promise;

    require("Test");
    require("TestAMD");
    var Test = global.Test;

    require("ThrowerI");
    var ThrowerI = global.ThrowerI;
    require("AMDThrowerI");
    var AMDThrowerI = global.AMDThrowerI;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var allTests = function(out, communicator, amd)
    {
        var p = new Promise();

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

                var Test = amd ? global.TestAMD : global.Test;

                var adapter;
                var thrower;

                communicator.createObjectAdapter("").then(
                    function(asyncResult, a)
                    {
                        adapter = a;
                        if(amd)
                        {
                            adapter.add(new AMDThrowerI(), communicator.stringToIdentity("thrower"));
                        }
                        else
                        {
                            adapter.add(new ThrowerI(), communicator.stringToIdentity("thrower"));
                        }

                        return adapter.activate();
                    }
                ).then(
                    function(asyncResult)
                    {
                        return base.ice_getConnection();
                    }
                ).then(
                    function(asyncResult, conn)
                    {
                        conn.setAdapter(adapter);
                        out.write("testing checked cast... ");
                        return Test.ThrowerPrx.checkedCast(base);
                    }
                ).then(
                    function(asyncResult, obj)
                    {
                        thrower = obj;
                        test(thrower !== null);
                        test(thrower.equals(base));
                        out.writeLine("ok");
                        out.write("catching exact types... ");
                        return thrower.throwAasA(1);
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
                    }
                ).then(
                    function()
                    {
                        out.write("testing memory limit marshal exception...");
                        return thrower.throwMemoryLimitException(null);
                    }
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
                    }
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
                    }
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
                    id.properties.setProperty("Ice.MessageSizeMax", "10");
                    id.properties.setProperty("Ice.Warn.Dispatch", "0");
                    c = Ice.initialize(id);
                    out.writeLine("testing bidir callbacks with synchronous dispatch...");
                    allTests(out, c, false).then(function(){
                            return c.destroy();
                        }).then(function(){
                            c = Ice.initialize(id);
                            out.writeLine("testing bidir callbacks with asynchronous dispatch...");
                            return allTests(out, c, true);
                        }).then(function(){
                            return c.destroy();
                        }).then(function(){
                            c = Ice.initialize(id);
                            var ref = "__echo:default -p 12010";
                            var base = c.stringToProxy(ref);
                            return global.Test.EchoPrx.checkedCast(base);
                        }).then(function(r, prx){
                            return prx.shutdown();
                        }).then(function(){
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
