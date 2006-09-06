// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Ice;

public final class ObjectAdapterI extends LocalObjectImpl implements ObjectAdapter
{
    public String
    getName()
    {
	//
        // No mutex lock necessary, _name is immutable.
	//
        return _name;
    }

    public synchronized Communicator
    getCommunicator()
    {
	checkForDeactivation();
	
	return _communicator;
    }

    public void
    activate()
    {
	IceInternal.LocatorInfo locatorInfo = null;
        boolean registerProcess = false;
	boolean printAdapterReady = false;

	synchronized(this)
	{
	    checkForDeactivation();
	    
	    //
	    // If the one off initializations of the adapter are already
	    // done, we just need to activate the incoming connection
	    // factories and we're done.
	    //
	    if(_activateOneOffDone)
	    {
		final int sz = _incomingConnectionFactories.size();
		for(int i = 0; i < sz; ++i)
		{
		    IceInternal.IncomingConnectionFactory factory =
			(IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(i);
		    factory.activate();
		}		
		return;
	    }

	    //
	    // One off initializations of the adapter: update the
	    // locator registry and print the "adapter ready"
	    // message. We set the _waitForActivate flag to prevent
	    // deactivation from other threads while these one off
	    // initializations are done.
	    //
	    _waitForActivate = true;
	    
	    locatorInfo = _locatorInfo;
	    final Properties properties = _instance.initializationData().properties;
	    registerProcess = properties.getPropertyAsInt(_name +".RegisterProcess") > 0;
	    printAdapterReady = properties.getPropertyAsInt("Ice.PrintAdapterReady") > 0;
	}

	try
	{
	    Ice.Identity dummy = new Ice.Identity();
	    dummy.name = "dummy";
	    updateLocatorRegistry(locatorInfo, createDirectProxy(dummy), registerProcess);
	}
	catch(Ice.LocalException ex)
	{
	    //
	    // If we couldn't update the locator registry, we let the
	    // exception go through and don't activate the adapter to
	    // allow to user code to retry activating the adapter
	    // later.
	    //
	    synchronized(this)
	    {
		_waitForActivate = false;
		notifyAll();
	    }
	    throw ex;
	}

	if(printAdapterReady)
	{
	    System.out.println(_name + " ready");
	}

	synchronized(this)
	{
	    assert(!_deactivated); // Not possible if _waitForActivate = true;
	    
	    //
	    // Signal threads waiting for the activation.
	    //
	    _waitForActivate = false;
	    notifyAll();

	    _activateOneOffDone = true;
	    
	    final int sz = _incomingConnectionFactories.size();
	    for(int i = 0; i < sz; ++i)
	    {
		IceInternal.IncomingConnectionFactory factory =
		    (IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(i);
		factory.activate();
	    }
	}	
    }

    public synchronized void
    hold()
    {
	checkForDeactivation();
	
        final int sz = _incomingConnectionFactories.size();
        for(int i = 0; i < sz; ++i)
        {
            IceInternal.IncomingConnectionFactory factory =
                (IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(i);
            factory.hold();
        }
    }

    public synchronized void
    waitForHold()
    {
	checkForDeactivation();

	final int sz = _incomingConnectionFactories.size();
	for(int i = 0; i < sz; ++i)
	{
	    IceInternal.IncomingConnectionFactory factory =
		(IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(i);
	    factory.waitUntilHolding();
	}
    } 

    public void
    deactivate()
    {
	IceInternal.OutgoingConnectionFactory outgoingConnectionFactory;
	java.util.ArrayList incomingConnectionFactories;
	IceInternal.LocatorInfo locatorInfo;
	synchronized(this)
	{
	    //
	    // Ignore deactivation requests if the object adapter has
	    // already been deactivated.
	    //
	    if(_deactivated)
	    {
		return;
	    }

	    //
	    //
	    // Wait for activation to complete. This is necessary to not 
	    // get out of order locator updates.
	    //
	    while(_waitForActivate)
	    {
		try
		{
		    wait();
		}
		catch(InterruptedException ex)
		{
		}
	    }

	    if(_routerInfo != null)
	    {
                //
                // Remove entry from the router manager.
                //
                _instance.routerManager().erase(_routerInfo.getRouter());

                //
                //  Clear this object adapter with the router.
                //
                _routerInfo.setAdapter(null);
	    }
	    
	    incomingConnectionFactories = new java.util.ArrayList(_incomingConnectionFactories);
	    outgoingConnectionFactory = _instance.outgoingConnectionFactory();
	    locatorInfo = _locatorInfo;

	    _deactivated = true;
	    
	    notifyAll();
	}

	try
	{
	    updateLocatorRegistry(locatorInfo, null, false);
	}
	catch(Ice.LocalException ex)
	{
	    //
	    // We can't throw exceptions in deactivate so we ignore
	    // failures to update the locator registry.
	    //
	}

	//
	// Must be called outside the thread synchronization, because
	// Connection::destroy() might block when sending a CloseConnection
	// message.
	//
	final int sz = incomingConnectionFactories.size();
	for(int i = 0; i < sz; ++i)
	{
	    IceInternal.IncomingConnectionFactory factory =
		(IceInternal.IncomingConnectionFactory)incomingConnectionFactories.get(i);
	    factory.destroy();
	}

	//
	// Must be called outside the thread synchronization, because
	// changing the object adapter might block if there are still
	// requests being dispatched.
	//
	outgoingConnectionFactory.removeAdapter(this);
    }

    public void
    waitForDeactivate()
    {
	synchronized(this)
	{
	    //
	    // First we wait for deactivation of the adapter itself, and
	    // for the return of all direct method calls using this
	    // adapter.
	    //
	    while(!_deactivated || _directCount > 0)
	    {
		try
		{
		    wait();
		}
		catch(InterruptedException ex)
		{
		}
	    }
	    
	    //
	    // If some other thread is currently deactivating, we wait
	    // until this thread is finished.
	    //
	    while(_waitForDeactivate)
	    {
		try
		{
		    wait();
		}
		catch(InterruptedException ex)
		{
		}
	    }
	    _waitForDeactivate = true;
	}
	
	
	//
	// Now we wait for until all incoming connection factories are
	// finished.
	//
	if(_incomingConnectionFactories != null)
	{
	    final int sz = _incomingConnectionFactories.size();
	    for(int i = 0; i < sz; ++i)
	    {
		IceInternal.IncomingConnectionFactory factory =
		    (IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(i);
		factory.waitUntilFinished();
	    }
	}
	
	//
	// Now it's also time to clean up our servants and servant
	// locators.
	//
	if(_instance != null) // Don't destroy twice.
	{
	    _servantManager.destroy();
	}
	
	//
	// Destroy the thread pool.
	//
	if(_threadPool != null)
	{
	    _threadPool.destroy();
	    _threadPool.joinWithAllThreads();
	}

	IceInternal.ObjectAdapterFactory objectAdapterFactory;

	synchronized(this)
	{
	    //
	    // Signal that waiting is complete.
	    //
	    _waitForDeactivate = false;
	    notifyAll();

	    //
	    // We're done, now we can throw away all incoming connection
	    // factories.
	    //
	    // For compatibility with C#, we set _incomingConnectionFactories
	    // to null so that the finalizer does not invoke methods on objects.
	    //
	    _incomingConnectionFactories = null;
	    
	    //
	    // Remove object references (some of them cyclic).
	    //
	    _instance = null;
	    _threadPool = null;
	    _communicator = null;
	    _routerEndpoints = null;
	    _routerInfo = null;
	    _publishedEndpoints = null;
	    _locatorInfo = null;

	    objectAdapterFactory = _objectAdapterFactory;
	    _objectAdapterFactory = null;
	}

	if(objectAdapterFactory != null)
	{
	    objectAdapterFactory.removeObjectAdapter(_name);
	}
    }

    public ObjectPrx
    add(Ice.Object object, Identity ident)
    {
        return addFacet(object, ident, "");
    }

    public synchronized ObjectPrx
    addFacet(Ice.Object object, Identity ident, String facet)
    {
	checkForDeactivation();
	checkIdentity(ident);
	
        //
        // Create a copy of the Identity argument, in case the caller
        // reuses it.
        //
        Identity id = new Identity();
        id.category = ident.category;
        id.name = ident.name;

	_servantManager.addServant(object, id, facet);

        return newProxy(id, facet);
    }

    public ObjectPrx
    addWithUUID(Ice.Object object)
    {
        return addFacetWithUUID(object, "");
    }

    public ObjectPrx
    addFacetWithUUID(Ice.Object object, String facet)
    {
        Identity ident = new Identity();
        ident.category = "";
        ident.name = Util.generateUUID();

        return addFacet(object, ident, facet);
    }

    public Ice.Object
    remove(Identity ident)
    {
        return removeFacet(ident, "");
    }

    public synchronized Ice.Object
    removeFacet(Identity ident, String facet)
    {
	checkForDeactivation();
        checkIdentity(ident);

	return _servantManager.removeServant(ident, facet);
    }

    public synchronized java.util.Map
    removeAllFacets(Identity ident)
    {
	checkForDeactivation();
        checkIdentity(ident);

	return _servantManager.removeAllFacets(ident);
    }

    public Ice.Object
    find(Identity ident)
    {
        return findFacet(ident, "");
    }

    public synchronized Ice.Object
    findFacet(Identity ident, String facet)
    {
	checkForDeactivation();
        checkIdentity(ident);

        return _servantManager.findServant(ident, facet);
    }

    public synchronized java.util.Map
    findAllFacets(Identity ident)
    {
	checkForDeactivation();
        checkIdentity(ident);

        return _servantManager.findAllFacets(ident);
    }

    public synchronized Ice.Object
    findByProxy(ObjectPrx proxy)
    {
	checkForDeactivation();

        IceInternal.Reference ref = ((ObjectPrxHelperBase)proxy).__reference();
        return findFacet(ref.getIdentity(), ref.getFacet());
    }

    public synchronized void
    addServantLocator(ServantLocator locator, String prefix)
    {
	checkForDeactivation();

	_servantManager.addServantLocator(locator, prefix);
    }

    public synchronized ServantLocator
    findServantLocator(String prefix)
    {
	checkForDeactivation();

	return _servantManager.findServantLocator(prefix);
    }

    public synchronized ObjectPrx
    createProxy(Identity ident)
    {
	checkForDeactivation();
        checkIdentity(ident);

        return newProxy(ident, "");
    }

    public synchronized ObjectPrx
    createDirectProxy(Identity ident)
    {
	checkForDeactivation();
        checkIdentity(ident);

        return newDirectProxy(ident, "");
    }

    public synchronized ObjectPrx
    createIndirectProxy(Identity ident)
    {
	checkForDeactivation();
        checkIdentity(ident);

        return newIndirectProxy(ident, "", _id);
    }

    public synchronized ObjectPrx
    createReverseProxy(Identity ident)
    {
	checkForDeactivation();
        checkIdentity(ident);

        //
        // Get all incoming connections for this object adapter.
        //
        java.util.LinkedList connections = new java.util.LinkedList();
        final int sz = _incomingConnectionFactories.size();
        for(int i = 0; i < sz; ++i)
        {
            IceInternal.IncomingConnectionFactory factory =
                (IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(i);
            ConnectionI[] conns = factory.connections();
            for(int j = 0; j < conns.length; ++j)
            {
                connections.add(conns[j]);
            }
        }

        //
        // Create a reference and return a reverse proxy for this
        // reference.
        //
        IceInternal.EndpointI[] endpoints = new IceInternal.EndpointI[0];
        ConnectionI[] arr = new ConnectionI[connections.size()];
        connections.toArray(arr);
        IceInternal.Reference ref =
	    _instance.referenceFactory().create(ident, _instance.getDefaultContext(), "",
                                                IceInternal.Reference.ModeTwoway, arr);
        return _instance.proxyFactory().referenceToProxy(ref);
    }

    public synchronized void
    setLocator(LocatorPrx locator)
    {
	checkForDeactivation();

	_locatorInfo = _instance.locatorManager().get(locator);
    }

    public boolean
    isLocal(ObjectPrx proxy)
    {
        IceInternal.Reference ref = ((ObjectPrxHelperBase)proxy).__reference();
        IceInternal.EndpointI[] endpoints;

	try
	{
	    IceInternal.IndirectReference ir = (IceInternal.IndirectReference)ref;
	    if(ir.getAdapterId().length() != 0)
	    {
		//
		// Proxy is local if the reference adapter id matches this
		// adapter name.
		//
		return ir.getAdapterId().equals(_id);
	    }
	    IceInternal.LocatorInfo info = ir.getLocatorInfo();
	    if(info != null)
	    {
		endpoints = info.getEndpoints(ir, ir.getLocatorCacheTimeout(), new Ice.BooleanHolder());
	    }
	    else
	    {
		return false;
	    }
	}
	catch(ClassCastException e)
	{
	    endpoints = ref.getEndpoints();
	}


	synchronized(this)
	{
	    checkForDeactivation();

	    //
	    // Proxies which have at least one endpoint in common with the
	    // endpoints used by this object adapter's incoming connection
	    // factories are considered local.
	    //
	    for(int i = 0; i < endpoints.length; ++i)
	    {
		final int sz = _incomingConnectionFactories.size();
		for(int j = 0; j < sz; j++)
		{
		    IceInternal.IncomingConnectionFactory factory =
			(IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(j);
		    if(factory.equivalent(endpoints[i]))
		    {
			return true;
		    }
		}
	    }
	    
	    //
	    // Proxies which have at least one endpoint in common with the
	    // router's server proxy endpoints (if any), are also considered
	    // local.
	    //
	    if(_routerInfo != null && _routerInfo.getRouter().equals(proxy.ice_getRouter()))
	    {
	        for(int i = 0; i < endpoints.length; ++i)
	        {
		    // _routerEndpoints is sorted.
		    if(java.util.Collections.binarySearch(_routerEndpoints, endpoints[i]) >= 0)
		    {
		        return true;
		    }
	        }
	    }
	    
	    return false;
	}
    }

    public void
    flushBatchRequests()
    {
	java.util.ArrayList f;
	synchronized(this)
	{
	    f = new java.util.ArrayList(_incomingConnectionFactories);
	}
	java.util.Iterator i = f.iterator();
	while(i.hasNext())
	{
	    ((IceInternal.IncomingConnectionFactory)i.next()).flushBatchRequests();
	}
    }

    public synchronized void
    incDirectCount()
    {
	checkForDeactivation();

	assert(_directCount >= 0);
	++_directCount;
    }

    public synchronized void
    decDirectCount()
    {
	// Not check for deactivation here!
	
	assert(_instance != null); // Must not be called after waitForDeactivate().
	
	assert(_directCount > 0);
	if(--_directCount == 0)
	{
	    notifyAll();
	}
    }

    public IceInternal.ThreadPool
    getThreadPool()
    {
	// No mutex lock necessary, _threadPool and _instance are
	// immutable after creation until they are removed in
	// waitForDeactivate().
	
	// Not check for deactivation here!
	
	assert(_instance != null); // Must not be called after waitForDeactivate().

	if(_threadPool != null)
	{
	    return _threadPool;
	}
	else
	{
	    return _instance.serverThreadPool();
	}
    }

    public IceInternal.ServantManager
    getServantManager()
    {	
	//
	// No mutex lock necessary, _servantManager is immutable.
	//
	return _servantManager;
    }

    //
    // Only for use by IceInternal.ObjectAdapterFactory
    //
    public
    ObjectAdapterI(IceInternal.Instance instance, Communicator communicator, 
    		   IceInternal.ObjectAdapterFactory objectAdapterFactory, String name, String endpointInfo,
		   RouterPrx router)
    {
	_deactivated = false;
        _instance = instance;
	_communicator = communicator;
	_objectAdapterFactory = objectAdapterFactory;
	_servantManager = new IceInternal.ServantManager(instance, name);
	_activateOneOffDone = false;
        _name = name;
	_id = instance.initializationData().properties.getProperty(name + ".AdapterId");
	_replicaGroupId = instance.initializationData().properties.getProperty(name + ".ReplicaGroupId");
	_directCount = 0;
 	_waitForActivate = false;
	_waitForDeactivate = false;
	
        try
        {
	    if(router == null)
	    {
	        String routerStr = _instance.initializationData().properties.getProperty(name + ".Router");
	        if(routerStr.length() > 0)
	        {
		    router = RouterPrxHelper.uncheckedCast(_instance.proxyFactory().stringToProxy(routerStr));
	        }
	    }
	    if(router != null)
	    {
                _routerInfo = _instance.routerManager().get(router);
                if(_routerInfo != null)
                {
		    //
		    // Make sure this router is not already registered with another adapter.
		    //
		    if(_routerInfo.getAdapter() != null)
		    {
		        throw new AlreadyRegisteredException("object adapter with router", 
							     _instance.identityToString(router.ice_getIdentity()));
		    }

                    //
                    // Add the router's server proxy endpoints to this object
                    // adapter.
                    //
                    ObjectPrxHelperBase proxy = (ObjectPrxHelperBase)_routerInfo.getServerProxy();
                    IceInternal.EndpointI[] endpoints = proxy.__reference().getEndpoints();
                    for(int i = 0; i < endpoints.length; ++i)
                    {
                        _routerEndpoints.add(endpoints[i]);
                    }
                    java.util.Collections.sort(_routerEndpoints); // Must be sorted.

	            //
	            // Remove duplicate endpoints, so we have a list of unique
	            // endpoints.
	            //
	            for(int i = 0; i < _routerEndpoints.size()-1;)
	            {
                        java.lang.Object o1 = _routerEndpoints.get(i);
                        java.lang.Object o2 = _routerEndpoints.get(i + 1);
                        if(o1.equals(o2))
                        {
                            _routerEndpoints.remove(i);
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
                    _routerInfo.setAdapter(this);

                    //
                    // Also modify all existing outgoing connections to the
                    // router's client proxy to use this object adapter for
                    // callbacks.
                    //      
                    _instance.outgoingConnectionFactory().setRouterInfo(_routerInfo);
                }
	    }
	    else
	    {
	        //
	        // Parse the endpoints, but don't store them in the adapter.
	        // The connection factory might change it, for example, to
	        // fill in the real port number.
	        //
	        java.util.ArrayList endpoints = parseEndpoints(endpointInfo);
	        for(int i = 0; i < endpoints.size(); ++i)
	        {
		    IceInternal.EndpointI endp = (IceInternal.EndpointI)endpoints.get(i);
                    _incomingConnectionFactories.add(new IceInternal.IncomingConnectionFactory(
							 instance, endp, this, _name));
                }

	        //
	        // Parse published endpoints. If set, these are used in proxies
	        // instead of the connection factory Endpoints.
	        //
	        String endpts = _instance.initializationData().properties.getProperty(name + ".PublishedEndpoints");
	        _publishedEndpoints = parseEndpoints(endpts);
	        if(_publishedEndpoints.size() == 0)
	        {
	            for(int i = 0; i < _incomingConnectionFactories.size(); ++i)
		    {
		        IceInternal.IncomingConnectionFactory factory =
		            (IceInternal.IncomingConnectionFactory)_incomingConnectionFactories.get(i);
		        _publishedEndpoints.add(factory.endpoint());
		    }
	        }

	        //
	        // Filter out any endpoints that are not meant to be published.
	        //
	        java.util.Iterator p = _publishedEndpoints.iterator();
	        while(p.hasNext())
	        {
	            IceInternal.EndpointI endpoint = (IceInternal.EndpointI)p.next();
		    if(!endpoint.publish())
		    {
		        p.remove();
		    }
	        }
	    }

	    String locator = _instance.initializationData().properties.getProperty(name + ".Locator");
	    if(locator.length() > 0)
	    {
		setLocator(LocatorPrxHelper.uncheckedCast(_instance.proxyFactory().stringToProxy(locator)));
	    }
	    else
	    {
		setLocator(_instance.referenceFactory().getDefaultLocator());
	    }

	    if(!_instance.threadPerConnection())
	    {
		int size = _instance.initializationData().properties.getPropertyAsInt(_name + ".ThreadPool.Size");
		int sizeMax = _instance.initializationData().properties.getPropertyAsInt(_name + ".ThreadPool.SizeMax");
		if(size > 0 || sizeMax > 0)
		{
		    _threadPool = new IceInternal.ThreadPool(_instance, _name + ".ThreadPool", 0);
		}
	    }
        }
        catch(LocalException ex)
        {
	    deactivate();
	    waitForDeactivate();
            throw ex;
        }
    }

    protected synchronized void
    finalize()
        throws Throwable
    {
        if(!_deactivated)
        {
	    //
	    // For compatibility with C#, we do not invoke methods on other objects
	    // in a finalizer.
	    //
            //_instance.initializationData().logger.warning("object adapter `" + _name + "' has not been deactivated");
        }
        else if(_instance != null)
        {
	    //
	    // For compatibility with C#, we do not invoke methods on other objects
	    // in a finalizer.
	    //
            //_instance.initializationData().logger.warning("object adapter `" + _name + "' deactivation had not been waited for");
        }
	else
	{
	    IceUtil.Assert.FinalizerAssert(_threadPool == null);
	    //IceUtil.Assert.FinalizerAssert(_servantManager == null); // Not cleared, it needs to be immutable.
	    IceUtil.Assert.FinalizerAssert(_communicator == null);
	    IceUtil.Assert.FinalizerAssert(_incomingConnectionFactories == null);
	    IceUtil.Assert.FinalizerAssert(_directCount == 0);
	    IceUtil.Assert.FinalizerAssert(!_waitForActivate);
	    IceUtil.Assert.FinalizerAssert(!_waitForDeactivate);
	}

        super.finalize();
    }

    private ObjectPrx
    newProxy(Identity ident, String facet)
    {
	if(_id.length() == 0)
	{
	    return newDirectProxy(ident, facet);
	}
	else if(_replicaGroupId.length() == 0)
	{	    
	    return newIndirectProxy(ident, facet, _id);
	}
	else
	{
	    return newIndirectProxy(ident, facet, _replicaGroupId);
	}
    }

    private ObjectPrx
    newDirectProxy(Identity ident, String facet)
    {
        IceInternal.EndpointI[] endpoints;

	int sz = _publishedEndpoints.size();
	endpoints = new IceInternal.EndpointI[sz + _routerEndpoints.size()];
	_publishedEndpoints.toArray(endpoints);

        //
        // Now we also add the endpoints of the router's server proxy, if
        // any. This way, object references created by this object adapter
        // will also point to the router's server proxy endpoints.
        //
        for(int i = 0; i < _routerEndpoints.size(); ++i)
        {
	    endpoints[sz + i] = (IceInternal.EndpointI)_routerEndpoints.get(i);
        }

        //
        // Create a reference and return a proxy for this reference.
        //
        ConnectionI[] connections = new ConnectionI[0];
        IceInternal.Reference reference =
	    _instance.referenceFactory().create(ident, new java.util.HashMap(), facet, 
						IceInternal.Reference.ModeTwoway, false, endpoints, null,
						_instance.defaultsAndOverrides().defaultCollocationOptimization);
        return _instance.proxyFactory().referenceToProxy(reference);
    }

    private ObjectPrx
    newIndirectProxy(Identity ident, String facet, String id)
    {
	//
	// Create a reference with the adapter id and return a proxy
	// for the reference.
	//
	IceInternal.EndpointI[] endpoints = new IceInternal.EndpointI[0];
	ConnectionI[] connections = new ConnectionI[0];
	IceInternal.Reference reference =
	    _instance.referenceFactory().create(ident, new java.util.HashMap(), facet, 
						IceInternal.Reference.ModeTwoway, false, id, null, _locatorInfo,
						_instance.defaultsAndOverrides().defaultCollocationOptimization,
						_instance.defaultsAndOverrides().defaultLocatorCacheTimeout);
	return _instance.proxyFactory().referenceToProxy(reference);
    }

    private void
    checkForDeactivation()
    {
	if(_deactivated)
	{
            ObjectAdapterDeactivatedException ex = new ObjectAdapterDeactivatedException();
	    ex.name = _name;
	    throw ex;
	}
    }

    private static void
    checkIdentity(Identity ident)
    {
        if(ident.name == null || ident.name.length() == 0)
        {
            IllegalIdentityException e = new IllegalIdentityException();
	    e.id = (Identity)ident.clone();
            throw e;
        }

        if(ident.category == null)
        {
            ident.category = "";
        }
    }

    private java.util.ArrayList
    parseEndpoints(String endpts)
    {
        endpts = endpts.toLowerCase();

	int beg;
	int end = 0;

	final String delim = " \t\n\r";

	java.util.ArrayList endpoints = new java.util.ArrayList();
	while(end < endpts.length())
	{
	    beg = IceUtil.StringUtil.findFirstNotOf(endpts, delim, end);
	    if(beg == -1)
	    {
		break;
	    }

	    end = endpts.indexOf(':', beg);
	    if(end == -1)
	    {
		end = endpts.length();
	    }

	    if(end == beg)
	    {
		++end;
		continue;
	    }

	    String s = endpts.substring(beg, end);
	    IceInternal.EndpointI endp = _instance.endpointFactoryManager().create(s);
	    if(endp == null)
	    {
	        Ice.EndpointParseException e = new Ice.EndpointParseException();
		e.str = s;
		throw e;
	    }
	    java.util.ArrayList endps = endp.expand(true);
	    endpoints.addAll(endps);

	    ++end;
	}

	return endpoints;
    }

    private void
    updateLocatorRegistry(IceInternal.LocatorInfo locatorInfo, Ice.ObjectPrx proxy, boolean registerProcess)
    {
	if(!registerProcess && _id.length() == 0)
	{
	    return; // Nothing to update.
	}

	//
	// We must get and call on the locator registry outside the
	// thread synchronization to avoid deadlocks. (we can't make
	// remote calls within the OA synchronization because the
	// remote call will indirectly call isLocal() on this OA with
	// the OA factory locked).
	//
	// TODO: This might throw if we can't connect to the
	// locator. Shall we raise a special exception for the
	// activate operation instead of a non obvious network
	// exception?
	//
	LocatorRegistryPrx locatorRegistry = locatorInfo != null ? locatorInfo.getLocatorRegistry() : null;
	String serverId = "";
	if(registerProcess)
	{
	    assert(_instance != null);
	    serverId = _instance.initializationData().properties.getProperty("Ice.ServerId");

	    if(locatorRegistry == null)
	    {
		_instance.initializationData().logger.warning(
		    "object adapter `" + _name + "' cannot register the process without a locator registry");
	    }
	    else if(serverId.length() == 0)
	    {
		_instance.initializationData().logger.warning(
		    "object adapter `" + _name + "' cannot register the process without a value for Ice.ServerId");
	    }
	}

	if(locatorRegistry == null)
	{
	    return;
	}

	if(_id.length() > 0)
	{
	    try
	    {
		if(_replicaGroupId.length() == 0)
		{
		    locatorRegistry.setAdapterDirectProxy(_id, proxy);
		}
		else
		{
		    locatorRegistry.setReplicatedAdapterDirectProxy(_id, _replicaGroupId, proxy);
		}
	    }
	    catch(AdapterNotFoundException ex)
	    {
		NotRegisteredException ex1 = new NotRegisteredException();
		ex1.kindOfObject = "object adapter";
		ex1.id = _id;
		throw ex1;
	    }
	    catch(InvalidReplicaGroupIdException ex)
	    {
		NotRegisteredException ex1 = new NotRegisteredException();
		ex1.kindOfObject = "replica group";
		ex1.id = _replicaGroupId;
		throw ex1;
	    }
	    catch(AdapterAlreadyActiveException ex)
	    {
		ObjectAdapterIdInUseException ex1 = new ObjectAdapterIdInUseException();
		ex1.id = _id;
		throw ex1;
	    }
	}
	
	if(registerProcess && serverId.length() > 0)
	{
	    try
	    {
		Process servant = new ProcessI(_communicator);
		Ice.ObjectPrx process = createDirectProxy(addWithUUID(servant).ice_getIdentity());
		locatorRegistry.setServerProcessProxy(serverId, ProcessPrxHelper.uncheckedCast(process));
	    }
	    catch(ServerNotFoundException ex)
	    {
		NotRegisteredException ex1 = new NotRegisteredException();
		ex1.id = serverId;
		ex1.kindOfObject = "server";
		throw ex1;
	    }
	}
    }    

    private static class ProcessI extends _ProcessDisp
    {
        ProcessI(Communicator communicator)
        {
            _communicator = communicator;
        }

        public void
        shutdown(Ice.Current current)
        {
            _communicator.shutdown();
        }

	public void
	writeMessage(String message, int fd, Ice.Current current)
	{
	    switch(fd)
	    {
		case 1:
		{
		    System.out.println(message);
		    break;
		}
		case 2:
		{
		    System.err.println(message);
		    break;
		}
	    }
	}

        private Communicator _communicator;
    }

    private boolean _deactivated;
    private IceInternal.Instance _instance;
    private Communicator _communicator;
    private IceInternal.ObjectAdapterFactory _objectAdapterFactory;
    private IceInternal.ThreadPool _threadPool;
    private IceInternal.ServantManager _servantManager;
    private boolean _activateOneOffDone;
    final private String _name;
    final private String _id;
    final private String _replicaGroupId;
    private java.util.ArrayList _incomingConnectionFactories = new java.util.ArrayList();
    private java.util.ArrayList _routerEndpoints = new java.util.ArrayList();
    private IceInternal.RouterInfo _routerInfo = null;
    private java.util.ArrayList _publishedEndpoints = new java.util.ArrayList();
    private IceInternal.LocatorInfo _locatorInfo;
    private int _directCount;
    private boolean _waitForActivate;
    private boolean _waitForDeactivate;
}
