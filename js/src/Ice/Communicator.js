// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){

        require("Ice/Instance");
        require("Ice/Promise");
        require("Ice/UUID");

        var Ice = global.Ice || {};

        var Instance = Ice.Instance;
        var Promise = Ice.Promise;
        var UUID = Ice.UUID;

        //
        // Ice.Communicator
        //
        var Communicator = function(initData)
        {
            this._instance = new Instance(initData);
        };

        Object.defineProperty(Communicator.prototype, "instance", {
            get: function() { return this._instance; }
        });

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

        Communicator.prototype.createObjectAdapter = function(name)
        {
            return this._instance.objectAdapterFactory().createObjectAdapter(name, null);
        };

        Communicator.prototype.createObjectAdapterWithEndpoints = function(name, endpoints)
        {
            if(name.length === 0)
            {
                name = UUID.generateUUID();
            }

            this.getProperties().setProperty(name + ".Endpoints", endpoints);
            return this._instance.objectAdapterFactory().createObjectAdapter(name, null);
        };

        Communicator.prototype.createObjectAdapterWithRouter = function(name, router)
        {
            if(name.length === 0)
            {
                name = UUID.generateUUID();
            }

            //
            // We set the proxy properties here, although we still use the proxy supplied.
            //
            var properties = this.proxyToProperty(router, name + ".Router");
            for(var e = properties.entries; e !== null; e = e.next)
            {
                this.getProperties().setProperty(e.key, e.value);
            }

            return this._instance.objectAdapterFactory().createObjectAdapter(name, router);
        };

        Communicator.prototype.addObjectFactory = function(factory, id)
        {
            this._instance.servantFactoryManager().add(factory, id);
        };

        Communicator.prototype.findObjectFactory = function(id)
        {
            // TODO
            //return this._instance.servantFactoryManager().find(id);
        };

        Communicator.prototype.getImplicitContext = function()
        {
            return this._instance.getImplicitContext();
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
            return this._instance.referenceFactory().getDefaultRouter();
        };

        Communicator.prototype.setDefaultRouter = function(router)
        {
            this._instance.setDefaultRouter(router);
        };

        Communicator.prototype.getDefaultLocator = function()
        {
            return this._instance.referenceFactory().getDefaultLocator();
        };

        Communicator.prototype.setDefaultLocator = function(locator)
        {
            this._instance.setDefaultLocator(locator);
        };

        Communicator.prototype.flushBatchRequests = function()
        {
            // TODO
        };

        Ice.Communicator = Communicator;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Communicator"));
