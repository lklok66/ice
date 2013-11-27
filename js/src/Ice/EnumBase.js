// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Ice.EnumBase
//
var EnumBase = function(name, value)
{
    Object.defineProperty(this, 'name', {
        enumerable: true,
        configurable: false,
        writable: false,
        value: name
    });
    Object.defineProperty(this, 'value', {
        enumerable: true,
        configurable: false,
        writable: false,
        value: value
    });
}

EnumBase.prototype.equals = function(rhs)
{
    if(this === rhs)
    {
        return true;
    }

    var proto = Object.getPrototypeOf(this);
    if(!(rhs instanceof proto.constructor))
    {
        return false;
    }

    return this.value == rhs.value;
}

EnumBase.prototype.hashCode = function()
{
    return this.value;
}

EnumBase.prototype.toString = function()
{
    return this.name;
}

module.exports = EnumBase;
