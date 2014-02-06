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

    var AMDThrowerI = function()
    {
    };

    AMDThrowerI.prototype = new TestAMD.Thrower;
    AMDThrowerI.prototype.constructor = AMDThrowerI;

    AMDThrowerI.prototype.shutdown = function(cb, current)
    {
        current.adapter.getCommunicator().shutdown();
        cb.ice_response();
    };

    AMDThrowerI.prototype.supportsUndeclaredExceptions = function(cb, current)
    {
        cb.ice_response(true);
    };

    AMDThrowerI.prototype.supportsAssertException = function(cb, current)
    {
        cb.ice_response(false);
    };

    AMDThrowerI.prototype.throwAasA = function(cb, a, current)
    {
        var ex = new TestAMD.A();
        ex.aMem = a;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwAorDasAorD = function(cb, a, current)
    {
        if(a > 0)
        {
            var ex = new TestAMD.A();
            ex.aMem = a;
            cb.ice_exception(ex);
        }
        else
        {
            var ex = new TestAMD.D();
            ex.dMem = a;
            cb.ice_exception(ex);
        }
    };

    AMDThrowerI.prototype.throwBasA = function(cb, a, b, current)
    {
        this.throwBasB(cb, a, b, current);
    };

    AMDThrowerI.prototype.throwBasB = function(cb, a, b, current)
    {
        var ex = new TestAMD.B();
        ex.aMem = a;
        ex.bMem = b;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwCasA = function(cb, a, b, c, current)
    {
        this.throwCasC(cb, a, b, c, current);
    };

    AMDThrowerI.prototype.throwCasB = function(cb, a, b, c, current)
    {
        this.throwCasC(cb, a, b, c, current);
    };

    AMDThrowerI.prototype.throwCasC = function(cb, a, b, c, current)
    {
        var ex = new TestAMD.C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwUndeclaredA = function(cb, a, current)
    {
        var ex = new TestAMD.A();
        ex.aMem = a;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwUndeclaredB = function(cb, a, b, current)
    {
        var ex = new TestAMD.B();
        ex.aMem = a;
        ex.bMem = b;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwUndeclaredC = function(cb, a, b, c, current)
    {
        var ex = new TestAMD.C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwLocalException = function(cb, current)
    {
        cb.ice_exception(new Ice.TimeoutException());
    };

    AMDThrowerI.prototype.throwLocalExceptionIdempotent = function(cb, current)
    {
        cb.ice_exception(new Ice.TimeoutException());
    };

    AMDThrowerI.prototype.throwNonIceException = function(cb, current)
    {
        cb.ice_exception(new Error());
    };

    AMDThrowerI.prototype.throwAssertException = function(cb, current)
    {
        test(false);
    };

    AMDThrowerI.prototype.throwMemoryLimitException = function(cb, seq, current)
    {
        cb.ice_response(Ice.Buffer.createNative(1024 * 20)); // 20KB is over the configured 10KB message size max.
    };

    AMDThrowerI.prototype.throwAfterResponse = function(cb, current)
    {
        cb.ice_response();

        throw new Error();
    };

    AMDThrowerI.prototype.throwAfterException = function(cb, current)
    {
        cb.ice_exception(new TestAMD.A());

        throw new Error();
    };

    global.AMDThrowerI = AMDThrowerI;
}(typeof (global) === "undefined" ? window : global));
