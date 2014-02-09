// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    var Ice = global.Ice;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var AMDMyDerivedClassI = function()
    {
    };

    AMDMyDerivedClassI.prototype = new TestAMD.MyDerivedClass;
    AMDMyDerivedClassI.prototype.constructor = AMDMyDerivedClassI;

    //
    // Override the Object "pseudo" operations to verify the operation mode.
    //

    AMDMyDerivedClassI.prototype.ice_isA = function(id, current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        return Ice.Object.prototype.ice_isA.call(this, id, current);
    };

    AMDMyDerivedClassI.prototype.ice_ping = function(current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        Ice.Object.prototype.ice_ping.call(this, current);
    };

    AMDMyDerivedClassI.prototype.ice_ids = function(current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        return Ice.Object.prototype.ice_ids.call(this, current);
    };

    AMDMyDerivedClassI.prototype.ice_id = function(current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        return Ice.Object.prototype.ice_id.call(this, current);
    };

    AMDMyDerivedClassI.prototype.shutdown_async = function(cb, current)
    {
        current.adapter.getCommunicator().shutdown();
        cb.ice_response();
    };

    AMDMyDerivedClassI.prototype.delay_async = function(cb, ms, current)
    {
        setTimeout(
            function()
            {
                cb.ice_response();
            }, ms);
    };

    AMDMyDerivedClassI.prototype.opVoid_async = function(cb, current)
    {
        test(current.mode === Ice.OperationMode.Normal);
        cb.ice_response();
    }

    AMDMyDerivedClassI.prototype.opBool_async = function(cb, p1, p2, current)
    {
        cb.ice_response(p2, p1);
    };

    AMDMyDerivedClassI.prototype.opBoolS_async = function(cb, p1, p2, current)
    {
        var p3 = p1.concat(p2);
        cb.ice_response(p1.reverse(), p3);
    };

    AMDMyDerivedClassI.prototype.opBoolSS_async = function(cb, p1, p2, current)
    {
        var p3 = p1.concat(p2);
        cb.ice_response(p1.reverse(), p3);
    };

    AMDMyDerivedClassI.prototype.opByte_async = function(cb, p1, p2, current)
    {
        cb.ice_response(p1, (p1 ^ p2) & 0xff);
    };

    AMDMyDerivedClassI.prototype.opByteBoolD_async = function(cb, p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        cb.ice_response(r, p1);
    };

    AMDMyDerivedClassI.prototype.opByteS_async = function(cb, p1, p2, current)
    {
        var p3 = Ice.Buffer.createNative(p1.length);
        for(var i = 0; i < p1.length; i++)
        {
            p3[i] = p1[p1.length - (i + 1)];
        }

        var r = Ice.Buffer.createNative(p1.length + p2.length);
        p1.copy(r);
        p2.copy(r, p1.length);
        cb.ice_response(r, p3);
    };

    AMDMyDerivedClassI.prototype.opByteSS_async = function(cb, p1, p2, current)
    {
        var r = p1.concat(p2);
        cb.ice_response(r, p1.reverse());
    };

    AMDMyDerivedClassI.prototype.opFloatDouble_async = function(cb, p1, p2, current)
    {
        cb.ice_response(p2, p1, p2);
    };

    AMDMyDerivedClassI.prototype.opFloatDoubleS_async = function(cb, p1, p2, current)
    {
        var r = p2.concat(p1);
        var p4 = p2.reverse();
        cb.ice_response(r, p1, p4);
    };

    AMDMyDerivedClassI.prototype.opFloatDoubleSS_async = function(cb, p1, p2, current)
    {
        var r = p2.concat(p2);
        var p4 = p2.reverse();
        cb.ice_response(r, p1, p4);
    };

    AMDMyDerivedClassI.prototype.opLongFloatD_async = function(cb, p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        cb.ice_response(r, p1);
    };

    AMDMyDerivedClassI.prototype.opMyClass_async = function(cb, p1, current)
    {
        var p2 = p1;
        var p3 = TestAMD.MyClassPrx.uncheckedCast(
            current.adapter.createProxy(current.adapter.getCommunicator().stringToIdentity("noSuchIdentity")));
        var r = TestAMD.MyClassPrx.uncheckedCast(current.adapter.createProxy(current.id));
        cb.ice_response(r, p2, p3);
    };

    AMDMyDerivedClassI.prototype.opMyEnum_async = function(cb, p1, current)
    {
        cb.ice_response(TestAMD.MyEnum.enum3, p1);
    };

    AMDMyDerivedClassI.prototype.opShortIntD_async = function(cb, p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        cb.ice_response(r, p1);
    };

    AMDMyDerivedClassI.prototype.opShortIntLong_async = function(cb, p1, p2, p3, current)
    {
        cb.ice_response(p3, p1, p2, p3);
    };

    AMDMyDerivedClassI.prototype.opShortIntLongS_async = function(cb, p1, p2, p3, current)
    {
        cb.ice_response(p3, p1, p2.reverse(), p3.concat(p3));
    };

    AMDMyDerivedClassI.prototype.opShortIntLongSS_async = function(cb, p1, p2, p3, current)
    {
        cb.ice_response(p3, p1, p2.reverse(), p3.concat(p3));
    };

    AMDMyDerivedClassI.prototype.opString_async = function(cb, p1, p2, current)
    {
        cb.ice_response(p1 + " " + p2, p2 + " " + p1);
    };

    AMDMyDerivedClassI.prototype.opStringMyEnumD_async = function(cb, p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        cb.ice_response(r, p1);
    };

    AMDMyDerivedClassI.prototype.opMyEnumStringD_async = function(cb, p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        cb.ice_response(r, p1);
    };

    AMDMyDerivedClassI.prototype.opMyStructMyEnumD_async = function(cb, p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        cb.ice_response(r, p1);
    };

    AMDMyDerivedClassI.prototype.opIntS_async = function(cb, s, current)
    {
        cb.ice_response(s.map(function(v, i, arr) { return -v; }));
    };

    AMDMyDerivedClassI.prototype.opByteSOneway_async = function(cb, s, current)
    {
        cb.ice_response();
    };

    AMDMyDerivedClassI.prototype.opContext_async = function(cb, current)
    {
        cb.ice_response(current.ctx);
    };

    AMDMyDerivedClassI.prototype.opDoubleMarshaling_async = function(cb, p1, p2, current)
    {
        var d = 1278312346.0 / 13.0;
        test(p1 === d);
        for(var i = 0; i < p2.length; ++i)
        {
            test(p2[i] === d);
        }
        cb.ice_response();
    };

    AMDMyDerivedClassI.prototype.opStringS_async = function(cb, p1, p2, current)
    {
        var p3 = p1.concat(p2);
        var r = p1.reverse();
        cb.ice_response(r, p3);
    };

    AMDMyDerivedClassI.prototype.opStringSS_async = function(cb, p1, p2, current)
    {
        var p3 = p1.concat(p2);
        var r = p2.reverse();
        cb.ice_response(r, p3);
    };

    AMDMyDerivedClassI.prototype.opStringSSS_async = function(cb, p1, p2, current)
    {
        var p3 = p1.concat(p2);
        var r = p2.reverse();
        cb.ice_response(r, p3);
    };

    AMDMyDerivedClassI.prototype.opStringStringD_async = function(cb, p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        cb.ice_response(r, p1);
    };

    AMDMyDerivedClassI.prototype.opStruct_async = function(cb, p1, p2, current)
    {
        p1.s.s = "a new string";
        cb.ice_response(p2, p1);
    };

    AMDMyDerivedClassI.prototype.opIdempotent_async = function(cb, current)
    {
        test(current.mode === Ice.OperationMode.Idempotent);
        cb.ice_response();
    };

    AMDMyDerivedClassI.prototype.opNonmutating_async = function(cb, current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        cb.ice_response();
    };

    AMDMyDerivedClassI.prototype.opDerived_async = function(cb, current)
    {
        cb.ice_response();
    };

    global.AMDMyDerivedClassI = AMDMyDerivedClassI;
}(typeof (global) === "undefined" ? window : global));
