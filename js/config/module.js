// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var global = this;

global.Ice = {};
global.Ice.__modules = {};
global.Ice.__require = function(name)
{
    var m =  global.Ice.__modules[name];
    return m !== undefined ? m.exports : {};
};

global.Ice.__defineModule = function(m, name)
{
    var module = {};
    module.exports = global;
    module.require = global.Ice.__require;
    global.Ice.__modules[name] = module;
    m.call(m, global, module, module.exports, module.require);
};
