// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Optional = function()
{
    this._value = undefined;
};

Optional.prototype.set = function(value)
{
    this._value = value;
};

Optional.prototype.clear = function()
{
    this._value = undefined;
};

Object.defineProperty(Optional.prototype, "value", {
    get: function() { return this._value; }
});
