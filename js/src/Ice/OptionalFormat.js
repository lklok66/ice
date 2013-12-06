// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var __ice_EnumBase = require("./EnumBase");

var OptionalFormat = function(_n, _v)
{
    __ice_EnumBase.call(this, _n, _v);
}

OptionalFormat.prototype = new __ice_EnumBase();
OptionalFormat.prototype.constructor = OptionalFormat;

Object.defineProperty(OptionalFormat, 'F1', {
    enumerable: true,
    value: new OptionalFormat("F1", 0)
});

Object.defineProperty(OptionalFormat, 'F2', {
    enumerable: true,
    value: new OptionalFormat("F2", 1)
});

Object.defineProperty(OptionalFormat, 'F4', {
    enumerable: true,
    value: new OptionalFormat("F4", 2)
});

Object.defineProperty(OptionalFormat, 'F8', {
    enumerable: true,
    value: new OptionalFormat("F8", 3)
});

Object.defineProperty(OptionalFormat, 'Size', {
    enumerable: true,
    value: new OptionalFormat("Size", 4)
});

Object.defineProperty(OptionalFormat, 'VSize', {
    enumerable: true,
    value: new OptionalFormat("VSize", 5)
});

Object.defineProperty(OptionalFormat, 'FSize', {
    enumerable: true,
    value: new OptionalFormat("FSize", 6)
});

Object.defineProperty(OptionalFormat, 'Class', {
    enumerable: true,
    value: new OptionalFormat("Class", 7)
});

module.exports = OptionalFormat;
