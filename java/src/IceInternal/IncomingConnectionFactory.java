// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

import java.util.Iterator;

public final class IncomingConnectionFactory implements Ice.ConnectionI.StartCallback
{
    private Thread _acceptThread;

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
        java.util.LinkedList<Ice.ConnectionI> connections;

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
            connections = new java.util.LinkedList<Ice.ConnectionI>(_connections);
        }

        //
        // Now we wait until each connection is in holding state.
        //
        java.util.ListIterator<Ice.ConnectionI> p = connections.listIterator();
        while(p.hasNext())
        {
            Ice.ConnectionI connection = p.next();
            connection.waitUntilHolding();
        }
    }

    public void
    waitUntilFinished()
    {
        java.util.LinkedList<Ice.ConnectionI> connections = null;

        synchronized(this)
        {
            if(_acceptor != null)
            {
                //
                // We connect to our own acceptor, which unblocks our acceptor
                // thread stuck in accept().
                //
                _acceptor.connectToSelf();
            }
            
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

            //
            // Clear the OA. See bug 1673 for the details of why this is necessary.
            //
            _adapter = null;

            //
            // We want to wait until all connections are finished outside the
            // thread synchronization.
            //
            if(_connections != null)
            {
                connections = new java.util.LinkedList<Ice.ConnectionI>(_connections);
            }

            // Join with the acceptor thread.
            while(_acceptThread != null && _acceptThread.isAlive())
            {
                try
                {
                    _acceptThread.join();
                }
                catch(InterruptedException e)
                {
                }
            }
            _acceptThread = null;
        }

        if(connections != null)
        {
            java.util.ListIterator<Ice.ConnectionI> p = connections.listIterator();
            while(p.hasNext())
            {
                Ice.ConnectionI connection = p.next();
                connection.waitUntilFinished();
            }
        }

        synchronized(this)
        {
            //
            // For consistency with C#, we set _connections to null rather than to a
            // new empty list so that our finalizer does not try to invoke any
            // methods on member objects.
            //
            _connections = null;
        }
    }

    public EndpointI
    endpoint()
    {
        // No mutex protection necessary, _endpoint is immutable.
        return _endpoint;
    }

    public synchronized java.util.LinkedList<Ice.ConnectionI>
    connections()
    {
        java.util.LinkedList<Ice.ConnectionI> connections = new java.util.LinkedList<Ice.ConnectionI>();

        //
        // Only copy connections which have not been destroyed.
        //
        java.util.ListIterator<Ice.ConnectionI> p = _connections.listIterator();
        while(p.hasNext())
        {
            Ice.ConnectionI connection = p.next();
            if(connection.isActiveOrHolding())
            {
                connections.add(connection);
            }
        }
        
        return connections;
    }

    public void
    flushBatchRequests()
    {
        java.util.Iterator<Ice.ConnectionI> p =
            connections().iterator(); // connections() is synchronized, no need to synchronize here.
        while(p.hasNext())
        {
            try
            {
                p.next().flushBatchRequests();
            }
            catch(Ice.LocalException ex)
            {
                // Ignore.
            }
        }
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

    //
    // Operations from ConnectionI.StartCallback
    //
    public synchronized void
    connectionStartCompleted(Ice.ConnectionI connection)
    {
        //
        // Initially, connections are in the holding state. If the factory is active
        // we activate the connection.
        //
        if(_state == StateActive)
        {
            connection.activate();
        }
    }

    public synchronized void
    connectionStartFailed(Ice.ConnectionI connection, Ice.LocalException ex)
    {
        if(_state == StateClosed)
        {
            return;
        }
        
        if(_warn)
        {
            warning(ex);
        }
        
        //
        // If the connection is finished, remove it right away from
        // the connection map. Otherwise, we keep it in the map, it
        // will eventually be reaped.
        //
        if(connection.isFinished())
        {
            _connections.remove(connection);
        }
    }

    public
    IncomingConnectionFactory(Instance instance, EndpointI endpoint, Ice.ObjectAdapter adapter, String adapterName)
    {
        _instance = instance;
        _endpoint = endpoint;
        _adapter = adapter;
        
        _warn = _instance.initializationData().properties.getPropertyAsInt("Ice.Warn.Connections") > 0 ? true : false;
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

        try
        {
            EndpointIHolder h = new EndpointIHolder();
            h.value = _endpoint;
            _transceiver = _endpoint.transceiver(h);

            if(_transceiver != null)
            {
                _endpoint = h.value;
                
                Ice.ConnectionI connection;
		try
		{
		    connection = new Ice.ConnectionI(_instance, _transceiver, _endpoint, _adapter);
		}
		catch(Ice.LocalException ex)
		{
		    try
		    {
			_transceiver.close();
		    }
		    catch(Ice.LocalException exc)
		    {
			// Ignore
		    }
		    throw ex;
		}
                connection.start(null);
                
                _connections.add(connection);
            }
            else
            {
                h.value = _endpoint;
                _acceptor = _endpoint.acceptor(h, adapterName);
                _endpoint = h.value;
                assert(_acceptor != null);
                _acceptor.listen();

                _acceptThread = new Thread(new Runnable()
                {
                    public void run()
                    {
                        acceptAsync();
                    }
                });
                _acceptThread.setName("Ice Acceptor Thread: " + _acceptor.toString());
                _acceptThread.start();
            }
        }
        catch(java.lang.Exception ex)
        {
            //
            // Clean up for finalizer.
            //

            if(_acceptor != null)
            {
                try
                {
                    _acceptor.close();
                }
                catch(Ice.LocalException e)
                {
                    // Here we ignore any exceptions in close().                        
                }
            }

            synchronized(this)
            {
                _state = StateClosed;
                _acceptor = null;
                _connections = null;
            }

            if(ex instanceof Ice.LocalException)
            {
                throw (Ice.LocalException)ex;
            }
            else
            {
                Ice.SyscallException e = new Ice.SyscallException();
                e.initCause(ex);
                throw e;
            }
        }
    }
    
    private void acceptAsync()
    {
        Transceiver transceiver = null;
        while(true)
        {
            Ice.ConnectionI connection = null;
            synchronized(this)
            {
                while(_state == StateHolding)
                {
                    try
                    {
                        wait();
                    }
                    catch(InterruptedException e)
                    {
                    }
                }
    
                //
                // Nothing left to do if the factory is closed.
                //
                if(_state == StateClosed)
                {
                    // We've transitioned to Closed.
                    //
                    // Check for a transceiver that was accepted while the factory was inactive.
                    //
                    if(transceiver != null)
                    {
                        try
                        {
                            transceiver.close();
                        }
                        catch(Ice.LocalException e)
                        {
                            // Here we ignore any exceptions in close().
                        }
                    }
                    
                    //
                    // Close the acceptor.
                    //
                    _acceptor.close();
                    _acceptor = null;
                    notifyAll();

                    return;
                }
        
                if(transceiver != null)
                {
                    //
                    // Reap connections for which destruction has completed.
                    //
                    for(Iterator<Ice.ConnectionI> p = _connections.iterator(); p.hasNext();)
                    {
                        Ice.ConnectionI con = p.next();
                        if(con.isFinished())
                        {
                            p.remove();
                        }
                    }

                    //
                    // Create a new connection.
                    //
                    try
                    {
                        connection = new Ice.ConnectionI(_instance, transceiver, _endpoint, _adapter);
                        _connections.add(connection);
                    }
                    catch(Ice.LocalException ex)
                    {
                        try
                        {
                            transceiver.close();
                        }
                        catch(Ice.LocalException e2)
                        {
                            // Ignore
                        }

                        if(_warn)
                        {
                            warning(ex);
                        }
                    }
                    transceiver = null;
                }
            }
            
            if(connection != null)
            {
                connection.start(this);
                connection = null;
            }
            
            try
            {
                transceiver = _acceptor.accept();
            }
            catch(Ice.SocketException ex)
            {
                // if(Network.noMoreFds(ex.InnerException))
                // {
                // fatalError(ex.InnerException);
                // }
    
                // Ignore socket exceptions.
            }
            catch(Ice.LocalException ex)
            {
                // Warn about other Ice local exceptions.
                if(_warn)
                {
                    warning(ex);
                }
            }
            catch(RuntimeException ex)
            {
                error("acceptAsync", ex);
            }
        }
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

                java.util.ListIterator<Ice.ConnectionI> p = _connections.listIterator();
                while(p.hasNext())
                {
                    Ice.ConnectionI connection = p.next();
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

                java.util.ListIterator<Ice.ConnectionI> p = _connections.listIterator();
                while(p.hasNext())
                {
                    Ice.ConnectionI connection = p.next();
                    connection.hold();
                }
                break;
            }

            case StateClosed:
            {
                java.util.ListIterator<Ice.ConnectionI> p = _connections.listIterator();
                while(p.hasNext())
                {   
                    Ice.ConnectionI connection = p.next();
                    connection.destroy(Ice.ConnectionI.ObjectAdapterDeactivated);
                }
                break;
            }
        }

        _state = state;
        notifyAll();
    }

    private void
    warning(Ice.LocalException ex)
    {
        java.io.StringWriter sw = new java.io.StringWriter();
        java.io.PrintWriter pw = new java.io.PrintWriter(sw);
        ex.printStackTrace(pw);
        pw.flush();
        String s = "connection exception:\n" + sw.toString() + '\n' + _acceptor.toString();
        _instance.initializationData().logger.warning(s);
    }

    private void
    error(String msg, Exception ex)
    {
        java.io.StringWriter sw = new java.io.StringWriter();
        java.io.PrintWriter pw = new java.io.PrintWriter(sw);
        ex.printStackTrace(pw);
        pw.flush();
        String s = msg + ":\n" + toString() + "\n" + sw.toString();
        _instance.initializationData().logger.error(s);
    }

    private Instance _instance;
    
    private Acceptor _acceptor;
    private final Transceiver _transceiver;
    private EndpointI _endpoint;

    private Ice.ObjectAdapter _adapter;

    private final boolean _warn;
    
    private java.util.List<Ice.ConnectionI> _connections = new java.util.LinkedList<Ice.ConnectionI>();

    private int _state;
}
