// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Ex = require("./Exception");
var ExUtil = require("./ExUtil");

var LocalEx = require("./LocalException").Ice;

var LocalExceptionWrapper = function(ex, retry)
{
    retry = retry === undefined ? false : retry;

    if(ex instanceof Ex.LocalException)
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

    Object.defineProperty(this, "inner", {
        get: function() { return this._ex; }
    });

    //
    // If true, always repeat the request. Don't take retry settings
    // or "at-most-once" guarantees into account.
    //
    // If false, only repeat the request if the retry settings allow
    // to do so, and if "at-most-once" does not need to be guaranteed.
    //
    Object.defineProperty(this, "retry", {
        get: function() { return this._retry; }
    });
}

LocalExceptionWrapper.prototype = new Error();
LocalExceptionWrapper.prototype.constructor = LocalExceptionWrapper;

LocalExceptionWrapper.throwWrapper = function(ex)
{
    if(ex instanceof Ex.UserException)
    {
        throw new LocalExceptionWrapper(new LocalEx.UnknownUserException(ex.ice_name()), false);
    }
    else if(ex instanceof Ex.LocalException)
    {
        if(ex instanceof LocalEx.UnknownException ||
           ex instanceof LocalEx.ObjectNotExistException ||
           ex instanceof LocalEx.OperationNotExistException ||
           ex instanceof LocalEx.FacetNotExistException)
        {
            throw new LocalExceptionWrapper(ex, false);
        }
        var e = new LocalEx.UnknownLocalException(ex.ice_name(), ex);
        throw new LocalExceptionWrapper(e, false);
    }

    var ue = new LocalEx.UnknownException(ExUtil.toString(ex), ex);
    throw new LocalExceptionWrapper(ue, false);
}

module.exports = LocalExceptionWrapper;
