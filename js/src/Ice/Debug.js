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
        
        require("Ice/Exception");
        
        var Ice = global.Ice || {};
        
        var Exception = Ice.Exception;
        
        var AssertionFailedException = function(message)
        {
            Error.call(this);
            Exception.captureStackTrace(this);
            this.message = message;
        };

        AssertionFailedException.prototype = new Error();
        
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
        Ice.Debug = Debug;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Debug"));
