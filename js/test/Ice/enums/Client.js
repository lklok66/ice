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
            
            setTimeout(function(){
                try
                {
                    out.write("testing stringToProxy... ");
                    var ref = "test:default -p 12010";
                    var base = communicator.stringToProxy(ref);
                    test(base !== null);
                    out.writeLine("ok");
                    
                    var proxy;
                    out.write("testing checked cast... ");
                    Test.TestIntfPrx.checkedCast(base).then(
                        function(asyncResult, obj)
                        {
                            proxy = obj;
                            test(proxy !== null);
                            test(proxy.equals(base));
                            out.writeLine("ok");
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            out.write("testing enum values... ");
                            test(Test.ByteEnum.benum1.value === 0);
                            test(Test.ByteEnum.benum2.value === 1);
                            test(Test.ByteEnum.benum3.value === Test.ByteConst1);
                            test(Test.ByteEnum.benum4.value === Test.ByteConst1 + 1);
                            test(Test.ByteEnum.benum5.value === Test.ShortConst1);
                            test(Test.ByteEnum.benum6.value === Test.ShortConst1 + 1);
                            test(Test.ByteEnum.benum7.value === Test.IntConst1);
                            test(Test.ByteEnum.benum8.value === Test.IntConst1 + 1);
                            test(Test.ByteEnum.benum9.value === Test.LongConst1);
                            test(Test.ByteEnum.benum10.value === Test.LongConst1 + 1);
                            test(Test.ByteEnum.benum11.value === Test.ByteConst2);

                            test(Test.ByteEnum.valueOf(0) === Test.ByteEnum.benum1);
                            test(Test.ByteEnum.valueOf(1) === Test.ByteEnum.benum2);
                            test(Test.ByteEnum.valueOf(Test.ByteConst1) === Test.ByteEnum.benum3);
                            test(Test.ByteEnum.valueOf(Test.ByteConst1+ 1) === Test.ByteEnum.benum4);
                            test(Test.ByteEnum.valueOf(Test.ShortConst1) === Test.ByteEnum.benum5);
                            test(Test.ByteEnum.valueOf(Test.ShortConst1 + 1) === Test.ByteEnum.benum6);
                            test(Test.ByteEnum.valueOf(Test.IntConst1) === Test.ByteEnum.benum7);
                            test(Test.ByteEnum.valueOf(Test.IntConst1 + 1) === Test.ByteEnum.benum8);
                            test(Test.ByteEnum.valueOf(Test.LongConst1) === Test.ByteEnum.benum9);
                            test(Test.ByteEnum.valueOf(Test.LongConst1 + 1) === Test.ByteEnum.benum10);
                            test(Test.ByteEnum.valueOf(Test.ByteConst2) === Test.ByteEnum.benum11);

                            test(Test.ShortEnum.senum1.value === 3);
                            test(Test.ShortEnum.senum2.value === 4);
                            test(Test.ShortEnum.senum3.value === Test.ByteConst1);
                            test(Test.ShortEnum.senum4.value === Test.ByteConst1 + 1);
                            test(Test.ShortEnum.senum5.value === Test.ShortConst1);
                            test(Test.ShortEnum.senum6.value === Test.ShortConst1 + 1);
                            test(Test.ShortEnum.senum7.value === Test.IntConst1);
                            test(Test.ShortEnum.senum8.value === Test.IntConst1 + 1);
                            test(Test.ShortEnum.senum9.value === Test.LongConst1);
                            test(Test.ShortEnum.senum10.value === Test.LongConst1+ 1);
                            test(Test.ShortEnum.senum11.value === Test.ShortConst2);

                            test(Test.ShortEnum.valueOf(3) === Test.ShortEnum.senum1);
                            test(Test.ShortEnum.valueOf(4) === Test.ShortEnum.senum2);
                            test(Test.ShortEnum.valueOf(Test.ByteConst1) === Test.ShortEnum.senum3);
                            test(Test.ShortEnum.valueOf(Test.ByteConst1 + 1) === Test.ShortEnum.senum4);
                            test(Test.ShortEnum.valueOf(Test.ShortConst1) === Test.ShortEnum.senum5);
                            test(Test.ShortEnum.valueOf(Test.ShortConst1 + 1) === Test.ShortEnum.senum6);
                            test(Test.ShortEnum.valueOf(Test.IntConst1) === Test.ShortEnum.senum7);
                            test(Test.ShortEnum.valueOf(Test.IntConst1 + 1) === Test.ShortEnum.senum8);
                            test(Test.ShortEnum.valueOf(Test.LongConst1) === Test.ShortEnum.senum9);
                            test(Test.ShortEnum.valueOf(Test.LongConst1 + 1) === Test.ShortEnum.senum10);
                            test(Test.ShortEnum.valueOf(Test.ShortConst2) === Test.ShortEnum.senum11);

                            test(Test.IntEnum.ienum1.value === 0);
                            test(Test.IntEnum.ienum2.value === 1);
                            test(Test.IntEnum.ienum3.value === Test.ByteConst1);
                            test(Test.IntEnum.ienum4.value === Test.ByteConst1 + 1);
                            test(Test.IntEnum.ienum5.value === Test.ShortConst1);
                            test(Test.IntEnum.ienum6.value === Test.ShortConst1 + 1);
                            test(Test.IntEnum.ienum7.value === Test.IntConst1);
                            test(Test.IntEnum.ienum8.value === Test.IntConst1 + 1);
                            test(Test.IntEnum.ienum9.value === Test.LongConst1);
                            test(Test.IntEnum.ienum10.value === Test.LongConst1 + 1);
                            test(Test.IntEnum.ienum11.value === Test.IntConst2);
                            test(Test.IntEnum.ienum12.value === Test.LongConst2);

                            test(Test.IntEnum.valueOf(0) === Test.IntEnum.ienum1);
                            test(Test.IntEnum.valueOf(1) === Test.IntEnum.ienum2);
                            test(Test.IntEnum.valueOf(Test.ByteConst1) === Test.IntEnum.ienum3);
                            test(Test.IntEnum.valueOf(Test.ByteConst1 + 1) === Test.IntEnum.ienum4);
                            test(Test.IntEnum.valueOf(Test.ShortConst1) === Test.IntEnum.ienum5);
                            test(Test.IntEnum.valueOf(Test.ShortConst1 + 1) === Test.IntEnum.ienum6);
                            test(Test.IntEnum.valueOf(Test.IntConst1) === Test.IntEnum.ienum7);
                            test(Test.IntEnum.valueOf(Test.IntConst1 + 1) === Test.IntEnum.ienum8);
                            test(Test.IntEnum.valueOf(Test.LongConst1) === Test.IntEnum.ienum9);
                            test(Test.IntEnum.valueOf(Test.LongConst1 + 1) === Test.IntEnum.ienum10);
                            test(Test.IntEnum.valueOf(Test.IntConst2) === Test.IntEnum.ienum11);
                            test(Test.IntEnum.valueOf(Test.LongConst2) === Test.IntEnum.ienum12);

                            test(Test.SimpleEnum.red.value === 0);
                            test(Test.SimpleEnum.green.value === 1);
                            test(Test.SimpleEnum.blue.value === 2);

                            test(Test.SimpleEnum.valueOf(0) === Test.SimpleEnum.red);
                            test(Test.SimpleEnum.valueOf(1) === Test.SimpleEnum.green);
                            test(Test.SimpleEnum.valueOf(2) === Test.SimpleEnum.blue);
                            
                            out.writeLine("ok");
                            out.write("testing enum streaming... ");

                            var os, bytes;
                            var encoding_1_0 = 
                                communicator.getProperties().getProperty("Ice.Default.EncodingVersion") === "1.0";

                            os = Ice.createOutputStream(communicator);
                            Test.ByteEnum.write(os, Test.ByteEnum.benum11);
                            bytes = os.finished();
                            test(bytes.length === 1); // ByteEnum should require one byte

                            os = Ice.createOutputStream(communicator);
                            Test.ShortEnum.write(os, Test.ShortEnum.senum11);
                            bytes = os.finished();
                            test(bytes.length === (encoding_1_0 ? 2 : 5));

                            os = Ice.createOutputStream(communicator);
                            Test.IntEnum.write(os, Test.IntEnum.ienum11);
                            bytes = os.finished();
                            test(bytes.length === (encoding_1_0 ? 4 : 5));

                            os = Ice.createOutputStream(communicator);
                            Test.SimpleEnum.write(os, Test.SimpleEnum.blue);
                            bytes = os.finished();
                            test(bytes.length === 1); // SimpleEnum should require one byte

                            out.writeLine("ok");
                        }, 
                        exceptionCB
                    ).then(
                        function()
                        {
                            out.write("testing enum operations... ");
                            return proxy.opByte(Test.ByteEnum.benum1);
                        }
                    ).then(
                        function(asyncResult, r, b1)
                        {
                            test(r === b1);
                            test(r === Test.ByteEnum.benum1);
                            return proxy.opByte(Test.ByteEnum.benum11);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, r, b11)
                        {
                            test(r === b11);
                            test(r === Test.ByteEnum.benum11);
                            return proxy.opShort(Test.ShortEnum.senum1);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, r, s1)
                        {
                            test(r === s1);
                            test(r === Test.ShortEnum.senum1);
                            return proxy.opShort(Test.ShortEnum.senum11);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, r, s11)
                        {
                            test(r === s11);
                            test(r === Test.ShortEnum.senum11);
                            return proxy.opInt(Test.IntEnum.ienum1);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, r, i1)
                        {
                            test(r === i1);
                            test(r === Test.IntEnum.ienum1);
                            return proxy.opInt(Test.IntEnum.ienum11);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, r, i11)
                        {
                            test(r === i11);
                            test(r === Test.IntEnum.ienum11);
                            return proxy.opInt(Test.IntEnum.ienum12);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, r, i12)
                        {
                            test(r === i12);
                            test(r === Test.IntEnum.ienum12);
                            return proxy.opSimple(Test.SimpleEnum.green);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, r, g)
                        {
                            test(r === g);
                            test(r === Test.SimpleEnum.green);
                            
                            out.writeLine("ok");
                            out.write("testing enum exceptions... ");
                            
                            var os, is;
                            try
                            {
                                os = Ice.createOutputStream(communicator);
                                os.writeByte(2); // Invalid enumerator
                                is = Ice.createInputStream(communicator, os.finished());
                                var e = Test.ByteEnum.read(is);
                                test(false);
                            }
                            catch(ex)
                            {
                                test(ex instanceof Ice.MarshalException);
                            }
                            
                            try
                            {
                                os = Ice.createOutputStream(communicator);
                                os.writeByte(128); // Invalid enumerator
                                is = Ice.createInputStream(communicator, os.finished());
                                Test.ByteEnum.read(is);
                                test(false);
                            }
                            catch(ex)
                            {
                                test(ex instanceof Ice.MarshalException);
                            }
                            
                            try
                            {
                                os = Ice.createOutputStream(communicator);
                                os.writeShort(-1); // Negative enumerators are not supported
                                is = Ice.createInputStream(communicator, os.finished());
                                Test.ShortEnum.read(is);
                                test(false);
                            }
                            catch(ex)
                            {
                                test(ex instanceof Ice.MarshalException);
                            }
                            
                            try
                            {
                                os = Ice.createOutputStream(communicator);
                                os.writeShort(0); // Invalid enumerator
                                is = Ice.createInputStream(communicator, os.finished());
                                var e = Test.ShortEnum.read(is);
                                test(false);
                            }
                            catch(ex)
                            {
                                test(ex instanceof Ice.MarshalException);
                            }
                            
                            try
                            {
                                os = Ice.createOutputStream(communicator);
                                os.writeShort(32767); // Invalid enumerator
                                is = Ice.createInputStream(communicator, os.finished());
                                Test.ShortEnum.read(is);
                                test(false);
                            }
                            catch(ex)
                            {
                                test(ex instanceof Ice.MarshalException);
                            }
                            
                            try
                            {
                                os = Ice.createOutputStream(communicator);
                                os.writeInt(-1); // Negative enumerators are not supported
                                is = Ice.createInputStream(communicator, os.finished());
                                Test.IntEnum.read(is);
                                test(false);
                            }
                            catch(ex)
                            {
                                test(ex instanceof Ice.MarshalException);
                            }
                            
                            try
                            {
                                os = Ice.createOutputStream(communicator);
                                os.writeInt(2); // Invalid enumerator
                                is = Ice.createInputStream(communicator, os.finished());
                                Test.IntEnum.read(is);
                                test(false);
                            }
                            catch(ex)
                            {
                                test(ex instanceof Ice.MarshalException);
                            }

                            out.writeLine("ok");
                            return proxy.shutdown();
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
            module.exports.test.Common.TestSuite.add("Ice/enums", run);
        }
        
        module.exports.test.Ice = module.exports.test.Ice || {};
        module.exports.test.Ice.enums = {run: run};
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/enums"));
