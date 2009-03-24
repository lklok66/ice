// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public final class Instance
{
    public Ice.InitializationData
    initializationData()
    {
        //
        // No check for destruction. It must be possible to access the
        // initialization data after destruction.
        //
        // No mutex lock, immutable.
        //
        return _initData;
    }

    public TraceLevels
    traceLevels()
    {
        // No mutex lock, immutable.
        return _traceLevels;
    }

    public DefaultsAndOverrides
    defaultsAndOverrides()
    {
        // No mutex lock, immutable.
        return _defaultsAndOverrides;
    }

    public synchronized RouterManager
    routerManager()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _routerManager;
    }

    public synchronized LocatorManager
    locatorManager()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _locatorManager;
    }

    public synchronized ReferenceFactory
    referenceFactory()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _referenceFactory;
    }
    
    public synchronized ProxyFactory
    proxyFactory()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _proxyFactory;
    }

    public synchronized OutgoingConnectionFactory
    outgoingConnectionFactory()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _outgoingConnectionFactory;
    }

    public synchronized ConnectionMonitor
    connectionMonitor()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _connectionMonitor;
    }

    public synchronized ObjectFactoryManager
    servantFactoryManager()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _servantFactoryManager;
    }

    public synchronized ObjectAdapterFactory
    objectAdapterFactory()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _objectAdapterFactory;
    }

    public synchronized int
    protocolSupport()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _protocolSupport;
    }

    public synchronized ThreadPool
    clientThreadPool()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        
        if(_clientThreadPool == null) // Lazy initialization.
        {
            _clientThreadPool = new ThreadPool(this, "Ice.ThreadPool.Client", 0);
        }

        return _clientThreadPool;
    }

    public synchronized ThreadPool
    serverThreadPool()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        
        if(_serverThreadPool == null) // Lazy initialization.
        {
            int timeout = _initData.properties.getPropertyAsInt("Ice.ServerIdleTime");
            _serverThreadPool = new ThreadPool(this, "Ice.ThreadPool.Server", timeout);
        }

        return _serverThreadPool;
    }

    public synchronized SelectorThread
    selectorThread()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }        

        if(_selectorThread == null) // Lazy initialization.
        {
            _selectorThread = new SelectorThread(this);
        }

        return _selectorThread;
    }

    public synchronized EndpointHostResolver
    endpointHostResolver()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }        

        if(_endpointHostResolver == null) // Lazy initialization.
        {
            _endpointHostResolver = new EndpointHostResolver(this);
        }

        return _endpointHostResolver;
    }

    synchronized public RetryQueue
    retryQueue()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }        

        return _retryQueue;
    }

    synchronized public Timer
    timer()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }        

        if(_timer == null) // Lazy initialization.
        {
            _timer = new Timer(this);
        }

        return _timer;
    }

    public synchronized EndpointFactoryManager
    endpointFactoryManager()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _endpointFactoryManager;
    }

    public synchronized Ice.PluginManager
    pluginManager()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return _pluginManager;
    }

    public int
    messageSizeMax()
    {
        // No mutex lock, immutable.
        return _messageSizeMax;
    }

    public int
    clientACM()
    {
        // No mutex lock, immutable.
        return _clientACM;
    }

    public int
    serverACM()
    {
        // No mutex lock, immutable.
        return _serverACM;
    }

    public synchronized void
    setDefaultContext(java.util.Map<String, String> ctx)
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        if(ctx == null || ctx.isEmpty())
        {
            _defaultContext = _emptyContext;
        }
        else
        {
            _defaultContext = new java.util.HashMap<String, String>(ctx);
        }
    }

    public synchronized java.util.Map<String, String>
    getDefaultContext()
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }

        return new java.util.HashMap<String, String>(_defaultContext);
    }

    public Ice.ImplicitContextI
    getImplicitContext()
    {
        return _implicitContext;
    }

    public void
    flushBatchRequests()
    {
        OutgoingConnectionFactory connectionFactory;
        ObjectAdapterFactory adapterFactory;

        synchronized(this)
        {
            if(_state == StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }
            
            connectionFactory = _outgoingConnectionFactory;
            adapterFactory = _objectAdapterFactory;
        }

        connectionFactory.flushBatchRequests();
        adapterFactory.flushBatchRequests();
    }

    public Ice.Identity
    stringToIdentity(String s)
    {
        return Ice.Util.stringToIdentity(s);
    }

    public String
    identityToString(Ice.Identity ident)
    {
        return Ice.Util.identityToString(ident);
    }

    
    public Ice.ObjectPrx 
    getAdmin()
    {
        Ice.ObjectAdapter adapter = null;
        String serverId = null;
        Ice.LocatorPrx defaultLocator = null;

        synchronized(this)
        {
            if(_state == StateDestroyed)
            {
                throw new Ice.CommunicatorDestroyedException();
            }

            final String adminOA = "Ice.Admin";
            
            if(_adminAdapter != null)
            {
                return _adminAdapter.createProxy(_adminIdentity);
            }
            else if(_initData.properties.getProperty(adminOA + ".Endpoints").length() == 0)
            {
                return null;
            }
            else
            {
                serverId = _initData.properties.getProperty("Ice.Admin.ServerId");
                String instanceName = _initData.properties.getProperty("Ice.Admin.InstanceName");

                defaultLocator = _referenceFactory.getDefaultLocator();
                
                if((defaultLocator != null && serverId.length() > 0) || instanceName.length() > 0)
                {
                    if(_adminIdentity == null)
                    {
                        if(instanceName.length() == 0)
                        {
                            instanceName = Ice.Util.generateUUID();
                        }
                        _adminIdentity = new Ice.Identity("admin", instanceName);
                        //
                        // Afterwards, _adminIdentity is read-only
                        //
                    }

                    //
                    // Create OA
                    //
                    _adminAdapter = _objectAdapterFactory.createObjectAdapter(adminOA, "", null);

                    //
                    // Add all facets to OA
                    //
                    java.util.Map<String, Ice.Object> filteredFacets = new java.util.HashMap<String, Ice.Object>();
                    java.util.Iterator<java.util.Map.Entry<String, Ice.Object> > p = _adminFacets.entrySet().iterator();
                    while(p.hasNext())
                    {
                        java.util.Map.Entry<String, Ice.Object> entry = p.next();

                        if(_adminFacetFilter.isEmpty() || _adminFacetFilter.contains(entry.getKey()))
                        {
                            _adminAdapter.addFacet(entry.getValue(), _adminIdentity, entry.getKey());
                        }
                        else
                        {
                            filteredFacets.put(entry.getKey(), entry.getValue());
                        }
                    }
                    _adminFacets = filteredFacets;
                    
                    adapter = _adminAdapter;
                }
            }
        }

        if(adapter == null)
        {
            return null;
        }
        else
        {
            try
            {
                adapter.activate();
            }
            catch(Ice.LocalException ex)
            {
                //
                // We cleanup _adminAdapter, however this error is not recoverable
                // (can't call again getAdmin() after fixing the problem)
                // since all the facets (servants) in the adapter are lost
                //
                adapter.destroy();
                synchronized(this)
                {
                    _adminAdapter = null;
                }
                throw ex;
            }

            Ice.ObjectPrx admin = adapter.createProxy(_adminIdentity);
            if(defaultLocator != null && serverId.length() > 0)
            {    
                Ice.ProcessPrx process = Ice.ProcessPrxHelper.uncheckedCast(admin.ice_facet("Process"));
                
                try
                {
                    //
                    // Note that as soon as the process proxy is registered, the communicator might be 
                    // shutdown by a remote client and admin facets might start receiving calls.
                    //
                    defaultLocator.getRegistry().setServerProcessProxy(serverId, process);
                }
                catch(Ice.ServerNotFoundException ex)
                {
                    if(_traceLevels.location >= 1)
                    {
                        StringBuilder s = new StringBuilder(128);
                        s.append("couldn't register server `");
                        s.append(serverId);
                        s.append("' with the locator registry:\n");
                        s.append("the server is not known to the locator registry");
                        _initData.logger.trace(_traceLevels.locationCat, s.toString());
                    }

                    throw new Ice.InitializationException("Locator knows nothing about server '" + serverId + "'");
                }
                catch(Ice.LocalException ex)
                {
                    if(_traceLevels.location >= 1)
                    {
                        StringBuilder s = new StringBuilder(128);
                        s.append("couldn't register server `");
                        s.append(serverId);
                        s.append("' with the locator registry:\n");
                        s.append(ex.toString());
                        _initData.logger.trace(_traceLevels.locationCat, s.toString());
                    }
                    throw ex;
                }

                if(_traceLevels.location >= 1)
                {
                    StringBuilder s = new StringBuilder(128);
                    s.append("registered server `");
                    s.append(serverId);
                    s.append("' with the locator registry");
                    _initData.logger.trace(_traceLevels.locationCat, s.toString());
                }
            }
            return admin;
        }
    }
    
    public synchronized void 
    addAdminFacet(Ice.Object servant, String facet)
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        
        if(_adminAdapter == null || (!_adminFacetFilter.isEmpty() && !_adminFacetFilter.contains(facet)))
        {
            if(_adminFacets.get(facet) != null)
            {
                throw new Ice.AlreadyRegisteredException("facet", facet);
            }
            _adminFacets.put(facet, servant);
        }
        else
        {
            _adminAdapter.addFacet(servant, _adminIdentity, facet);
        }
    }

    public synchronized Ice.Object 
    removeAdminFacet(String facet)
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        
        Ice.Object result = null;
        if(_adminAdapter == null || (!_adminFacetFilter.isEmpty() && !_adminFacetFilter.contains(facet)))
        {
            result = _adminFacets.remove(facet);

            if(result == null)
            {
                throw new Ice.NotRegisteredException("facet", facet);
            }
        }
        else
        {
            result = _adminAdapter.removeFacet(_adminIdentity, facet);
        }
        return result;
    }

    public synchronized void
    setDefaultLocator(Ice.LocatorPrx locator)
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        
        _referenceFactory = _referenceFactory.setDefaultLocator(locator);
    }

    public synchronized void
    setDefaultRouter(Ice.RouterPrx router)
    {
        if(_state == StateDestroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        
        _referenceFactory = _referenceFactory.setDefaultRouter(router);
    }

    public void
    setLogger(Ice.Logger logger)
    {
        // 
        // No locking, as it can only be called during plug-in loading
        //
        _initData.logger = logger;
    }

    //
    // Only for use by Ice.CommunicatorI
    //
    public
    Instance(Ice.Communicator communicator, Ice.InitializationData initData)
    {
        _state = StateActive;
        _initData = initData;

        try
        {
            if(_initData.properties == null)
            {
                _initData.properties = Ice.Util.createProperties();
            }

            synchronized(Instance.class)
            {
                if(!_oneOffDone)
                {
                    String stdOut = _initData.properties.getProperty("Ice.StdOut");
                    String stdErr = _initData.properties.getProperty("Ice.StdErr");
                    
                    java.io.PrintStream outStream = null;
                    
                    if(stdOut.length() > 0)
                    {
                        //
                        // We need to close the existing stdout for JVM thread dump to go
                        // to the new file
                        //
                        System.out.close();
                        
                        try
                        {
                            outStream = new java.io.PrintStream(new java.io.FileOutputStream(stdOut, true));
                        }
                        catch(java.io.FileNotFoundException ex)
                        {
                            Ice.FileException fe = new Ice.FileException();
                            fe.path = stdOut;
                            fe.initCause(ex);
                            throw fe;
                        }

                        System.setOut(outStream);
                    }
                    if(stdErr.length() > 0)
                    {
                        //
                        // close for consistency with stdout
                        //
                        System.err.close();
                        
                        if(stdErr.equals(stdOut))
                        {
                            System.setErr(outStream); 
                        }
                        else
                        {
                            try
                            {
                                System.setErr(new java.io.PrintStream(new java.io.FileOutputStream(stdErr, true)));
                            }
                            catch(java.io.FileNotFoundException ex)
                            {
                                Ice.FileException fe = new Ice.FileException();
                                fe.path = stdErr;
                                fe.initCause(ex);
                                throw fe;
                            }

                        }
                    }
                    _oneOffDone = true;
                }
            }

            if(_initData.logger == null)
            {
                if(_initData.properties.getPropertyAsInt("Ice.UseSyslog") > 0)
                {
                    _initData.logger = new Ice.SysLoggerI(_initData.properties.getProperty("Ice.ProgramName"));
                }
                else
                {
                    _initData.logger = Ice.Util.getProcessLogger();
                }
            }

            validatePackages();

            _traceLevels = new TraceLevels(_initData.properties);

            _defaultsAndOverrides = new DefaultsAndOverrides(_initData.properties);

            {
                final int defaultMessageSizeMax = 1024;
                int num = _initData.properties.getPropertyAsIntWithDefault("Ice.MessageSizeMax", defaultMessageSizeMax);
                if(num < 1)
                {
                    _messageSizeMax = defaultMessageSizeMax * 1024; // Ignore non-sensical values.
                }
                else if(num > 0x7fffffff / 1024)
                {
                    _messageSizeMax = 0x7fffffff;
                }
                else
                {
                    _messageSizeMax = num * 1024; // Property is in kilobytes, _messageSizeMax in bytes
                }
            }

            //
            // Client ACM enabled by default. Server ACM disabled by default.
            //
            _clientACM = _initData.properties.getPropertyAsIntWithDefault("Ice.ACM.Client", 60);
            _serverACM = _initData.properties.getPropertyAsInt("Ice.ACM.Server");

            _implicitContext = Ice.ImplicitContextI.create(_initData.properties.getProperty("Ice.ImplicitContext"));

            _routerManager = new RouterManager();

            _locatorManager = new LocatorManager(_initData.properties);

            _referenceFactory = new ReferenceFactory(this, communicator);

            _proxyFactory = new ProxyFactory(this);

            boolean ipv4 = _initData.properties.getPropertyAsIntWithDefault("Ice.IPv4", 1) > 0;
            boolean ipv6 = _initData.properties.getPropertyAsIntWithDefault("Ice.IPv6", 0) > 0;
            if(!ipv4 && !ipv6)
            {
                throw new Ice.InitializationException("Both IPV4 and IPv6 support cannot be disabled.");
            }
            else if(ipv4 && ipv6)
            {
                _protocolSupport = Network.EnableBoth;
            }
            else if(ipv4)
            {
                _protocolSupport = Network.EnableIPv4;
            }
            else
            {
                _protocolSupport = Network.EnableIPv6;
            }
            _endpointFactoryManager = new EndpointFactoryManager(this);
            EndpointFactory tcpEndpointFactory = new TcpEndpointFactory(this);
            _endpointFactoryManager.add(tcpEndpointFactory);
            EndpointFactory udpEndpointFactory = new UdpEndpointFactory(this);
            _endpointFactoryManager.add(udpEndpointFactory);

            _pluginManager = new Ice.PluginManagerI(communicator);
           
            _defaultContext = _emptyContext;
          
            _outgoingConnectionFactory = new OutgoingConnectionFactory(this);

            _servantFactoryManager = new ObjectFactoryManager();

            _objectAdapterFactory = new ObjectAdapterFactory(this, communicator);

            _retryQueue = new RetryQueue(this);

            //
            // Add Process and Properties facets
            //
            String[] facetFilter = _initData.properties.getPropertyAsList("Ice.Admin.Facets");
            if(facetFilter.length > 0)
            {
                _adminFacetFilter.addAll(java.util.Arrays.asList(facetFilter));
            }

            _adminFacets.put("Properties", new PropertiesAdminI(_initData.properties));
            _adminFacets.put("Process", new ProcessI(communicator));

        }
        catch(Ice.LocalException ex)
        {
            destroy();
            throw ex;
        }
    }

    protected synchronized void
    finalize()
        throws Throwable
    {
        IceUtilInternal.Assert.FinalizerAssert(_state == StateDestroyed);
        IceUtilInternal.Assert.FinalizerAssert(_referenceFactory == null);
        IceUtilInternal.Assert.FinalizerAssert(_proxyFactory == null);
        IceUtilInternal.Assert.FinalizerAssert(_outgoingConnectionFactory == null);
        IceUtilInternal.Assert.FinalizerAssert(_connectionMonitor == null);
        IceUtilInternal.Assert.FinalizerAssert(_servantFactoryManager == null);
        IceUtilInternal.Assert.FinalizerAssert(_objectAdapterFactory == null);
        IceUtilInternal.Assert.FinalizerAssert(_clientThreadPool == null);
        IceUtilInternal.Assert.FinalizerAssert(_serverThreadPool == null);
        IceUtilInternal.Assert.FinalizerAssert(_selectorThread == null);
        IceUtilInternal.Assert.FinalizerAssert(_endpointHostResolver == null);
        IceUtilInternal.Assert.FinalizerAssert(_timer == null);
        IceUtilInternal.Assert.FinalizerAssert(_routerManager == null);
        IceUtilInternal.Assert.FinalizerAssert(_locatorManager == null);
        IceUtilInternal.Assert.FinalizerAssert(_endpointFactoryManager == null);
        IceUtilInternal.Assert.FinalizerAssert(_pluginManager == null);
        IceUtilInternal.Assert.FinalizerAssert(_retryQueue == null);

        super.finalize();
    }

    public void
    finishSetup(Ice.StringSeqHolder args)
    {
        //
        // Load plug-ins.
        //
        Ice.PluginManagerI pluginManagerImpl = (Ice.PluginManagerI)_pluginManager;
        pluginManagerImpl.loadPlugins(args);

        //
        // Get default router and locator proxies. Don't move this
        // initialization before the plug-in initialization!!! The proxies
        // might depend on endpoint factories to be installed by plug-ins.
        //
        Ice.RouterPrx router = Ice.RouterPrxHelper.uncheckedCast(_proxyFactory.propertyToProxy("Ice.Default.Router"));
        if(router != null)
        {
            _referenceFactory = _referenceFactory.setDefaultRouter(router);
        }

        Ice.LocatorPrx loc = Ice.LocatorPrxHelper.uncheckedCast(_proxyFactory.propertyToProxy("Ice.Default.Locator"));
        if(loc != null)
        {
            _referenceFactory = _referenceFactory.setDefaultLocator(loc);
        }

        //
        // Start connection monitor if necessary. Set the check interval to
        // 1/10 of the ACM timeout with a minmal value of 1 second and a
        // maximum value of 5 minutes.
        //
        int interval = 0;
        if(_clientACM > 0 && _serverACM > 0)
        {
            if(_clientACM < _serverACM)
            {
                interval = _clientACM;
            }
            else
            {
                interval = _serverACM;
            }
        }
        else if(_clientACM > 0)
        {
            interval = _clientACM;
        }
        else if(_serverACM > 0)
        {
            interval = _serverACM;
        }
        if(interval > 0)
        {
            interval = java.lang.Math.min(300, java.lang.Math.max(5, (int)interval / 10));
        }
        interval = _initData.properties.getPropertyAsIntWithDefault("Ice.MonitorConnections", interval);
        if(interval > 0)
        {
            _connectionMonitor = new ConnectionMonitor(this, interval);
        }

        //
        // Thread pool initialization is now lazy initialization in
        // clientThreadPool() and serverThreadPool().
        //
        
        //
        // This must be done last as this call creates the Ice.Admin object adapter
        // and eventually registers a process proxy with the Ice locator (allowing 
        // remote clients to invoke on Ice.Admin facets as soon as it's registered).
        //
        if(_initData.properties.getPropertyAsIntWithDefault("Ice.Admin.DelayCreation", 0) <= 0)
        {
            getAdmin();
        }
    }

    //
    // Only for use by Ice.CommunicatorI
    //
    public void
    destroy()
    {
        synchronized(this)
        {
            //
            // If the _state is not StateActive then the instance is
            // either being destroyed, or has already been destroyed.
            //
            if(_state != StateActive)
            {
                return;
            }
            
            //
            // We cannot set state to StateDestroyed otherwise instance
            // methods called during the destroy process (such as
            // outgoingConnectionFactory() from
            // ObjectAdapterI::deactivate() will cause an exception.
            //
            _state = StateDestroyInProgress;
        }

        if(_objectAdapterFactory != null)
        {
            _objectAdapterFactory.shutdown();
        }

        if(_outgoingConnectionFactory != null)
        {
            _outgoingConnectionFactory.destroy();
        }

        if(_objectAdapterFactory != null)
        {
            _objectAdapterFactory.destroy();
        }
        
        if(_outgoingConnectionFactory != null)
        {
            _outgoingConnectionFactory.waitUntilFinished();
        }

        if(_retryQueue != null)
        {
            _retryQueue.destroy();
        }
        
        ThreadPool serverThreadPool = null;
        ThreadPool clientThreadPool = null;
        SelectorThread selectorThread = null;
        EndpointHostResolver endpointHostResolver = null;

        synchronized(this)
        {
            _objectAdapterFactory = null;
            _outgoingConnectionFactory = null;
            _retryQueue = null;

            if(_connectionMonitor != null)
            {
                _connectionMonitor.destroy();
                _connectionMonitor = null;
            }

            if(_serverThreadPool != null)
            {
                _serverThreadPool.destroy();
                serverThreadPool = _serverThreadPool;
                _serverThreadPool = null;       
            }

            if(_clientThreadPool != null)
            {
                _clientThreadPool.destroy();
                clientThreadPool = _clientThreadPool;
                _clientThreadPool = null;
            }

            if(_selectorThread != null)
            {
                _selectorThread.destroy();
                selectorThread = _selectorThread;
                _selectorThread = null;
            }

            if(_endpointHostResolver != null)
            {
                _endpointHostResolver.destroy();
                endpointHostResolver = _endpointHostResolver;
                _endpointHostResolver = null;
            }

            if(_timer != null)
            {
                _timer._destroy();
                _timer = null;
            }

            if(_servantFactoryManager != null)
            {
                _servantFactoryManager.destroy();
                _servantFactoryManager = null;
            }

            if(_referenceFactory != null)
            {
                _referenceFactory.destroy();
                _referenceFactory = null;
            }
            
            // _proxyFactory.destroy(); // No destroy function defined.
            _proxyFactory = null;

            if(_routerManager != null)
            {
                _routerManager.destroy();
                _routerManager = null;
            }

            if(_locatorManager != null)
            {
                _locatorManager.destroy();
                _locatorManager = null;
            }

            if(_endpointFactoryManager != null)
            {
                _endpointFactoryManager.destroy();
                _endpointFactoryManager = null;
            }

            if(_pluginManager != null)
            {
                _pluginManager.destroy();
                _pluginManager = null;
            }
            
            _adminAdapter = null;
            _adminFacets.clear();

            _state = StateDestroyed;
        }

        //
        // Join with threads outside the synchronization.
        //
        if(clientThreadPool != null)
        {
            clientThreadPool.joinWithAllThreads();
        }
        if(serverThreadPool != null)
        {
            serverThreadPool.joinWithAllThreads();
        }
        if(selectorThread != null)
        {
            selectorThread.joinWithThread();
        }
        if(endpointHostResolver != null)
        {
            endpointHostResolver.joinWithThread();
        }

        if(_initData.properties.getPropertyAsInt("Ice.Warn.UnusedProperties") > 0)
        {
            java.util.List<String> unusedProperties = ((Ice.PropertiesI)_initData.properties).getUnusedProperties();
            if(unusedProperties.size() != 0)
            {
                String message = "The following properties were set but never read:";
                java.util.Iterator<String> p = unusedProperties.iterator();
                while(p.hasNext())
                {
                    message += "\n    " + p.next();
                }
                _initData.logger.warning(message);
            }
        }
    }

    private void
    validatePackages()
    {
        final String prefix = "Ice.Package.";
        java.util.Map<String, String> map = _initData.properties.getPropertiesForPrefix(prefix);
        java.util.Iterator<java.util.Map.Entry<String, String> > p = map.entrySet().iterator();
        while(p.hasNext())
        {
            java.util.Map.Entry<String, String> e = p.next();
            String key = e.getKey();
            String pkg = e.getValue();
            if(key.length() == prefix.length())
            {
                _initData.logger.warning("ignoring invalid property: " + key + "=" + pkg);
            }
            String module = key.substring(prefix.length());
            String className = pkg + "." + module + "._Marker";
            try
            {
                Class.forName(className);
            }
            catch(java.lang.Exception ex)
            {
                _initData.logger.warning("unable to validate package: " + key + "=" + pkg);
            }
        }
    }

    private static final int StateActive = 0;
    private static final int StateDestroyInProgress = 1;
    private static final int StateDestroyed = 2;
    private int _state;

    private final Ice.InitializationData _initData; // Immutable, not reset by destroy().
    private final TraceLevels _traceLevels; // Immutable, not reset by destroy().
    private final DefaultsAndOverrides _defaultsAndOverrides; // Immutable, not reset by destroy().
    private final int _messageSizeMax; // Immutable, not reset by destroy().
    private final int _clientACM; // Immutable, not reset by destroy().
    private final int _serverACM; // Immutable, not reset by destroy().
    private final Ice.ImplicitContextI _implicitContext;
    private RouterManager _routerManager;
    private LocatorManager _locatorManager;
    private ReferenceFactory _referenceFactory;
    private ProxyFactory _proxyFactory;
    private OutgoingConnectionFactory _outgoingConnectionFactory;
    private ConnectionMonitor _connectionMonitor;
    private ObjectFactoryManager _servantFactoryManager;
    private ObjectAdapterFactory _objectAdapterFactory;
    private int _protocolSupport;
    private ThreadPool _clientThreadPool;
    private ThreadPool _serverThreadPool;
    private SelectorThread _selectorThread;
    private EndpointHostResolver _endpointHostResolver;
    private RetryQueue _retryQueue;
    private Timer _timer;
    private EndpointFactoryManager _endpointFactoryManager;
    private Ice.PluginManager _pluginManager;
    private java.util.Map<String, String> _defaultContext;

    private Ice.ObjectAdapter _adminAdapter;
    private java.util.Map<String, Ice.Object> _adminFacets = new java.util.HashMap<String, Ice.Object>();
    private java.util.Set<String> _adminFacetFilter = new java.util.HashSet<String>();
    private Ice.Identity _adminIdentity;

    private static java.util.Map<String, String> _emptyContext = new java.util.HashMap<String, String>();

    private static boolean _oneOffDone = false;
}
