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

        require("Ice/AsyncResultBase");
        require("Ice/Debug");
        require("Ice/Identity");
        require("Ice/LocalException");
        require("Ice/Promise");
        require("Ice/PropertyNames");
        require("Ice/Router");
        require("Ice/ServantManager");
        require("Ice/StringUtil");
        require("Ice/UUID");

        var Ice = global.Ice || {};

        var AsyncResultBase = Ice.AsyncResultBase;
        var Debug = Ice.Debug;
        var Identity = Ice.Identity;
        var Promise = Ice.Promise;
        var PropertyNames = Ice.PropertyNames;
        var ServantManager = Ice.ServantManager;
        var StringUtil = Ice.StringUtil;
        var UUID = Ice.UUID;

        //
        // Only for use by IceInternal.ObjectAdapterFactory
        //
        var ObjectAdapterI = function(instance, communicator, objectAdapterFactory, name, router, noConfig)
        {
            this._deactivated = false;
            this._instance = instance;
            this._communicator = communicator;
            this._objectAdapterFactory = objectAdapterFactory;
            this._hasAcmTimeout = false;
            this._acmTimeout = 0;
            this._servantManager = new ServantManager(instance, name);
            this._activateOneOffDone = false;
            this._name = name;
            this._routerEndpoints = [];
            this._routerInfo = null;
            this._publishedEndpoints = [];
            this._destroying = false;
            this._destroyed = false;
            this._noConfig = noConfig;
            this._waitForDeactivatePromises = [];
            this._destroyPromises = [];
            this._id = "";
            this._replicaGroupId = "";

            if(this._noConfig)
            {
                this._reference = this._instance.referenceFactory().createFromString("dummy -t", "");
                return;
            }

            var properties = this._instance.initializationData().properties;
            var unknownProps = [];
            var noProps = this.filterProperties(unknownProps);

            //
            // Warn about unknown object adapter properties.
            //
            if(unknownProps.length !== 0 && properties.getPropertyAsIntWithDefault("Ice.Warn.UnknownProperties", 1) > 0)
            {
                var message = ["found unknown properties for object adapter `" + name + "':"];
                for(var i = 0; i < unknownProps.length; ++i)
                {
                    message.push("\n    " + p);
                }
                this._instance.initializationData().logger.warning(message.join(""));
            }

            //
            // Make sure named adapter has some configuration.
            //
            if(router === null && noProps)
            {
                this._deactivated = true;
                this._destroyed = true;
                this._instance = null;

                var ex = new Ice.InitializationException();
                ex.reason = "object adapter `" + this._name + "' requires configuration";
                throw ex;
            }

            //
            // Setup a reference to be used to get the default proxy options
            // when creating new proxies. By default, create twoway proxies.
            //
            var proxyOptions = properties.getPropertyWithDefault(this._name + ".ProxyOptions", "-t");
            try
            {
                this._reference = this._instance.referenceFactory().createFromString("dummy " + proxyOptions, "");
            }
            catch(e)
            {
                if(e instanceof Ice.ProxyParseException)
                {
                    var ex = new Ice.InitializationException();
                    ex.reason = "invalid proxy options `" + proxyOptions + "' for object adapter `" + this._name + "'";
                    throw ex;
                }
                else
                {
                    throw e;
                }
            }

            try
            {
                this._hasAcmTimeout = properties.getProperty(this._name + ".ACM").length > 0;
                if(this._hasAcmTimeout)
                {
                    this._acmTimeout = properties.getPropertyAsInt(this._name + ".ACM");
                    this._instance.connectionMonitor().checkIntervalForACM(this._acmTimeout);
                }

                if(router === null)
                {
                    router = Ice.RouterPrx.uncheckedCast(
                        this._instance.proxyFactory().propertyToProxy(this._name + ".Router"));
                }
                if(router !== null)
                {
                    this._routerInfo = this._instance.routerManager().get(router);
                    if(this._routerInfo !== null)
                    {
                        //
                        // Make sure this router is not already registered with another adapter.
                        //
                        if(this._routerInfo.getAdapter() !== null)
                        {
                            throw new Ice.AlreadyRegisteredException(
                                "object adapter with router",
                                this._instance.identityToString(router.ice_getIdentity()));
                        }

                        //
                        // Add the router's server proxy endpoints to this object
                        // adapter.
                        //
                        var endpoints = this._routerInfo.getServerEndpoints();
                        for(var i = 0; i < endpoints.length; ++i)
                        {
                            this._routerEndpoints.push(endpoints[i]);
                        }
                        this._routerEndpoints.sort(     // Must be sorted.
                            function(e1, e2)
                            {
                                return e1.compareTo(e2);
                            });

                        //
                        // Remove duplicate endpoints, so we have a list of unique
                        // endpoints.
                        //
                        for(var i = 0; i < this._routerEndpoints.length - 1;)
                        {
                            var e1 = this._routerEndpoints[i];
                            var e2 = this._routerEndpoints[i + 1];
                            if(e1.equals(e2))
                            {
                                this._routerEndpoints.splice(i, 1);
                            }
                            else
                            {
                                ++i;
                            }
                        }

                        //
                        // Associate this object adapter with the router. This way,
                        // new outgoing connections to the router's client proxy will
                        // use this object adapter for callbacks.
                        //
                        this._routerInfo.setAdapter(this);

                        //
                        // Also modify all existing outgoing connections to the
                        // router's client proxy to use this object adapter for
                        // callbacks.
                        //
                        this._instance.outgoingConnectionFactory().setRouterInfo(this._routerInfo);
                    }
                }
                else
                {
                    var endpoints = properties.getProperty(this._name + ".Endpoints");
                    if(endpoints.length > 0)
                    {
                        throw new Ice.FeatureNotSupportedException("object adapter endpoints not supported");
                    }
                }
            }
            catch(ex)
            {
                this.destroy();
                throw ex;
            }
        };

        ObjectAdapterI.prototype.getName = function()
        {
            //
            // No mutex lock necessary, _name is immutable.
            //
            return this._noConfig ? "" : this._name;
        };

        ObjectAdapterI.prototype.getCommunicator = function()
        {
            return this._communicator;
        };

        ObjectAdapterI.prototype.activate = function()
        {
            var promise = new AsyncResultBase(this._communicator, "activate", null, null, this);

            if(this.checkForDeactivation(promise))
            {
                return promise;
            }

            if(this._activateOneOffDone)
            {
                promise.succeed(promise);
                return promise;
            }

            var printAdapterReady = false;
            if(!this._noConfig)
            {
                var properties = this._instance.initializationData().properties;
                printAdapterReady = properties.getPropertyAsInt("Ice.PrintAdapterReady") > 0;
            }

            if(printAdapterReady)
            {
                console.log(this._name + " ready"); // TODO: Is console.log portable?
            }

            Debug.assert(!this._deactivated);

            this._activateOneOffDone = true;

            promise.succeed(promise);

            return promise;
        };

        ObjectAdapterI.prototype.hold = function()
        {
            this.checkForDeactivation();
        };

        ObjectAdapterI.prototype.waitForHold = function()
        {
            var promise = new AsyncResultBase(this._communicator, "waitForHold", null, null, this);

            if(this.checkForDeactivation(promise))
            {
                return promise;
            }

            promise.succeed();

            return promise;
        };

        ObjectAdapterI.prototype.deactivate = function()
        {
            var promise = new AsyncResultBase(this._communicator, "deactivate", null, null, this);

            if(this._deactivated)
            {
                promise.succeed();
                return promise;
            }

            this._deactivated = true;

            this._instance.outgoingConnectionFactory().removeAdapter(this);

            promise.succeed();

            this.checkWaitForDeactivate();

            return promise;
        };

        ObjectAdapterI.prototype.waitForDeactivate = function()
        {
            var promise = new AsyncResultBase(this._communicator, "waitForDeactivate", null, null, this);

            if(this._deactivated)
            {
                promise.succeed(promise);
                return promise;
            }

            this._waitForDeactivatePromises.push(promise);

            this.checkWaitForDeactivate();

            return promise;
        };

        ObjectAdapterI.prototype.checkWaitForDeactivate = function()
        {
            //
            // Wait for deactivation of the adapter itself.
            //
            if(!this._deactivated)
            {
                return;
            }

            this._waitForDeactivatePromises.forEach(
                function(p, i, arr)
                {
                    p.succeed(p);
                });
            this._waitForDeactivatePromises = [];
        };

        ObjectAdapterI.prototype.isDeactivated = function()
        {
            return this._deactivated;
        };

        ObjectAdapterI.prototype.destroy = function()
        {
            var promise = new AsyncResultBase(this._communicator, "destroy", null, null, this);

            if(this._destroying)
            {
                //
                // Destroy already in progress.
                //
                this._destroyPromises.push(promise);
                return promise;
            }

            //
            // Object adapter is already destroyed.
            //
            if(this._destroyed)
            {
                promise.succeed(promise);
                return promise;
            }

            this._destroying = true;

            this._destroyPromises.push(promise);

            //
            // Deactivate and wait for completion.
            //
            var self = this;
            this.deactivate().then(
                function(r)
                {
                    return self.waitForDeactivate();
                }
            ).then(
                function(r)
                {
                    //
                    // Now it's also time to clean up our servants and servant
                    // locators.
                    //
                    self._servantManager.destroy();

                    //
                    // Signal that destroying is complete.
                    //
                    self._destroying = false;
                    self._destroyed = true;

                    //
                    // Remove object references (some of them cyclic).
                    //
                    self._instance = null;
                    self._routerEndpoints = null;
                    self._routerInfo = null;
                    self._reference = null;

                    var objectAdapterFactory = self._objectAdapterFactory;
                    self._objectAdapterFactory = null;

                    if(objectAdapterFactory !== null)
                    {
                        objectAdapterFactory.removeObjectAdapter(self);
                    }

                    self._destroyPromises.forEach(
                        function(p, i, arr)
                        {
                            p.succeed(p);
                        });
                    self._destroyPromises = [];
                }
            ).exception(
                function(ex)
                {
                    self._destroyPromises.forEach(
                        function(p, i, arr)
                        {
                            p.fail(ex);
                        });
                    self._destroyPromises = [];
                });

            return promise;
        };

        ObjectAdapterI.prototype.add = function(object, ident)
        {
            return this.addFacet(object, ident, "");
        };

        ObjectAdapterI.prototype.addFacet = function(object, ident, facet)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            //
            // Create a copy of the Identity argument, in case the caller
            // reuses it.
            //
            var id = ident.clone();

            this._servantManager.addServant(object, id, facet);

            return this.newProxy(id, facet);
        };

        ObjectAdapterI.prototype.addWithUUID = function(object)
        {
            return this.addFacetWithUUID(object, "");
        };

        ObjectAdapterI.prototype.addFacetWithUUID = function(object, facet)
        {
            var ident = new Identity();
            ident.category = "";
            ident.name = UUID.generateUUID();

            return this.addFacet(object, ident, facet);
        };

        ObjectAdapterI.prototype.addDefaultServant = function(servant, category)
        {
            this.checkForDeactivation();

            this._servantManager.addDefaultServant(servant, category);
        };

        ObjectAdapterI.prototype.remove = function(ident)
        {
            return this.removeFacet(ident, "");
        };

        ObjectAdapterI.prototype.removeFacet = function(ident, facet)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            return this._servantManager.removeServant(ident, facet);
        };

        ObjectAdapterI.prototype.removeAllFacets = function(ident)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            return this._servantManager.removeAllFacets(ident);
        };

        ObjectAdapterI.prototype.removeDefaultServant = function(category)
        {
            this.checkForDeactivation();

            return this._servantManager.removeDefaultServant(category);
        };

        ObjectAdapterI.prototype.find = function(ident)
        {
            return this.findFacet(ident, "");
        };

        ObjectAdapterI.prototype.findFacet = function(ident, facet)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            return this._servantManager.findServant(ident, facet);
        };

        ObjectAdapterI.prototype.findAllFacets = function(ident)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            return this._servantManager.findAllFacets(ident);
        };

        ObjectAdapterI.prototype.findByProxy = function(proxy)
        {
            this.checkForDeactivation();

            var ref = proxy.__reference();
            return this.findFacet(ref.getIdentity(), ref.getFacet());
        };

        ObjectAdapterI.prototype.findDefaultServant = function(category)
        {
            this.checkForDeactivation();

            return this._servantManager.findDefaultServant(category);
        };

        ObjectAdapterI.prototype.addServantLocator = function(locator, prefix)
        {
            this.checkForDeactivation();

            this._servantManager.addServantLocator(locator, prefix);
        };

        ObjectAdapterI.prototype.removeServantLocator = function(prefix)
        {
            this.checkForDeactivation();

            return this._servantManager.removeServantLocator(prefix);
        };

        ObjectAdapterI.prototype.findServantLocator = function(prefix)
        {
            this.checkForDeactivation();

            return this._servantManager.findServantLocator(prefix);
        };

        ObjectAdapterI.prototype.createProxy = function(ident)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            return this.newProxy(ident, "");
        };

        ObjectAdapterI.prototype.createDirectProxy = function(ident)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            return this.newDirectProxy(ident, "");
        };

        ObjectAdapterI.prototype.createIndirectProxy = function(ident)
        {
            this.checkForDeactivation();
            this.checkIdentity(ident);

            return this.newIndirectProxy(ident, "", this._id);
        };

        ObjectAdapterI.prototype.setLocator = function(locator)
        {
            this.checkForDeactivation();

            throw new Ice.FeatureNotSupportedException("setLocator not supported");
        };

        ObjectAdapterI.prototype.refreshPublishedEndpoints = function()
        {
            throw new Ice.FeatureNotSupportedException("refreshPublishedEndpoints not supported");
        };

        ObjectAdapterI.prototype.getEndpoints = function()
        {
            return [];
        };

        ObjectAdapterI.prototype.getPublishedEndpoints = function()
        {
            return [];
        };

        ObjectAdapterI.prototype.isLocal = function(proxy)
        {
            //
            // NOTE: it's important that isLocal() doesn't perform any blocking operations as
            // it can be called for AMI invocations if the proxy has no delegate set yet.
            //

            var ref = proxy.__reference();
            if(ref.isWellKnown())
            {
                //
                // Check the active servant map to see if the well-known
                // proxy is for a local object.
                //
                return this._servantManager.hasServant(ref.getIdentity());
            }
            else if(ref.isIndirect())
            {
                //
                // Proxy is local if the reference adapter id matches this
                // adapter id or replica group id.
                //
                //return ref.getAdapterId() === this._id || ref.getAdapterId() === this._replicaGroupId;
                return false;
            }
            else
            {
                var endpoints = ref.getEndpoints();

                this.checkForDeactivation();

                //
                // Proxies which have at least one endpoint in common with the
                // router's server proxy endpoints (if any), are also considered
                // local.
                //
                if(this._routerInfo !== null && this._routerInfo.getRouter().equals(proxy.ice_getRouter()))
                {
                    for(var i = 0; i < endpoints.length; ++i)
                    {
                        for(var j = 0; j < this._routerEndpoints.length; ++j)
                        {
                            if(endpoints[i].equivalent(this._routerEndpoints[j]))
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        };

        ObjectAdapterI.prototype.getServantManager = function()
        {
            //
            // _servantManager is immutable.
            //
            return this._servantManager;
        };

        ObjectAdapterI.prototype.getACM = function()
        {
            // No check for deactivation here!

            Debug.assert(this._instance !== null); // Must not be called after destroy().

            if(this._hasAcmTimeout)
            {
                return this._acmTimeout;
            }
            else
            {
                return this._instance.serverACM();
            }
        };

        ObjectAdapterI.prototype.newProxy = function(ident, facet)
        {
            if(this._id.length === 0)
            {
                return this.newDirectProxy(ident, facet);
            }
            else if(this._replicaGroupId.length === 0)
            {
                return this.newIndirectProxy(ident, facet, this._id);
            }
            else
            {
                return this.newIndirectProxy(ident, facet, this._replicaGroupId);
            }
        };

        ObjectAdapterI.prototype.newDirectProxy = function(ident, facet)
        {
            var endpoints = [];

            //
            // Now we also add the endpoints of the router's server proxy, if
            // any. This way, object references created by this object adapter
            // will also point to the router's server proxy endpoints.
            //
            for(var i = 0; i < this._routerEndpoints.length; ++i)
            {
                endpoints.push(this._routerEndpoints[i]);
            }

            //
            // Create a reference and return a proxy for this reference.
            //
            var ref = this._instance.referenceFactory().create(ident, facet, this._reference, endpoints);
            return this._instance.proxyFactory().referenceToProxy(ref);
        };

        ObjectAdapterI.prototype.newIndirectProxy = function(ident, facet, id)
        {
            //
            // Create a reference with the adapter id and return a proxy
            // for the reference.
            //
            var ref = this._instance.referenceFactory().create(ident, facet, this._reference, id);
            return this._instance.proxyFactory().referenceToProxy(ref);
        };

        ObjectAdapterI.prototype.checkForDeactivation = function(promise)
        {
            if(this._deactivated)
            {
                var ex = new Ice.ObjectAdapterDeactivatedException();
                ex.name = this.getName();

                if(promise !== undefined)
                {
                    promise.fail(ex);
                    return true;
                }
                else
                {
                    throw ex;
                }
            }

            return false;
        };

        ObjectAdapterI.prototype.checkIdentity = function(ident)
        {
            if(ident.name === undefined || ident.name === null || ident.name.length === 0)
            {
                var e = new Ice.IllegalIdentityException();
                e.id = ident.clone();
                throw e;
            }

            if(ident.category === undefined || ident.category === null)
            {
                ident.category = "";
            }
        };

        var _suffixes =
        [
            "ACM",
            "AdapterId",
            "Endpoints",
            "Locator",
            "Locator.EncodingVersion",
            "Locator.EndpointSelection",
            "Locator.ConnectionCached",
            "Locator.PreferSecure",
            "Locator.CollocationOptimized",
            "Locator.Router",
            "PublishedEndpoints",
            "RegisterProcess",
            "ReplicaGroupId",
            "Router",
            "Router.EncodingVersion",
            "Router.EndpointSelection",
            "Router.ConnectionCached",
            "Router.PreferSecure",
            "Router.CollocationOptimized",
            "Router.Locator",
            "Router.Locator.EndpointSelection",
            "Router.Locator.ConnectionCached",
            "Router.Locator.PreferSecure",
            "Router.Locator.CollocationOptimized",
            "Router.Locator.LocatorCacheTimeout",
            "Router.LocatorCacheTimeout",
            "ProxyOptions",
            "ThreadPool.Size",
            "ThreadPool.SizeMax",
            "ThreadPool.SizeWarn",
            "ThreadPool.StackSize",
            "ThreadPool.Serialize"
        ];

        ObjectAdapterI.prototype.filterProperties = function(unknownProps)
        {
            //
            // Do not create unknown properties list if Ice prefix, i.e., Ice, Glacier2, etc.
            //
            var addUnknown = true;
            var prefix = this._name + ".";
            for(var i = 0; i < PropertyNames.clPropNames[i].length; ++i)
            {
                if(prefix.indexOf(PropertyNames.clPropNames[i] + ".") === 0)
                {
                    addUnknown = false;
                    break;
                }
            }

            var noProps = true;
            var props = this._instance.initializationData().properties.getPropertiesForPrefix(prefix);
            for(var e = props.entries; e !== null; e = e.next)
            {
                var valid = false;
                for(var i = 0; i < _suffixes.length; ++i)
                {
                    if(e.key === prefix + _suffixes[i])
                    {
                        noProps = false;
                        valid = true;
                        break;
                    }
                }

                if(!valid && addUnknown)
                {
                    unknownProps.push(prop);
                }
            }

            return noProps;
        };

        Ice.ObjectAdapterI = ObjectAdapterI;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ObjectAdapterI"));
