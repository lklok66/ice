// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var AssertionFailedException = function(message)
{
    Error.call(this);
    Error.captureStackTrace(this, this.constructor);
    this.message = message;
}

AssertionFailedException.prototype = new Error();

Object.defineProperty(AssertionFailedException.prototype, "name", {
    get: function() { return "AssertionFailedException"; }
});

AssertionFailedException.prototype.constructor = AssertionFailedException;

module.exports.AssertionFailedException = AssertionFailedException;

module.exports.assert = function(b, msg)
{
    if(!b)
    {
        throw new AssertionFailedException(msg === undefined ? "assertion failed" : msg);
    }
}
