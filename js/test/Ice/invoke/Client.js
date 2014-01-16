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
            //
            // re-throw exception so it propagates to final exception
            // handler.
            //
            var exceptionCB = function(ex){ throw ex; };
            var failCB = function(){ test(false); };
            
            setTimeout(function(){
                try
                {
                    var testString = "This is a test string";
                    var ref = "test:default -p 12010";
                    var base = communicator.stringToProxy(ref);
                    var cl;
                    var oneway;
                    
                    Test.MyClassPrx.checkedCast(base).then(
                        function(asyncResult, obj)
                        {
                            cl = obj;
                            oneway = Test.MyClassPrx.uncheckedCast(cl.ice_oneway());
                            out.write("testing ice_invoke... ");
                            return oneway.ice_invoke("opOneway", Ice.OperationMode.Normal, null, null);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, ok, outEncaps)
                        {
                            test(ok);
                            
                            var outS = Ice.createOutputStream(communicator);
                            outS.startEncapsulation();
                            outS.writeString(testString);
                            outS.endEncapsulation();
                            var inEncaps = outS.finished();
                            
                            return cl.ice_invoke("opString", Ice.OperationMode.Normal, inEncaps);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, ok, outEncaps)
                        {
                            test(ok);
                            var inS = Ice.createInputStream(communicator, outEncaps);
                            inS.startEncapsulation();
                            var s = inS.readString();
                            test(s == testString);
                            s = inS.readString();
                            inS.endEncapsulation();
                            test(s == testString);
                            
                            return cl.ice_invoke("opException", Ice.OperationMode.Normal, null);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, ok, outEncaps)
                        {
                            test(!ok);
                            var inS = Ice.createInputStream(communicator, outEncaps);
                            inS.startEncapsulation();
                            try
                            {
                                inS.throwException();
                            }
                            catch(ex)
                            {
                                test(ex instanceof Test.MyException);
                            }
                            inS.endEncapsulation();
                            out.writeLine("ok");
                            return cl.shutdown();
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
        module.exports.run = run;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/binding"));

            