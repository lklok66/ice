// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{

    using System.Diagnostics;

    public class Instance
    {
	public virtual Ice.Properties properties()
	{
	    // No mutex lock, immutable.
	    return _properties;
	}
	
	public virtual Ice.Logger logger()
	{
	    lock(this)
	    {
		//
		// Don't throw CommunicatorDestroyedException if destroyed. We
		// need the logger also after destructions.
		//
		return _logger;
	    }
	}
	
	public virtual void logger(Ice.Logger logger)
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		_logger = logger;
	    }
	}
	
	public virtual Ice.Stats stats()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _stats;
	    }
	}
	
	public virtual void stats(Ice.Stats stats)
	{
	    lock(this)
	    {
		if(_destroyed)
		{
			throw new Ice.CommunicatorDestroyedException();
		}
		
		_stats = stats;
	    }
	}
	
	public virtual TraceLevels traceLevels()
	{
	    // No mutex lock, immutable.
	    return _traceLevels;
	}
	
	public virtual DefaultsAndOverrides defaultsAndOverrides()
	{
	    // No mutex lock, immutable.
	    return _defaultsAndOverrides;
	}
	
	public virtual RouterManager routerManager()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _routerManager;
	    }
	}
	
	public virtual LocatorManager locatorManager()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _locatorManager;
	    }
	}
	
	public virtual ReferenceFactory referenceFactory()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _referenceFactory;
	    }
	}
	
	public virtual ProxyFactory proxyFactory()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _proxyFactory;
	    }
	}
	
	public virtual OutgoingConnectionFactory outgoingConnectionFactory()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _outgoingConnectionFactory;
	    }
	}
	
	public virtual ConnectionMonitor connectionMonitor()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _connectionMonitor;
	    }
	}
	
	public virtual ObjectFactoryManager servantFactoryManager()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _servantFactoryManager;
	    }
	}
	
	public virtual ObjectAdapterFactory objectAdapterFactory()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _objectAdapterFactory;
	    }
	}
	
	public virtual ThreadPool clientThreadPool()
	{
	    lock(this)
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
	}
	
	public virtual ThreadPool serverThreadPool()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		if(_serverThreadPool == null)
		// Lazy initialization.
		{
		    int timeout = _properties.getPropertyAsInt("Ice.ServerIdleTime");
		    _serverThreadPool = new ThreadPool(this, "Ice.ThreadPool.Server", timeout);
		}
		
		return _serverThreadPool;
	    }
	}
	
	public virtual EndpointFactoryManager endpointFactoryManager()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _endpointFactoryManager;
	    }
	}
	
	public virtual Ice.PluginManager pluginManager()
	{
	    lock(this)
	    {
		if(_destroyed)
		{
		    throw new Ice.CommunicatorDestroyedException();
		}
		
		return _pluginManager;
	    }
	}
	
	public virtual int
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
	setDefaultContext(Ice.Context ctx)
	{
	    if(ctx == null || ctx.Count == 0)
	    {
	        _defaultContext = _emptyContext;
	    }
	    else
	    {
	        _defaultContext = (Ice.Context)ctx.Clone();
	    }
	}

	public Ice.Context
	getDefaultContext()
	{
	    return (Ice.Context)_defaultContext.Clone();
	}

	public virtual void flushBatchRequests()
	{
	    OutgoingConnectionFactory connectionFactory;
	    ObjectAdapterFactory adapterFactory;
	    
	    lock(this)
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
	
	public virtual BufferManager bufferManager()
	{
	    // No mutex lock, immutable.
	    return _bufferManager;
	}
	
	//
	// Only for use by Ice.CommunicatorI
	//
	public Instance(Ice.Communicator communicator, Ice.Properties properties)
	{
	    _destroyed = false;
	    _properties = properties;
	        
	    try
	    {
		lock(_staticLock)
		{
		    if(!_oneOffDone)
		    {
			string stdOut = _properties.getProperty("Ice.StdOut");
			string stdErr = _properties.getProperty("Ice.StdErr");
			
			System.IO.StreamWriter outStream = null;
			
			if(stdOut.Length > 0)
			{
			    outStream = System.IO.File.AppendText(stdOut);
			    outStream.AutoFlush = true;
			    System.Console.SetOut(outStream);
			}
			if(stdErr.Length > 0)
			{
			    if(stdErr.Equals(stdOut))
			    {
				System.Console.SetError(outStream); 
			    }
			    else
			    {
				System.IO.StreamWriter errStream = System.IO.File.AppendText(stdErr);
				errStream.AutoFlush = true;
				System.Console.SetError(errStream);
			    }
			}

			_oneOffDone = true;
		    }
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
		
		_traceLevels = new TraceLevels(_properties);
		
		_defaultsAndOverrides = new DefaultsAndOverrides(_properties);
		
		{
		    const int defaultMessageSizeMax = 1024;
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
	
	~Instance()
	{
	    Debug.Assert(_destroyed);
	    Debug.Assert(_referenceFactory == null);
	    Debug.Assert(_proxyFactory == null);
	    Debug.Assert(_outgoingConnectionFactory == null);
	    Debug.Assert(_connectionMonitor == null);
	    Debug.Assert(_servantFactoryManager == null);
	    // Debug.Assert(_userExceptionFactoryManager == null);
	    Debug.Assert(_objectAdapterFactory == null);
	    Debug.Assert(_clientThreadPool == null);
	    Debug.Assert(_serverThreadPool == null);
	    Debug.Assert(_routerManager == null);
	    Debug.Assert(_locatorManager == null);
	    Debug.Assert(_endpointFactoryManager == null);
	    Debug.Assert(_pluginManager == null);
	}
	
	public virtual void finishSetup(ref string[] args)
	{
	    //
	    // Load plug-ins.
	    //
	    Ice.PluginManagerI pluginManagerImpl = (Ice.PluginManagerI)_pluginManager;
	    pluginManagerImpl.loadPlugins(ref args);
	    
	    //
	    // Get default router and locator proxies. Don't move this
	    // initialization before the plug-in initialization!!! The proxies
	    // might depend on endpoint factories to be installed by plug-ins.
	    //
	    if(_defaultsAndOverrides.defaultRouter.Length > 0)
	    {
		_referenceFactory.setDefaultRouter(Ice.RouterPrxHelper.uncheckedCast(
			_proxyFactory.stringToProxy(_defaultsAndOverrides.defaultRouter)));
	    }
	    
	    if(_defaultsAndOverrides.defaultLocator.Length > 0)
	    {
		_referenceFactory.setDefaultLocator(Ice.LocatorPrxHelper.uncheckedCast(
			_proxyFactory.stringToProxy(_defaultsAndOverrides.defaultLocator)));
	    }
	    
	    //
	    // Show process id if requested (but only once).
	    //
	    if(!_printProcessIdDone && _properties.getPropertyAsInt("Ice.PrintProcessId") > 0)
	    {
		lock(this)
		{
		    if(!_printProcessIdDone)	// Double-checked locking
		    {
			System.Console.WriteLine(Process.GetCurrentProcess().Id);
			_printProcessIdDone = true;
		    }
		}
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
	public virtual void destroy()
	{
	    Debug.Assert(!_destroyed);
	    
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
	    
	    lock(this)
	    {
		_objectAdapterFactory = null;
		
		_outgoingConnectionFactory = null;
		
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
	
	private bool _destroyed;
	private volatile Ice.Properties _properties; // Immutable, not reset by destroy().
	private Ice.Logger _logger; // Not reset by destroy().
	private Ice.Stats _stats; // Not reset by destroy().
	private volatile TraceLevels _traceLevels; // Immutable, not reset by destroy().
	private volatile DefaultsAndOverrides _defaultsAndOverrides; // Immutable, not reset by destroy().
	private volatile int _messageSizeMax; // Immutable, not reset by destroy().
	private volatile int _clientConnectionIdleTime; // Immutable, not reset by destroy().
	private volatile int _serverConnectionIdleTime; // Immutable, not reset by destroy().
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
	private EndpointFactoryManager _endpointFactoryManager;
	private Ice.PluginManager _pluginManager;
	private Ice.Context _defaultContext;
	private static Ice.Context _emptyContext = new Ice.Context();
	private volatile BufferManager _bufferManager; // Immutable, not reset by destroy().
	private volatile static bool _printProcessIdDone = false;

        private static bool _oneOffDone = false;
        private static System.Object _staticLock = new System.Object();

    }

}
