// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var require = typeof(module) !== "undefined" ? module.require : function(){};
    require("Ice/Ice");
    require("Glacier2/Glacier2");

    var Ice = this.Ice;
    
    require("Callback");
    var Test = this.Test;
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
        var failCB = function () { test(false); }

        setTimeout(function(){
            try
            {
                out.write("testing stringToProxy for router... ");
                var routerBase = communicator.stringToProxy("Glacier2/router:default -p 12347");
                test(routerBase !== null);
                out.writeLine("ok");
                
                var router, base, session, twoway, category, processBase, process;
                out.write("testing checked cast for router... ");
                Glacier2.RouterPrx.checkedCast(routerBase).then(
                    function(asyncResult, o)
                    {
                        router = o;
                        test(router !== null);
                        out.writeLine("ok");
                        
                        out.write("installing router with communicator... ");
                        communicator.setDefaultRouter(router);
                        out.writeLine("ok");
                        
                        out.write("getting the session timeout... ");
                        return router.getSessionTimeout();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, timeout)
                    {
                        test(timeout.low === 30);
                        out.writeLine("ok");
                        
                        out.write("testing stringToProxy for server object... ");
                        base = communicator.stringToProxy("c1/callback:tcp -p 12010");
                        out.writeLine("ok");
                        
                        out.write("trying to ping server before session creation... ");
                        return base.ice_ping();
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(ex instanceof Ice.ConnectionLostException))
                        {
                            throw ex;
                        }
                        out.writeLine("ok");
                        
                        out.write("trying to create session with wrong password... ");
                        return router.createSession("userid", "xxx");
                    }
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(ex instanceof Glacier2.PermissionDeniedException))
                        {
                            throw ex;
                        }
                        out.writeLine("ok");
                        
                        out.write("trying to destroy non-existing session... ");
                        return router.destroySession();
                    }
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(ex instanceof Glacier2.SessionNotExistException))
                        {
                            throw ex;
                        }
                        out.writeLine("ok");

                        out.write("creating session with correct password... ");
                        return router.createSession("userid", "abc123");
                    }
                ).then(
                    function(asyncResult, s)
                    {
                        session = s;
                        out.writeLine("ok");
                        
                        out.write("trying to create a second session... ");
                        return router.createSession("userid", "abc123");
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(ex instanceof Glacier2.CannotCreateSessionException))
                        {
                            throw ex;
                        }
                        out.writeLine("ok");
                        
                        out.write("pinging server after session creation... ");
                        return base.ice_ping();
                    }
                ).then(
                    function(asyncResult)
                    {
                        out.writeLine("ok");
                        
                        out.write("testing checked cast for server object... ");
                        return Test.CallbackPrx.checkedCast(base);
                    },
                    function(ex)
                    {
                        console.log(ex);
                        console.log(ex.stack);
                        proccess.exit(1);
                    }
                ).then(
                    function(asyncResult, o)
                    {
                        twoway = o;
                        test(twoway !== null);
                        out.writeLine("ok");
                        
                        out.write("getting category from router... ");
                        return router.getCategoryForClient();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult, c)
                    {
                        category = c;
                        out.writeLine("ok");
                        
                        out.write("testing server shutdown... ");
                        return twoway.shutdown();
                        // No ping, otherwise the router prints a warning message if it's
                        // started with --Ice.Warn.Connections.
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        out.writeLine("ok");
                        
                        out.write("destroying session... ");
                        return router.destroySession();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        out.writeLine("ok");
                        
                        out.write("trying to ping server after session destruction... ");
                        return base.ice_ping();
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(ex instanceof Ice.ConnectionLostException))
                        {
                            throw ex;
                        }
                        out.writeLine("ok");
                        
                        out.write("uninstalling router with communicator... ");
                        communicator.setDefaultRouter(null);
                        out.writeLine("ok");
                        
                        out.write("testing stringToProxy for process object... ");
                        processBase = communicator.stringToProxy("Glacier2/admin -f Process:tcp -h 127.0.0.1 -p 12348");
                        out.writeLine("ok");
                        
                        out.write("testing checked cast for admin object... ");
                        return Ice.ProcessPrx.checkedCast(processBase);
                    }
                ).then(
                    function(asyncResult, o)
                    {
                        process = o;
                        test(process !== null);
                        out.writeLine("ok");
                        
                        out.write("testing Glacier2 shutdown... ");
                        return process.shutdown();
                    },
                    exceptionCB
                ).then(
                    function(asyncResult)
                    {
                        return process.ice_ping();
                    },
                    exceptionCB
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(ex instanceof Ice.LocalException))
                        {
                            throw ex;
                        }
                        out.writeLine("ok");
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
                    id.properties.setProperty("Ice.Warn.Dispatch", "0");
                    id.properties.setProperty("Ice.Warn.Connections", "0");
                    id.properties.setProperty("Ice.Trace.Protocol", "0");
                    id.properties.setProperty("Ice.Trace.Network", "0");
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
    this.__test__ = run;
}());
