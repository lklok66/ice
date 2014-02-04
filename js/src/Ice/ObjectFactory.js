// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    var Ice = global.Ice || {};
    
    require("Ice/Class");
    
    var ObjectFactory = Ice.__defineClass({
        create: function(type)
        {
            throw new Error("not implemented");
        },
        destroy: function()
        {
            throw new Error("not implemented");
        }
    });
    
    Ice.ObjectFactory = ObjectFactory;
    global.Ice = Ice;
}(typeof (global) === "undefined" ? window : global));
