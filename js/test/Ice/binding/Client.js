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
    
    require("Test");
    var Test = global.Test;
    var Promise = Ice.Promise;
    var ArrayUtil = Ice.ArrayUtil;
    
    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };
    
    var communicator;
    var com;
    var allTests = function(out, initData)
    {
        var p = new Promise();
        
        var initialize = function()
        {
            var p = new Promise();
            if(communicator)
            {
                communicator.destroy().then(
                    function(asyncResult)
                    {
                        communicator = Ice.initialize(initData);
                        com = Test.RemoteCommunicatorPrx.uncheckedCast(communicator.stringToProxy("communicator:default -p 12010"));
                        p.succeed();
                    }
                ).exception(
                    function(ex)
                    {
                        p.fail(ex);
                    }
                );
            }
            else
            {
                communicator = Ice.initialize(initData);
                p.succeed();
            }
            return p;
        }
        
        var failCB = function(){ test(false); };
        
        var createTestIntfPrx = function(adapters)
        {
            var p = new Promise();
            setTimeout(function(){
                var endpoints = [];
                var closePromises = [];
                var test = null;
                var promises = adapters.map(function(adapter){ return adapter.getTestIntf(); });
                
                Promise.all.apply(Promise, promises).then(
                    function()
                    {
                        var args = Array.prototype.slice.call(arguments);
                        args.forEach(
                            function(r)
                            {
                                test = r[1];
                                endpoints = endpoints.concat(test.ice_getEndpoints());
                            });
                        adapters.forEach(
                            function(adapter)
                            {
                                var conn = adapter.ice_getCachedConnection();
                                if(conn != null)
                                {
                                    closePromises.push(conn.close(false));
                                }
                            });
                        
                        test = test.ice_endpoints(endpoints);
                        if(closePromises.length > 0)
                        {
                            Promise.all.apply(Promise, closePromises).then(
                                function()
                                {
                                    p.succeed(Test.TestIntfPrx.uncheckedCast(test));
                                },
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                        }
                        else
                        {
                            p.succeed(Test.TestIntfPrx.uncheckedCast(test));
                        }
                    },
                    function(ex)
                    {
                        p.fail(ex)
                    }).exception( function(ex){ p .fail(ex); } );
            });
            return p;
        };
        
        var deactivate = function(communicator, adapters)
        {
            var p = new Promise();
            var f1 = function(adapters)
            {
                var adapter = adapters.shift();
                communicator.deactivateObjectAdapter(adapter).then(
                    function(asyncResult)
                    {
                        if(adapters.length > 0)
                        {
                            f1(adapters)
                        }
                        else
                        {
                            p.succeed();
                        }
                    }
                ).exception(
                    function(ex)
                    {
                        p.fail(ex); 
                    }
                );
            };
            setTimeout(
                function(){
                    f1(ArrayUtil.clone(adapters));
                });
            return p;
        };
        
        setTimeout(function(){
            try
            {
            
                var ref;
                var adapter, test1, test2, test3, conn1, conn2, conn3;
                var adapters;
                var names;
                var prx;
                
                var multipleRandomEndpoints = function()
                {
                    var testPromise = new Promise();
                    setTimeout(
                        function()
                        {
                            out.write("testing binding with multiple random endpoints... ");
                            names = ["AdapterRandom11", "AdapterRandom12", "AdapterRandom13", "AdapterRandom14", "AdapterRandom15"];
                            Promise.all.apply(Promise, 
                                names.map(function(name) { return com.createObjectAdapter(name, "default"); })).then(
                            function()
                            {
                                adapters = Array.prototype.slice.call(arguments).map(function(r) { return r[1]; });
                                var count = 20;
                                var adapterCount = adapters.length;
                                var proxies = new Array(10);
                                var nextInt = function(n) { return Math.floor((Math.random() * n)); };
                                var p = new Promise();
                                var f1 = function(count, adapterCount, proxies)
                                {
                                    var p1 = count === 10 ? com.deactivateObjectAdapter(adapters[2]) : Promise.succeed();
                                    p1.then(
                                        function()
                                        {
                                            var p2 = new Promise();
                                            if(count === 10)
                                            {
                                                adapterCount--;
                                            }
                                            var f2 = function(i)
                                            {
                                                var adpts = new Array(nextInt(adapters.length) + 1);
                                                for(var j = 0; j < adpts.length; ++j)
                                                {
                                                    adpts[j] = adapters[nextInt(adapters.length)];
                                                }
                                                createTestIntfPrx(adpts).then(
                                                    function(prx)
                                                    {
                                                        proxies[i] = prx;
                                                        if(i < 10)
                                                        {
                                                            f2(++i);
                                                        }
                                                        else
                                                        {
                                                            p2.succeed(proxies);
                                                        }
                                                    }
                                                ).exception(
                                                    function(ex)
                                                    {
                                                        p2.fail(ex);
                                                    }
                                                );
                                            };
                                            
                                            setTimeout(
                                                function(){
                                                    f2(0);
                                                });
                                            
                                            return p2;
                                        }
                                    ).then(
                                        function(proxies){
                                            proxies.forEach(
                                                function(p){
                                                    p.getAdapterName();
                                                });
                                            
                                            var allPing = [];
                                            proxies.forEach(
                                                function(proxy){
                                                    var p = new Promise();
                                                    proxy.ice_ping().then(
                                                        function(asyncResult){
                                                            p.succeed();
                                                        },
                                                        function(ex){
                                                            if((ex instanceof Ice.LocalException))
                                                            {
                                                                p.succeed();
                                                            }
                                                            else
                                                            {
                                                                p.fail(ex);
                                                            }
                                                        });
                                                    allPing.push(p);
                                                });
                                            
                                            var f3 = function(p, adapters)
                                            {
                                                adapter = adapters.shift();
                                                adapter.getTestIntf().then(
                                                    function(asyncResult, prx)
                                                    {
                                                        return prx.ice_getConnection();
                                                    },
                                                    function(ex)
                                                    {
                                                        throw ex;
                                                    }
                                                ).then(
                                                    function(asyncResult, conn)
                                                    {
                                                        return conn.close(false);
                                                    },
                                                    function(ex)
                                                    {
                                                        if(ex instanceof Ice.LocalException)
                                                        {
                                                            // Expected if adapter is down.
                                                            return;
                                                        }
                                                        else
                                                        {
                                                            throw ex;
                                                        }
                                                    }
                                                ).then(
                                                    function()
                                                    {
                                                        if(adapters.length > 0)
                                                        {
                                                            f3(p, adapters);
                                                        }
                                                        else
                                                        {
                                                            p.succeed();
                                                        }
                                                    }
                                                ).exception(
                                                    function(ex)
                                                    {
                                                        p.fail(ex);
                                                    }
                                                );
                                            };
                                            
                                            var p4 = new Promise();
                                            
                                            Promise.all.apply(Promise, allPing).then(
                                                function()
                                                {
                                                    var connections = [];
                                                    proxies.forEach(
                                                        function(p){
                                                            var conn = p.ice_getCachedConnection();
                                                            if(conn !== null)
                                                            {
                                                                if(connections.indexOf(conn) !== -1)
                                                                {
                                                                    connections.push(conn);
                                                                }
                                                            }
                                                        });
                                                    test(connections.length <= adapters.length);
                                                }
                                            ).then(
                                                function()
                                                {
                                                    var p5 = new Promise();
                                                    f3(p5, ArrayUtil.clone(adapters));
                                                    
                                                    return p5;
                                                }
                                            ).then(
                                                function()
                                                {
                                                    p4.succeed();
                                                }
                                            ).exception(
                                                function(ex)
                                                {
                                                    p4.fail(ex);
                                                }
                                            );
                                            return p4;
                                        }
                                    ).then(
                                        function()
                                        {
                                            if(count === 0)
                                            {
                                                p.succeed();
                                            }
                                            else
                                            {
                                                f1(--count, adapterCount, proxies);
                                            }
                                        }
                                    ).exception(
                                        function(ex)
                                        {
                                            p.fail(ex);
                                        }
                                    );
                                };
                                
                                setTimeout(function(){
                                    f1(count, adapterCount, proxies);
                                });
                                
                                return p;
                            }
                        ).then(
                            function()
                            {
                                out.writeLine("ok");
                                return testPromise.succeed();
                            }
                        ).exception(
                            function(ex)
                            {
                                testPromise.fail()
                            }
                        );
                    });
                    return testPromise;
                };
                
                initialize().then(
                    function()
                    {
                        out.write("testing stringToProxy... ");
                        ref = "communicator:default -p 12010";
                        com = Test.RemoteCommunicatorPrx.uncheckedCast(communicator.stringToProxy(ref));
                        test(com !== null);
                        out.writeLine("ok");
                        
                        out.write("testing binding with single endpoint... ");
                        adapter, test1, test2, test3, conn1, conn2, conn3;
                        adapters = [];
                        names = ["Adapter11", "Adapter12", "Adapter13"];
                    
                        return com.createObjectAdapter("Adapter", "default");
                    }
                ).then(
                    function(asyncResult, obj)
                    {
                        adapter = obj;
                        return Promise.all(
                            adapter.getTestIntf(),
                            adapter.getTestIntf());
                    }
                ).then(
                    function(r1, r2)
                    {
                        test1 = r1[1];
                        test2 = r2[1];
                        
                        return Promise.all(
                            test1.ice_getConnection(),
                            test2.ice_getConnection());
                    }
                ).then(
                    function(r1, r2)
                    {
                        conn1 = r1[1];
                        conn2 = r2[1];
                        test(conn1 === conn2);
                        return Promise.all(
                            test1.ice_ping(),
                            test2.ice_ping());
                    }
                ).then(
                    function(r1, r2)
                    {
                        return com.deactivateObjectAdapter(adapter);
                    }
                ).then(
                    function(asyncResult)
                    {
                        test3 = Test.TestIntfPrx.uncheckedCast(test1);
                        
                        return Promise.all(
                            test3.ice_getConnection(),
                            test1.ice_getConnection());
                    }
                ).then(
                    function(r1, r2)
                    {
                        conn3 = r1[1];
                        conn1 = r2[1];
                        test(conn3 === conn1);
                        
                        return Promise.all(
                            test3.ice_getConnection(),
                            test2.ice_getConnection());
                        
                    }
                ).then(
                    function(r1, r2)
                    {
                        conn3 = r1[1];
                        conn2 = r2[1];
                        test(conn3 === conn2);
                        return test3.ice_ping();
                    }
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(typeof(window) == 'undefined' && ex instanceof Ice.ConnectionRefusedException) &&
                            !(typeof(window) != 'undefined' && ex instanceof Ice.ConnectFailedException))
                        {
                            throw ex;
                        }
                        out.writeLine("ok");
                        return initialize();
                    }
                ).then(
                    function()
                    {
                        out.write("testing binding with multiple endpoints... ");
                        
                        return Promise.all(
                            com.createObjectAdapter("Adapter11", "default"),
                            com.createObjectAdapter("Adapter12", "default"),
                            com.createObjectAdapter("Adapter13", "default"));
                    }
                ).then(
                    //
                    // Ensure that when a connection is opened it's reused for new
                    // proxies and that all endpoints are eventually tried.
                    //
                    function(r1, r2, r3)
                    {
                        adapters.push(r1[1]);
                        adapters.push(r2[1]);
                        adapters.push(r3[1]);
                        var p = new Promise();
                        var f1 = function(names)
                        {
                            var adpts = ArrayUtil.clone(adapters);
                            createTestIntfPrx(adpts).then(
                                function(obj)
                                {
                                    test1 = obj;
                                    ArrayUtil.shuffle(adpts);
                                    return createTestIntfPrx(adpts);
                                }
                            ).then(
                                function(obj)
                                {
                                    test2 = obj;
                                    ArrayUtil.shuffle(adpts);
                                    return createTestIntfPrx(adpts);
                                }
                            ).then(
                                function(obj)
                                {
                                    test3 = obj;
                                    return Promise.all(
                                        test1.ice_getConnection(),
                                        test2.ice_getConnection());
                                }
                            ).then(
                                function(r1, r2)
                                {
                                    test(r1[1] === r2[1]);
                                    return Promise.all(
                                        test2.ice_getConnection(),
                                        test3.ice_getConnection());
                                }
                            ).then(
                                function(r1, r2)
                                {
                                    test(r1[1] === r2[1]);
                                    return test1.getAdapterName();
                                }
                            ).then(
                                function(asyncResult, name)
                                {
                                    if(names.indexOf(name) !== -1)
                                    {
                                        names.splice(names.indexOf(name), 1);
                                    }
                                    return test1.ice_getConnection();
                                }
                            ).then(
                                function(asyncResult, conn)
                                {
                                    return conn.close(false);
                                }
                            ).then(
                                function()
                                {
                                    if(names.length > 0)
                                    {
                                        f1(names);
                                    }
                                    else
                                    {
                                        p.succeed();
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                }
                            );
                        }
                        setTimeout(function(){
                            f1(ArrayUtil.clone(names));
                        });
                        return p;
                    }
                ).then(
                    function()
                    {
                        //
                        // Ensure that the proxy correctly caches the connection (we
                        // always send the request over the same connection.)
                        //
                        
                        var p = new Promise();
                        
                        var f1 = function(adapters)
                        {
                            var adapter = adapters.shift();
                            adapter.getTestIntf().then(
                                function(asyncResult, test)
                                {
                                    return test.ice_ping();
                                }
                            ).then(
                                function()
                                {
                                    if(adapters.length > 0)
                                    {
                                        f1(adapters);
                                    }
                                    else
                                    {
                                        p.succeed();
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                        }
                        setTimeout(
                            function()
                            {
                                f1(ArrayUtil.clone(adapters))
                            });
                        return p;
                    }
                ).then(
                    function()
                    {
                        var p = new Promise();
                        createTestIntfPrx(adapters).then(
                            function(test1)
                            {
                                var i = 0;
                                var nRetry = 10;
                                var adapterName;
                                
                                var f1 = function()
                                {
                                    test1.getAdapterName().then(
                                        function(asyncResult, name)
                                        {
                                            test(adapterName === name);
                                            i++;
                                            if(i < nRetry)
                                            {
                                                f1();
                                            }
                                            else
                                            {
                                                test(i == nRetry);
                                                p.succeed();
                                            }
                                        }
                                    ).exception(
                                        function(ex)
                                        {
                                            p.fail(ex);
                                        }
                                    );
                                };
                                
                                test1.getAdapterName().then(
                                    function(asyncResult, name)
                                    {
                                        adapterName = name;
                                        f1();
                                    }
                                ).exception(
                                    function(ex)
                                    {
                                        p.fail(ex);
                                    }
                                );
                            });
                        
                        return p;
                    }
                ).then(
                    function()
                    {
                        var p = new Promise();
                        var f1 = function(adapters)
                        {
                            var adapter = adapters.shift();
                            adapter.getTestIntf().then(
                                function(asyncResult, test)
                                {
                                    return test.ice_getConnection();
                                }
                            ).then(
                                function(asyncResult, conn)
                                {
                                    conn.close(false);
                                }
                            ).then(
                                function()
                                {
                                    if(adapters.length > 0)
                                    {
                                        f1(adapters);
                                    }
                                    else
                                    {
                                        p.succeed();
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                }
                            );
                        }
                        setTimeout(
                            function()
                            {
                                f1(ArrayUtil.clone(adapters))
                            });
                        return p;
                    }
                ).then(
                    function()
                    {
                        names = ["Adapter12", "Adapter13"];
                        return com.deactivateObjectAdapter(adapters[0]);
                    }
                ).then(
                    function(asyncResult)
                    {
                        var p = new Promise();
                        var f1 = function(names)
                        {
                            var adpts = ArrayUtil.clone(adapters);
                            createTestIntfPrx(adpts).then(
                                function(obj)
                                {
                                    test1 = obj;
                                    ArrayUtil.shuffle(adpts);
                                    return createTestIntfPrx(adpts);
                                }
                            ).then(
                                function(obj)
                                {
                                    test2 = obj;
                                    ArrayUtil.shuffle(adpts);
                                    return createTestIntfPrx(adpts);
                                }
                            ).then(
                                function(obj)
                                {
                                    test3 = obj;
                                    
                                    var p = new Promise();
                                    var conn1, conn2;
                                    
                                    test1.ice_getConnection().then(
                                        function(asyncResult, conn)
                                        {
                                            conn1 = conn
                                            return test2.ice_getConnection();
                                        }
                                    ).then(
                                        function(asyncResult, conn)
                                        {
                                            conn2 = conn;
                                            test(conn1 === conn2);
                                            p.succeed();
                                        }
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
                                    var p = new Promise();
                                    var conn1, conn2;
                                    test2.ice_getConnection().then(
                                        function(asyncResult, conn)
                                        {
                                            conn1 = conn
                                            return test3.ice_getConnection();
                                        }
                                    ).then(
                                        function(asyncResult, conn)
                                        {
                                            conn2 = conn;
                                            test(conn1 === conn2);
                                            p.succeed();
                                        }
                                    ).exception(
                                        function(ex)
                                        {
                                            p.fail(ex);
                                        });
                                    return p;
                                }
                            ).then(
                                function()
                                {
                                    return test1.getAdapterName();
                                }
                            ).then(
                                function(asyncResult, name)
                                {
                                    if(names.indexOf(name) !== -1)
                                    {
                                        names.splice(names.indexOf(name), 1);
                                    }
                                    return test1.ice_getConnection();
                                }
                            ).then(
                                function(asyncResult, conn)
                                {
                                    return conn.close(false);
                                }
                            ).then(
                                function()
                                {
                                    if(names.length > 0)
                                    {
                                        f1(names);
                                    }
                                    else
                                    {
                                        p.succeed();
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                }
                            );
                        };
                        setTimeout(function(){
                            f1(ArrayUtil.clone(names));
                        });
                        return p;
                    }
                ).then(
                    function()
                    {
                            return com.deactivateObjectAdapter(adapters[2]);
                    }
                ).then(
                    function(asyncResult)
                    {
                        return createTestIntfPrx(adapters);
                    }
                ).then(
                    function(prx)
                    {
                        return prx.getAdapterName();
                    }
                ).then(
                    function(asyncResult, name)
                    {
                        test(name == "Adapter12");
                        return deactivate(com, adapters);
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");
                        return initialize();
                    }
                ).then(
                    function()
                    {
                        //
                        // Skip this test with IE it open too many connections IE doesn't allow more than 6 connections.
                        //
                        if(typeof(navigator) === "undefined" || navigator.userAgent.indexOf("MSIE") === -1)
                        {
                            return multipleRandomEndpoints();
                        }
                    }
                ).then(
                    function()
                    {
                        return initialize();
                    }
                ).then(
                    function()
                    {
                        out.write("testing random endpoint selection... ");
                        names = ["Adapter21", "Adapter22", "Adapter23"];
                        return Promise.all.apply(Promise, 
                                names.map(function(name) { return com.createObjectAdapter(name, "default"); }));
                    }
                ).then(
                    function()
                    {
                        adapters = Array.prototype.slice.call(arguments).map(function(r) { return r[1]; });
                        return createTestIntfPrx(adapters);
                    }
                ).then(
                    function(prx)
                    {
                        test(prx.ice_getEndpointSelection() == Ice.EndpointSelectionType.Random);
                        
                        var p = new Promise();
                        
                        var f1 = function()
                        {
                            prx.getAdapterName().then(
                                function(asyncResult, name)
                                {
                                    if(names.indexOf(name) !== -1)
                                    {
                                        names.splice(names.indexOf(name), 1);
                                    }
                                    return prx.ice_getConnection();
                                }
                            ).then(
                                function(asyncResult, conn)
                                {
                                    return conn.close(false);
                                }
                            ).then(
                                function()
                                {
                                    if(names.length > 0)
                                    {
                                        f1();
                                    }
                                    else
                                    {
                                        p.succeed(prx);
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                }
                            );
                        };
                        
                        setTimeout(f1);
                        
                        return p;
                    }
                ).then(
                    function(obj)
                    {
                        prx = obj;
                        prx = Test.TestIntfPrx.uncheckedCast(
                                                    prx.ice_endpointSelection(Ice.EndpointSelectionType.Random));
                        test(prx.ice_getEndpointSelection() === Ice.EndpointSelectionType.Random);
                        names = ["Adapter21", "Adapter22", "Adapter23"];
                        var p = new Promise();
                        
                        var f1 = function()
                        {
                            prx.getAdapterName().then(
                                function(asyncResult, name)
                                {
                                    if(names.indexOf(name) !== -1)
                                    {
                                        names.splice(names.indexOf(name), 1);
                                    }
                                    return prx.ice_getConnection();
                                }
                            ).then(
                                function(asyncResult, conn)
                                {
                                    return conn.close(false);
                                }
                            ).then(
                                function()
                                {
                                    if(names.length > 0)
                                    {
                                        f1();
                                    }
                                    else
                                    {
                                        p.succeed(prx);
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                }
                            );
                        };
                        
                        setTimeout(f1);
                        
                        return p;
                    }
                ).then(
                    function()
                    {
                        return  deactivate(com, adapters);
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");
                        return initialize();
                    }
                ).then(
                    function()
                    {
                        out.write("testing ordered endpoint selection... ");
                        names = ["Adapter31", "Adapter32", "Adapter33"];
                        return Promise.all.apply(Promise, 
                                names.map(function(name) { return com.createObjectAdapter(name, "default"); }));
                    }
                ).then(
                    function()
                    {
                        adapters = Array.prototype.slice.call(arguments).map(function(r) { return r[1]; });
                        return createTestIntfPrx(adapters);
                    }
                ).then(
                    function(obj)
                    {
                        prx = obj;
                        prx = Test.TestIntfPrx.uncheckedCast(
                                                    prx.ice_endpointSelection(Ice.EndpointSelectionType.Ordered));
                        test(prx.ice_getEndpointSelection() === Ice.EndpointSelectionType.Ordered);
                        var i, nRetry = 5;
                        var p = new Promise();
                        var f1 = function(i, idx, names)
                        {
                            prx.getAdapterName().then(
                                function(asyncResult, name)
                                {
                                    test(name === names[0]);
                                }
                            ).then(
                                function()
                                {
                                    if(i < nRetry)
                                    {
                                        f1(++i, idx, names);
                                    }
                                    else
                                    {
                                        com.deactivateObjectAdapter(adapters[idx]).then(
                                            function()
                                            {
                                                if(names.length > 1)
                                                {
                                                    names.shift()
                                                    f1(0, ++idx, names);
                                                }
                                                else
                                                {
                                                    p.succeed();
                                                }
                                            }
                                        ).exception(
                                            function(ex)
                                            {
                                                p.fail(ex);
                                            });
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                        };
                        
                        setTimeout(
                            function(){
                                f1(0, 0, ArrayUtil.clone(names));
                            });
                        return p;
                    }
                ).then(
                    function()
                    {
                        return prx.getAdapterName();
                    }
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(typeof(window) == 'undefined' && ex instanceof Ice.ConnectionRefusedException) &&
                            !(typeof(window) != 'undefined' && ex instanceof Ice.ConnectFailedException))
                        {
                            throw ex;
                        }
                        return prx.ice_getEndpoints();
                    }
                ).then(
                    function(endpoints)
                    {
                        var p = new Promise();
                        var nRetry = 5;
                        var j = 2;
                        var f1 = function(i, names)
                        {
                            com.createObjectAdapter(names[0], endpoints[j--].toString()).then(
                                function(asyncResult, obj)
                                {
                                    return obj.getTestIntf();
                                }
                            ).then(
                                function(asyncResult, obj)
                                {
                                    prx = obj;
                                    var f2 = function(i, names)
                                    {
                                        prx.getAdapterName().then(
                                            function(asyncResult, name)
                                            {
                                                test(name === names[0]);
                                                if(i < nRetry)
                                                {
                                                    f2(++i, names);
                                                }
                                                else if(names.length > 1)
                                                {
                                                    names.shift();
                                                    f1(0, names);
                                                }
                                                else
                                                {
                                                    p.succeed();
                                                }
                                            }
                                        ).exception(
                                            function(ex){
                                                p.fail(ex);
                                            });
                                    };
                                    setTimeout(
                                        function(){
                                            f2(0, names);
                                        });
                                }
                            ).exception(
                                function(ex){
                                    p.fail(ex);
                                });
                        };
                        setTimeout(function(){
                            f1(0, ["Adapter36", "Adapter35", "Adapter34"]);
                        });
                        return p;
                    }
                ).then(
                    function()
                    {
                        return deactivate(com, adapters);
                    }
                ).then(
                    function(){
                        out.writeLine("ok");
                        return initialize
                    }
                ).then(
                    function()
                    {
                        out.write("testing per request binding with single endpoint... ");
                        return com.createObjectAdapter("Adapter41", "default");
                    }
                ).then(
                    function(asyncResult, adapter)
                    {
                        var p = new Promise();
                        var f1 = function()
                        {
                            adapter.getTestIntf().then(
                                function(asyncResult, obj)
                                {
                                    test1 = Test.TestIntfPrx.uncheckedCast(obj.ice_connectionCached(false));
                                    return adapter.getTestIntf()
                                }
                            ).then(
                                function(asyncResult, obj)
                                {
                                    test2 = Test.TestIntfPrx.uncheckedCast(obj.ice_connectionCached(false));
                                    test(!test1.ice_isConnectionCached());
                                    test(!test2.ice_isConnectionCached());
                                    return Promise.all(
                                        test1.ice_getConnection(),
                                        test2.ice_getConnection());
                                }
                            ).then(
                                function(r1, r2)
                                {
                                    test(r1[1] == r2[1]);
                                    return test1.ice_ping();
                                }
                            ).then(
                                function(asyncResult)
                                {
                                    return com.deactivateObjectAdapter(adapter);
                                }
                            ).then(
                                function(asyncResult)
                                {
                                    var test3 = Test.TestIntfPrx.uncheckedCast(test1);
                                    return Promise.all(test3.ice_getConnection(),
                                                        test1.ice_getConnection());
                                }
                            ).then(
                                failCB,
                                function(ex)
                                {
                                    if(!(typeof(window) == 'undefined' && ex instanceof Ice.ConnectionRefusedException) &&
                                        !(typeof(window) != 'undefined' && ex instanceof Ice.ConnectFailedException))
                                    {
                                        throw ex;
                                    }
                                    p.succeed();
                                }
                            ).exception(
                                function(ex){
                                    p.fail(ex);
                                }
                            );
                        };
                        setTimeout(f1);
                        return p;
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");
                        return initialize();
                    }
                ).then(
                    function()
                    {
                        out.write("testing per request binding with multiple endpoints... ");
                        names = ["Adapter51", "Adapter52", "Adapter53"];
                        return Promise.all.apply(Promise, 
                                names.map(function(name) { return com.createObjectAdapter(name, "default"); }));
                    }
                ).then(
                    function()
                    {
                        adapters = Array.prototype.slice.call(arguments).map(function(r) { return r[1]; });
                        
                        var f2 = function(p, prx)
                        {
                            prx.getAdapterName().then(
                                function(asyncResult, name)
                                {
                                    if(names.indexOf(name) !== -1)
                                    {
                                        names.splice(names.indexOf(name), 1);
                                    }
                                    if(names.length > 0)
                                    {
                                        f2(p, prx);
                                    }
                                    else
                                    {
                                        p.succeed();
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex)
                                });
                        };
                        
                        var p1 = new Promise();
                        var f1 = function()
                        {
                            createTestIntfPrx(adapters).then(
                                function(obj)
                                {
                                    prx = obj;
                                    prx = Test.TestIntfPrx.uncheckedCast(prx.ice_connectionCached(false));
                                }
                            ).then(
                                function()
                                {
                                    test(!prx.ice_isConnectionCached());
                                    f2(p1, prx);
                                }
                            ).exception(
                                function(ex)
                                {
                                    p1.fail(ex);
                                });
                        };
                        setTimeout(f1);
                        
                        var p2 = new Promise();
                        p1.then(
                            function()
                            {
                                return com.deactivateObjectAdapter(adapters[0]);
                            }
                        ).then(
                            function()
                            {
                                names = ["Adapter52", "Adapter53"];
                                f2(p2, prx);
                            }
                        ).exception(
                            function(ex)
                            {
                                p2.fail(ex);
                            });
                        
                        var p3 = new Promise();
                        
                        p2.then(
                            function()
                            {
                                return com.deactivateObjectAdapter(adapters[0]);
                            }
                        ).then(
                            function()
                            {
                                names = ["Adapter52"];
                                f2(p3, prx);
                            }
                        ).exception(
                            function(ex)
                            {
                                p2.fail(ex);
                            });
                        
                        return p3;
                    }
                ).then(
                    function()
                    {
                        return deactivate(com, adapters);
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");
                        return initialize();
                    }
                ).then(
                    function()
                    {
                        out.write("testing per request binding and ordered endpoint selection... ");
                        names = ["Adapter61", "Adapter62", "Adapter63"];
                        return Promise.all.apply(Promise, 
                                names.map(function(name) { return com.createObjectAdapter(name, "default"); }));
                    }
                ).then(
                    function()
                    {
                        adapters = Array.prototype.slice.call(arguments).map(function(r) { return r[1]; });
                        return createTestIntfPrx(adapters);
                    }
                ).then(
                    function(obj)
                    {
                        prx = obj;
                        prx = Test.TestIntfPrx.uncheckedCast(
                                                    prx.ice_endpointSelection(Ice.EndpointSelectionType.Ordered));
                        test(prx.ice_getEndpointSelection() == Ice.EndpointSelectionType.Ordered);
                        prx = Test.TestIntfPrx.uncheckedCast(prx.ice_connectionCached(false));
                        test(!prx.ice_isConnectionCached());
                        var nRetry = 5;
                        var p = new Promise();
                        var f1 = function(i, idx, names)
                        {
                            prx.getAdapterName().then(
                                function(asyncResult, name)
                                {
                                    test(name === names[0]);
                                }
                            ).then(
                                function()
                                {
                                    if(i < nRetry)
                                    {
                                        f1(++i, idx, names);
                                    }
                                    else
                                    {
                                        com.deactivateObjectAdapter(adapters[idx]).then(
                                            function()
                                            {
                                                if(names.length > 1)
                                                {
                                                    names.shift()
                                                    f1(0, ++idx, names);
                                                }
                                                else
                                                {
                                                    p.succeed();
                                                }
                                            }
                                        ).exception(
                                            function(ex)
                                            {
                                                p.fail(ex);
                                            });
                                    }
                                }
                            ).exception(
                                function(ex)
                                {
                                    p.fail(ex);
                                });
                        };
                        
                        setTimeout(
                            function(){
                                f1(0, 0, ArrayUtil.clone(names));
                            });
                        return p;
                    }
                ).then(
                    function()
                    {
                        return prx.getAdapterName();
                    }
                ).then(
                    failCB,
                    function(ex)
                    {
                        if(!(typeof(window) == 'undefined' && ex instanceof Ice.ConnectionRefusedException) &&
                            !(typeof(window) != 'undefined' && ex instanceof Ice.ConnectFailedException))
                        {
                            throw ex;
                        }
                        return prx.ice_getEndpoints();
                    }
                ).then(
                    function(endpoints)
                    {
                        var p = new Promise();
                        var nRetry = 5;
                        var j = 2;
                        var f1 = function(i, names)
                        {
                            com.createObjectAdapter(names[0], endpoints[j--].toString()).then(
                                function()
                                {
                                    var f2 = function(i, names)
                                    {
                                        prx.getAdapterName().then(
                                            function(asyncResult, name)
                                            {
                                                test(name === names[0]);
                                                if(i < nRetry)
                                                {
                                                    f2(++i, names);
                                                }
                                                else if(names.length > 1)
                                                {
                                                    names.shift();
                                                    f1(0, names);
                                                }
                                                else
                                                {
                                                    p.succeed();
                                                }
                                            }
                                        ).exception(
                                            function(ex){
                                                p.fail(ex);
                                            });
                                    };
                                    setTimeout(
                                        function(){
                                            f2(0, names);
                                        });
                                }
                            ).exception(
                                function(ex){
                                    p.fail(ex);
                                });
                        };
                        setTimeout(function(){
                            f1(0, ["Adapter66", "Adapter65", "Adapter64"]);
                        });
                        return p;
                    }
                ).then(
                    function()
                    {
                        out.writeLine("ok");
                        return com.shutdown();
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
                try
                {
                    allTests(out, id).then(function(){ 
                            return communicator.destroy();
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
