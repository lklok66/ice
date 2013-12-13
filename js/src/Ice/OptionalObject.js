// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var ExUtil = require("./ExUtil");

var OptionalObject = function(opt, cls, type)
{
    this._opt = opt;
    this._cls = cls;
    this._type = type;
};

Object.defineProperty(OptionalObject.prototype, "opt", {
    get: function() { return this._opt; }
});

Object.defineProperty(OptionalObject.prototype, "cls", {
    get: function() { return this._cls; }
});

Object.defineProperty(OptionalObject.prototype, "type", {
    get: function() { return this._type; }
});

OptionalObject.prototype.patch = function( v)
{
    if(v === null || v instanceof this._cls)
    {
        this.opt.set(v);
    }
    else
    {
        ExUtil.throwUOE(this._type, v);
    }
};

OptionalObject.prototype.invoke = function(obj)
{
    this.patch(obj);
};

module.exports = OptionalObject;
