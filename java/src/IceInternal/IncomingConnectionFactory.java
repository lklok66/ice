// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public final class IncomingConnectionFactory extends EventHandler
{
    public synchronized void
    activate()
    {
        setState(StateActive);
    }

    public synchronized void
    hold()
    {
        setState(StateHolding);
    }

    public synchronized void
    destroy()
    {
        setState(StateClosed);
    }

    public void
    waitUntilHolding()
    {
	java.util.LinkedList connections;

	synchronized(this)
	{
	    //
	    // First we wait until the connection factory itself is in holding
	    // state.
	    //
	    while(_state < StateHolding)
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
	    // We want to wait until all connections are in holding state
	    // outside the thread synchronization.
	    //
	    connections = (java.util.LinkedList)_connections.clone();
	}

	//
	// Now we wait until each connection is in holding state.
	//
	java.util.ListIterator p = connections.listIterator();
	while(p.hasNext())
	{
	    Ice.ConnectionI connection = (Ice.ConnectionI)p.next();
	    connection.waitUntilHolding();
	}
    }

    public void
    waitUntilFinished()
    {
	Thread threadPerIncomingConnectionFactory = null;
	java.util.LinkedList connections;

	synchronized(this)
	{
	    //
	    // First we wait until the factory is destroyed. If we are using
	    // an acceptor, we also wait for it to be closed.
	    //
	    while(_state != StateClosed || _acceptor != null)
	    {
		try
		{
		    wait();
		}
		catch(InterruptedException ex)
		{
		}
	    }

	    threadPerIncomingConnectionFactory = _threadPerIncomingConnectionFactory;
	    _threadPerIncomingConnectionFactory = null;

	    //
	    // We want to wait until all connections are finished outside the
	    // thread synchronization.
	    //
	    connections = _connections;
	    _connections = new java.util.LinkedList();
	}

	if(threadPerIncomingConnectionFactory != null)
	{
	    while(true)
	    {
		try
		{
		    threadPerIncomingConnectionFactory.join();
		    break;
		}
		catch(InterruptedException ex)
		{
		}
	    }
	}

	java.util.ListIterator p = connections.listIterator();
	while(p.hasNext())
	{
	    Ice.ConnectionI connection = (Ice.ConnectionI)p.next();
	    connection.waitUntilFinished();
	}
    }

    public Endpoint
    endpoint()
    {
        // No mutex protection necessary, _endpoint is immutable.
        return _endpoint;
    }

    public boolean
    equivalent(Endpoint endp)
    {
        if(_transceiver != null)
        {
            return endp.equivalent(_transceiver);
        }

        assert(_acceptor != null);
        return endp.equivalent(_acceptor);
    }

    public synchronized Ice.ConnectionI[]
    connections()
    {
	java.util.LinkedList connections = new java.util.LinkedList();

	//
	// Only copy connections which have not been destroyed.
	//
        java.util.ListIterator p = _connections.listIterator();
        while(p.hasNext())
        {
            Ice.ConnectionI connection = (Ice.ConnectionI)p.next();
            if(!connection.isDestroyed())
            {
                connections.add(connection);
            }
        }

        Ice.ConnectionI[] arr = new Ice.ConnectionI[connections.size()];
        connections.toArray(arr);
        return arr;
    }

    public void
    flushBatchRequests()
    {
        Ice.ConnectionI[] c = connections(); // connections() is synchronized, so no need to synchronize here.
	for(int i = 0; i < c.length; i++)
	{
	    try
	    {
		c[i].flushBatchRequests();
	    }
	    catch(Ice.LocalException ex)
	    {
		// Ignore.
	    }
	}
    }

    //
    // Operations from EventHandler.
    //

    public boolean
    datagram()
    {
	assert(!_instance.threadPerConnection()); // Only for use with a thread pool.
        return _endpoint.datagram();
    }

    public boolean
    readable()
    {
	assert(!_instance.threadPerConnection()); // Only for use with a thread pool.
        return false;
    }

    public void
    read(BasicStream unused)
    {
	assert(!_instance.threadPerConnection()); // Only for use with a thread pool.
        assert(false); // Must not be called.
    }

    public void
    message(BasicStream unused, ThreadPool threadPool)
    {
	assert(!_instance.threadPerConnection()); // Only for use with a thread pool.

	Ice.ConnectionI connection = null;

	synchronized(this)
	{
	    try
	    {
		if(_state != StateActive)
		{
		    Thread.yield();
		    return;
		}
		
		//
		// Reap connections for which destruction has completed.
		//
		java.util.ListIterator p = _connections.listIterator();
		while(p.hasNext())
		{
		    Ice.ConnectionI con = (Ice.ConnectionI)p.next();
		    if(con.isFinished())
		    {
			p.remove();
		    }
		}
		
		//
		// Now accept a new connection.
		//
		Transceiver transceiver;
		try
		{
		    transceiver = _acceptor.accept(0);
		}
		catch(Ice.TimeoutException ex)
		{
		    // Ignore timeouts.
		    return;
		}
		catch(Ice.LocalException ex)
		{
		    // Warn about other Ice local exceptions.
		    if(_warn)
		    {
			warning(ex);
		    }
		    return;
		}

		assert(transceiver != null);

		try
		{
		    connection = new Ice.ConnectionI(_instance, transceiver, _endpoint, _adapter);
		}
		catch(Ice.LocalException ex)
		{
		    return;
		}

		_connections.add(connection);
	    }
	    finally
	    {
		//
		// This makes sure that we promote a follower before
		// we leave the scope of the mutex above, but after we
		// call accept() (if we call it).
		//
		threadPool.promoteFollower();
	    }
	}

	assert(connection != null);
	
	//
	// We validate and activate outside the thread
	// synchronization, to not block the factory.
	//
	try
	{
	    connection.validate();
	}
        catch(Ice.LocalException ex)
	{
	    synchronized(this)
	    {
		connection.waitUntilFinished(); // We must call waitUntilFinished() for cleanup.
		_connections.remove(connection);
		return;
	    }
	}

	connection.activate();
    }

    public synchronized void
    finished(ThreadPool threadPool)
    {
	assert(!_instance.threadPerConnection()); // Only for use with a thread pool.

        threadPool.promoteFollower();

	if(_state == StateActive)
	{
	    registerWithPool();
	}
	else if(_state == StateClosed)
	{
	    _acceptor.close();
	    _acceptor = null;
	    notifyAll();
	}
    }

    public void
    exception(Ice.LocalException ex)
    {
        assert(false); // Must not be called.
    }

    public synchronized String
    toString()
    {
        if(_transceiver != null)
        {
	    return _transceiver.toString();
        }

        assert(_acceptor != null);	
	return _acceptor.toString();
    }

    public
    IncomingConnectionFactory(Instance instance, Endpoint endpoint, Ice.ObjectAdapter adapter)
    {
        super(instance);
        _endpoint = endpoint;
        _adapter = adapter;
	_registeredWithPool = false;
	_warn = _instance.properties().getPropertyAsInt("Ice.Warn.Connections") > 0 ? true : false;
        _state = StateHolding;

	DefaultsAndOverrides defaultsAndOverrides = _instance.defaultsAndOverrides();
	if(defaultsAndOverrides.overrideTimeout)
	{
	    _endpoint = _endpoint.timeout(defaultsAndOverrides.overrideTimeoutValue);
	}

	if(defaultsAndOverrides.overrideCompress)
	{
	    _endpoint = _endpoint.compress(defaultsAndOverrides.overrideCompressValue);
	}

	EndpointHolder h = new EndpointHolder();
	h.value = _endpoint;
	_transceiver = _endpoint.serverTransceiver(h);
	if(_transceiver != null)
	{
	    _endpoint = h.value;
	    
	    Ice.ConnectionI connection = null;
	    
	    try
	    {
		connection = new Ice.ConnectionI(_instance, _transceiver, _endpoint, _adapter);
		connection.validate();
	    }
	    catch(Ice.LocalException ex)
	    {
		//
		// If a connection object was constructed, then
		// validate() must have raised the exception.
		//
		if(connection != null)
		{
		    connection.waitUntilFinished(); // We must call waitUntilFinished() for cleanup.
		}
		
		return;
	    }
	    
	    _connections.add(connection);
	}
	else
	{
	    h.value = _endpoint;
	    _acceptor = _endpoint.acceptor(h);
	    _endpoint = h.value;
	    assert(_acceptor != null);
	    _acceptor.listen();
	    
	    if(_instance.threadPerConnection())
	    {
		try
		{
		    //
		    // If we are in thread per connection mode, we also use
		    // one thread per incoming connection factory, that
		    // accepts new connections on this endpoint.
		    //
		    _threadPerIncomingConnectionFactory = new ThreadPerIncomingConnectionFactory();
		    _threadPerIncomingConnectionFactory.start();
		}
		catch(java.lang.Exception ex)
		{
		    error("cannot create thread for incoming connection factory", ex);
		    
		    try
		    {
			_acceptor.close();
		    }
		    catch(Ice.LocalException e)
		    {
			// Here we ignore any exceptions in close().			
		    }

		    Ice.SyscallException e = new Ice.SyscallException();
		    e.initCause(ex);
		    throw e;
		}
	    }
        }
    }

    protected void
    finalize()
        throws Throwable
    {
	assert(_state == StateClosed);
	assert(_acceptor == null);
	assert(_connections.size() == 0);
	assert(_threadPerIncomingConnectionFactory == null);

        //
        // Destroy the EventHandler's stream, so that its buffer
        // can be reclaimed.
        //
        super._stream.destroy();

        super.finalize();
    }

    private static final int StateActive = 0;
    private static final int StateHolding = 1;
    private static final int StateClosed = 2;

    private void
    setState(int state)
    {
        if(_state == state) // Don't switch twice.
        {
            return;
        }

        switch(state)
        {
            case StateActive:
            {
                if(_state != StateHolding) // Can only switch from holding to active.
                {
                    return;
                }
		if(!_instance.threadPerConnection())
		{
		    registerWithPool();
		}

                java.util.ListIterator p = _connections.listIterator();
                while(p.hasNext())
                {
                    Ice.ConnectionI connection = (Ice.ConnectionI)p.next();
                    connection.activate();
                }
                break;
            }

            case StateHolding:
            {
                if(_state != StateActive) // Can only switch from active to holding.
                {
                    return;
                }
		if(!_instance.threadPerConnection())
		{
		    unregisterWithPool();
		}

                java.util.ListIterator p = _connections.listIterator();
                while(p.hasNext())
                {
                    Ice.ConnectionI connection = (Ice.ConnectionI)p.next();
                    connection.hold();
                }
                break;
            }

            case StateClosed:
            {
		if(_instance.threadPerConnection())
		{
		    if(_acceptor != null)
		    {
			//
			// Connect to our own acceptor, which unblocks our
			// thread per incoming connection factory stuck in accept().
			//
			_acceptor.connectToSelf();
		    }
		}
		else
		{
		    //
		    // If we come from holding state, we first need to
		    // register again before we unregister.
		    //
		    if(_state == StateHolding)
		    {
			registerWithPool();
		    }
		    unregisterWithPool();
		}

                java.util.ListIterator p = _connections.listIterator();
                while(p.hasNext())
                {   
                    Ice.ConnectionI connection = (Ice.ConnectionI)p.next();
                    connection.destroy(Ice.ConnectionI.ObjectAdapterDeactivated);
                }
		break;
            }
        }

        _state = state;
	notifyAll();
    }

    private void
    registerWithPool()
    {
	assert(!_instance.threadPerConnection()); // Only for use with a thread pool.

        if(_acceptor != null && !_registeredWithPool)
	{
	    ((Ice.ObjectAdapterI)_adapter).getThreadPool()._register(_acceptor.fd(), this);
	    _registeredWithPool = true;
        }
    }

    private void
    unregisterWithPool()
    {
	assert(!_instance.threadPerConnection()); // Only for use with a thread pool.

        if(_acceptor != null && _registeredWithPool)
	{
	    ((Ice.ObjectAdapterI)_adapter).getThreadPool().unregister(_acceptor.fd());
	    _registeredWithPool = false;
        }
    }

    private void
    warning(Ice.LocalException ex)
    {
        java.io.StringWriter sw = new java.io.StringWriter();
        java.io.PrintWriter pw = new java.io.PrintWriter(sw);
        ex.printStackTrace(pw);
        pw.flush();
        String s = "connection exception:\n" + sw.toString() + '\n' + _acceptor.toString();
        _instance.logger().warning(s);
    }

    private void
    error(String msg, Exception ex)
    {
	java.io.StringWriter sw = new java.io.StringWriter();
	java.io.PrintWriter pw = new java.io.PrintWriter(sw);
	ex.printStackTrace(pw);
	pw.flush();
	String s = msg + ":\n" + toString() + sw.toString();
	_instance.logger().error(s);
    }

    private void
    run()
    {
	assert(_acceptor != null);

	while(true)
	{
	    //
	    // We must accept new connections outside the thread
	    // synchronization, because we use blocking accept.
	    //
	    Transceiver transceiver = null;
	    try
	    {
		transceiver = _acceptor.accept(-1);
	    }
	    catch(Ice.SocketException ex)
	    {
		// Do not ignore SocketException in Java.
		throw ex;
	    }
	    catch(Ice.TimeoutException ex)
	    {
		// Ignore timeouts.
	    }
	    catch(Ice.LocalException ex)
	    {
		// Warn about other Ice local exceptions.
		if(_warn)
		{
		    warning(ex);
		}
	    }

	    Ice.ConnectionI connection = null;

	    synchronized(this)
	    {
		while(_state == StateHolding)
		{
		    try
		    {
			wait();
		    }
		    catch(InterruptedException ex)
		    {
		    }
		}

		if(_state == StateClosed)
		{
		    if(transceiver != null)
		    {
			try
			{
			    transceiver.close();
			}
			catch(Ice.LocalException ex)
			{
			    // Here we ignore any exceptions in close().
			}
		    }

		    try
		    {
			_acceptor.close();
		    }
		    catch(Ice.LocalException ex)
		    {
			_acceptor = null;
			notifyAll();
			throw ex;
		    }

		    _acceptor = null;
		    notifyAll();
		    return;
		}

		assert(_state == StateActive);

		//
		// Reap connections for which destruction has completed.
		//
		java.util.ListIterator p = _connections.listIterator();
		while(p.hasNext())
		{
		    Ice.ConnectionI con = (Ice.ConnectionI)p.next();
		    if(con.isFinished())
		    {
			p.remove();
		    }
		}

		//
		// Create a connection object for the connection.
		//
		if(transceiver != null)
		{
		    try
		    {
			connection = new Ice.ConnectionI(_instance, transceiver, _endpoint, _adapter);
		    }
		    catch(Ice.LocalException ex)
		    {
			return;
		    }

		    _connections.add(connection);
		}
	    }

	    //
	    // In thread per connection mode, the connection's thread
	    // will take care of connection validation and activation
	    // (for non-datagram connections). We don't want to block
	    // this thread waiting until validation is complete,
	    // because in contrast to thread pool mode, it is the only
	    // thread that can accept connections with this factory's
	    // acceptor. Therefore we don't call validate() and
	    // activate() from the connection factory in thread per
	    // connection mode.
	    //
	}
    }

    private class ThreadPerIncomingConnectionFactory extends Thread
    {
	public void
	run()
	{
	    try
	    {
		IncomingConnectionFactory.this.run();
	    }
	    catch(Exception ex)
	    {
		IncomingConnectionFactory.this.error("exception in thread per incoming connection factory", ex);
	    }
	}
    }
    private Thread _threadPerIncomingConnectionFactory;

    private Acceptor _acceptor;
    private final Transceiver _transceiver;
    private Endpoint _endpoint;

    private final Ice.ObjectAdapter _adapter;

    private boolean _registeredWithPool;

    private final boolean _warn;

    private java.util.LinkedList _connections = new java.util.LinkedList();

    private int _state;
}
