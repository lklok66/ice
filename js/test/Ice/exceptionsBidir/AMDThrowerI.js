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

    AMDThrowerI.prototype.shutdown_async = function(cb, current)
    {
        current.adapter.getCommunicator().shutdown();
        cb.ice_response();
    };

    AMDThrowerI.prototype.supportsUndeclaredExceptions_async = function(cb, current)
    {
        cb.ice_response(true);
    };

    AMDThrowerI.prototype.supportsAssertException_async = function(cb, current)
    {
        cb.ice_response(false);
    };

    AMDThrowerI.prototype.throwAasA_async = function(cb, a, current)
    {
        var ex = new TestAMD.A();
        ex.aMem = a;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwAorDasAorD_async = function(cb, a, current)
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

    AMDThrowerI.prototype.throwBasA_async = function(cb, a, b, current)
    {
        this.throwBasB(cb, a, b, current);
    };

    AMDThrowerI.prototype.throwBasB_async = function(cb, a, b, current)
    {
        var ex = new TestAMD.B();
        ex.aMem = a;
        ex.bMem = b;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwCasA_async = function(cb, a, b, c, current)
    {
        this.throwCasC(cb, a, b, c, current);
    };

    AMDThrowerI.prototype.throwCasB_async = function(cb, a, b, c, current)
    {
        this.throwCasC(cb, a, b, c, current);
    };

    AMDThrowerI.prototype.throwCasC_async = function(cb, a, b, c, current)
    {
        var ex = new TestAMD.C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwUndeclaredA_async = function(cb, a, current)
    {
        var ex = new TestAMD.A();
        ex.aMem = a;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwUndeclaredB_async = function(cb, a, b, current)
    {
        var ex = new TestAMD.B();
        ex.aMem = a;
        ex.bMem = b;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwUndeclaredC_async = function(cb, a, b, c, current)
    {
        var ex = new TestAMD.C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    };

    AMDThrowerI.prototype.throwLocalException_async = function(cb, current)
    {
        cb.ice_exception(new Ice.TimeoutException());
    };

    AMDThrowerI.prototype.throwLocalExceptionIdempotent_async = function(cb, current)
    {
        cb.ice_exception(new Ice.TimeoutException());
    };

    AMDThrowerI.prototype.throwNonIceException_async = function(cb, current)
    {
        cb.ice_exception(new Error());
    };

    AMDThrowerI.prototype.throwAssertException_async = function(cb, current)
    {
        test(false);
    };

    AMDThrowerI.prototype.throwMemoryLimitException_async = function(cb, seq, current)
    {
        cb.ice_response(Ice.Buffer.createNative(1024 * 20)); // 20KB is over the configured 10KB message size max.
    };

    AMDThrowerI.prototype.throwAfterResponse_async = function(cb, current)
    {
        cb.ice_response();

        throw new Error();
    };

    AMDThrowerI.prototype.throwAfterException_async = function(cb, current)
    {
        cb.ice_exception(new TestAMD.A());

        throw new Error();
    };

    global.AMDThrowerI = AMDThrowerI;
}(typeof (global) === "undefined" ? window : global));
