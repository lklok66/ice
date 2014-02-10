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

    var InitialI = function()
    {
    };

    InitialI.prototype = new Test.Initial;
    InitialI.prototype.constructor = InitialI;

    InitialI.prototype.shutdown = function(current)
    {
        current.adapter.getCommunicator().shutdown();
    };

    InitialI.prototype.pingPong = function(obj, current)
    {
        return [obj];
    };

    InitialI.prototype.opOptionalException = function(a, b, o, current)
    {
        var ex = new Test.OptionalException();
        if(a !== undefined)
        {
            ex.a = a;
        }
        else
        {
            ex.a = undefined; // The member "a" has a default value.
        }
        if(b !== undefined)
        {
            ex.b = b;
        }
        if(o !== undefined)
        {
            ex.o = o;
        }
        throw ex;
    };

    InitialI.prototype.opDerivedException = function(a, b, o, current)
    {
        var ex = new Test.DerivedException();
        if(a !== undefined)
        {
            ex.a = a;
        }
        else
        {
            ex.a = undefined; // The member "a" has a default value.
        }
        if(b !== undefined)
        {
            ex.b = b;
            ex.ss = b;
        }
        else
        {
            ex.ss = undefined; // The member "ss" has a default value.
        }
        if(o !== undefined)
        {
            ex.o = o;
            ex.o2 = o;
        }
        throw ex;
    };

    InitialI.prototype.opRequiredException = function(a, b, o, current)
    {
        var ex = new Test.RequiredException();
        if(a !== undefined)
        {
            ex.a = a;
        }
        else
        {
            ex.a = undefined; // The member "a" has a default value.
        }
        if(b !== undefined)
        {
            ex.b = b;
            ex.ss = b;
        }
        if(o !== undefined)
        {
            ex.o = o;
            ex.o2 = o;
        }
        throw ex;
    };

    InitialI.prototype.opByte = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opByteReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opBool = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opBoolReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opShort = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opShortReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opInt = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opIntReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opLong = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opLongReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFloat = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFloatReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opDouble = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opDoubleReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opString = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opStringReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opMyEnum = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opMyEnumReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSmallStruct = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSmallStructReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFixedStruct = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFixedStructReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opVarStruct = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opVarStructReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opOneOptional = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opOneOptionalReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opOneOptionalProxy = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opOneOptionalProxyReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opByteSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opByteSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opBoolSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opBoolSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opShortSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opShortSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opIntSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opIntSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opLongSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opLongSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFloatSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFloatSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opDoubleSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opDoubleSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opStringSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opStringSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSmallStructSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSmallStructSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSmallStructList = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSmallStructListReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFixedStructSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFixedStructSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFixedStructList = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opFixedStructListReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opVarStructSeq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opVarStructSeqReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSerializable = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opSerializableReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opIntIntDict = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opIntIntDictReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opStringIntDict = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opStringIntDictReq = function(p1, current)
    {
        return [p1, p1];
    };

    InitialI.prototype.opClassAndUnknownOptional = function(p, current)
    {
    };

    InitialI.prototype.sendOptionalClass = function(req, current)
    {
    };

    InitialI.prototype.returnOptionalClass = function(req, current)
    {
        return [new Test.OneOptional(53)];
    };

    InitialI.prototype.supportsRequiredParams = function(current)
    {
        return false;
    };

    InitialI.prototype.supportsJavaSerializable = function(current)
    {
        return false;
    };

    InitialI.prototype.supportsCsharpSerializable = function(current)
    {
        return false;
    };

    global.InitialI = InitialI;
}(typeof (global) === "undefined" ? window : global));
