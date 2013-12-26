// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(module, exports, require){
        //
        // Exception utilities
        //

        var UnexpectedObjectException = require("Ice/LocalException").Ice.UnexpectedObjectException;
        var MemoryLimitException = require("Ice/LocalException").Ice.MemoryLimitException;
        var ExUtil = {};

        ExUtil.toString = function(ex)
        {
            // TODO: Best way to stringify exception?
            return ex.toString();
        };

        ExUtil.throwUOE = function(expectedType, actualType)
        {
            throw new UnexpectedObjectException("expected element of type `" + expectedType + "' but received '" +
                                                actualType, actualType, expectedType);
        };

        ExUtil.throwMemoryLimitException = function(requested, maximum)
        {
            throw new MemoryLimitException("requested " + requested + " bytes, maximum allowed is " + maximum +
                                           " bytes (see Ice.MessageSizeMax)");
        };

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.ExUtil = ExUtil;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ExUtil"));
