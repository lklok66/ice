// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{

    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Net.Sockets;
    using System.Threading;
    using IceUtilInternal;

    public sealed class OutgoingConnectionFactory
    {
        public interface CreateConnectionCallback
        {
            void setConnection(Ice.ConnectionI connection, bool compress);
            void setException(Ice.LocalException ex);
        }

        public void destroy()
        {
            lock(this)
            {
                if(_destroyed)
                {
                    return;
                }

                foreach(LinkedList connections in _connections.Values)
                {
                    foreach(Ice.ConnectionI c in connections)
                    {
                        c.destroy(Ice.ConnectionI.CommunicatorDestroyed);
                    }
                }

                _destroyed = true;
                Monitor.PulseAll(this);
            }
        }

        public void waitUntilFinished()
        {
            Dictionary<ConnectorInfo, LinkedList> connections = null;

            lock(this)
            {
                //
                // First we wait until the factory is destroyed. We also
                // wait until there are no pending connections
                // anymore. Only then we can be sure the _connections
                // contains all connections.
                //
                while(!_destroyed || _pending.Count > 0 || _pendingConnectCount > 0)
                {
                    Monitor.Wait(this);
                }

                //
                // We want to wait until all connections are finished outside the
                // thread synchronization.
                //
                if(_connections != null)
                {
                    connections = new Dictionary<ConnectorInfo, LinkedList>(_connections);
                }
            }

            //
            // Now we wait until the destruction of each connection is finished.
            //
            foreach(LinkedList cl in connections.Values)
            {
                foreach(Ice.ConnectionI c in cl)
                {
                    c.waitUntilFinished();
                }
            }

            lock(this)
            {
                _connections = null;
                _connectionsByEndpoint = null;
            }
        }

        public Ice.ConnectionI create(EndpointI[] endpts, bool hasMore, Ice.EndpointSelectionType selType,
                                      out bool compress)
        {
            Debug.Assert(endpts.Length > 0);

            //
            // Apply the overrides.
            //
            List<EndpointI> endpoints = applyOverrides(endpts);

            //
            // Try to find a connection to one of the given endpoints.
            //
            Ice.ConnectionI connection = findConnection(endpoints, out compress);
            if(connection != null)
            {
                return connection;
            }

            Ice.LocalException exception = null;

            //
            // If we didn't find a connection with the endpoints, we create the connectors
            // for the endpoints.
            //
            List<ConnectorInfo> connectors = new List<ConnectorInfo>();
            for(int i = 0; i < endpoints.Count; ++i)
            {
                EndpointI endpoint = endpoints[i];

                try
                {
                    //
                    // Create connectors for the endpoint.
                    //
                    List<Connector> cons = endpoint.connectors();
                    Debug.Assert(cons.Count > 0);

                    //
                    // Shuffle connectors if endpoint selection type is Random.
                    //
                    if(selType == Ice.EndpointSelectionType.Random)
                    {
                        for(int j = 0; j < cons.Count - 2; ++j)
                        {
                            int r = rand_.Next(cons.Count - j) + j;
                            Debug.Assert(r >= j && r < cons.Count);
                            if(r != j)
                            {
                                Connector tmp = cons[j];
                                cons[j] = cons[r];
                                cons[r] = tmp;
                            }
                        }
                    }

                    foreach(Connector conn in cons)
                    {
                        connectors.Add(new ConnectorInfo(conn, endpoint));
                    }
                }
                catch(Ice.LocalException ex)
                {
                    exception = ex;
                    handleException(exception, hasMore || i < endpoints.Count - 1);
                }
            }

            if(connectors.Count == 0)
            {
                Debug.Assert(exception != null);
                throw exception;
            }

            //
            // Try to get a connection to one of the connectors. A null result indicates that no
            // connection was found and that we should try to establish the connection (and that
            // the connectors were added to _pending to prevent other threads from establishing
            // the connection).
            //
            connection = getConnection(connectors, null, out compress);
            if(connection != null)
            {
                return connection;
            }

            //
            // Try to establish the connection to the connectors.
            //
            DefaultsAndOverrides defaultsAndOverrides = instance_.defaultsAndOverrides();
            ConnectorInfo ci = null;
            for(int i = 0; i < connectors.Count; ++i)
            {
                ci = connectors[i];
                try
                {
                    connection = createConnection(ci.connector.connect(), ci);
                    connection.start(null);

                    if(defaultsAndOverrides.overrideCompress)
                    {
                        compress = defaultsAndOverrides.overrideCompressValue;
                    }
                    else
                    {
                        compress = ci.endpoint.compress();
                    }
                    connection.activate();
                    break;
                }
                catch(Ice.CommunicatorDestroyedException ex)
                {
                    exception = ex;
                    handleException(exception, ci, connection, hasMore || i < connectors.Count - 1);
                    connection = null;
                    break; // No need to continue
                }
                catch(Ice.LocalException ex)
                {
                    exception = ex;
                    handleException(exception, ci, connection, hasMore || i < connectors.Count - 1);
                    connection = null;
                }
            }

            //
            // Finish creating the connection (this removes the connectors from the _pending
            // list and notifies any waiting threads).
            //
            if(connection != null)
            {
                finishGetConnection(connectors, ci, connection, null);
            }
            else
            {
                finishGetConnection(connectors, exception, null);
            }

            if(connection == null)
            {
                Debug.Assert(exception != null);
                throw exception;
            }

            return connection;
        }

        public void create(EndpointI[] endpts, bool hasMore, Ice.EndpointSelectionType selType,
                           CreateConnectionCallback callback)
        {
            Debug.Assert(endpts.Length > 0);

            //
            // Apply the overrides.
            //
            List<EndpointI> endpoints = applyOverrides(endpts);

            //
            // Try to find a connection to one of the given endpoints.
            //
            try
            {
                bool compress;
                Ice.ConnectionI connection = findConnection(endpoints, out compress);
                if(connection != null)
                {
                    callback.setConnection(connection, compress);
                    return;
                }
            }
            catch(Ice.LocalException ex)
            {
                callback.setException(ex);
                return;
            }

            ConnectCallback cb = new ConnectCallback(this, endpoints, hasMore, callback, selType);
            cb.getConnectors();
        }

        public void setRouterInfo(IceInternal.RouterInfo routerInfo)
        {
            lock(this)
            {
                if(_destroyed)
                {
                    throw new Ice.CommunicatorDestroyedException();
                }

                Debug.Assert(routerInfo != null);

                //
                // Search for connections to the router's client proxy
                // endpoints, and update the object adapter for such
                // connections, so that callbacks from the router can be
                // received over such connections.
                //
                Ice.ObjectAdapter adapter = routerInfo.getAdapter();
                DefaultsAndOverrides defaultsAndOverrides = instance_.defaultsAndOverrides();
                EndpointI[] endpoints = routerInfo.getClientEndpoints();
                for(int i = 0; i < endpoints.Length; i++)
                {
                    EndpointI endpoint = endpoints[i];

                    //
                    // Modify endpoints with overrides.
                    //
                    if(defaultsAndOverrides.overrideTimeout)
                    {
                        endpoint = endpoint.timeout(defaultsAndOverrides.overrideTimeoutValue);
                    }

                    //
                    // The Ice.ConnectionI object does not take the compression flag of
                    // endpoints into account, but instead gets the information
                    // about whether messages should be compressed or not from
                    // other sources. In order to allow connection sharing for
                    // endpoints that differ in the value of the compression flag
                    // only, we always set the compression flag to false here in
                    // this connection factory.
                    //
                    endpoint = endpoint.compress(false);

                    foreach(LinkedList connections in _connections.Values)
                    {
                        foreach(Ice.ConnectionI connection in connections)
                        {
                            if(connection.endpoint().Equals(endpoint))
                            {
                                connection.setAdapter(adapter);
                            }
                        }
                    }
                }
            }
        }

        public void removeAdapter(Ice.ObjectAdapter adapter)
        {
            lock(this)
            {
                if(_destroyed)
                { 
                    return;
                }

                foreach(LinkedList connectionList in _connections.Values)
                {
                    foreach(Ice.ConnectionI connection in connectionList)
                    {
                        if(connection.getAdapter() == adapter)
                        {
                            connection.setAdapter(null);
                        }
                    }
                }
            }
        }

        public void flushBatchRequests()
        {
            LinkedList c = new LinkedList();

            lock(this)
            {
                foreach(LinkedList connectionList in _connections.Values)
                {
                    foreach(Ice.ConnectionI conn in connectionList)
                    {
                        c.Add(conn);
                    }
                }
            }

            foreach(Ice.ConnectionI conn in c)
            {
                try
                {
                    conn.flushBatchRequests();
                }
                catch(Ice.LocalException)
                {
                    // Ignore.
                }
            }
        }

        //
        // Only for use by Instance.
        //
        internal OutgoingConnectionFactory(Instance instance)
        {
            instance_ = instance;
            _destroyed = false;
            _pendingConnectCount = 0;
        }

        private List<EndpointI> applyOverrides(EndpointI[] endpts)
        {
            DefaultsAndOverrides defaultsAndOverrides = instance_.defaultsAndOverrides();
            List<EndpointI> endpoints = new List<EndpointI>();
            for(int i = 0; i < endpts.Length; i++)
            {
                //
                // Modify endpoints with overrides.
                //
                if(defaultsAndOverrides.overrideTimeout)
                {
                    endpoints.Add(endpts[i].timeout(defaultsAndOverrides.overrideTimeoutValue));
                }
                else
                {
                    endpoints.Add(endpts[i]);
                }
            }

            return endpoints;
        }

        private Ice.ConnectionI findConnection(List<EndpointI> endpoints, out bool compress)
        {
            lock(this)
            {
                if(_destroyed)
                {
                    throw new Ice.CommunicatorDestroyedException();
                }

                DefaultsAndOverrides defaultsAndOverrides = instance_.defaultsAndOverrides();
                Debug.Assert(endpoints.Count > 0);

                foreach(EndpointI endpoint in endpoints)
                {
                    LinkedList connectionList = null;
                    if(!_connectionsByEndpoint.TryGetValue(endpoint, out connectionList))
                    {
                        continue;
                    }

                    foreach(Ice.ConnectionI connection in connectionList)
                    {
                        if(connection.isActiveOrHolding()) // Don't return destroyed or unvalidated connections
                        {
                            if(defaultsAndOverrides.overrideCompress)
                            {
                                compress = defaultsAndOverrides.overrideCompressValue;
                            }
                            else
                            {
                                compress = endpoint.compress();
                            }
                            return connection;
                        }
                    }
                }

                compress = false; // Satisfy the compiler
                return null;
            }
        }

        //
        // Must be called while synchronized.
        //
        private Ice.ConnectionI findConnection(List<ConnectorInfo> connectors, out bool compress)
        {
            DefaultsAndOverrides defaultsAndOverrides = instance_.defaultsAndOverrides();
            foreach(ConnectorInfo ci in connectors)
            {
                if(_pending.ContainsKey(ci))
                {
                    continue;
                }

                LinkedList connectionList = null;
                if(!_connections.TryGetValue(ci, out connectionList))
                {
                    continue;
                }

                foreach(Ice.ConnectionI connection in connectionList)
                {
                    if(connection.isActiveOrHolding()) // Don't return destroyed or un-validated connections
                    {
                        if(!connection.endpoint().Equals(ci.endpoint))
                        {
                            LinkedList conList = null;
                            if(!_connectionsByEndpoint.TryGetValue(ci.endpoint, out conList))
                            {
                                conList = new LinkedList();
                                _connectionsByEndpoint.Add(ci.endpoint, conList);
                            }
                            conList.Add(connection);
                        }

                        if(defaultsAndOverrides.overrideCompress)
                        {
                            compress = defaultsAndOverrides.overrideCompressValue;
                        }
                        else
                        {
                            compress = ci.endpoint.compress();
                        }
                        return connection;
                    }
                }
            }

            compress = false; // Satisfy the compiler
            return null;
        }

        internal void incPendingConnectCount()
        {
            //
            // Keep track of the number of pending connects. The outgoing connection factory 
            // waitUntilFinished() method waits for all the pending connects to terminate before
            // to return. This ensures that the communicator client thread pool isn't destroyed
            // too soon and will still be available to execute the ice_exception() callbacks for
            // the asynchronous requests waiting on a connection to be established.
            //
            
            lock(this)
            {
                if(_destroyed)
                {
                    throw new Ice.CommunicatorDestroyedException();
                }
                ++_pendingConnectCount;
            }
        }

        internal void decPendingConnectCount()
        {
            lock(this)
            {
                --_pendingConnectCount;
                Debug.Assert(_pendingConnectCount >= 0);
                if(_destroyed && _pendingConnectCount == 0)
                {
                    Monitor.PulseAll(this);
                }
            }
        }

        private Ice.ConnectionI getConnection(List<ConnectorInfo> connectors, ConnectCallback cb, out bool compress)
        {
            lock(this)
            {
                if(_destroyed)
                {
                    throw new Ice.CommunicatorDestroyedException();
                }

                //
                // Reap connections for which destruction has completed.
                //
                List<ConnectorInfo> removedConnections = new List<ConnectorInfo>();
                foreach(KeyValuePair<ConnectorInfo, LinkedList> e in _connections)
                {
                    LinkedList.Enumerator q = (LinkedList.Enumerator)e.Value.GetEnumerator();
                    while(q.MoveNext())
                    {
                        Ice.ConnectionI con = (Ice.ConnectionI)q.Current;
                        if(con.isFinished())
                        {
                            q.Remove();
                        }
                    }

                    if(e.Value.Count == 0)
                    {
                        removedConnections.Add(e.Key);
                    }
                }
                foreach(ConnectorInfo ci in removedConnections)
                {
                    _connections.Remove(ci);
                }

                List<EndpointI> removedEndpoints = new List<EndpointI>();
                foreach(KeyValuePair<EndpointI, LinkedList> e in _connectionsByEndpoint)
                {
                    LinkedList.Enumerator q = (LinkedList.Enumerator)e.Value.GetEnumerator();
                    while(q.MoveNext())
                    {
                        Ice.ConnectionI con = (Ice.ConnectionI)q.Current;
                        if(con.isFinished())
                        {
                            q.Remove();
                        }
                    }

                    if(e.Value.Count == 0)
                    {
                        removedEndpoints.Add(e.Key);
                    }
                }
                foreach(EndpointI endpoint in removedEndpoints)
                {
                    _connectionsByEndpoint.Remove(endpoint);
                }

                //
                // Try to get the connection. We may need to wait for other threads to
                // finish if one of them is currently establishing a connection to one
                // of our connectors.
                //
                while(true)
                {
                    if(_destroyed)
                    {
                        throw new Ice.CommunicatorDestroyedException();
                    }

                    //
                    // Search for a matching connection. If we find one, we're done.
                    //
                    Ice.ConnectionI connection = findConnection(connectors, out compress);
                    if(connection != null)
                    {
                        return connection;
                    }

                    if(addToPending(cb, connectors))
                    {
                        //
                        // If a callback is not specified we wait until another thread notifies us about a 
                        // change to the pending list. Otherwise, if a callback is provided we're done: 
                        // when the pending list changes the callback will be notified and will try to 
                        // get the connection again.
                        //
                        if(cb == null)
                        {
                            Monitor.Wait(this);
                        }
                        else
                        {
                            return null;
                        }
                    }
                    else
                    {
                        //
                        // If no thread is currently establishing a connection to one of our connectors,
                        // we get out of this loop and start the connection establishment to one of the
                        // given connectors.
                        //
                        break;
                    }
                }
            }

            //
            // At this point, we're responsible for establishing the connection to one of 
            // the given connectors. If it's a non-blocking connect, calling nextConnector
            // will start the connection establishment. Otherwise, we return null to get
            // the caller to establish the connection.
            //
            if(cb != null)
            {
                cb.nextConnector();
            }

            compress = false; // Satisfy the compiler
            return null;
        }

        private Ice.ConnectionI createConnection(Transceiver transceiver, ConnectorInfo ci)
        {
            lock(this)
            {
                Debug.Assert(_pending.ContainsKey(ci) && transceiver != null);

                //
                // Create and add the connection to the connection map. Adding the connection to the map
                // is necessary to support the interruption of the connection initialization and validation
                // in case the communicator is destroyed.
                //
                try
                {
                    if(_destroyed)
                    {
                        throw new Ice.CommunicatorDestroyedException();
                    }

                    Ice.ConnectionI connection = new Ice.ConnectionI(instance_, transceiver,
                                                                     ci.endpoint.compress(false), null);

                    LinkedList connectionList = null;
                    if(!_connections.TryGetValue(ci, out connectionList))
                    {
                        connectionList = new LinkedList();
                        _connections.Add(ci, connectionList);
                    }
                    connectionList.Add(connection);
                    connectionList = null;
                    if(!_connectionsByEndpoint.TryGetValue(ci.endpoint, out connectionList))
                    {
                        connectionList = new LinkedList();
                        _connectionsByEndpoint.Add(ci.endpoint, connectionList);
                    }
                    connectionList.Add(connection);
                    return connection;
                }
                catch(Ice.LocalException)
                {
                    try
                    {
                        transceiver.close();
                    }
                    catch(Ice.LocalException)
                    {
                        // Ignore
                    }
                    throw;
                }
            }
        }

        private void finishGetConnection(List<ConnectorInfo> connectors, 
                                         ConnectorInfo ci, 
                                         Ice.ConnectionI connection,
                                         ConnectCallback cb)
        {
            Set connectionCallbacks = new Set();
            if(cb != null)
            {
                connectionCallbacks.Add(cb);
            }

            Set callbacks = new Set();
            lock(this)
            {
                foreach(ConnectorInfo c in connectors)
                {
                    Set s = null;
                    if(_pending.TryGetValue(c, out s))
                    {
                        foreach(ConnectCallback cc in s)
                        {
                            if(cc.hasConnector(ci))
                            {
                                connectionCallbacks.Add(cc);
                            }
                            else
                            {
                                callbacks.Add(cc);
                            }
                        }
                        _pending.Remove(c);
                    }
                }

                foreach(ConnectCallback cc in connectionCallbacks)
                {
                    cc.removeFromPending();
                    callbacks.Remove(cc);
                }
                foreach(ConnectCallback cc in callbacks)
                {
                    cc.removeFromPending();
                }
                Monitor.PulseAll(this);
            }

            bool compress;
            DefaultsAndOverrides defaultsAndOverrides = instance_.defaultsAndOverrides();
            if(defaultsAndOverrides.overrideCompress)
            {
                compress = defaultsAndOverrides.overrideCompressValue;
            }
            else
            {
                compress = ci.endpoint.compress();
            }

            foreach(ConnectCallback cc in callbacks)
            {
                cc.getConnection();
            }
            foreach(ConnectCallback cc in connectionCallbacks)
            {
                cc.setConnection(connection, compress);
            }
        }

        private void finishGetConnection(List<ConnectorInfo> connectors, Ice.LocalException ex, ConnectCallback cb)
        {
            Set failedCallbacks = new Set();
            if(cb != null)
            {
                failedCallbacks.Add(cb);
            }

            Set callbacks = new Set();
            lock(this)
            {
                foreach(ConnectorInfo c in connectors)
                {
                    Set s = null;
                    if(_pending.TryGetValue(c, out s))
                    {
                        foreach(ConnectCallback cc in s)
                        {
                            if(cc.removeConnectors(connectors))
                            {
                                failedCallbacks.Add(cc);
                            }
                            else
                            {
                                callbacks.Add(cc);
                            }
                        }
                        _pending.Remove(c);
                    }
                }

                foreach(ConnectCallback cc in callbacks)
                {
                    Debug.Assert(!failedCallbacks.Contains(cc));
                    cc.removeFromPending();
                }
                Monitor.PulseAll(this);
            }

            foreach(ConnectCallback cc in callbacks)
            {
                cc.getConnection();
            }
            foreach(ConnectCallback cc in failedCallbacks)
            {
                cc.setException(ex);
            }
        }

        private void handleException(Ice.LocalException ex, ConnectorInfo ci, Ice.ConnectionI connection,
                                     bool hasMore)
        {
            TraceLevels traceLevels = instance_.traceLevels();
            if(traceLevels.retry >= 2)
            {
                System.Text.StringBuilder s = new System.Text.StringBuilder();
                s.Append("connection to endpoint failed");
                if(ex is Ice.CommunicatorDestroyedException)
                {
                    s.Append("\n");
                }
                else
                {
                    if(hasMore)
                    {
                        s.Append(", trying next endpoint\n");
                    }
                    else
                    {
                        s.Append(" and no more endpoints to try\n");
                    }
                }
                s.Append(ex);
                instance_.initializationData().logger.trace(traceLevels.retryCat, s.ToString());
            }

            if(connection != null && connection.isFinished())
            {
                //
                // If the connection is finished, we remove it right away instead of
                // waiting for the reaping.
                //
                // NOTE: it's possible for the connection to not be finished yet. That's
                // for instance the case when using thread per connection and if it's the
                // thread which is calling back the outgoing connection factory to notify
                // it of the failure.
                //
                lock(this)
                {
                    LinkedList connectionList = null;
                    if(_connections.TryGetValue(ci, out connectionList))
                    {
                        connectionList.Remove(connection);
                        if(connectionList.Count == 0)
                        {
                            _connections.Remove(ci);
                        }
                    }

                    if(_connectionsByEndpoint.TryGetValue(ci.endpoint, out connectionList))
                    {
                        connectionList.Remove(connection);
                        if(connectionList.Count == 0)
                        {
                            _connectionsByEndpoint.Remove(ci.endpoint);
                        }
                    }
                }
            }
        }

        private bool
        addToPending(ConnectCallback cb, List<ConnectorInfo> connectors)
        {
            //
            // Add the callback to each connector pending list.
            //
            bool found = false;
            foreach(ConnectorInfo ci in connectors)
            {
                Set cbs = null;
                if(_pending.TryGetValue(ci, out cbs))
                {
                    found = true;
                    if(cb != null)
                    {
                        cbs.Add(cb); // Add the callback to each pending connector.
                    }
                }
            }

            if(found)
            {
                return true;
            }

            //
            // If there's no pending connection for the given connectors, we're 
            // responsible for its establishment. We add empty pending lists, 
            // other callbacks to the same connectors will be queued.
            //
            foreach(ConnectorInfo ci in connectors)
            {
                if(!_pending.ContainsKey(ci))
                {
                    _pending.Add(ci, new Set());
                }
            }
            return false;
        }

        private void
        removeFromPending(ConnectCallback cb, List<ConnectorInfo> connectors)
        {
            foreach(ConnectorInfo ci in connectors)
            {
                Set cbs = null;
                if(_pending.TryGetValue(ci, out cbs))
                {
                    cbs.Remove(cb);
                }
            }
        }

        internal void handleException(Ice.LocalException ex, bool hasMore)
        {
            TraceLevels traceLevels = instance_.traceLevels();
            if(traceLevels.retry >= 2)
            {
                System.Text.StringBuilder s = new System.Text.StringBuilder();
                s.Append("couldn't resolve endpoint host");
                if(ex is Ice.CommunicatorDestroyedException)
                {
                    s.Append("\n");
                }
                else
                {
                    if(hasMore)
                    {
                        s.Append(", trying next endpoint\n");
                    }
                    else
                    {
                        s.Append(" and no more endpoints to try\n");
                    }
                }
                s.Append(ex);
                instance_.initializationData().logger.trace(traceLevels.retryCat, s.ToString());
            }
        }

        private class ConnectorInfo
        {
            internal ConnectorInfo(Connector c, EndpointI e)
            {
                connector = c;
                endpoint = e;
            }

            public override bool Equals(object obj)
            {
                ConnectorInfo r = (ConnectorInfo)obj;
                return connector.Equals(r.connector);
            }

            public override int GetHashCode()
            {
                return connector.GetHashCode();
            }

            public Connector connector;
            public EndpointI endpoint;
        }

        private class ConnectCallback : Ice.ConnectionI.StartCallback, EndpointI_connectors
        {
            internal ConnectCallback(OutgoingConnectionFactory f, List<EndpointI> endpoints, bool more,
                                     CreateConnectionCallback cb, Ice.EndpointSelectionType selType)
            {
                _factory = f;
                _endpoints = endpoints;
                _hasMore = more;
                _callback = cb;
                _selType = selType;
                _endpointsIter = 0;
            }

            //
            // Methods from ConnectionI.StartCallback
            //
            public void connectionStartCompleted(Ice.ConnectionI connection)
            {
                connection.activate();
                _factory.finishGetConnection(_connectors, _current, connection, this);
            }

            public void connectionStartFailed(Ice.ConnectionI connection, Ice.LocalException ex)
            {
                _factory.handleException(ex, _current, connection, _hasMore || _iter < _connectors.Count);
                if(ex is Ice.CommunicatorDestroyedException) // No need to continue.
                {
                    _factory.finishGetConnection(_connectors, ex, this);
                }
                else if(_iter < _connectors.Count) // Try the next connector.
                {
                    nextConnector();
                }
                else
                {
                    _factory.finishGetConnection(_connectors, ex, this);
                }
            }

            //
            // Methods from EndpointI_connectors
            //
            public void connectors(List<Connector> cons)
            {
                //
                // Shuffle connectors if endpoint selection type is Random.
                //
                if(_selType == Ice.EndpointSelectionType.Random)
                {
                    for(int j = 0; j < cons.Count - 2; ++j)
                    {
                        int r = OutgoingConnectionFactory.rand_.Next(cons.Count - j) + j;
                        Debug.Assert(r >= j && r < cons.Count);
                        if(r != j)
                        {
                            Connector tmp = cons[j];
                            cons[j] = cons[r];
                            cons[r] = tmp;
                        }
                    }
                }

                foreach(Connector connector in cons)
                {
                    _connectors.Add(new ConnectorInfo(connector, _currentEndpoint));
                }

                if(_endpointsIter < _endpoints.Count)
                {
                    nextEndpoint();
                }
                else
                {
                    Debug.Assert(_connectors.Count > 0);

                    //
                    // We now have all the connectors for the given endpoints. We can try to obtain the
                    // connection.
                    //
                    _iter = 0;
                    getConnection();
                }
            }

            public void exception(Ice.LocalException ex)
            {
                _factory.handleException(ex, _hasMore || _endpointsIter < _endpoints.Count);
                if(_endpointsIter < _endpoints.Count)
                {
                    nextEndpoint();
                }
                else if(_connectors.Count > 0)
                {
                    //
                    // We now have all the connectors for the given endpoints. We can try to obtain the
                    // connection.
                    //
                    _iter = 0;
                    getConnection();
                }
                else
                {
                    _callback.setException(ex);
                    _factory.decPendingConnectCount(); // Must be called last.
                }
            }

            public void setConnection(Ice.ConnectionI connection, bool compress)
            {
                //
                // Callback from the factory: the connection to one of the callback
                // connectors has been established.
                //
                _callback.setConnection(connection, compress);
                _factory.decPendingConnectCount(); // Must be called last.
            }

            public void setException(Ice.LocalException ex)
            {
                //
                // Callback from the factory: connection establishment failed.
                //
                _callback.setException(ex);
                _factory.decPendingConnectCount(); // Must be called last.
            }

            public bool hasConnector(ConnectorInfo ci)
            {
                return _connectors.Contains(ci);
            }
            public bool removeConnectors(List<ConnectorInfo> connectors)
            {
                foreach(ConnectorInfo ci in connectors)
                {
                    while(_connectors.Remove(ci)); // Remove all of them.
                }
                return _connectors.Count == 0;
            }

            public void removeFromPending()
            {
                _factory.removeFromPending(this, _connectors);
            }

            public void getConnectors()
            {
                try
                {
                    //
                    // Notify the factory that there's an async connect pending. This is necessary
                    // to prevent the outgoing connection factory to be destroyed before all the
                    // pending asynchronous connects are finished.
                    //
                    _factory.incPendingConnectCount();
                }
                catch(Ice.LocalException ex)
                {
                    _callback.setException(ex);
                    return;
                }

                nextEndpoint();
            }

            void nextEndpoint()
            {
                try
                {
                    Debug.Assert(_endpointsIter < _endpoints.Count);
                    _currentEndpoint = _endpoints[_endpointsIter++];
                    _currentEndpoint.connectors_async(this);
                }
                catch(Ice.LocalException ex)
                {
                    exception(ex);
                }
            }

            internal void getConnection()
            {
                try
                {
                    //
                    // If all the connectors have been created, we ask the factory to get a
                    // connection.
                    //
                    bool compress;
                    Ice.ConnectionI connection = _factory.getConnection(_connectors, this, out compress);
                    if(connection == null)
                    {
                        //
                        // A null return value from getConnection indicates that the connection
                        // is being established and that everthing has been done to ensure that
                        // the callback will be notified when the connection establishment is 
                        // done.
                        // 
                        return;
                    }

                    _callback.setConnection(connection, compress);
                    _factory.decPendingConnectCount(); // Must be called last.
                }
                catch(Ice.LocalException ex)
                {
                    _callback.setException(ex);
                    _factory.decPendingConnectCount(); // Must be called last.
                }
            }

            internal void nextConnector()
            {
                Ice.ConnectionI connection = null;
                try
                {
                    Debug.Assert(_iter < _connectors.Count);
                    _current = _connectors[_iter++];
                    connection = _factory.createConnection(_current.connector.connect(), _current);
                    connection.start(this);
                }
                catch(Ice.LocalException ex)
                {
                    connectionStartFailed(connection, ex);
                }
            }

            private OutgoingConnectionFactory _factory;
            private bool _hasMore;
            private CreateConnectionCallback _callback;
            private List<EndpointI> _endpoints;
            private Ice.EndpointSelectionType _selType;
            private int _endpointsIter;
            private EndpointI _currentEndpoint;
            private List<ConnectorInfo> _connectors = new List<ConnectorInfo>();
            private int _iter;
            private ConnectorInfo _current;
        }

        private readonly Instance instance_;
        private bool _destroyed;

        private Dictionary<ConnectorInfo, LinkedList> _connections = new Dictionary<ConnectorInfo, LinkedList>();
        private Dictionary<EndpointI, LinkedList> _connectionsByEndpoint = new Dictionary<EndpointI, LinkedList>();
        private Dictionary<ConnectorInfo, Set> _pending = new Dictionary<ConnectorInfo, Set>();
        private int _pendingConnectCount;

        private static System.Random rand_ = new System.Random(unchecked((int)System.DateTime.Now.Ticks));
    }

    public sealed class IncomingConnectionFactory : Ice.ConnectionI.StartCallback
    {
        public void activate()
        {
            lock(this)
            {
                setState(StateActive);
            }
        }

        public void hold()
        {
            lock(this)
            {
                setState(StateHolding);
            }
        }

        public void destroy()
        {
            lock(this)
            {
                setState(StateClosed);
            }
        }

        public void waitUntilHolding()
        {
            LinkedList connections;

            lock(this)
            {
                //
                // First we wait until the connection factory itself is in
                // holding state.
                //
                while(_state < StateHolding)
                {
                    Monitor.Wait(this);
                }

                //
                // We want to wait until all connections are in holding state
                // outside the thread synchronization.
                //
                connections = (LinkedList)_connections.Clone();
            }

            //
            // Now we wait until each connection is in holding state.
            //
            foreach(Ice.ConnectionI connection in connections)
            {
                connection.waitUntilHolding();
            }
        }

        public void waitUntilFinished()
        {
            LinkedList connections = null;

            lock(this)
            {
                //
                // First we wait until the factory is destroyed. If we are using
                // an acceptor, we also wait for it to be closed.
                //
                while(_state != StateClosed || _acceptor != null)
                {
                    Monitor.Wait(this);
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
                    connections = new LinkedList(_connections);
                }
            }

            if(connections != null)
            {
                foreach(Ice.ConnectionI connection in connections)
                {
                    connection.waitUntilFinished();
                }
            }

            lock(this)
            {
                _connections = null;
            }
        }

        public EndpointI endpoint()
        {
            // No mutex protection necessary, _endpoint is immutable.
            return _endpoint;
        }

        public LinkedList connections()
        {
            lock(this)
            {
                LinkedList connections = new LinkedList();

                //
                // Only copy connections which have not been destroyed.
                //
                foreach(Ice.ConnectionI connection in _connections)
                {
                    if(connection.isActiveOrHolding())
                    {
                        connections.Add(connection);
                    }
                }

                return connections;
            }
        }

        public void flushBatchRequests()
        {
            //
            // connections() is synchronized, no need to synchronize here.
            //
            foreach(Ice.ConnectionI connection in connections())
            {
                try
                {
                    connection.flushBatchRequests();
                }
                catch(Ice.LocalException)
                {
                    // Ignore.
                }
            }
        }

        public override string ToString()
        {
            if(_transceiver != null)
            {
                return _transceiver.ToString();
            }

            Debug.Assert(_acceptor != null);
            return _acceptor.ToString();
        }

        //
        // Operations from ConnectionI.StartCallback
        //
        public void connectionStartCompleted(Ice.ConnectionI connection)
        {
            lock(this)
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
        }

        public void connectionStartFailed(Ice.ConnectionI connection, Ice.LocalException ex)
        {
            lock(this)
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
                    _connections.Remove(connection);
                }
            }
        }

        public IncomingConnectionFactory(Instance instance, EndpointI endpoint, Ice.ObjectAdapter adapter,
                                         string adapterName)
        {
            _instance = instance;
            _endpoint = endpoint;
            _adapter = adapter;
            _pendingTransceiver = null;
            _accepting = false;

            _warn = 
                _instance.initializationData().properties.getPropertyAsInt("Ice.Warn.Connections") > 0 ? true : false;
            _connections = new LinkedList();
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
                EndpointI h = _endpoint;
                _transceiver = _endpoint.transceiver(ref h);

                if(_transceiver != null)
                {
                    _endpoint = h;

                    Ice.ConnectionI connection = null;
                    try
                    {
                        connection = new Ice.ConnectionI(_instance, _transceiver, _endpoint, _adapter);
                    }
                    catch(Ice.LocalException)
                    {
                        try
                        {
                            _transceiver.close();
                        }
                        catch(Ice.LocalException)
                        {
                            // Ignore
                        }
                        throw;
                    }
                    connection.start(null);

                    _connections.Add(connection);
                }
                else
                {
                    h = _endpoint;
                    _acceptor = _endpoint.acceptor(ref h, adapterName);
                    _endpoint = h;
                    Debug.Assert(_acceptor != null);
                    _acceptor.listen();
                }
            }
            catch(System.Exception ex)
            {
                //
                // Clean up.
                //

                if(_acceptor != null)
                {
                    try
                    {
                        _acceptor.close();
                    }
                    catch(Ice.LocalException)
                    {
                        // Here we ignore any exceptions in close().                    
                    }
                }

                lock(this)
                {
                    _state = StateClosed;
                    _acceptor = null;
                    _connections = null;
                }

                if(ex is Ice.LocalException)
                {
                    throw;
                }
                else
                {
                    throw new Ice.SyscallException(ex);
                }
            }
        }

        private void acceptAsync(object state)
        {
            //
            // This method is responsible for accepting incoming connections. It ensures that an accept
            // is always pending until the factory is held or closed.
            //
            // Usually this method is invoked as an AsyncCallback, i.e., when an asynchronous I/O
            // operation completes. It can also be invoked via the QueueUserWorkItem method in the
            // .NET thread pool.
            //

            //
            // Return immediately if called as the result of an accept operation completing synchronously.
            //
            IAsyncResult result = (IAsyncResult)state;
            if(result != null && result.CompletedSynchronously)
            {
                return;
            }

            Ice.ConnectionI connection = null;

            lock(this)
            {
                Debug.Assert(_accepting);

                //
                // Nothing left to do if the factory is closed.
                //
                if(_state == StateClosed)
                {
                    _accepting = false;
                    return;
                }

                Debug.Assert(_acceptor != null);

                //
                // Finish accepting a new connection if necessary.
                //
                Transceiver transceiver = null;
                if(result != null)
                {
                    try
                    {
                        transceiver = _acceptor.endAccept(result); // Does not block.
                    }
                    catch(Ice.SocketException ex)
                    {
                        if(Network.noMoreFds(ex.InnerException))
                        {
                            fatalError(ex.InnerException);
                        }

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
                }

                if(_state == StateHolding)
                {
                    //
                    // In the holding state, we need to store the pending transceiver. We'll process it later
                    // if the factory becomes active again.
                    //
                    if(transceiver != null)
                    {
                        Debug.Assert(_pendingTransceiver == null);
                        _pendingTransceiver = transceiver;
                    }
                    _accepting = false;
                    return;
                }

                Debug.Assert(_state == StateActive);

                //
                // Check for a pending transceiver.
                //
                if(transceiver == null && _pendingTransceiver != null)
                {
                    transceiver = _pendingTransceiver;
                    _pendingTransceiver = null;
                }

                if(transceiver != null)
                {
                    //
                    // Reap connections for which destruction has completed.
                    //
                    LinkedList.Enumerator p = (LinkedList.Enumerator)_connections.GetEnumerator();
                    while(p.MoveNext())
                    {
                        Ice.ConnectionI con = (Ice.ConnectionI)p.Current;
                        if(con.isFinished())
                        {
                            p.Remove();
                        }
                    }

                    //
                    // Create a new connection.
                    //
                    try
                    {
                        connection = new Ice.ConnectionI(_instance, transceiver, _endpoint, _adapter);
                        _connections.Add(connection);
                    }
                    catch(Ice.LocalException ex)
                    {
                        try
                        {
                            transceiver.close();
                        }
                        catch(Ice.LocalException)
                        {
                            // Ignore
                        }

                        if(_warn)
                        {
                            warning(ex);
                        }
                    }
                }

                //
                // Start another accept.
                //
                try
                {
                    result = _acceptor.beginAccept(new AsyncCallback(acceptAsync), null);

                    //
                    // If the accept completes synchronously, we'll save the pending transceiver and
                    // schedule a work item to invoke this method again.
                    //
                    // If the accept requires a callback, there is nothing else to do; this method will
                    // be called when the accept completes.
                    //
                    if(result.CompletedSynchronously)
                    {
                        try
                        {
                            Debug.Assert(_pendingTransceiver == null);
                            _pendingTransceiver = _acceptor.endAccept(result); // Does not block.
                        }
                        catch(Ice.SocketException ex)
                        {
                            if(Network.noMoreFds(ex.InnerException))
                            {
                                fatalError(ex.InnerException);
                            }                
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
                        
                        _instance.asyncIOThread().queue(acceptAsync);
                    }
                }
                catch(Ice.SocketException ex)
                {
                    if(Network.noMoreFds(ex.InnerException))
                    {
                        fatalError(ex.InnerException);
                    }

                    //
                    // Ignore socket exceptions and start another accept.
                    //
                    _instance.asyncIOThread().queue(acceptAsync);
                }
                catch(Ice.LocalException ex)
                {
                    //
                    // Warn about other Ice local exceptions.
                    //
                    if(_warn)
                    {
                        warning(ex);
                    }

                    //
                    // Start another accept.
                    //
                    _instance.asyncIOThread().queue(acceptAsync);
                }
            }

            if(connection != null)
            {
                connection.start(this);
            }
        }

        private const int StateActive = 0;
        private const int StateHolding = 1;
        private const int StateClosed = 2;

        private void setState(int state)
        {
            if(_state == state) // Don't switch twice.
            {
                return;
            }

            switch (state)
            {
                case StateActive: 
                {
                    if(_state != StateHolding) // Can only switch from holding to active.
                    {
                        return;
                    }
                    if(_acceptor != null)
                    {
                        //
                        // Schedule a callback to begin accepting connections.
                        //
                        if(!_accepting)
                        {
                            _accepting = true;
                            _instance.asyncIOThread().queue(acceptAsync);
                        }
                    }

                    foreach(Ice.ConnectionI connection in _connections)
                    {
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

                    foreach(Ice.ConnectionI connection in _connections)
                    {
                        connection.hold();
                    }
                    break;
                }

                case StateClosed: 
                {
                    if(_acceptor != null)
                    {
                        //
                        // Check for a transceiver that was accepted while the factory was inactive.
                        //
                        if(_pendingTransceiver != null)
                        {
                            try
                            {
                                _pendingTransceiver.close();
                            }
                            catch(Ice.LocalException)
                            {
                                // Here we ignore any exceptions in close().
                            }
                            _pendingTransceiver = null;
                        }
                        
                        //
                        // Close the acceptor.
                        //
                        _acceptor.close();
                        _acceptor = null;
                    }
                
                    foreach(Ice.ConnectionI connection in _connections)
                    {
                        connection.destroy(Ice.ConnectionI.ObjectAdapterDeactivated);
                    }
                    break;
                }
            }

            _state = state;
            Monitor.PulseAll(this);
        }

        private void warning(Ice.LocalException ex)
        {
            _instance.initializationData().logger.warning("connection exception:\n" + ex + '\n' +
                                                          _acceptor.ToString());
        }

        private void fatalError(System.Exception ex)
        {
            string s = "fatal error: can't accept more connections:\n" + ex +'\n' + _acceptor.ToString();
            _instance.initializationData().logger.error(s);
            System.Environment.FailFast(s);
        }

        private Instance _instance;

        private Acceptor _acceptor;
        private readonly Transceiver _transceiver;
        private EndpointI _endpoint;

        private Ice.ObjectAdapter _adapter;

        private Transceiver _pendingTransceiver; // A transceiver that was accepted while the factory was inactive.
        private bool _accepting; // True if the factory is actively accepting new connections.

        private readonly bool _warn;

        private LinkedList _connections;

        private int _state;
    }

}
