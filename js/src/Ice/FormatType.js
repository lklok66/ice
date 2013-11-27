// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var __ice_EnumBase = require("./EnumBase");

var FormatType = function(_n, _v)
{
    __ice_EnumBase.call(this, _n, _v);
}

FormatType.prototype = new __ice_EnumBase();
FormatType.prototype.constructor = FormatType;

Object.defineProperty(FormatType, 'DefaultFormat', {
    enumerable: true,
    value: new FormatType("DefaultFormat", 0)
});

Object.defineProperty(FormatType, 'CompactFormat', {
    enumerable: true,
    value: new FormatType("CompactFormat", 1)
});

Object.defineProperty(FormatType, 'SlicedFormat', {
    enumerable: true,
    value: new FormatType("SlicedFormat", 2)
});

module.exports = FormatType;
