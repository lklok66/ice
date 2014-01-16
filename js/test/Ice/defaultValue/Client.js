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

        var run = function(out, id)
        {
            var p = new Ice.Promise();
            setTimeout(
                function()
                {
                    var c = null;
                    try
                    {
                        out.write("testing default values... ");
                        var v = new Test.Struct1();
                        test(!v.boolFalse);
                        test(v.boolTrue);
                        test(v.b === 254);
                        test(v.s === 16000);
                        test(v.i === 3);
                        test(v.l === 4);
                        test(v.f === 5.1);
                        test(v.d === 6.2);
                        test(v.str === "foo \\ \"bar\n \r\n\t\u000b\f\u0007\b? \u0007 \u0007");
                        test(v.c1 === Test.Color.red);
                        test(v.c2 === Test.Color.green);
                        test(v.c3 === Test.Color.blue);
                        test(v.nc1 === Test.Nested.Color.red);
                        test(v.nc2 === Test.Nested.Color.green);
                        test(v.nc3 === Test.Nested.Color.blue);
                        test(v.noDefault === null);
                        test(v.zeroI === 0);
                        test(v.zeroL === 0);
                        test(v.zeroF === 0);
                        test(v.zeroDotF === 0);
                        test(v.zeroD === 0);
                        test(v.zeroDotD === 0);
                        
                        v = new Test.Struct2();
                        test(v.boolTrue === Test.ConstBool);
                        test(v.b === Test.ConstByte);
                        test(v.s === Test.ConstShort);
                        test(v.i === Test.ConstInt);
                        test(v.l === Test.ConstLong);
                        test(v.f === Test.ConstFloat);
                        test(v.d === Test.ConstDouble);
                        test(v.str ==  Test.ConstString);
                        test(v.c1 === Test.ConstColor1);
                        test(v.c2 === Test.ConstColor2);
                        test(v.c3 === Test.ConstColor3);
                        test(v.nc1 === Test.ConstNestedColor1);
                        test(v.nc2 === Test.ConstNestedColor2);
                        test(v.nc3 === Test.ConstNestedColor3);
                        
                        v = new Test.Base();
                        test(!v.boolFalse);
                        test(v.boolTrue);
                        test(v.b === 1);
                        test(v.s === 2);
                        test(v.i === 3);
                        test(v.l === 4);
                        test(v.f === 5.1);
                        test(v.d === 6.2);
                        test(v.str == "foo \\ \"bar\n \r\n\t\u000b\f\u0007\b? \007 \u0007");
                        test(v.noDefault === null);
                        test(v.zeroI === 0);
                        test(v.zeroL === 0);
                        test(v.zeroF === 0);
                        test(v.zeroDotF === 0);
                        test(v.zeroD === 0);
                        test(v.zeroDotD === 0);
                        
                        v = new Test.Derived();
                        test(!v.boolFalse);
                        test(v.boolTrue);
                        test(v.b === 1);
                        test(v.s === 2);
                        test(v.i === 3);
                        test(v.l === 4);
                        test(v.f === 5.1);
                        test(v.d === 6.2);
                        test(v.str == "foo \\ \"bar\n \r\n\t\u000b\f\u0007\b? \007 \u0007");
                        test(v.c1 === Test.Color.red);
                        test(v.c2 === Test.Color.green);
                        test(v.c3 === Test.Color.blue);
                        test(v.nc1 === Test.Nested.Color.red);
                        test(v.nc2 === Test.Nested.Color.green);
                        test(v.nc3 === Test.Nested.Color.blue);
                        test(v.noDefault === null);
                        test(v.zeroI === 0);
                        test(v.zeroL === 0);
                        test(v.zeroF === 0);
                        test(v.zeroDotF === 0);
                        test(v.zeroD === 0);
                        test(v.zeroDotD === 0);
                        
                        v = new Test.BaseEx();
                        test(!v.boolFalse);
                        test(v.boolTrue);
                        test(v.b === 1);
                        test(v.s === 2);
                        test(v.i === 3);
                        test(v.l === 4);
                        test(v.f === 5.1);
                        test(v.d === 6.2);
                        test(v.str == "foo \\ \"bar\n \r\n\t\u000b\f\u0007\b? \007 \u0007");
                        test(v.noDefault === null);
                        test(v.zeroI === 0);
                        test(v.zeroL === 0);
                        test(v.zeroF === 0);
                        test(v.zeroDotF === 0);
                        test(v.zeroD === 0);
                        test(v.zeroDotD === 0);
                        
                        v = new Test.DerivedEx();
                        test(!v.boolFalse);
                        test(v.boolTrue);
                        test(v.b === 1);
                        test(v.s === 2);
                        test(v.i === 3);
                        test(v.l === 4);
                        test(v.f === 5.1);
                        test(v.d === 6.2);
                        test(v.str == "foo \\ \"bar\n \r\n\t\u000b\f\u0007\b? \007 \u0007");
                        test(v.noDefault === null);
                        test(v.c1 === Test.Color.red);
                        test(v.c2 === Test.Color.green);
                        test(v.c3 === Test.Color.blue);
                        test(v.nc1 === Test.Nested.Color.red);
                        test(v.nc2 === Test.Nested.Color.green);
                        test(v.nc3 === Test.Nested.Color.blue);
                        test(v.zeroI === 0);
                        test(v.zeroL === 0);
                        test(v.zeroF === 0);
                        test(v.zeroDotF === 0);
                        test(v.zeroD === 0);
                        test(v.zeroDotD === 0);
                        
                        out.writeLine("ok");
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
}(typeof module !== "undefined" ? module : undefined, "test/Ice/defaultValue"));
