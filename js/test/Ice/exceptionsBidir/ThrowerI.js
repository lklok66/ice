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

    var ThrowerI = function()
    {
    };

    ThrowerI.prototype = new Test.Thrower;
    ThrowerI.prototype.constructor = ThrowerI;

    ThrowerI.prototype.shutdown = function(current)
    {
        current.adapter.getCommunicator().shutdown();
    };

    ThrowerI.prototype.supportsUndeclaredExceptions = function(current)
    {
        return [true];
    };

    ThrowerI.prototype.supportsAssertException = function(current)
    {
        return [false];
    };

    ThrowerI.prototype.throwAasA = function(a, current)
    {
        var ex = new Test.A();
        ex.aMem = a;
        throw ex;
    };

    ThrowerI.prototype.throwAorDasAorD = function(a, current)
    {
        if(a > 0)
        {
            var ex = new Test.A();
            ex.aMem = a;
            throw ex;
        }
        else
        {
            var ex = new Test.D();
            ex.dMem = a;
            throw ex;
        }
    };

    ThrowerI.prototype.throwBasA = function(a, b, current)
    {
        this.throwBasB(a, b, current);
    };

    ThrowerI.prototype.throwBasB = function(a, b, current)
    {
        var ex = new Test.B();
        ex.aMem = a;
        ex.bMem = b;
        throw ex;
    };

    ThrowerI.prototype.throwCasA = function(a, b, c, current)
    {
        this.throwCasC(a, b, c, current);
    };

    ThrowerI.prototype.throwCasB = function(a, b, c, current)
    {
        this.throwCasC(a, b, c, current);
    };

    ThrowerI.prototype.throwCasC = function(a, b, c, current)
    {
        var ex = new Test.C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        throw ex;
    };

    ThrowerI.prototype.throwUndeclaredA = function(a, current)
    {
        var ex = new Test.A();
        ex.aMem = a;
        throw ex;
    };

    ThrowerI.prototype.throwUndeclaredB = function(a, b, current)
    {
        var ex = new Test.B();
        ex.aMem = a;
        ex.bMem = b;
        throw ex;
    };

    ThrowerI.prototype.throwUndeclaredC = function(a, b, c, current)
    {
        var ex = new Test.C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        throw ex;
    };

    ThrowerI.prototype.throwLocalException = function(current)
    {
        throw new Ice.TimeoutException();
    };

    ThrowerI.prototype.throwLocalExceptionIdempotent = function(current)
    {
        throw new Ice.TimeoutException();
    };

    ThrowerI.prototype.throwNonIceException = function(current)
    {
        throw new Error();
    };

    ThrowerI.prototype.throwAssertException = function(current)
    {
        test(false);
    };

    ThrowerI.prototype.throwMemoryLimitException = function(seq, current)
    {
        return [Ice.Buffer.createNative(1024 * 20)]; // 20KB is over the configured 10KB message size max.
    };

    ThrowerI.prototype.throwAfterResponse = function(current)
    {
        //
        // Only relevant for AMD.
        //
    };

    ThrowerI.prototype.throwAfterException = function(current)
    {
        //
        // Only relevant for AMD.
        //
        throw new Test.A();
    };

    global.ThrowerI = ThrowerI;
}(typeof (global) === "undefined" ? window : global));
