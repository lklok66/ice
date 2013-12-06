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

__ice_EnumBase.defineEnum(OptionalFormat, {'F1':0, 'F2':1, 'F4':2, 'F8':3, 'Size':4, 'VSize':5, 'FSize':6, 'Class':7});

module.exports = OptionalFormat;
