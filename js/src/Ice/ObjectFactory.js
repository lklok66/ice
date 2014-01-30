// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var global = this;
    var Ice = global.Ice || {};
    
    var ObjectFactory = function()
    {
    };

    ObjectFactory.prototype.create = function(type)
    {
        throw new Error("not implemented");
    };

    ObjectFactory.prototype.destroy = function()
    {
        throw new Error("not implemented");
    };

    Ice.ObjectFactory = ObjectFactory;
    global.Ice = Ice;
}());
