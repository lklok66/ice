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
        //
        // Exception utilities
        //

        require("Ice/LocalException");
        
        var Ice = global.Ice || {};
        
        //
        // Local aliases.
        //
        var UnexpectedObjectException = Ice.UnexpectedObjectException;
        var MemoryLimitException = Ice.MemoryLimitException;
        var ExUtil = {};
        
        ExUtil.toString = function(ex)
        {
            if(!ex.stack)
            {
                return ex.toString();
            }
            else
            {
                return ex.stack;
            }
        };

        ExUtil.throwUOE = function(expectedType, v)
        {
            var type = v.ice_id();
            throw new UnexpectedObjectException("expected element of type `" + expectedType + "' but received '" +
                                                type, type, expectedType);
        };

        ExUtil.throwMemoryLimitException = function(requested, maximum)
        {
            throw new MemoryLimitException("requested " + requested + " bytes, maximum allowed is " + maximum +
                                           " bytes (see Ice.MessageSizeMax)");
        };

        Ice.ExUtil = ExUtil;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ExUtil"));
