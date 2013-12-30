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
        var AssertionFailedException = function(message)
        {
            Error.call(this);
            Error.captureStackTrace(this, this.constructor);
            this.message = message;
        };

        AssertionFailedException.prototype = new Error();

        Object.defineProperty(AssertionFailedException.prototype, "name", {
            get: function() { return "AssertionFailedException"; }
        });

        AssertionFailedException.prototype.constructor = AssertionFailedException;

        var Debug = {};

        Debug.AssertionFailedException = AssertionFailedException;

        Debug.assert = function(b, msg)
        {
            if(!b)
            {
                throw new AssertionFailedException(msg === undefined ? "assertion failed" : msg);
            }
        };


        global.Ice = global.Ice || {};
        global.Ice.Debug = Debug;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Debug"));
