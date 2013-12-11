// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var IceModule = function()
{
    this.exports = {};
    this.__filename = file;
}

IceModule.modules = {};

IceModule.require = function(path)
{
    var m = Module.modules[path];
    return m ? m.exports : {};
}

IceModule.define = function(file, fn)
{
    var m = new Module(file);
    Module.modules[file] = m;
    fn.call(null, m, m.exports);
}

//Global require
var require = IceModule.require;
