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
    var Test = global.Test;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var MyDerivedClassI = function()
    {
    };

    MyDerivedClassI.prototype = new Test.MyDerivedClass;
    MyDerivedClassI.prototype.constructor = MyDerivedClassI;

    //
    // Override the Object "pseudo" operations to verify the operation mode.
    //

    MyDerivedClassI.prototype.ice_isA = function(id, current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        return Ice.Object.prototype.ice_isA.call(this, id, current);
    };

    MyDerivedClassI.prototype.ice_ping = function(current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        Ice.Object.prototype.ice_ping.call(this, current);
    };

    MyDerivedClassI.prototype.ice_ids = function(current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        return Ice.Object.prototype.ice_ids.call(this, current);
    };

    MyDerivedClassI.prototype.ice_id = function(current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
        return Ice.Object.prototype.ice_id.call(this, current);
    };

    MyDerivedClassI.prototype.shutdown = function(current)
    {
        current.adapter.getCommunicator().shutdown();
    };

    MyDerivedClassI.prototype.delay = function(cb, ms, current)
    {
        setTimeout(
            function()
            {
                cb.ice_response();
            }, ms);
    };

    MyDerivedClassI.prototype.opVoid = function(current)
    {
        test(current.mode === Ice.OperationMode.Normal);
    }

    MyDerivedClassI.prototype.opBool = function(p1, p2, current)
    {
        return [p2, p1];
    };

    MyDerivedClassI.prototype.opBoolS = function(p1, p2, current)
    {
        var p3 = p1.concat(p2);
        return [p1.reverse(), p3];
    };

    MyDerivedClassI.prototype.opBoolSS = function(p1, p2, current)
    {
        var p3 = p1.concat(p2);
        return [p1.reverse(), p3];
    };

    MyDerivedClassI.prototype.opByte = function(p1, p2, current)
    {
        return [p1, (p1 ^ p2) & 0xff];
    };

    MyDerivedClassI.prototype.opByteBoolD = function(p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        return [r, p1];
    };

    MyDerivedClassI.prototype.opByteS = function(p1, p2, current)
    {
        var p3 = Ice.Buffer.createNative(p1.length);
        for(var i = 0; i < p1.length; i++)
        {
            p3[i] = p1[p1.length - (i + 1)];
        }

        var r = Ice.Buffer.createNative(p1.length + p2.length);
        p1.copy(r);
        p2.copy(r, p1.length);
        return [r, p3];
    };

    MyDerivedClassI.prototype.opByteSS = function(p1, p2, current)
    {
        var r = p1.concat(p2);
        return [r, p1.reverse()];
    };

    MyDerivedClassI.prototype.opFloatDouble = function(p1, p2, current)
    {
        return [p2, p1, p2];
    };

    MyDerivedClassI.prototype.opFloatDoubleS = function(p1, p2, current)
    {
        var r = p2.concat(p1);
        var p4 = p2.reverse();
        return [r, p1, p4];
    };

    MyDerivedClassI.prototype.opFloatDoubleSS = function(p1, p2, current)
    {
        var r = p2.concat(p2);
        var p4 = p2.reverse();
        return [r, p1, p4];
    };

    MyDerivedClassI.prototype.opLongFloatD = function(p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        return [r, p1];
    };

    MyDerivedClassI.prototype.opMyClass = function(p1, current)
    {
        var p2 = p1;
        var p3 = Test.MyClassPrx.uncheckedCast(
            current.adapter.createProxy(current.adapter.getCommunicator().stringToIdentity("noSuchIdentity")));
        var r = Test.MyClassPrx.uncheckedCast(current.adapter.createProxy(current.id));
        return [r, p2, p3];
    };

    MyDerivedClassI.prototype.opMyEnum = function(p1, current)
    {
        return [Test.MyEnum.enum3, p1];
    };

    MyDerivedClassI.prototype.opShortIntD = function(p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        return [r, p1];
    };

    MyDerivedClassI.prototype.opShortIntLong = function(p1, p2, p3, current)
    {
        return [p3, p1, p2, p3];
    };

    MyDerivedClassI.prototype.opShortIntLongS = function(p1, p2, p3, current)
    {
        return [p3, p1, p2.reverse(), p3.concat(p3)];
    };

    MyDerivedClassI.prototype.opShortIntLongSS = function(p1, p2, p3, current)
    {
        return [p3, p1, p2.reverse(), p3.concat(p3)];
    };

    MyDerivedClassI.prototype.opString = function(p1, p2, current)
    {
        return [p1 + " " + p2, p2 + " " + p1];
    };

    MyDerivedClassI.prototype.opStringMyEnumD = function(p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        return [r, p1];
    };

    MyDerivedClassI.prototype.opMyEnumStringD = function(p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        return [r, p1];
    };

    MyDerivedClassI.prototype.opMyStructMyEnumD = function(p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        return [r, p1];
    };

    MyDerivedClassI.prototype.opIntS = function(s, current)
    {
        return [s.map(function(v, i, arr) { return -v; })];
    };

    MyDerivedClassI.prototype.opByteSOneway = function(s, current)
    {
    };

    MyDerivedClassI.prototype.opContext = function(current)
    {
        return [current.ctx];
    };

    MyDerivedClassI.prototype.opDoubleMarshaling = function(p1, p2, current)
    {
        var d = 1278312346.0 / 13.0;
        test(p1 === d);
        for(var i = 0; i < p2.length; ++i)
        {
            test(p2[i] === d);
        }
    };

    MyDerivedClassI.prototype.opStringS = function(p1, p2, current)
    {
        var p3 = p1.concat(p2);
        var r = p1.reverse();
        return [r, p3];
    };

    MyDerivedClassI.prototype.opStringSS = function(p1, p2, current)
    {
        var p3 = p1.concat(p2);
        var r = p2.reverse();
        return [r, p3];
    };

    MyDerivedClassI.prototype.opStringSSS = function(p1, p2, current)
    {
        var p3 = p1.concat(p2);
        var r = p2.reverse();
        return [r, p3];
    };

    MyDerivedClassI.prototype.opStringStringD = function(p1, p2, current)
    {
        var r = p1.clone();
        r.merge(p2);
        return [r, p1];
    };

    MyDerivedClassI.prototype.opStruct = function(p1, p2, current)
    {
        p1.s.s = "a new string";
        return [p2, p1];
    };

    MyDerivedClassI.prototype.opIdempotent = function(current)
    {
        test(current.mode === Ice.OperationMode.Idempotent);
    };

    MyDerivedClassI.prototype.opNonmutating = function(current)
    {
        test(current.mode === Ice.OperationMode.Nonmutating);
    };

    MyDerivedClassI.prototype.opDerived = function(current)
    {
    };

    global.MyDerivedClassI = MyDerivedClassI;
}(typeof (global) === "undefined" ? window : global));
