// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

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

module.exports.Exception = Exception;

//
// Ice.LocalException
//
var LocalException = function(cause)
{
    Exception.call(this, cause);
};

LocalException.prototype = new Exception();
LocalException.prototype.constructor = LocalException;

LocalException.prototype.ice_name = function()
{
    return "Ice::LocalException";
};

module.exports.LocalException = LocalException;

//
// Ice.UserException
//
var UserException = function(cause)
{
    Exception.call(this, cause);
};

UserException.prototype = new Exception();
UserException.prototype.constructor = UserException;

UserException.prototype.ice_name = function()
{
    return "Ice::UserException";
};

module.exports.UserException = UserException;
