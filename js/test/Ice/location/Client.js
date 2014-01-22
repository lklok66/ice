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
            var failCB = function(asyncResult) { test(false); }

            setTimeout(function(){
                try
                {
                    var manager = Test.ServerManagerPrx.uncheckedCast(
                                    communicator.stringToProxy("ServerManager:default -p 12010"));
                    test(manager !== null);
                    
                    var locator = Test.TestLocatorPrx.uncheckedCast(communicator.getDefaultLocator());
                    test(locator !== null);

                    var registry;
                    
                    var base, base2, base3, base4, base5, base6;
                    var bases;
                    var obj, obj2, obj3, obj4, obj5, obj6;
                    var hello;
                    
                    locator.getRegistry().then(
                        function(asyncResult, obj)
                        {
                            registry = Test.TestLocatorRegistryPrx.uncheckedCast(obj);
                            test(registry !== null);

                            out.write("testing stringToProxy... ");
                            base = communicator.stringToProxy("test @ TestAdapter");
                            base2 = communicator.stringToProxy("test @ TestAdapter");
                            base3 = communicator.stringToProxy("test");
                            base4 = communicator.stringToProxy("ServerManager");
                            base5 = communicator.stringToProxy("test2");
                            base6 = communicator.stringToProxy("test @ ReplicatedAdapter");
                            bases = [base, base2, base3, base4, base5, base6];
                            out.writeLine("ok");
                            
                            out.write("testing ice_locator and ice_getLocator... ");
                            test(Ice.proxyIdentityCompare(base.ice_getLocator(), communicator.getDefaultLocator()) === 0);
                            var anotherLocator = 
                                Ice.LocatorPrx.uncheckedCast(communicator.stringToProxy("anotherLocator"));
                            base = base.ice_locator(anotherLocator);
                            test(Ice.proxyIdentityCompare(base.ice_getLocator(), anotherLocator) === 0);
                            communicator.setDefaultLocator(null);
                            base = communicator.stringToProxy("test @ TestAdapter");
                            test(base.ice_getLocator() === null);
                            base = base.ice_locator(anotherLocator);
                            test(Ice.proxyIdentityCompare(base.ice_getLocator(), anotherLocator) === 0);
                            communicator.setDefaultLocator(locator);
                            base = communicator.stringToProxy("test @ TestAdapter");
                            test(Ice.proxyIdentityCompare(base.ice_getLocator(), communicator.getDefaultLocator()) === 0);
                            
                            //
                            // We also test ice_router/ice_getRouter (perhaps we should add a
                            // test/Ice/router test?)
                            //
                            test(base.ice_getRouter() === null);
                            var anotherRouter = Ice.RouterPrx.uncheckedCast(communicator.stringToProxy("anotherRouter"));
                            base = base.ice_router(anotherRouter);
                            test(Ice.proxyIdentityCompare(base.ice_getRouter(), anotherRouter) === 0);
                            var router = Ice.RouterPrx.uncheckedCast(communicator.stringToProxy("dummyrouter"));
                            communicator.setDefaultRouter(router);
                            base = communicator.stringToProxy("test @ TestAdapter");
                            test(Ice.proxyIdentityCompare(base.ice_getRouter(), communicator.getDefaultRouter()) === 0);
                            communicator.setDefaultRouter(null);
                            base = communicator.stringToProxy("test @ TestAdapter");
                            test(base.ice_getRouter() == null);
                            out.writeLine("ok");
                            
                            out.write("starting server... ");
                            return manager.startServer();
                            
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing checked cast... ");
                            
                            return Test.TestIntfPrx.checkedCast(base);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            obj = o;
                            test(obj !== null);
                            return Test.TestIntfPrx.checkedCast(base2);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            obj2 = o;
                            test(obj2 !== null);
                            return Test.TestIntfPrx.checkedCast(base3);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            obj3 = o;
                            test(obj3 !== null);
                            return Test.ServerManagerPrx.checkedCast(base4);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            obj4 = o;
                            test(obj4 !== null);
                            return Test.TestIntfPrx.checkedCast(base5);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            obj5 = o;
                            test(obj !== null);
                            return Test.TestIntfPrx.checkedCast(base6);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            obj6 = o;
                            test(obj6 !== null);
                            out.writeLine("ok");
                            out.write("testing id@AdapterId indirect proxy... ");
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj2.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing id@ReplicaGroupId indirect proxy... ");
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            
                            return obj6.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing identity indirect proxy... ");
                            obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj3.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj2.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj2.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj3.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj2.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj3.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj2.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj5.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing proxy with unknown identity... ");
                            base = communicator.stringToProxy("unknown/unknown");
                            return base.ice_ping();
                        },
                        exceptionCB
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.NotRegisteredException))
                            {
                                throw ex;
                            }
                            test(ex.kindOfObject == "object");
                            test(ex.id == "unknown/unknown");
                            out.writeLine("ok");
                            out.write("testing proxy with unknown adapter... ");
                            base = communicator.stringToProxy("test @ TestAdapterUnknown");
                            return base.ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.NotRegisteredException))
                            {
                                throw ex;
                            }
                            test(ex.kindOfObject == "object adapter");
                            test(ex.id == "TestAdapterUnknown");
                            out.writeLine("ok");
                            out.write("testing locator cache timeout... ");
                            return locator.getRequestCount();
                        }
                    ).then(
                        function(asyncResult, count)
                        {
                            var p = new Promise();
                            // No locator cache.
                            communicator.stringToProxy("test@TestAdapter").ice_locatorCacheTimeout(0).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(++count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // No locator cache.
                            communicator.stringToProxy("test@TestAdapter").ice_locatorCacheTimeout(0).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(++count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // 1s timeout.
                            communicator.stringToProxy("test@TestAdapter").ice_locatorCacheTimeout(1).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count == newCount);
                                    setTimeout(
                                        function(){
                                            p.succeed(count);
                                        }, 1200);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // 1s timeout.
                            communicator.stringToProxy("test@TestAdapter").ice_locatorCacheTimeout(1).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(++count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // No locator cache.
                            communicator.stringToProxy("test").ice_locatorCacheTimeout(0).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    count += 2;
                                    test(count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // 1s timeout.
                            communicator.stringToProxy("test").ice_locatorCacheTimeout(1).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    setTimeout(
                                        function(){
                                            p.fail(ex);
                                        }, 1200);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // No locator cache.
                            communicator.stringToProxy("test").ice_locatorCacheTimeout(0).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    count += 2;
                                    test(count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // 1s timeout.
                            communicator.stringToProxy("test@TestAdapter").ice_locatorCacheTimeout(-1).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // 1s timeout.
                            communicator.stringToProxy("test").ice_locatorCacheTimeout(-1).ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // 1s timeout.
                            communicator.stringToProxy("test@TestAdapter").ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            // 1s timeout.
                            communicator.stringToProxy("test").ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            test(communicator.stringToProxy("test").ice_locatorCacheTimeout(99).ice_getLocatorCacheTimeout() === 99);
                            out.writeLine("ok");
                            out.write("testing proxy from server... ");
                            return Test.TestIntfPrx.checkedCast(communicator.stringToProxy("test@TestAdapter"));
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            obj = o;
                            return Promise.all(obj.getHello(), obj.getReplicatedHello());
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            hello = arguments[0][1];
                            test(hello.ice_getAdapterId() == "TestAdapter");
                            hello = arguments[1][1];
                            test(hello.ice_getAdapterId() == "ReplicatedAdapter");
                            return hello.sayHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing proxy from server after shutdown... ");
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return hello.sayHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing locator request queuing... ");
                            return obj.getReplicatedHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            hello = o.ice_locatorCacheTimeout(0).ice_connectionCached(false);
                            return locator.getRequestCount();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, count)
                        {
                            var p = new Promise();
                            hello.ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(++count == newCount);
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex){
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var all = [];
                            for(var i = 0; i < 1000; ++i)
                            {
                                all.push(hello.sayHello());
                            }
                            
                            var p = new Promise();
                            
                            Promise.all.apply(Promise, all).then(
                                function()
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count < newCount);
                                    test(newCount < count + 999);
                                    if(newCount > count + 800)
                                    {
                                        out.write("queuing = " + (newCount - count));
                                    }
                                    hello = hello.ice_adapterId("unknown");
                                    count = newCount;
                                    p.succeed(count);
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function(count)
                        {
                            var p = new Promise();
                            
                            var all = 0;
                            exCB = function(ex)
                            {
                                if(!(ex instanceof Ice.NotRegisteredException))
                                {
                                    p.fail(ex);
                                }
                                
                                if(all < 999)
                                {
                                    all++;
                                }
                                else
                                {
                                    p.succeed();
                                }
                            };
                            
                            okCB = function()
                            {
                                p.fail("test failed");
                            };
                            
                            for(var i = 0; i < 1000; ++i)
                            {
                                hello.sayHello().then(okCB, exCB);
                            }
                            
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            out.writeLine("ok");
                            out.write("testing adapter locator cache... ");
                            return communicator.stringToProxy("test@TestAdapter3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.NotRegisteredException))
                            {
                                throw ex;
                            }
                            test(ex.kindOfObject == "object adapter");
                            test(ex.id == "TestAdapter3");
                            
                            return locator.findAdapterById("TestAdapter");
                        }
                    ).then(
                        function(asyncResult, adapter)
                        {
                            return registry.setAdapterDirectProxy("TestAdapter3", adapter);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test@TestAdapter3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            registry.setAdapterDirectProxy("TestAdapter3", communicator.stringToProxy("dummy:tcp"));
                            return communicator.stringToProxy("test@TestAdapter3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test@TestAdapter3").ice_locatorCacheTimeout(0).ice_ping();
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
                            return communicator.stringToProxy("test@TestAdapter3").ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.LocalException))
                            {
                                throw ex;
                            }
                            return locator.findAdapterById("TestAdapter");
                        }
                    ).then(
                        function(asyncResult, adapter)
                        {
                            registry.setAdapterDirectProxy("TestAdapter3", adapter);
                            return communicator.stringToProxy("test@TestAdapter3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing well-known object locator cache... ");
                            return registry.addObject(communicator.stringToProxy("test3@TestUnknown"));
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.NotRegisteredException))
                            {
                                throw ex;
                            }
                            test(ex.kindOfObject == "object adapter");
                            test(ex.id == "TestUnknown");
                        }
                    ).then(
                        function()
                        {
                            return registry.addObject(communicator.stringToProxy("test3@TestAdapter4")); // Update
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return registry.setAdapterDirectProxy("TestAdapter4", communicator.stringToProxy("dummy:tcp"));
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test3").ice_ping();
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
                            return locator.findAdapterById("TestAdapter");
                        }
                    ).then(
                        function(asyncResult, adapter)
                        {
                            return registry.setAdapterDirectProxy("TestAdapter4", adapter);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return registry.setAdapterDirectProxy("TestAdapter4", communicator.stringToProxy("dummy:tcp"));
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test@TestAdapter4").ice_locatorCacheTimeout(0).ice_ping();
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
                            return communicator.stringToProxy("test@TestAdapter4").ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.LocalException))
                            {
                                throw ex;
                            }
                            return communicator.stringToProxy("test3").ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.LocalException))
                            {
                                throw ex;
                            }
                            return registry.addObject(communicator.stringToProxy("test3@TestAdapter"));
                        }
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test3").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return registry.addObject(communicator.stringToProxy("test4"));
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return communicator.stringToProxy("test4").ice_ping();
                        },
                        exceptionCB
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.NoEndpointException))
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("testing locator cache background updates... ");
                            
                            var initData = new Ice.InitializationData();
                            initData.properties = communicator.getProperties().clone();
                            initData.properties.setProperty("Ice.BackgroundLocatorCacheUpdates", "1");
                            var ic = Ice.initialize(initData);
                            
                            var p = new Promise();
                            
                            locator.findAdapterById("TestAdapter").then(
                                function(asyncResult, adapter)
                                {
                                    return registry.setAdapterDirectProxy("TestAdapter5", adapter);
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult)
                                {
                                    return registry.addObject(communicator.stringToProxy("test3@TestAdapter"));
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, count)
                                {
                                    var p1 = new Promise();
                                    // No locator cache.
                                    ic.stringToProxy("test@TestAdapter5").ice_locatorCacheTimeout(0).ice_ping().then(
                                        function(asyncResult)
                                        {
                                            // No locator cache.
                                            return ic.stringToProxy("test3").ice_locatorCacheTimeout(0).ice_ping();
                                        },
                                        exceptionCB
                                    ).then(
                                        function(asyncResult)
                                        {
                                            return locator.getRequestCount();
                                        },
                                        exceptionCB
                                    ).then(
                                        function(asyncResult, newCount)
                                        {
                                            count += 3;
                                            test(count === newCount);
                                            p1.succeed(count);
                                        },
                                        exceptionCB
                                    ).exception(
                                        function(ex){
                                            p1.fail(ex);
                                        });
                                        
                                    return p1;
                                },
                                exceptionCB
                            ).then(
                                function(count)
                                {
                                    var p1 = new Promise();
                                    registry.setAdapterDirectProxy("TestAdapter5", null).then(
                                        function(asyncResult)
                                        {
                                            return registry.addObject(communicator.stringToProxy("test3:tcp"));
                                        },
                                        exceptionCB
                                    ).then(
                                        function(asyncResult)
                                        {
                                            // 10s timeout.
                                            return ic.stringToProxy("test@TestAdapter5").ice_locatorCacheTimeout(10).ice_ping();
                                        },
                                        exceptionCB
                                    ).then(
                                        function(asyncResult)
                                        {
                                            // 10s timeout.
                                            return ic.stringToProxy("test3").ice_locatorCacheTimeout(10).ice_ping();
                                        },
                                        exceptionCB
                                    ).then(
                                        function(asyncResult)
                                        {
                                            return locator.getRequestCount();
                                        },
                                        exceptionCB
                                    ).then(
                                        function(asyncResult, newCount)
                                        {
                                            test(count = newCount);
                                            setTimeout(
                                                function(){
                                                    p1.succeed(count);
                                                }, 1200);
                                        },
                                        exceptionCB
                                    ).exception(
                                        function(ex)
                                        {
                                            p1.fail(ex);
                                        });
                                    return p1;
                                },
                                exceptionCB
                            ).then(
                                function(count)
                                {
                                    var p1 = new Promise();
                                    // The following request should trigger the background updates but still use the cached endpoints
                                    // and therefore succeed.
                                    
                                    // 1s timeout.
                                    ic.stringToProxy("test@TestAdapter5").ice_locatorCacheTimeout(1).ice_ping().then(
                                        function(asyncResult)
                                        {
                                            // 1s timeout.
                                            return ic.stringToProxy("test3").ice_locatorCacheTimeout(1).ice_ping();
                                        },
                                        exceptionCB
                                    ).then(
                                        function(asyncResult)
                                        {
                                            p1.succeed();
                                        },
                                        exceptionCB
                                    ).exception(
                                        function(ex){
                                            p1.fail(ex);
                                        });
                                    
                                    return p1;
                                },
                                exceptionCB
                            ).then(
                                function(){
                                    var p1 = new Promise();
                                    
                                    var f1 = function()
                                    {
                                        ic.stringToProxy("test@TestAdapter5").ice_locatorCacheTimeout(1).ice_ping().then(
                                            function(asyncResult)
                                            {
                                                console("1) setTimeout f1");
                                                setTimeout(function(){ f1(); }, 10000);
                                            },
                                            function(ex)
                                            {
                                                if(ex instanceof Ice.LocalException)
                                                {
                                                    p1.succeed();
                                                }
                                                else
                                                {
                                                    p1.fail(ex);
                                                }
                                            }
                                        ).exception(
                                            function(ex)
                                            {
                                                p1.fail(ex);
                                            });
                                    };
                                    
                                    f1();
                                    
                                    return p1;
                                },
                                exceptionCB
                            ).then(
                                function()
                                {
                                    var p1 = new Promise();
                                    
                                    var f1 = function()
                                    {
                                        ic.stringToProxy("test3").ice_locatorCacheTimeout(1).ice_ping().then(
                                            function(asyncResult)
                                            {
                                                setTimeout(function(){ f1(); }, 10000);
                                            },
                                            function(ex)
                                            {
                                                if(ex instanceof Ice.LocalException)
                                                {
                                                    p1.succeed();
                                                }
                                                else
                                                {
                                                    p1.fail(ex);
                                                }
                                            }
                                        ).exception(
                                            function(ex)
                                            {
                                                p1.fail(ex);
                                            });
                                    };
                                    
                                    f1();
                                    return p1;
                                },
                                exceptionCB
                            ).then(
                                function()
                                {
                                    return ic.destroy();
                                },
                                exceptionCB
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
                            return p;
                        }
                    ).then(
                        function()
                        {
                            out.writeLine("ok");
                            out.write("testing proxy from server after shutdown... ");
                            return obj.getReplicatedHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            hello = o;
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return manager.startServer();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return hello.sayHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing object migration...");
                            return Test.HelloPrx.checkedCast(communicator.stringToProxy("hello"));
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            hello = o;
                            return obj.migrateHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return hello.sayHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj.migrateHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return hello.sayHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return obj.migrateHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return hello.sayHello();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing locator encoding resolution... ");
                            return Test.HelloPrx.checkedCast(communicator.stringToProxy("hello"));
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, o)
                        {
                            return locator.getRequestCount();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, count)
                        {
                            var p = new Promise();
                            
                            var prx = communicator.stringToProxy("test@TestAdapter").ice_encodingVersion(
                                                                                                    Ice.Encoding_1_1);
                            return prx.ice_ping().then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(count == newCount);
                                    return communicator.stringToProxy("test@TestAdapter10").ice_encodingVersion(
                                                                                        Ice.Encoding_1_0).ice_ping();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(++count == newCount);
                                    return communicator.stringToProxy("test -e 1.0@TestAdapter10-2").ice_ping();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult)
                                {
                                    return locator.getRequestCount();
                                },
                                exceptionCB
                            ).then(
                                function(asyncResult, newCount)
                                {
                                    test(++count == newCount);
                                    p.succeed();
                                },
                                exceptionCB
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                            
                            return p;
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            out.writeLine("ok")
                            out.write("shutdown server... ");
                            return obj.shutdown();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing whether server is gone... ");
                            return obj2.ice_ping();
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
                            return obj3.ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.LocalException))
                            {
                                throw ex;
                            }
                            return obj5.ice_ping();
                        }
                    ).then(
                        failCB,
                        function(ex)
                        {
                            if(!(ex instanceof Ice.LocalException))
                            {
                                throw ex;
                            }
                            out.writeLine("ok");
                            out.write("shutdown server manager... ");
                            manager.shutdown();
                        }
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
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
                        id.properties.setProperty("Ice.Default.Locator", "locator:default -p 12010");
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
        module.exports.run = run;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/location"));
