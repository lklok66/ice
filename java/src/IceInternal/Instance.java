// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public class Instance
{
    public Ice.Properties
    properties()
    {
	// No mutex lock, immutable.
        return _properties;
    }

    public synchronized Ice.Logger
    logger()
    {
	//
	// Don't throw CommunicatorDestroyedException if destroyed. We
	// need the logger also after destructions.
	//
        return _logger;
    }

    public synchronized void
    logger(Ice.Logger logger)
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        _logger = logger;
    }

    public synchronized Ice.Stats
    stats()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _stats;
    }

    public synchronized void
    stats(Ice.Stats stats)
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        _stats = stats;
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
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _routerManager;
    }

    public synchronized LocatorManager
    locatorManager()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _locatorManager;
    }

    public synchronized ReferenceFactory
    referenceFactory()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _referenceFactory;
    }

    public synchronized ProxyFactory
    proxyFactory()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _proxyFactory;
    }

    public synchronized OutgoingConnectionFactory
    outgoingConnectionFactory()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _outgoingConnectionFactory;
    }

    public synchronized ConnectionMonitor
    connectionMonitor()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _connectionMonitor;
    }

    public synchronized ObjectFactoryManager
    servantFactoryManager()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _servantFactoryManager;
    }

    public synchronized ObjectAdapterFactory
    objectAdapterFactory()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _objectAdapterFactory;
    }

    public synchronized ThreadPool
    clientThreadPool()
    {
	if(_destroyed)
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
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}
	
	if(_serverThreadPool == null) // Lazy initialization.
	{
	    int timeout = _properties.getPropertyAsInt("Ice.ServerIdleTime");
	    _serverThreadPool = new ThreadPool(this, "Ice.ThreadPool.Server", timeout);
	}

        return _serverThreadPool;
    }

    public boolean
    threadPerConnection()
    {
	return _threadPerConnection;
    }

    public int
    threadPerConnectionStackSize()
    {
	return _threadPerConnectionStackSize;
    }

    public synchronized EndpointFactoryManager
    endpointFactoryManager()
    {
	if(_destroyed)
	{
	    throw new Ice.CommunicatorDestroyedException();
	}

        return _endpointFactoryManager;
    }

    public synchronized Ice.PluginManager
    pluginManager()
    {
	if(_destroyed)
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
    clientConnectionIdleTime()
    {
	// No mutex lock, immutable.
	return _clientConnectionIdleTime;
    }

    public int
    serverConnectionIdleTime()
    {
	// No mutex lock, immutable.
	return _serverConnectionIdleTime;
    }

    public void
    setDefaultContext(java.util.Map ctx)
    {
	if(ctx == null || ctx.isEmpty())
	{
	    _defaultContext = _emptyContext;
	}
	else
	{
	    _defaultContext = new java.util.HashMap(ctx);
	}
    }

    public java.util.Map
    getDefaultContext()
    {
        return new java.util.HashMap(_defaultContext);
    }

    public void
    flushBatchRequests()
    {
	OutgoingConnectionFactory connectionFactory;
	ObjectAdapterFactory adapterFactory;

	synchronized(this)
	{
	    
	    if(_destroyed)
	    {
		throw new Ice.CommunicatorDestroyedException();
	    }
	    
	    connectionFactory = _outgoingConnectionFactory;
	    adapterFactory = _objectAdapterFactory;
	}

	connectionFactory.flushBatchRequests();
	adapterFactory.flushBatchRequests();
    }

    public BufferManager
    bufferManager()
    {
	// No mutex lock, immutable.
        return _bufferManager;
    }

    //
    // Only for use by Ice.CommunicatorI
    //
    public
    Instance(Ice.Communicator communicator, Ice.Properties properties)
    {
        _destroyed = false;
        _properties = properties;

        try
        {
	    try
	    {
		synchronized(Instance.class)
		{
		    if(!_oneOffDone)
		    {
			String stdOut = _properties.getProperty("Ice.StdOut");
			String stdErr = _properties.getProperty("Ice.StdErr");
			
			java.io.PrintStream outStream = null;
			
			if(stdOut.length() > 0)
			{
			    //
			    // We need to close the existing stdout for JVM thread dump to go
			    // to the new file
			    //
			    System.out.close();

			    outStream = new java.io.PrintStream(new java.io.FileOutputStream(stdOut, true));
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
				System.setErr(new java.io.PrintStream(new java.io.FileOutputStream(stdErr, true)));
			    }
			}
			_oneOffDone = true;
		    }
		}
	    }
	    catch(java.io.FileNotFoundException ex)
	    {
		Ice.SyscallException se = new Ice.SyscallException();
		se.initCause(ex);
		throw se;
	    }

	    if(_properties.getPropertyAsInt("Ice.UseSyslog") > 0)
	    {
		_logger = new Ice.SysLoggerI(_properties.getProperty("Ice.ProgramName"));
	    }
	    else
	    {
		_logger = new Ice.LoggerI(_properties.getProperty("Ice.ProgramName"),
					  _properties.getPropertyAsInt("Ice.Logger.Timestamp") > 0);
	    }

	    _stats = null; // There is no default statistics callback object.

            validatePackages();

            _traceLevels = new TraceLevels(_properties);

            _defaultsAndOverrides = new DefaultsAndOverrides(_properties);

	    {
		final int defaultMessageSizeMax = 1024;
		int num = _properties.getPropertyAsIntWithDefault("Ice.MessageSizeMax", defaultMessageSizeMax);
		if(num < 1)
		{
		    _messageSizeMax = defaultMessageSizeMax * 1024; // Ignore stupid values.
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

	    int clientConnectionIdleTime = 0;
	    int serverConnectionIdleTime = 0;

	    {
		int num = _properties.getPropertyAsInt("Ice.ConnectionIdleTime");
		if(num > 0)
		{
		    clientConnectionIdleTime = num;
		    serverConnectionIdleTime = num;
		}
	    }
	    
	    {
		int num = _properties.getPropertyAsIntWithDefault("Ice.ConnectionIdleTime.Client", 60);
		if(num > 0)
		{
		    clientConnectionIdleTime = num;
		}
	    }

	    {
		int num = _properties.getPropertyAsInt("Ice.ConnectionIdleTime.Server");
		if(num > 0)
		{
		    serverConnectionIdleTime = num;
		}
	    }

	    _clientConnectionIdleTime = clientConnectionIdleTime;
	    _serverConnectionIdleTime = serverConnectionIdleTime;

	    _threadPerConnection = _properties.getPropertyAsInt("Ice.ThreadPerConnection") > 0;

	    {
		int stackSize = _properties.getPropertyAsInt("Ice.ThreadPerConnection.StackSize");
		if(stackSize < 0)
		{
		    stackSize = 0;
		}
		_threadPerConnectionStackSize = stackSize;
	    }

            _routerManager = new RouterManager();

            _locatorManager = new LocatorManager();

            _referenceFactory = new ReferenceFactory(this);

            _proxyFactory = new ProxyFactory(this);

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

            _bufferManager = new BufferManager(); // Must be created before the ThreadPool
        }
        catch(Ice.LocalException ex)
        {
            destroy();
            throw ex;
        }
    }

    protected void
    finalize()
        throws Throwable
    {
        assert(_destroyed);
        assert(_referenceFactory == null);
        assert(_proxyFactory == null);
        assert(_outgoingConnectionFactory == null);
	assert(_connectionMonitor == null);
        assert(_servantFactoryManager == null);
        assert(_objectAdapterFactory == null);
        assert(_clientThreadPool == null);
        assert(_serverThreadPool == null);
        assert(_routerManager == null);
        assert(_locatorManager == null);
        assert(_endpointFactoryManager == null);
        assert(_pluginManager == null);

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
	if(_defaultsAndOverrides.defaultRouter.length() > 0)
	{
	    _referenceFactory.setDefaultRouter(Ice.RouterPrxHelper.uncheckedCast(
		    _proxyFactory.stringToProxy(_defaultsAndOverrides.defaultRouter)));
	}

	if(_defaultsAndOverrides.defaultLocator.length() > 0)
	{
	    _referenceFactory.setDefaultLocator(Ice.LocatorPrxHelper.uncheckedCast(
		    _proxyFactory.stringToProxy(_defaultsAndOverrides.defaultLocator)));
	}
	
	//
	// Start connection monitor if necessary.
	//
	int interval = 0;
	if(_clientConnectionIdleTime > 0 && _serverConnectionIdleTime > 0)
	{
	    if(_clientConnectionIdleTime < _serverConnectionIdleTime)
	    {
		interval = _clientConnectionIdleTime;
	    }
	    else
	    {
		interval = _serverConnectionIdleTime;
	    }
	}
	else if(_clientConnectionIdleTime > 0)
	{
	    interval = _clientConnectionIdleTime;
	}
	else if(_serverConnectionIdleTime > 0)
	{
	    interval = _serverConnectionIdleTime;
	}
	interval = _properties.getPropertyAsIntWithDefault("Ice.MonitorConnections", interval);
	if(interval > 0)
	{
	    _connectionMonitor = new ConnectionMonitor(this, interval);
	}

        //
        // Thread pool initialization is now lazy initialization in
        // clientThreadPool() and serverThreadPool().
        //
    }

    //
    // Only for use by Ice.CommunicatorI
    //
    public void
    destroy()
    {
	assert(!_destroyed);

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
            _objectAdapterFactory.waitForShutdown();
        }
	
        if(_outgoingConnectionFactory != null)
        {
            _outgoingConnectionFactory.waitUntilFinished();
        }
	
	ThreadPool serverThreadPool = null;
	ThreadPool clientThreadPool = null;

	synchronized(this)
	{
	    _objectAdapterFactory = null;

	    _outgoingConnectionFactory = null;

	    if(_connectionMonitor != null)
	    {
		_connectionMonitor._destroy();
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
	    
	    // No destroy function defined.
	    // _proxyFactory.destroy();
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
	    
	    _destroyed = true;
	}

	//
	// Join with the thread pool threads outside the
	// synchronization.
	//
	if(clientThreadPool != null)
	{
	    clientThreadPool.joinWithAllThreads();
	}
	if(serverThreadPool != null)
	{
	    serverThreadPool.joinWithAllThreads();
	}
    }

    private void
    validatePackages()
    {
        final String prefix = "Ice.Package.";
        java.util.Map map = _properties.getPropertiesForPrefix(prefix);
        java.util.Iterator p = map.entrySet().iterator();
        while(p.hasNext())
        {
            java.util.Map.Entry e = (java.util.Map.Entry)p.next();
            String key = (String)e.getKey();
            String pkg = (String)e.getValue();
            if(key.length() == prefix.length())
            {
                _logger.warning("ignoring invalid property: " + key + "=" + pkg);
            }
            String module = key.substring(prefix.length());
            String className = pkg + "." + module + "._Marker";
            try
            {
                Class.forName(className);
            }
            catch(java.lang.Exception ex)
            {
                _logger.warning("unable to validate package: " + key + "=" + pkg);
            }
        }
    }

    private boolean _destroyed;
    private final Ice.Properties _properties; // Immutable, not reset by destroy().
    private Ice.Logger _logger; // Not reset by destroy().
    private Ice.Stats _stats; // Not reset by destroy().
    private final TraceLevels _traceLevels; // Immutable, not reset by destroy().
    private final DefaultsAndOverrides _defaultsAndOverrides; // Immutable, not reset by destroy().
    private final int _messageSizeMax; // Immutable, not reset by destroy().
    private final int _clientConnectionIdleTime; // Immutable, not reset by destroy().
    private final int _serverConnectionIdleTime; // Immutable, not reset by destroy().
    private RouterManager _routerManager;
    private LocatorManager _locatorManager;
    private ReferenceFactory _referenceFactory;
    private ProxyFactory _proxyFactory;
    private OutgoingConnectionFactory _outgoingConnectionFactory;
    private ConnectionMonitor _connectionMonitor;
    private ObjectFactoryManager _servantFactoryManager;
    private ObjectAdapterFactory _objectAdapterFactory;
    private ThreadPool _clientThreadPool;
    private ThreadPool _serverThreadPool;
    private final boolean _threadPerConnection;
    private final int _threadPerConnectionStackSize;
    private EndpointFactoryManager _endpointFactoryManager;
    private Ice.PluginManager _pluginManager;
    private java.util.Map _defaultContext;
    private static java.util.Map _emptyContext = new java.util.HashMap();
    private final BufferManager _bufferManager; // Immutable, not reset by destroy().

    private static boolean _oneOffDone = false;
}
