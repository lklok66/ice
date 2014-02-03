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
    require("Ice/Class");
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
    var Communicator = Ice.__defineClass({
        __init__: function(initData)
        {
            this._instance = new Instance(initData);
        },
        //
        // Certain initialization tasks need to be completed after the
        // constructor.
        //
        finishSetup: function(promise)
        {
            this._instance.finishSetup(this, promise);
        },
        destroy: function()
        {
            return this._instance.destroy();
        },
        shutdown: function()
        {
            // TODO
            //this._instance.objectAdapterFactory().shutdown();
        },
        waitForShutdown: function()
        {
            // TODO
            //return this._instance.objectAdapterFactory().waitForShutdown();
        },
        isShutdown: function()
        {
            // TODO
            //return this._instance.objectAdapterFactory().isShutdown();
        },
        stringToProxy: function(s)
        {
            return this._instance.proxyFactory().stringToProxy(s);
        },
        proxyToString: function(proxy)
        {
            return this._instance.proxyFactory().proxyToString(proxy);
        },
        propertyToProxy: function(s)
        {
            return this._instance.proxyFactory().propertyToProxy(s);
        },
        proxyToProperty: function(proxy, prefix)
        {
            return this._instance.proxyFactory().proxyToProperty(proxy, prefix);
        },
        stringToIdentity: function(s)
        {
            return this._instance.stringToIdentity(s);
        },
        identityToString: function(ident)
        {
            return this._instance.identityToString(ident);
        },
        createObjectAdapter: function(name)
        {
            return this._instance.objectAdapterFactory().createObjectAdapter(name, null);
        },
        createObjectAdapterWithEndpoints: function(name, endpoints)
        {
            if(name.length === 0)
            {
                name = UUID.generateUUID();
            }

            this.getProperties().setProperty(name + ".Endpoints", endpoints);
            return this._instance.objectAdapterFactory().createObjectAdapter(name, null);
        },
        createObjectAdapterWithRouter: function(name, router)
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
        },
        addObjectFactory: function(factory, id)
        {
            this._instance.servantFactoryManager().add(factory, id);
        },
        findObjectFactory: function(id)
        {
            // TODO
            //return this._instance.servantFactoryManager().find(id);
        },
        getImplicitContext: function()
        {
            return this._instance.getImplicitContext();
        },
        getProperties: function()
        {
            return this._instance.initializationData().properties;
        },
        getLogger: function()
        {
            return this._instance.initializationData().logger;
        },
        getDefaultRouter: function()
        {
            return this._instance.referenceFactory().getDefaultRouter();
        },
        setDefaultRouter: function(router)
        {
            this._instance.setDefaultRouter(router);
        },
        getDefaultLocator: function()
        {
            return this._instance.referenceFactory().getDefaultLocator();
        },
        setDefaultLocator: function(locator)
        {
            this._instance.setDefaultLocator(locator);
        },
        flushBatchRequests: function()
        {
            return this._instance.outgoingConnectionFactory().flushAsyncBatchRequests();
        }
    });
    
    Object.defineProperty(Communicator.prototype, "instance", {
        get: function() { return this._instance; }
    });

    Ice.Communicator = Communicator;
    global.Ice = Ice;
}());
