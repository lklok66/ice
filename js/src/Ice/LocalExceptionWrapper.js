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
        require("Ice/ExUtil");
        require("Ice/Debug");
        require("Ice/LocalException");
        
        var Ice = global.Ice || {};
        
        var ExUtil = Ice.ExUtil;
        var Debug = Ice.Debug;

        var LocalExceptionWrapper = function(ex, retry)
        {
            retry = retry === undefined ? false : retry;

            if(ex instanceof Ice.LocalException)
            {
                this._ex = ex;
                this._retry = retry;
            }
            else
            {
                Debug.assert(ex instanceof LocalExceptionWrapper);
                this._ex = ex._ex;
                this._retry = ex._retry;
            }
        };

        LocalExceptionWrapper.prototype = new Error();
        LocalExceptionWrapper.prototype.constructor = LocalExceptionWrapper;

        Object.defineProperty(LocalExceptionWrapper.prototype, "inner", {
            get: function() { return this._ex; }
        });

        //
        // If true, always repeat the request. Don't take retry settings
        // or "at-most-once" guarantees into account.
        //
        // If false, only repeat the request if the retry settings allow
        // to do so, and if "at-most-once" does not need to be guaranteed.
        //
        Object.defineProperty(LocalExceptionWrapper.prototype, "retry", {
            get: function() { return this._retry; }
        });

        LocalExceptionWrapper.throwWrapper = function(ex)
        {
            if(ex instanceof Ice.UserException)
            {
                throw new LocalExceptionWrapper(new Ice.UnknownUserException(ex.ice_name()), false);
            }
            else if(ex instanceof Ice.LocalException)
            {
                if(ex instanceof Ice.UnknownException ||
                ex instanceof Ice.ObjectNotExistException ||
                ex instanceof Ice.OperationNotExistException ||
                ex instanceof Ice.FacetNotExistException)
                {
                    throw new LocalExceptionWrapper(ex, false);
                }
                var e = new Ice.UnknownLocalException(ex.ice_name(), ex);
                throw new LocalExceptionWrapper(e, false);
            }

            var ue = new Ice.UnknownException(ExUtil.toString(ex), ex);
            throw new LocalExceptionWrapper(ue, false);
        };

        Ice.LocalExceptionWrapper = LocalExceptionWrapper;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/LocalExceptionWrapper"));