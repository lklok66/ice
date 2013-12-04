// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Property = function Property(pattern, deprecated, deprecatedBy)
{
    this._pattern = pattern;
    this._deprecated = deprecated;
    this._deprecatedBy = deprecatedBy;
}

Property.prototype.pattern = function()
{
    return this._pattern;
}

Property.prototype.deprecated = function()
{
    return this._deprecated;
}

Property.prototype.deprecatedBy = function()
{
    return this._deprecatedBy;
}


module.exports = Property;
