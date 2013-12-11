// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var HashMap = require("./HashMap"),
    LocalEx = require("./LocalException");

//
// Only for use by Instance
//
var ObjectFactoryManager = function()
{
    this._factoryMap = new HashMap(); // java.util.HashMap<String, Ice.ObjectFactory>()
};

ObjectFactoryManager.prototype.add = function(factory, id)
{
    var o, ex;
    o = this._factoryMap.get(id);
    if(o !== undefined)
    {
        ex = new LocalEx.AlreadyRegisteredException();
        ex.id = id;
        ex.kindOfObject = "object factory";
        throw ex;
    }
    this._factoryMap.put(id, factory);
};

ObjectFactoryManager.prototype.remove = function(id)
{
    var factory, ex;
    factory = this._factoryMap.get(id);
    if(factory === undefined)
    {
        ex = new LocalEx.NotRegisteredException();
        ex.id = id;
        ex.kindOfObject = "object factory";
        throw ex;
    }
    this._factoryMap.delete(id);
    factory.destroy();   
};

ObjectFactoryManager.prototype.find = function(id)
{
    return this._factoryMap.get(id);
};

ObjectFactoryManager.prototype.destroy = function()
{
    var oldMap = this._factoryMap,
        e = oldMap.entries;
    this._factoryMap = new HashMap(); // java.util.HashMap<String, Ice.ObjectFactory>();

    while(e !== null)
    {
        e.value.destroy();
        e = e.next;
    }
};
