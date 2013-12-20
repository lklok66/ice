// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


var EnumBase = require("./EnumBase").Ice.EnumBase;

var FormatType = function(_n, _v)
{
    EnumBase.call(this, _n, _v);
};

FormatType.prototype = new EnumBase();
FormatType.prototype.constructor = FormatType;

EnumBase.defineEnum(FormatType, {'DefaultFormat':0, 'CompactFormat':1, 'SlicedFormat':2});

module.exports.Ice = {};
module.exports.Ice.FormatType = FormatType;
