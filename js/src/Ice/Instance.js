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
        require("Ice/ConnectionMonitor");
        require("Ice/Debug");
        require("Ice/DefaultsAndOverrides");
        require("Ice/EndpointFactoryManager");
        require("Ice/HashMap");
        require("Ice/ImplicitContextI");
        require("Ice/IdentityUtil");
        require("Ice/LocatorManager");
        require("Ice/Logger");
        require("Ice/Network");
        require("Ice/ObjectFactoryManager");
        require("Ice/OutgoingConnectionFactory");
        require("Ice/Promise");
        require("Ice/Properties");
        require("Ice/ProxyFactory");
        require("Ice/RouterManager");
        require("Ice/TcpEndpointFactory");
        require("IceWS/EndpointFactory");
        require("Ice/Timer");
        require("Ice/TraceLevels");
        require("Ice/Reference");
        require("Ice/LocalException");
        require("Ice/Exception");
        require("Ice/ProcessLogger");
        
        var Ice = global.Ice || {};
        
        var AsyncResultBase = Ice.AsyncResultBase;
        var ConnectionMonitor = Ice.ConnectionMonitor;
        var Debug = Ice.Debug;
        var DefaultsAndOverrides = Ice.DefaultsAndOverrides;
        var EndpointFactoryManager = Ice.EndpointFactoryManager;
        var HashMap = Ice.HashMap;
        var ImplicitContextI = Ice.ImplicitContextI;
        var LocatorManager = Ice.LocatorManager;
        var Logger = Ice.Logger;
        var Network = Ice.Network;
        var ObjectFactoryManager = Ice.ObjectFactoryManager;
        var OutgoingConnectionFactory = Ice.OutgoingConnectionFactory;
        var Promise = Ice.Promise;
        var Properties = Ice.Properties;
        var ProxyFactory = Ice.ProxyFactory;
        var RouterManager = Ice.RouterManager;
        var Timer = Ice.Timer;
        var TraceLevels = Ice.TraceLevels;
        var ReferenceFactory = Ice.ReferenceFactory;

        var StateActive = 0;
        var StateDestroyInProgress = 1;
        var StateDestroyed = 2;

        //
        // Instance - only for use by Communicator
        //
        var Instance = function(initData)
        {
            this._state = StateActive;
            this._initData = initData;

            this._traceLevels = null;
            this._defaultsAndOverrides = null;
            this._messageSizeMax = null;
            this._clientACM = null;
            this._serverACM = null;
            this._implicitContext = null;
            this._routerManager = null;
            this._locatorManager = null;
            this._referenceFactory = null;
            this._proxyFactory = null;
            this._outgoingConnectionFactory = null;
            this._connectionMonitor = null;
            this._servantFactoryManager = null;
            this._objectAdapterFactory = null;
            this._protocolSupport = null;
            this._retryQueue = null;
            this._endpointHostResolver = null;
            this._endpointFactoryManager = null;
            this._exceptionFactoryMap = null;

            this._adminAdapter = null;
            this._adminFacets = new HashMap();
            this._adminFacetFilter = [];
            this._adminIdentity = null;
        };

        Instance.prototype.initializationData = function()
        {
            //
            // No check for destruction. It must be possible to access the
            // initialization data after destruction.
            //
            // This value is immutable.
            //
            return this._initData;
        };

        Instance.prototype.traceLevels = function()
        {
            // This value is immutable.
            Debug.assert(this._traceLevels !== null);
            return this._traceLevels;
        };

        Instance.prototype.defaultsAndOverrides = function()
        {
            // This value is immutable.
            Debug.assert(this._defaultsAndOverrides !== null);
            return this._defaultsAndOverrides;
        };

        Instance.prototype.routerManager = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._routerManager !== null);
            return this._routerManager;
        };

        Instance.prototype.locatorManager = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._locatorManager !== null);
            return this._locatorManager;
        };

        Instance.prototype.referenceFactory = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._referenceFactory !== null);
            return this._referenceFactory;
        };

        Instance.prototype.proxyFactory = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._proxyFactory !== null);
            return this._proxyFactory;
        };

        Instance.prototype.outgoingConnectionFactory = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._outgoingConnectionFactory !== null);
            return this._outgoingConnectionFactory;
        };

        Instance.prototype.preferIPv6 = function()
        {
            return this._preferIPv6;
        };

        Instance.prototype.networkProxy = function()
        {
            return this._networkProxy;
        };

        Instance.prototype.connectionMonitor = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._connectionMonitor !== null);
            return this._connectionMonitor;
        };

        Instance.prototype.servantFactoryManager = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._servantFactoryManager !== null);
            return this._servantFactoryManager;
        };

        /* TODO
        Instance.prototype.objectAdapterFactory = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._objectAdapterFactory !== null);
            return this._objectAdapterFactory;
        }*/

        Instance.prototype.protocolSupport = function()
        {
            if(this._state == StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            return this._protocolSupport;
        };

        /*
        Instance.prototype.endpointHostResolver = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._endpointHostResolver !== null);
            return this._endpointHostResolver;
        }

        Instance.prototype.retryQueue = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._retryQueue !== null);
            return this._retryQueue;
        }
        */

        Instance.prototype.timer = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._timer !== null);
            return this._timer;
        };

        Instance.prototype.endpointFactoryManager = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._endpointFactoryManager !== null);
            return this._endpointFactoryManager;
        };

        /* TODO
        Instance.prototype.pluginManager = function()
        {
            if(this._state === StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            Debug.assert(this._pluginManager !== null);
            return this._pluginManager;
        }
        */

        Instance.prototype.messageSizeMax = function()
        {
            // This value is immutable.
            return this._messageSizeMax;
        };

        Instance.prototype.clientACM = function()
        {
            // This value is immutable.
            return this._clientACM;
        };

        Instance.prototype.serverACM = function()
        {
            // This value is immutable.
            return this._serverACM;
        };

        Instance.prototype.getImplicitContext = function()
        {
            return this._implicitContext;
        };

        Instance.prototype.stringToIdentity = function(s)
        {
            return Ice.stringToIdentity(s);
        };

        Instance.prototype.identityToString = function(ident)
        {
            return Ice.identityToString(ident);
        };

        Instance.prototype.setDefaultLocator = function(locator)
        {
            if(this._state == StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            this._referenceFactory = this._referenceFactory.setDefaultLocator(locator);
        };

        Instance.prototype.setDefaultRouter = function(router)
        {
            if(this._state == StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            this._referenceFactory = this._referenceFactory.setDefaultRouter(router);
        };

        Instance.prototype.setLogger = function(logger)
        {
            this._initData.logger = logger;
        };

        Instance.prototype.finishSetup = function(communicator, promise)
        {
            //
            // If promise == null, it means the caller is requesting a synchronous setup.
            // Otherwise, we resolve the promise after all initialization is complete.
            //

            try
            {
                this._timer = new Timer();

                if(this._initData.properties === null)
                {
                    this._initData.properties = Properties.createProperties();
                }

                if(this._initData.logger === null)
                {
                    this._initData.logger = Ice.getProcessLogger();
                }

                this._traceLevels = new TraceLevels(this._initData.properties);
                
                this._defaultsAndOverrides = new DefaultsAndOverrides(this._initData.properties);

                var defMessageSizeMax = 1024;
                var num = this._initData.properties.getPropertyAsIntWithDefault("Ice.MessageSizeMax", defMessageSizeMax);
                if(num < 1)
                {
                    this._messageSizeMax = defMessageSizeMax * 1024; // Ignore non-sensical values.
                }
                else if(num > 0x7fffffff / 1024)
                {
                    this._messageSizeMax = 0x7fffffff;
                }
                else
                {
                    this._messageSizeMax = num * 1024; // Property is in kilobytes, _messageSizeMax in bytes
                }

                //
                // Client ACM enabled by default. Server ACM disabled by default.
                //
                this._clientACM = this._initData.properties.getPropertyAsIntWithDefault("Ice.ACM.Client", 60);
                this._serverACM = this._initData.properties.getPropertyAsInt("Ice.ACM.Server");

                this._implicitContext = ImplicitContextI.create(this._initData.properties.getProperty("Ice.ImplicitContext"));

                this._routerManager = new RouterManager();

                this._locatorManager = new LocatorManager(this._initData.properties);

                this._referenceFactory = new ReferenceFactory(this, communicator);

                this._proxyFactory = new ProxyFactory(this);

                // TODO: Network proxy
                this._networkProxy = null;

                var ipv4 = this._initData.properties.getPropertyAsIntWithDefault("Ice.IPv4", 1) > 0;
                var ipv6 = this._initData.properties.getPropertyAsIntWithDefault("Ice.IPv6", 0) > 0;
                if(!ipv4 && !ipv6)
                {
                    throw new Ice.InitializationException("Both IPV4 and IPv6 support cannot be disabled");
                }
                else if(ipv4 && ipv6)
                {
                    this._protocolSupport = Network.EnableBoth;
                }
                else if(ipv4)
                {
                    this._protocolSupport = Network.EnableIPv4;
                }
                else
                {
                    this._protocolSupport = Network.EnableIPv6;
                }
                this._preferIPv6 = this._initData.properties.getPropertyAsInt("Ice.PreferIPv6Address") > 0;
                
                this._endpointFactoryManager = new EndpointFactoryManager(this);
                this._endpointFactoryManager.add(new Ice.TcpEndpointFactory(this));
                this._endpointFactoryManager.add(new IceWS.EndpointFactory(this, false));
                this._endpointFactoryManager.add(new IceWS.EndpointFactory(this, true));

                /* TODO
                //
                // DatagramSocket is supported in AIR 2 or later.
                //
                if(Capabilities.AIR)
                {
                    //
                    // We still need to make sure the current run time supports it.
                    //
                    if(UdpEndpointFactory.isSupported)
                    {
                        var udpEndpointFactory:EndpointFactory = new UdpEndpointFactory(this);
                        _endpointFactoryManager.add(udpEndpointFactory);
                    }
                }

                //
                // Outgoing SSL is supported in AIR 2 or later, and Flash Player 11 or later.
                //
                if(Capabilities.AIR || Capabilities.version >= 11)
                {
                    //
                    // We still need to make sure the current run time supports it.
                    //
                    if(SslEndpointFactory.isSupported)
                    {
                        var sslEndpointFactory:EndpointFactory = new SslEndpointFactory(this);
                        _endpointFactoryManager.add(sslEndpointFactory);
                    }
                }
                */

                this._outgoingConnectionFactory = new OutgoingConnectionFactory(communicator, this);
                this._servantFactoryManager = new ObjectFactoryManager();

                /* TODO
                this._objectAdapterFactory = new ObjectAdapterFactory(this, communicator);

                this._endpointHostResolver = new EndpointHostResolver(this);

                this._retryQueue = new RetryQueue(this);

                this._exceptionFactoryMap = new HashMap();
                */

                /* TODO
                //
                // Add Process and Properties facets
                //
                var facetFilter = this._initData.properties.getPropertyAsList("Ice.Admin.Facets");
                if(facetFilter.length > 0)
                {
                    this._adminFacetFilter.concat(facetFilter);
                }

                this._adminFacets.put("Properties", new PropertiesAdminI(_initData.properties));
                this._adminFacets.put("Process", new ProcessI(communicator));*/

                //
                // Get default router and locator proxies. Don't move this
                // initialization before the plug-in initialization!!! The proxies
                // might depend on endpoint factories to be installed by plug-ins.
                //
                var router = Ice.RouterPrx.uncheckedCast(this._proxyFactory.propertyToProxy("Ice.Default.Router"));
                if(router !== null)
                {
                    this._referenceFactory = this._referenceFactory.setDefaultRouter(router);
                }

                var loc = Ice.LocatorPrx.uncheckedCast(this._proxyFactory.propertyToProxy("Ice.Default.Locator"));
                if(loc !== null)
                {
                    this._referenceFactory = this._referenceFactory.setDefaultLocator(loc);
                }

                //
                // Create the connection monitor and ensure the interval for
                // monitoring connections is appropriate for client & server
                // ACM.
                //
                var interval = this._initData.properties.getPropertyAsInt("Ice.MonitorConnections");
                this._connectionMonitor = new ConnectionMonitor(this, interval);
                this._connectionMonitor.checkIntervalForACM(this._clientACM);
                this._connectionMonitor.checkIntervalForACM(this._serverACM);

                /*
                //
                // This must be done last as this call creates the Ice.Admin object adapter
                // and eventually registers a process proxy with the Ice locator (allowing
                // remote clients to invoke on Ice.Admin facets as soon as it's registered).
                //
                if(this._initData.properties.getPropertyAsIntWithDefault("Ice.Admin.DelayCreation", 0) <= 0)
                {
                    if(this.checkAdmin(promise !== null))
                    {
                        //
                        // If the user calls initializeWithAdmin and the Admin OA is properly
                        // configured, we call getAdmin now and delay completion of initializeWithAdmin
                        // until getAdmin completes.
                        //
                        this.getAdmin(communicator).then(
                            function(admin)
                            {
                                promise.succeed(communicator);
                            }).exception(
                                function(ex)
                                {
                                    this.destroy().then(
                                        function()
                                        {
                                            promise.fail(ex);
                                        }).exception(
                                            function(e)
                                            {
                                                promise.fail(ex);
                                            });
                                });
                        return;
                    }
                }
                */

                if(promise !== null)
                {
                    promise.succeed(communicator);
                }
            }
            catch(ex)
            {
                if(promise !== null)
                {
                    if(ex instanceof Ice.LocalException)
                    {
                        this.destroy().then(
                            function()
                            {
                                promise.fail(ex);
                            }).exception(
                                function(e)
                                {
                                    promise.fail(ex);
                                });
                    }
                    else
                    {
                        promise.fail(ex);
                    }
                }
                else
                {
                    if(ex instanceof Ice.LocalException)
                    {
                        this.destroy();
                    }
                    throw ex;
                }
            }
        };

        //
        // Only for use by Ice.CommunicatorI
        //
        Instance.prototype.destroy = function()
        {
            var promise = new AsyncResultBase(null, "destroy", null, this, null);

            //
            // If the _state is not StateActive then the instance is
            // either being destroyed, or has already been destroyed.
            //
            if(this._state != StateActive)
            {
                promise.succeed(promise);
                return promise;
            }

            try
            {
                //
                // We cannot set state to StateDestroyed otherwise instance
                // methods called during the destroy process (such as
                // outgoingConnectionFactory() from
                // ObjectAdapterI::deactivate() will cause an exception.
                //
                this._state = StateDestroyInProgress;

                /* TODO
                if(this._objectAdapterFactory)
                {
                    _objectAdapterFactory.shutdown().whenCompleted(
                        function(r:Ice.AsyncResult):void
                        {
                            objectAdapterFactoryShutdown(ar);
                        },
                        function(r:Ice.AsyncResult, ex:Ice.Exception):void
                        {
                            ar.__exception(ex);
                        });
                }
                else
                {
                    objectAdapterFactoryShutdown(ar);
                }
                */

                if(this._outgoingConnectionFactory !== null)
                {
                    this._outgoingConnectionFactory.destroy();
                }

                if(this._objectAdapterFactory !== null)
                {
                    this._objectAdapterFactory.destroy();
                }

                if(this._outgoingConnectionFactory !== null)
                {
                    var self = this;
                    this._outgoingConnectionFactory.waitUntilFinished().then(
                        function()
                        {
                            self.outgoingConnectionFactoryFinished(promise);
                        }).exception(
                            function(ex)
                            {
                                promise.fail(ex);
                            });
                }
                else
                {
                    this.outgoingConnectionFactoryFinished(promise);
                }
            }
            catch(ex)
            {
                promise.fail(ex);
            }

            return promise;
        };

        Instance.prototype.outgoingConnectionFactoryFinished = function(promise)
        {
            /* TODO
            if(this._retryQueue)
            {
                this._retryQueue.destroy();
            }

            this._endpointHostResolver = null;
            this._objectAdapterFactory = null;
            this._outgoingConnectionFactory = null;
            this._retryQueue = null;
            */

            if(this._connectionMonitor)
            {
                this._connectionMonitor.destroy();
                this._connectionMonitor = null;
            }

            if(this._timer)
            {
                this._timer.destroy();
                this._timer = null;
            }

            if(this._servantFactoryManager)
            {
                this._servantFactoryManager.destroy();
                this._servantFactoryManager = null;
            }

            if(this._referenceFactory)
            {
                //this._referenceFactory.destroy(); // No destroy function defined.
                this._referenceFactory = null;
            }

            // this._proxyFactory.destroy(); // No destroy function defined.
            this._proxyFactory = null;

            if(this._routerManager)
            {
                this._routerManager.destroy();
                this._routerManager = null;
            }

            if(this._locatorManager)
            {
                this._locatorManager.destroy();
                this._locatorManager = null;
            }

            if(this._endpointFactoryManager)
            {
                this._endpointFactoryManager.destroy();
                this._endpointFactoryManager = null;
            }

            /*
            if(this._exceptionFactoryMap)
            {
                this._exceptionFactoryMap.clear();
                this._exceptionFactoryMap = null;
            }

            this._adminAdapter = null;
            this._adminFacets.clear();
            */

            this._state = StateDestroyed;

            /* TODO
            if(_initData.properties.getPropertyAsInt("Ice.Warn.UnusedProperties") > 0)
            {
                var unusedProperties:Vector.<String> = (_initData.properties as Ice.PropertiesI).getUnusedProperties();
                if(unusedProperties.length > 0)
                {
                    var message:Vector.<String> = new Vector.<String>();
                    message.push("The following properties were set but never read:");
                    for each(var p:String in unusedProperties)
                    {
                        message.push("\n    ");
                        message.push(p);
                    }
                    _initData.logger.warning(message.join(""));
                }
            }
            */

            promise.succeed(promise);
        };

        Ice.Instance = Instance;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Instance"));
