// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
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

    var allTests = function(out, communicator)
    {
        var failCB = function(){ test(false); };
        
        var defaultHost, info, ipinfo, p1, endps, opaqueEndpoint, ipEndpoint, base, testIntf;
        
        var p = new Ice.Promise();
        var test = function(b)
        {
            if(!b)
            {
                try
                {
                    throw new Error("test failed");
                }
                catch(err)
                {
                    p.fail(err);
                    throw err;
                }
            }
        };
        
        Promise.try(
            function()
            {
                defaultHost = communicator.getProperties().getProperty("Ice.Default.Host");
                out.write("testing proxy endpoint information... ");
                p1 = communicator.stringToProxy("test -t:default -h tcphost -p 10000 -t 1200 -z:" +
                                                    "opaque -e 1.8 -t 100 -v ABCD");
                
                endps = p1.ice_getEndpoints();
                ipEndpoint = endps[0].getInfo();
                test(ipEndpoint.host == "tcphost");
                test(ipEndpoint.port == 10000);
                test(ipEndpoint.timeout == 1200);
                test(ipEndpoint.compress);
                test(!ipEndpoint.datagram());
                if(typeof(window) == "undefined")
                {
                    test(ipEndpoint.type() == Ice.TCPEndpointType);
                }
                else
                {
                    test(ipEndpoint.type() == IceWS.WSEndpointType);
                }
            
                opaqueEndpoint = endps[1].getInfo();
                test(opaqueEndpoint.rawEncoding.equals(new Ice.EncodingVersion(1, 8)));
                out.writeLine("ok");

                out.write("test connection endpoint information... ");
                base = communicator.stringToProxy("test:default -p 12010");
                return Test.TestIntfPrx.checkedCast(base);
            }
        ).then(
            function(obj)
            {
                testIntf = obj;
                return base.ice_getConnection();
            }
        ).then(
            function(conn)
            {
                ipinfo = conn.getEndpoint().getInfo();
                test(ipinfo.port == 12010);
                test(!ipinfo.compress);
                test(ipinfo.host == defaultHost);

                return testIntf.getEndpointInfoAsContext();
            }
        ).then(
            function(ctx)
            {
                test(ctx.get("host") == ipinfo.host);
                test(ctx.get("compress") == "false");
                test(parseInt(ctx.get("port")) > 0);
                out.writeLine("ok");
                out.write("testing connection information... ");
                return base.ice_getConnection();
            }
        ).then(
            function(conn)
            {
                info = conn.getInfo();
                test(!info.incoming);
                test(info.adapterName.length === 0);
                if(typeof(window) == "undefined")
                {
                    test(info.localPort > 0);
                    test(info.localAddress == defaultHost ||
                            (info.localAddress == "::1" && defaultHost == "0:0:0:0:0:0:0:1"));
                }
                test(info.remotePort == 12010);
                test(info.remoteAddress == defaultHost ||
                        (info.remoteAddress == "::1" && defaultHost == "0:0:0:0:0:0:0:1"));
                return testIntf.getConnectionInfoAsContext();
            }
        ).then(
            function(ctx)
            {
                test(ctx.get("incoming") == "true");
                test(ctx.get("adapterName") == "TestAdapter");
                if(typeof(window) == "undefined")
                {
                    test(ctx.get("remoteAddress") == info.localAddress);
                    test(ctx.get("remotePort") == info.localPort);
                }
                test(ctx.get("localAddress") == info.remoteAddress);
                test(ctx.get("localPort") == info.remotePort);

                out.writeLine("ok");
                return testIntf.shutdown();
            }
        ).then(
            function()
            {
                p.succeed();
            },
            function(ex)
            {
                p.fail(ex);
            }
        );
        return p;
    };

    var run = function(out, id)
    {
        return Promise.try(
            function()
            {
                var c = Ice.initialize(id);
                return allTests(out, c).finally(
                    function()
                    {
                        if(c)
                        {
                            return c.destroy();
                        }
                    });
            });
    };
    global.__test__ = run;
}(typeof (global) === "undefined" ? window : global));
