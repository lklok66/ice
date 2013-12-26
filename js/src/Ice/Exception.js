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
        module.exports.Ice = module.exports.Ice || {};

        //
        // Ice.Exception
        //
        var Exception = function(cause)
        {
            this.ice_cause = cause;
        };

        Exception.prototype = new Error();
        Exception.prototype.constructor = Exception;

        Exception.prototype.ice_name = function()
        {
            return "Ice::Exception";
        };

        Exception.prototype.toString = function()
        {
            return this.ice_name();
        };

        module.exports.Ice.Exception = Exception;

        //
        // Ice.LocalException
        //
        var LocalException = function(cause)
        {
            Exception.call(this, cause);
            Error.captureStackTrace(this, LocalException);
        };

        LocalException.prototype = new Exception();
        LocalException.prototype.constructor = LocalException;

        LocalException.prototype.ice_name = function()
        {
            return "Ice::LocalException";
        };

        module.exports.Ice.LocalException = LocalException;

        //
        // Ice.UserException
        //
        var UserException = function(cause)
        {
            Exception.call(this, cause);
            Error.captureStackTrace(this, UserException);
        };

        UserException.prototype = new Exception();
        UserException.prototype.constructor = UserException;

        UserException.prototype.ice_name = function()
        {
            return "Ice::UserException";
        };

        UserException.prototype.__write = function(os)
        {
            os.startWriteException(null);
            this.__writeImpl(os);
            os.endWriteException();
        };

        UserException.prototype.__read = function(is)
        {
            is.startReadException();
            this.__readImpl(is);
            is.endReadException(false);
        };

        module.exports.Ice.UserException = UserException;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Exception"));
