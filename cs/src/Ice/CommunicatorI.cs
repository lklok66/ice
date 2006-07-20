// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace Ice
{
	
    sealed class CommunicatorI : LocalObjectImpl, Communicator
    {
	public void destroy()
	{
	    instance_.destroy();
	}

	public void shutdown()
	{
	    instance_.objectAdapterFactory().shutdown();
	}
	
	public void waitForShutdown()
	{
	    instance_.objectAdapterFactory().waitForShutdown();
	}
	
	public Ice.ObjectPrx stringToProxy(string s)
	{
	    return instance_.proxyFactory().stringToProxy(s);
	}
	
	public string proxyToString(Ice.ObjectPrx proxy)
	{
	    return instance_.proxyFactory().proxyToString(proxy);
	}

	public Ice.Identity stringToIdentity(string s)
	{
	    return instance_.stringToIdentity(s);
	}

	public string identityToString(Ice.Identity ident)
	{
	    return instance_.identityToString(ident);
	}

	public ObjectAdapter createObjectAdapter(string name)
	{
	    return createObjectAdapterWithEndpoints(name, getProperties().getProperty(name + ".Endpoints"));
	}
	
	public ObjectAdapter createObjectAdapterWithEndpoints(string name, string endpoints)
	{
	    return instance_.objectAdapterFactory().createObjectAdapter(name, endpoints, null);
	}
	
	public ObjectAdapter createObjectAdapterWithRouter(string name, RouterPrx router)
	{
	    return instance_.objectAdapterFactory().createObjectAdapter(name, "", router);
	}
	
	public void addObjectFactory(ObjectFactory factory, string id)
	{
	    instance_.servantFactoryManager().add(factory, id);
	}
	
	public ObjectFactory findObjectFactory(string id)
	{
	    return instance_.servantFactoryManager().find(id);
	}
	
	public Properties getProperties()
	{
	    return instance_.initializationData().properties;
	}
	
	public Logger getLogger()
	{
	    return instance_.initializationData().logger;
	}

	public Stats getStats()
	{
	    return instance_.initializationData().stats;
	}

	public RouterPrx getDefaultRouter()
	{
	    return instance_.referenceFactory().getDefaultRouter();
	}

	public void setDefaultRouter(RouterPrx router)
	{
	    instance_.referenceFactory().setDefaultRouter(router);
	}

	public LocatorPrx getDefaultLocator()
	{
	    return instance_.referenceFactory().getDefaultLocator();
	}

	public void setDefaultLocator(LocatorPrx locator)
	{
	    instance_.referenceFactory().setDefaultLocator(locator);
	}
	
	public Ice.Context getDefaultContext()
	{
	    return instance_.getDefaultContext();
	}
	
	public void setDefaultContext(Ice.Context ctx)
	{
	    instance_.setDefaultContext(ctx);
	}

	public PluginManager getPluginManager()
	{
      	    return instance_.pluginManager();
	}

	public void flushBatchRequests()
	{
	    instance_.flushBatchRequests();
	}
	
	internal CommunicatorI(InitializationData initData)
	{
	    instance_ = new IceInternal.Instance(this, initData);
	}
	
#if DEBUG
	~CommunicatorI()
	{
	    lock(this)
	    {
		if(!instance_.destroyed())
		{
		    if(!System.Environment.HasShutdownStarted)
		    {
			instance_.initializationData().logger.warning(
				"Ice::Communicator::destroy() has not been called");
		    }
		    else
		    {
		        System.Console.Error.WriteLine("Ice::Communicator::destroy() has not been called");
		    }
		}
	    }
	}
#endif

	//
	// Certain initialization tasks need to be completed after the
	// constructor.
	//
	internal void finishSetup(ref string[] args)
	{
	    try
	    {
		instance_.finishSetup(ref args);
	    }
	    catch(System.Exception)
	    {
		instance_.destroy();
		throw;
	    }
	}
	
	//
	// For use by Util.getInstance()
	//
	internal IceInternal.Instance getInstance()
	{
	    return instance_;
	}
	
	
	private IceInternal.Instance instance_;
    }

}
