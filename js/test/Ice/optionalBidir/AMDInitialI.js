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
    var TestAMD = global.TestAMD;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var AMDInitialI = function()
    {
    };

    AMDInitialI.prototype = new TestAMD.Initial;
    AMDInitialI.prototype.constructor = AMDInitialI;

    AMDInitialI.prototype.shutdown_async = function(cb, current)
    {
        current.adapter.getCommunicator().shutdown();
        cb.ice_response();
    };

    AMDInitialI.prototype.pingPong_async = function(cb, obj, current)
    {
        cb.ice_response(obj);
    };

    AMDInitialI.prototype.opOptionalException_async = function(cb, a, b, o, current)
    {
        var ex = new TestAMD.OptionalException();
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
        cb.ice_exception(ex);
    };

    AMDInitialI.prototype.opDerivedException_async = function(cb, a, b, o, current)
    {
        var ex = new TestAMD.DerivedException();
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
        cb.ice_exception(ex);
    };

    AMDInitialI.prototype.opRequiredException_async = function(cb, a, b, o, current)
    {
        var ex = new TestAMD.RequiredException();
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
        cb.ice_exception(ex);
    };

    AMDInitialI.prototype.opByte_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opByteReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opBool_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opBoolReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opShort_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opShortReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opInt_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opIntReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opLong_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opLongReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFloat_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFloatReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opDouble_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opDoubleReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opString_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opStringReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opMyEnum_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opMyEnumReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSmallStruct_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSmallStructReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFixedStruct_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFixedStructReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opVarStruct_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opVarStructReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opOneOptional_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opOneOptionalReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opOneOptionalProxy_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opOneOptionalProxyReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opByteSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opByteSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opBoolSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opBoolSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opShortSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opShortSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opIntSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opIntSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opLongSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opLongSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFloatSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFloatSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opDoubleSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opDoubleSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opStringSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opStringSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSmallStructSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSmallStructSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSmallStructList_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSmallStructListReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFixedStructSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFixedStructSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFixedStructList_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opFixedStructListReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opVarStructSeq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opVarStructSeqReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSerializable_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opSerializableReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opIntIntDict_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opIntIntDictReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opStringIntDict_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opStringIntDictReq_async = function(cb, p1, current)
    {
        cb.ice_response(p1, p1);
    };

    AMDInitialI.prototype.opClassAndUnknownOptional_async = function(cb, p, current)
    {
        cb.ice_response();
    };

    AMDInitialI.prototype.sendOptionalClass_async = function(cb, req, current)
    {
        cb.ice_response();
    };

    AMDInitialI.prototype.returnOptionalClass_async = function(cb, req, current)
    {
        cb.ice_response(new TestAMD.OneOptional(53));
    };

    AMDInitialI.prototype.supportsRequiredParams_async = function(cb, current)
    {
        cb.ice_response(false);
    };

    AMDInitialI.prototype.supportsJavaSerializable_async = function(cb, current)
    {
        cb.ice_response(false);
    };

    AMDInitialI.prototype.supportsCsharpSerializable_async = function(cb, current)
    {
        cb.ice_response(false);
    };

    global.AMDInitialI = AMDInitialI;
}(typeof (global) === "undefined" ? window : global));
