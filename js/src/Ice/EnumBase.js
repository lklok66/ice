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
    this._name = name;
    this._value = value;
};

Object.defineProperty(EnumBase.prototype, 'name', {
    enumerable: true,
    get: function() { return this._name; }
});

Object.defineProperty(EnumBase.prototype, 'value', {
    enumerable: true,
    get: function() { return this._value; }
});

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

    return this._value == rhs._value;
};

EnumBase.prototype.hashCode = function()
{
    return this._value;
};

EnumBase.prototype.toString = function()
{
    return this._name;
};

EnumBase.defineEnum = function(type, enumerators)
{
    var enums = [];
    var maxValue = 0;
    for(var e in enumerators)
    {
        var value = enumerators[e];
        var enumerator = new type(e, value);
        enums[value] = enumerator;
        Object.defineProperty(type, e, {
            enumerable: true,
            value: enumerator
        });
        if(value > maxValue)
        {
            maxValue = value;
        }
    }

    Object.defineProperty(type, 'valueOf', {
        value: function(v) { 
            if(v === undefined)
            {
                return type;
            }
            return enums[v]; }
    });
    
    Object.defineProperty(type, 'maxValue', {
        value: function() { return maxValue; }
    });
};

module.exports = EnumBase;
