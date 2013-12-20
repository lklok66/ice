// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var HashMap = require("./HashMap").Ice.HashMap;

function TypeRegistry()
{
    var __TypeRegistry__ = new HashMap();
    
    var register = function(typeId, type)
    {
        __TypeRegistry__.set(typeId, type);
    };
    
    var find = function(typeId)
    {
        return __TypeRegistry__.get(typeId);
    };
    
    return {register: register, find: find};
}

module.exports.Ice = {};
module.exports.Ice.ClassRegistry = TypeRegistry();
module.exports.Ice.ExceptionRegistry = TypeRegistry();
