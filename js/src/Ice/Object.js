// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Ice.Object
//
// Using IceObject in this file to avoid collisions with the native Object.
//
var IceObject = function()
{
};

IceObject.prototype.__notImplemented = function()
{
    throw new Error("not implemented");
};

IceObject.prototype.ice_isA = function(s, current)
{
    return this.ice_ids().indexOf(s) >= 0;
};

IceObject.prototype.ice_ping = function(current)
{
};

IceObject.__ids = ["::Ice::Object"];

IceObject.prototype.ice_ids = function(current)
{
    return IceObject.__ids;
};

IceObject.ice_staticId = function()
{
    return IceObject.__ids[0];
};

IceObject.prototype.ice_id = function(current)
{
    return IceObject.__ids[0];
};

IceObject.prototype.ice_preMarshal = function()
{
};

IceObject.prototype.ice_postMarshal = function()
{
};

module.exports = IceObject;
