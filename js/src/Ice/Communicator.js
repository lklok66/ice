// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Instance = require("./Instance");
var Promise = require("./Promise");

//
// Ice.Communicator
//
var Communicator = function(initData)
{
    this._instance = new Instance(initData);
};

//
// Certain initialization tasks need to be completed after the
// constructor.
//
Communicator.prototype.finishSetup = function(promise)
{
    this._instance.finishSetup(this, promise);
};

Communicator.prototype.destroy = function()
{
    return this._instance.destroy();
};

Communicator.prototype.shutdown = function()
{
    // TODO
    //this._instance.objectAdapterFactory().shutdown();
};

Communicator.prototype.waitForShutdown = function()
{
    // TODO
    //return this._instance.objectAdapterFactory().waitForShutdown();
};

Communicator.prototype.isShutdown = function()
{
    // TODO
    //return this._instance.objectAdapterFactory().isShutdown();
};

Communicator.prototype.stringToProxy = function(s)
{
    return this._instance.proxyFactory().stringToProxy(s);
};

Communicator.prototype.proxyToString = function(proxy)
{
    return this._instance.proxyFactory().proxyToString(proxy);
};

Communicator.prototype.propertyToProxy = function(s)
{
    return this._instance.proxyFactory().propertyToProxy(s);
};

Communicator.prototype.proxyToProperty = function(proxy, prefix)
{
    return this._instance.proxyFactory().proxyToProperty(proxy, prefix);
};

Communicator.prototype.stringToIdentity = function(s)
{
    return this._instance.stringToIdentity(s);
};

Communicator.prototype.identityToString = function(ident)
{
    return this._instance.identityToString(ident);
};

Communicator.prototype.addObjectFactory = function(factory, id)
{
    // TODO
    //this._instance.servantFactoryManager().add(factory, id);
};

Communicator.prototype.findObjectFactory = function(id)
{
    // TODO
    //return this._instance.servantFactoryManager().find(id);
};

Communicator.prototype.getImplicitContext = function()
{
    // TODO
};

Communicator.prototype.getProperties = function()
{
    return this._instance.initializationData().properties;
};

Communicator.prototype.getLogger = function()
{
    return this._instance.initializationData().logger;
};

Communicator.prototype.getDefaultRouter = function()
{
    // TODO
    //return this._instance.referenceFactory().getDefaultRouter();
};

Communicator.prototype.setDefaultRouter = function(router)
{
    // TODO
    //this._instance.setDefaultRouter(router);
};

Communicator.prototype.getDefaultLocator = function()
{
    // TODO
    //return this._instance.referenceFactory().getDefaultLocator();
};

Communicator.prototype.setDefaultLocator = function(locator)
{
    // TODO
    //this._instance.setDefaultLocator(locator);
};

Communicator.prototype.flushBatchRequests = function()
{
    // TODO
};

module.exports = Communicator;
