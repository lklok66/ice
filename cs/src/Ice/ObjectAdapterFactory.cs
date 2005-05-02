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

    using System.Collections;
    using System.Diagnostics;
    using IceUtil;

    public sealed class ObjectAdapterFactory
    {
	public void shutdown()
	{
	    lock(this)
	    {
		//
		// Ignore shutdown requests if the object adapter factory has
		// already been shut down.
		//
		if(_instance == null)
		{
		    return;
		}
		
		foreach(Ice.ObjectAdapter adapter in _adapters.Values)
		{
		    adapter.deactivate();
		}
		
		_instance = null;
		_communicator = null;
		
		System.Threading.Monitor.PulseAll(this);
	    }
	}
	
	public void waitForShutdown()
	{
	    lock(this)
	    {
		//
		// First we wait for the shutdown of the factory itself.
		//
		while(_instance != null)
		{
		    System.Threading.Monitor.Wait(this);
		}
		
		//
		// If some other thread is currently shutting down, we wait
		// until this thread is finished.
		//
		while(_waitForShutdown)
		{
		    System.Threading.Monitor.Wait(this);
		}
		_waitForShutdown = true;
	    }

	    //
	    // Now we wait for deactivation of each object adapter.
	    //
	    if(_adapters != null)
	    {
		foreach(Ice.ObjectAdapter adapter in _adapters.Values)
		{
		    adapter.waitForDeactivate();
		}

		//
		// We're done, now we can throw away the object adapters.
		//
		// We set _adapters to null because our destructor must not
		// invoke methods on member objects.
		//
		_adapters = null;
	    }
	    
	    lock(this)
	    {
		//
		// Signal that waiting is complete.
		//
		_waitForShutdown = false;
		System.Threading.Monitor.PulseAll(this);
	    }
	}
	
	public Ice.ObjectAdapter createObjectAdapter(string name)
	{
	    lock(this)
	    {
		if(_instance == null)
		{
		    throw new Ice.ObjectAdapterDeactivatedException();
		}
		
		Ice.ObjectAdapter adapter = (Ice.ObjectAdapter)_adapters[name];
		if(adapter != null)
		{
		    return adapter;
		}
		
		adapter = new Ice.ObjectAdapterI(_instance, _communicator, name);
		_adapters[name] = adapter;
		return adapter;
	    }
	}
	
	public Ice.ObjectAdapter findObjectAdapter(Ice.ObjectPrx proxy)
	{
	    lock(this)
	    {
		if(_instance == null)
		{
		    return null;
		}
		
		IEnumerator i = _adapters.Values.GetEnumerator();
		while(i.MoveNext())
		{
		    Ice.ObjectAdapterI adapter = (Ice.ObjectAdapterI)i.Current;
		    try
		    {
			if(adapter.isLocal(proxy))
			{
			    return adapter;
			}
		    }
		    catch(Ice.ObjectAdapterDeactivatedException)
		    {
			// Ignore.
		    }
		}
		
		return null;
	    }
	}
	
	public void flushBatchRequests()
	{
	    LinkedList a = new LinkedList();
	    lock(this)
	    {
		foreach(Ice.ObjectAdapterI adapter in _adapters.Values)
		{
		    a.Add(adapter);
		}
	    }
	    foreach(Ice.ObjectAdapterI adapter in a)
	    {
		adapter.flushBatchRequests();
	    }
	}
	
	//
	// Only for use by Instance.
	//
	internal ObjectAdapterFactory(Instance instance, Ice.Communicator communicator)
	{
	    _instance = instance;
	    _communicator = communicator;
	    _adapters = new Hashtable();
	    _waitForShutdown = false;
	}
	
#if DEBUG
	~ObjectAdapterFactory()
	{
	    lock(this)
	    {
		IceUtil.Assert.FinalizerAssert(_instance == null);
		IceUtil.Assert.FinalizerAssert(_communicator == null);
		IceUtil.Assert.FinalizerAssert(_adapters == null);
		IceUtil.Assert.FinalizerAssert(!_waitForShutdown);
	    }
	}
#endif
	
	private Instance _instance;
	private Ice.Communicator _communicator;
	private Hashtable _adapters;
	private bool _waitForShutdown;
    }

}
