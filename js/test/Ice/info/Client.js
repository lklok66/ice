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

    var allTests = function(out, communicator)
    {
        var p = new Promise();
        
        var failCB = function(){ test(false); };
        
        setTimeout(function(){
            try
            {
                var defaultHost = communicator.getProperties().getProperty("Ice.Default.Host");
                var info, ipinfo;
                out.write("testing proxy endpoint information... ");
                var p1 = communicator.stringToProxy("test -t:default -h tcphost -p 10000 -t 1200 -z:" +
                                                    "opaque -e 1.8 -t 100 -v ABCD");
                
                var endps = p1.ice_getEndpoints();


                var ipEndpoint = endps[0].getInfo();
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
            
                var opaqueEndpoint = endps[1].getInfo();
                test(opaqueEndpoint.rawEncoding.equals(new Ice.EncodingVersion(1, 8)));
                out.writeLine("ok");

                out.write("test connection endpoint information... ");
                var base = communicator.stringToProxy("test:default -p 12010");
                var testIntf;
                Test.TestIntfPrx.checkedCast(base).then(
                    function(asyncResult, obj)
                    {
                        testIntf = obj;
                        return base.ice_getConnection();
                    }
                ).then(
                    function(asyncResult, conn)
                    {
                        ipinfo = conn.getEndpoint().getInfo();
                        test(ipinfo.port == 12010);
                        test(!ipinfo.compress);
                        test(ipinfo.host == defaultHost);

                        return testIntf.getEndpointInfoAsContext();
                    }
                ).then(
                    function(asyncResult, ctx)
                    {
                        test(ctx.get("host") == ipinfo.host);
                        test(ctx.get("compress") == "false");
                        test(parseInt(ctx.get("port")) > 0);
                        out.writeLine("ok");
                        out.write("testing connection information... ");
                        return base.ice_getConnection();
                    }
                ).then(
                    function(asyncResult, conn)
                    {
                        info = conn.getInfo();
                        test(!info.incoming);
                        test(info.adapterName.length == 0);
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
                    function(asyncResult, ctx)
                    {
                        test(ctx.get("incoming") == "true");
                        test(ctx.get("adapterName") == "TestAdapter");
                        if(typeof(window) == undefined)
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
                    }
                ).exception(
                    function(ex){
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
    global.__test__ = run;
}(typeof (global) === "undefined" ? window : global));
