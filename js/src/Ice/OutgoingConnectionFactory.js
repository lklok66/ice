// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var ArrayUtil = require("./ArrayUtil");
var ConnectionI = require("./ConnectionI");
var Debug = require("./Debug");
var Ex = require("./Exception");
var ExUtil = require("./ExUtil");
var HashMap = require("./HashMap");

var Endpt = require("./EndpointTypes").Ice;
var LocalEx = require("./LocalException").Ice;

//
// Only for use by Instance.
//
var OutgoingConnectionFactory = function(communicator, instance)
{
    this._communicator = communicator;
    this._instance = instance;
    this._destroyed = false;

    // TODO
    //this._reaper = new ConnectionReaper();

    this._connections = new ConnectionListMap(); // map<Connector, Vector<Ice.ConnectionI>>
    this._connections.comparator = HashMap.compareEquals;
    this._connectionsByEndpoint = new ConnectionListMap(); // map<EndpointI, Vector<Ice.ConnectionI>>
    this._connectionsByEndpoint.comparator = HashMap.compareEquals;
    this._pending = new HashMap(); // map<Connector, HashMap<ConnectCallback, 1>>
    this._pending.comparator = HashMap.compareEquals;
    this._pendingConnectCount = 0;

    this._waitComplete = null;
    this._waitResult = null;
}

OutgoingConnectionFactory.prototype.destroy = function()
{
    if(this._destroyed)
    {
        return;
    }

    for(var e = this._connections.entries; e != null; e = e.next)
    {
        var connectionList = e.value;
        for(var i = 0; i < connectionList.length; ++i)
        {
            connectionList[i].destroy(ConnectionI.CommunicatorDestroyed);
        }
    }

    this._destroyed = true;
    this._communicator = null;
    this.checkFinished();
}

OutgoingConnectionFactory.prototype.waitUntilFinished = function(complete, ar)
{
    this._waitComplete = complete;
    this._waitResult = ar;
    this.checkFinished();
}

OutgoingConnectionFactory.prototype.create = function(
    endpts,
    hasMore,
    selType,
    successCallback,    // function(connection, compress)
    exceptionCallback,  // function(ex)
    cbContext)
{
    Debug.assert(endpts.length > 0);

    //
    // Apply the overrides.
    //
    var endpoints = this.applyOverrides(endpts);

    //
    // Try to find a connection to one of the given endpoints.
    //
    try
    {
        var compress = { value: false };
        var connection = this.findConnectionByEndpoint(endpoints, compress);
        if(connection !== null)
        {
            successCallback.call(cbContext === undefined ? successCallback : cbContext, connection, compress.value);
            return;
        }
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            exceptionCallback.call(cbContext === undefined ? exceptionCallback : cbContext, ex);
            return;
        }
        else
        {
            throw ex;
        }
    }

    var cb = new ConnectCallback(this, endpoints, hasMore, selType, successCallback, exceptionCallback, cbContext);
    cb.getConnectors();
}

OutgoingConnectionFactory.prototype.setRouterInfo = function(
    routerInfo,
    successCallback,    // function()
    exceptionCallback,  // function(ex)
    cbContext)
{
    if(this._destroyed)
    {
        exceptionCallback.call(cbContext === undefined ? exceptionCallback : cbContext,
                               new LocalEx.CommunicatorDestroyedException());
        return;
    }

    Debug.assert(routerInfo !== null);

    var self = this;
    routerInfo.getClientEndpoints(
        function(endpoints)
        {
            self.gotClientEndpoints(endpoints, routerInfo, successCallback, cbContext);
        },
        exceptionCallback, cbContext);
}

OutgoingConnectionFactory.prototype.gotClientEndpoints = function(endpoints, routerInfo, successCallback, cbContext)
{
    //
    // Search for connections to the router's client proxy
    // endpoints, and update the object adapter for such
    // connections, so that callbacks from the router can be
    // received over such connections.
    //
    var adapter = routerInfo.getAdapter();
    var defaultsAndOverrides = this._instance.defaultsAndOverrides();
    for(var i = 0; i < endpoints.length; ++i)
    {
        var endpoint = endpoints[i];

        //
        // Modify endpoints with overrides.
        //
        if(defaultsAndOverrides.overrideTimeout)
        {
            endpoint = endpoint.changeTimeout(defaultsAndOverrides.overrideTimeoutValue);
        }

        //
        // The Connection object does not take the compression flag of
        // endpoints into account, but instead gets the information
        // about whether messages should be compressed or not from
        // other sources. In order to allow connection sharing for
        // endpoints that differ in the value of the compression flag
        // only, we always set the compression flag to false here in
        // this connection factory.
        //
        endpoint = endpoint.changeCompress(false);

        for(var e = this._connections.entries; e != null; e = e.next)
        {
            var connectionList = e.value;
            for(var i = 0; i < connectionList.length; ++i)
            {
                if(connectionList[i].endpoint().equals(endpoint))
                {
                    connectionList[i].setAdapter(adapter);
                }
            }
        }
    }

    successCallback.call(cbContext === undefined ? successCallback : cbContext);
}

OutgoingConnectionFactory.prototype.removeAdapter = function(adapter)
{
    if(this._destroyed)
    {
        return;
    }

    for(var e = this._connections.entries; e != null; e = e.next)
    {
        var connectionList = e.value;
        for(var i = 0; i < connectionList.length; ++i)
        {
            if(connectionList[i].getAdapter() === adapter)
            {
                connectionList[i].setAdapter(null);
            }
        }
    }
}

OutgoingConnectionFactory.prototype.flushAsyncBatchRequests = function(outAsync)
{
    var c = [];

    if(!this._destroyed)
    {
        for(var e = this._connections.entries; e != null; e = e.next)
        {
            var connectionList = e.value;
            for(var i = 0; i < connectionList.length; ++i)
            {
                if(connectionList[i].isActiveOrHolding())
                {
                    c.push(connectionList[i]);
                }
            }
        }
    }

    for(var i = 0; i < c.length; ++i)
    {
        try
        {
            outAsync.flushConnection(c[i]);
        }
        catch(ex)
        {
            if(ex instanceof Ex.LocalException)
            {
                // Ignore.
            }
            else
            {
                throw ex;
            }
        }
    }
}

OutgoingConnectionFactory.prototype.applyOverrides = function(endpts)
{
    var defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    var endpoints = [];
    for(var i = 0; i < endpts.length; ++i)
    {
        var endpoint = endpts[i];

        //
        // Modify endpoints with overrides.
        //
        if(defaultsAndOverrides.overrideTimeout)
        {
            endpoints.push(endpoint.changeTimeout(defaultsAndOverrides.overrideTimeoutValue));
        }
        else
        {
            endpoints.push(endpoint);
        }
    }

    return endpoints;
}

OutgoingConnectionFactory.prototype.findConnectionByEndpoint = function(endpoints, compress)
{
    if(this._destroyed)
    {
        throw new LocalEx.CommunicatorDestroyedException();
    }

    var defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    Debug.assert(endpoints.length > 0);

    for(var i = 0; i < endpoints.length; ++i)
    {
        var endpoint = endpoints[i];
        var connectionList = this._connectionsByEndpoint.get(endpoint);
        if(connectionList === undefined)
        {
            continue;
        }

        for(var i = 0; i < connectionList.length; ++i)
        {
            if(connectionList[i].isActiveOrHolding()) // Don't return destroyed or un-validated connections
            {
                if(defaultsAndOverrides.overrideCompress)
                {
                    compress.value = defaultsAndOverrides.overrideCompressValue;
                }
                else
                {
                    compress.value = endpoint.compress();
                }
                return connectionList[i];
            }
        }
    }

    return null;
}

OutgoingConnectionFactory.prototype.findConnection = function(connectors, compress)
{
    var defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    for(var i = 0; i < connectors.length; ++i)
    {
        var ci = connectors[i];

        if(this._pending.has(ci.connector))
        {
            continue;
        }

        var connectionList = this._connections.get(ci.connector);
        if(connectionList === undefined)
        {
            continue;
        }

        for(var i = 0; i < connectionList.length; ++i)
        {
            if(connectionList[i].isActiveOrHolding()) // Don't return destroyed or un-validated connections
            {
                if(defaultsAndOverrides.overrideCompress)
                {
                    compress.value = defaultsAndOverrides.overrideCompressValue;
                }
                else
                {
                    compress.value = ci.endpoint.compress();
                }
                return connectionList[i];
            }
        }
    }

    return null;
}

OutgoingConnectionFactory.prototype.incPendingConnectCount = function()
{
    //
    // Keep track of the number of pending connects. The outgoing connection factory
    // waitUntilFinished() method waits for all the pending connects to terminate before
    // to return. This ensures that the communicator client thread pool isn't destroyed
    // too soon and will still be available to execute the ice_exception() callbacks for
    // the asynchronous requests waiting on a connection to be established.
    //

    if(this._destroyed)
    {
        throw new LocalEx.CommunicatorDestroyedException();
    }
    ++this._pendingConnectCount;
}

OutgoingConnectionFactory.prototype.decPendingConnectCount = function()
{
    --this._pendingConnectCount;
    Debug.assert(this._pendingConnectCount >= 0);
    if(this._destroyed && this._pendingConnectCount === 0)
    {
        this.checkFinished();
    }
}

OutgoingConnectionFactory.prototype.getConnection = function(connectors, cb, compress)
{
    if(this._destroyed)
    {
        throw new LocalEx.CommunicatorDestroyedException();
    }

    //
    // Reap closed connections
    //
    var cons = this._reaper.swapConnections();
    if(cons !== null)
    {
        for(var i = 0; i < cons.length; ++i)
        {
            var c = cons[i];
            this._connections.removeConnection(c.connector(), c);
            this._connectionsByEndpoint.removeConnection(c.endpoint(), c);
            this._connectionsByEndpoint.removeConnection(c.endpoint().changeCompress(true), c);
        }
    }

    //
    // Try to get the connection.
    //
    while(true)
    {
        if(this._destroyed)
        {
            throw new LocalEx.CommunicatorDestroyedException();
        }

        //
        // Search for a matching connection. If we find one, we're done.
        //
        var connection = this.findConnection(connectors, compress);
        if(connection !== null)
        {
            return connection;
        }

        if(this.addToPending(cb, connectors))
        {
            //
            // A connection is already pending.
            //
            return null;
        }
        else
        {
            //
            // No connection is currently pending to one of our connectors, so we
            // get out of this loop and start the connection establishment to one of the
            // given connectors.
            //
            break;
        }
    }

    //
    // At this point, we're responsible for establishing the connection to one of
    // the given connectors. If it's a non-blocking connect, calling nextConnector
    // will start the connection establishment. Otherwise, we return null to get
    // the caller to establish the connection.
    //
    cb.nextConnector();

    return null;
}

OutgoingConnectionFactory.prototype.createConnection = function(transceiver, ci)
{
    Debug.assert(this._pending.has(ci.connector) && transceiver !== null); }

    //
    // Create and add the connection to the connection map. Adding the connection to the map
    // is necessary to support the interruption of the connection initialization and validation
    // in case the communicator is destroyed.
    //
    var connection = null;
    try
    {
        if(this._destroyed)
        {
            throw new LocalEx.CommunicatorDestroyedException();
        }

        connection = new ConnectionI(this._communicator, this._instance, this._reaper, transceiver, ci.connector,
                                     ci.endpoint.changeCompress(false), null);
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            try
            {
                transceiver.close();
            }
            catch(exc)
            {
                // Ignore
            }
        }
        throw ex;
    }

    this._connections.set(ci.connector, connection);
    this._connectionsByEndpoint.set(connection.endpoint(), connection);
    this._connectionsByEndpoint.set(connection.endpoint().changeCompress(true), connection);
    return connection;
}

OutgoingConnectionFactory.prototype.finishGetConnection = function(connectors, ci, connection, cb)
{
    // cb is-a ConnectCallback

    var connectionCallbacks = new HashMap(); // connectionCallbacks is a hash set
    if(cb !== null)
    {
        connectionCallbacks.set(cb, 1);
    }

    var callbacks = new HashMap(); // callbacks is a hash set
    for(var i = 0; i < connectors.length; ++i)
    {
        var c = connectors[i];
        var cbs = this._pending.get(c.connector);
        if(cbs !== undefined)
        {
            this._pending.delete(c.connector);
            for(var e = cbs.entries; e != null; e = e.next)
            {
                var cc = e.key;
                if(cc.hasConnector(ci))
                {
                    connectionCallbacks.set(cc, 1);
                }
                else
                {
                    callbacks.set(cc, 1);
                }
            }
        }
    }

    for(var e = connectionCallbacks.entries; e != null; e = e.next)
    {
        var cc = e.key;
        cc.removeFromPending();
        callbacks.delete(cc);
    }
    for(var e = callbacks.entries; e != null; e = e.next)
    {
        var cc = e.key;
        cc.removeFromPending();
    }

    var compress;
    var defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    if(defaultsAndOverrides.overrideCompress)
    {
        compress = defaultsAndOverrides.overrideCompressValue;
    }
    else
    {
        compress = ci.endpoint.compress();
    }

    for(var e = callbacks.entries; e != null; e = e.next)
    {
        var cc = e.key;
        cc.getConnection();
    }
    for(var e = connectionCallbacks.entries; e != null; e = e.next)
    {
        var cc = e.key;
        cc.setConnection(connection, compress);
    }

    this.checkFinished();
}

OutgoingConnectionFactory.prototype.finishGetConnectionEx = function(connectors, ex, cb)
{
    // cb is-a ConnectCallback

    var failedCallbacks = new HashMap(); // failedCallbacks is a hash set
    if(cb !== null)
    {
        failedCallbacks.set(cb, 1);
    }

    var callbacks = new HashMap(); // callbacks is a hash set
    for(var i = 0; i < connectors.length; ++i)
    {
        var c = connectors[i];
        var cbs = this._pending.get(c.connector);
        if(cbs !== undefined)
        {
            this._pending.delete(c.connector);
            for(var e = cbs.entries; e != null; e = e.next)
            {
                var cc = e.key;
                if(cc.removeConnectors(connectors))
                {
                    failedCallbacks.set(cc, 1);
                }
                else
                {
                    callbacks.set(cc, 1);
                }
            }
        }
    }

    for(var e = callbacks.entries; e != null; e = e.next)
    {
        var cc = e.key;
        Debug.assert(!failedCallbacks.has(cc));
        cc.removeFromPending();
    }
    this.checkFinished();

    for(var e = callbacks.entries; e != null; e = e.next)
    {
        var cc = e.key;
        cc.getConnection();
    }
    for(var e = failedCallbacks.entries; e != null; e = e.next)
    {
        var cc = e.key;
        cc.setException(ex);
    }
}

OutgoingConnectionFactory.prototype.addToPending = function(cb, connectors)
{
    // cb is-a ConnectCallback

    //
    // Add the callback to each connector pending list.
    //
    var found = false;
    for(var i = 0; i < connectors.length; ++i)
    {
        var p = connectors[i];
        var cbs = this._pending.get(p.connector);
        if(cbs !== undefined)
        {
            found = true;
            if(cb !== null)
            {
                cbs.set(cb, 1); // Add the callback to each pending connector.
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
    for(var i = 0; i < connectors.length; ++i)
    {
        var p = connectors[i];
        if(!this._pending.has(p.connector))
        {
            this._pending.set(p.connector, new HashMap());
        }
    }

    return false;
}

OutgoingConnectionFactory.prototype.removeFromPending = function(cb, connectors)
{
    // cb is-a ConnectCallback

    for(var i = 0; i < connectors.length; ++i)
    {
        var p = connectors[i];
        var cbs = this._pending.get(p.connector);
        if(cbs !== undefined)
        {
            cbs.delete(cb);
        }
    }
}

OutgoingConnectionFactory.prototype.handleConnectionException = function(ex, hasMore)
{
    var traceLevels = this._instance.traceLevels();
    if(traceLevels.retry >= 2)
    {
        var s = [];
        s.push("connection to endpoint failed");
        if(ex instanceof LocalEx.CommunicatorDestroyedException)
        {
            s.push("\n");
        }
        else
        {
            if(hasMore)
            {
                s.push(", trying next endpoint\n");
            }
            else
            {
                s.push(" and no more endpoints to try\n");
            }
        }
        s.push(ExUtil.toString(ex));
        this._instance.initializationData().logger.trace(traceLevels.retryCat, s.join(""));
    }
}

OutgoingConnectionFactory.prototype.handleException = function(ex, hasMore)
{
    var traceLevels:TraceLevels = this._instance.traceLevels();
    if(traceLevels.retry >= 2)
    {
        var s = [];
        s.push("couldn't resolve endpoint host");
        if(ex instanceof LocalEx.CommunicatorDestroyedException)
        {
            s.push("\n");
        }
        else
        {
            if(hasMore)
            {
                s.push(", trying next endpoint\n");
            }
            else
            {
                s.push(" and no more endpoints to try\n");
            }
        }
        s.push(ExUtil.toString(ex));
        this._instance.initializationData().logger.trace(traceLevels.retryCat, s.join(""));
    }
}

OutgoingConnectionFactory.prototype.checkFinished = function()
{
    //
    // Can't continue until the factory is destroyed and there are no pending connections.
    //
    if(this._waitComplete === null || !this._destroyed || this._pending.size > 0 || this._pendingConnectCount > 0)
    {
        return;
    }

    //
    // Count the number of connections.
    //
    var size = 0;
    for(var e = this._connections.entries; e != null; e = e.next)
    {
        var connectionList = e.value;
        size += connectionList.length;
    }

    //
    // Now we wait until the destruction of each connection is finished.
    //
    if(size > 0)
    {
        var cb = new ConnectionsFinished(size, this.connectionsFinished, this);
        for(var e = this._connections.entries; e != null; e = e.next)
        {
            var connectionList = e.value;
            for(var i = 0; i < connectionList.length; ++i)
            {
                connectionList[i].waitUntilFinished(cb.finished, cb);
            }
        }
    }
    else
    {
        this.connectionsFinished();
    }
}

OutgoingConnectionFactory.prototype.connectionsFinished = function()
{
    // Ensure all the connections are finished and reapable at this point.
    var cons = this._reaper.swapConnections();
    if(cons !== null)
    {
        var size = 0;
        for(var e = this._connections.entries; e != null; e = e.next)
        {
            var connectionList = e.value;
            size += connectionList.length;
        }
        Debug.assert(cons.length == size);
        this._connections.clear();
        this._connectionsByEndpoint.clear();
    }
    else
    {
        Debug.assert(this._connections.size == 0);
        Debug.assert(this._connectionsByEndpoint.size == 0);
    }

    Debug.assert(this._waitComplete !== null);
    this._waitComplete(this._waitResult); // TODO: Call context? Or use promise?
}

modules.export = OutgoingConnectionFactory;

//
// Value is a Vector<Ice.ConnectionI>
//
var ConnectionListMap = function(h)
{
    HashMap.call(this, h);
}

ConnectionListMap.prototype = new HashMap();
ConnectionListMap.prototype.constructor = ConnectionListMap;

ConnectionListMap.prototype.set = function(key, value)
{
    var list = this.get(key);
    if(list === undefined)
    {
        list = [];
        HashMap.prototype.set.call(this, key, list);
    }
    Debug.assert(value instanceof ConnectionI);
    list.push(value);
    return undefined;
}

ConnectionListMap.prototype.removeConnection = function(key, conn)
{
    var list = this.get(key);
    Debug.assert(list !== null);
    var idx = list.indexOf(conn);
    Debug.assert(idx != -1);
    list.splice(idx, 1);
    if(list.length === 0)
    {
        this.delete(key);
    }
}

var ConnectorInfo = function(c, e)
{
    this.connector = c;
    this.endpoint = e;
}

ConnectorInfo.prototype.equals = function(rhs)
{
    return this.connector.equals(rhs.connector);
}

ConnectorInfo.prototype.hashCode = function()
{
    return this.connector.hashCode();
}

var ConnectCallback = function(
    f,
    endpoints,
    more,
    selType,
    successCallback,    // function(connection, compress)
    exceptionCallback,  // function(ex)
    cbContext)
{
    this._factory = f;
    this._endpoints = endpoints;
    this._hasMore = more;
    this._selType = selType;
    this._successCallback = successCallback;
    this._exceptionCallback = exceptionCallback;
    this._cbContext = cbContext;
    this._endpointsIndex = 0;
    this._index = 0;
    this._currentEndpoint = null;
    this._connectors = [];
    this._current = null;
}

//
// Methods from ConnectionI_StartCallback
//
ConnectCallback.prototype.connectionStartCompleted = function(connection)
{
    connection.activate();
    this._factory.finishGetConnection(this._connectors, this._current, connection, this);
}

ConnectCallback.prototype.connectionStartFailed = function(connection, ex)
{
    Debug.assert(this._current != null);

    this._factory.handleConnectionException(ex, this._hasMore || this._index < this._connectors.length);
    if(ex instanceof LocalEx.CommunicatorDestroyedException) // No need to continue.
    {
        this._factory.finishGetConnectionEx(this._connectors, ex, this);
    }
    else if(this._index < this._connectors.length) // Try the next connector.
    {
        this.nextConnector();
    }
    else
    {
        this._factory.finishGetConnectionEx(this._connectors, ex, this);
    }
}

//
// Methods from EndpointI_connectors
//
ConnectCallback.prototype.connectors = function(cons)
{
    //
    // Shuffle connectors if endpoint selection type is Random.
    //
    if(this._selType === Endpt.EndpointSelectionType.Random)
    {
        ArrayUtil.shuffle(cons);
    }

    for(var i = 0; i < cons.length; ++i)
    {
        this._connectors.push(new ConnectorInfo(cons[i], this._currentEndpoint));
    }

    if(this._endpointsIndex < this._endpoints.length)
    {
        this.nextEndpoint();
    }
    else
    {
        Debug.assert(this._connectors.length > 0);

        //
        // We now have all the connectors for the given endpoints. We can try to obtain the
        // connection.
        //
        this._index = 0;
        this.getConnection();
    }
}

ConnectCallback.prototype.exception = function(ex)
{
    this._factory.handleException(ex, this._hasMore || this._endpointsIndex < this._endpoints.length);
    if(this._endpointsIndex < this._endpoints.length)
    {
        this.nextEndpoint();
    }
    else if(this._connectors.length > 0)
    {
        //
        // We now have all the connectors for the given endpoints. We can try to obtain the
        // connection.
        //
        this._index = 0;
        this.getConnection();
    }
    else
    {
        this._exceptionCallback.call(this._cbContext === undefined ? this._exceptionCallback : this._cbContext, ex);
        this._factory.decPendingConnectCount(); // Must be called last.
    }
}

ConnectCallback.prototype.setConnection = function(connection, compress)
{
    //
    // Callback from the factory: the connection to one of the callback
    // connectors has been established.
    //
    this._successCallback.call(this._cbContext === undefined ? this._successCallback : this._cbContext, connection,
                               compress);
    this._factory.decPendingConnectCount(); // Must be called last.
}

ConnectCallback.prototype.setException = function(ex)
{
    //
    // Callback from the factory: connection establishment failed.
    //
    this._exceptionCallback.call(this._cbContext === undefined ? this._exceptionCallback : this._cbContext, ex);
    this._factory.decPendingConnectCount(); // Must be called last.
}

ConnectCallback.prototype.hasConnector = function(ci)
{
    return this.findConnectorInfo(ci) != -1;
}

ConnectCallback.prototype.findConnectorInfo = function(ci)
{
    for(var index = 0; index < this._connectors.length; ++index)
    {
        if(ci.equals(this._connectors[index]))
        {
            return index;
        }
    }
    return -1;
}

ConnectCallback.prototype.removeConnectors = function(connectors)
{
    var len = this._connectors.length;
    for(var i = 0; i < connectors.length; ++i)
    {
        var idx = this.findConnectorInfo(connectors[i]);
        if(idx !== -1)
        {
            this._connectors.splice(idx, 1);
        }
    }
    this._index = 0;
    return this._connectors.length == 0;
}

ConnectCallback.prototype.removeFromPending = function()
{
    this._factory.removeFromPending(this, this._connectors);
}

ConnectCallback.prototype.getConnectors = function()
{
    try
    {
        //
        // Notify the factory that there's an async connect pending. This is necessary
        // to prevent the outgoing connection factory to be destroyed before all the
        // pending asynchronous connects are finished.
        //
        this._factory.incPendingConnectCount();
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            this._exceptionCallback.call(this._cbContext === undefined ? this._exceptionCallback : this._cbContext, ex);
            return;
        }
        else
        {
            throw ex;
        }
    }

    this.nextEndpoint();
}

ConnectCallback.prototype.nextEndpoint = function()
{
    try
    {
        Debug.assert(this._endpointsIndex < this._endpoints.length);
        this._currentEndpoint = this._endpoints[this._endpointsIndex++];
        this._currentEndpoint.connectors_async(this); // TODO
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            this.exception(ex);
        }
        else
        {
            throw ex;
        }
    }
}

ConnectCallback.prototype.getConnection = function()
{
    try
    {
        //
        // If all the connectors have been created, we ask the factory to get a
        // connection.
        //
        var compress = { value: false };
        var connection = this._factory.getConnection(this._connectors, this, compress);
        if(connection === null)
        {
            //
            // A null return value from getConnection indicates that the connection
            // is being established and that everthing has been done to ensure that
            // the callback will be notified when the connection establishment is
            // done.
            //
            return;
        }

        this._successCallback.call(this._cbContext === undefined ? this._successCallback : this._cbContext, connection,
                                   compress.value);
        this._factory.decPendingConnectCount(); // Must be called last.
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            this._exceptionCallback.call(this._cbContext === undefined ? this._exceptionCallback : this._cbContext, ex);
            this._factory.decPendingConnectCount(); // Must be called last.
        }
        else
        {
            throw ex;
        }
    }
}

ConnectCallback.prototype.nextConnector = function()
{
    var connection = null;
    try
    {
        Debug.assert(this._index < this._connectors.length);
        this._current = this._connectors[this._index++];
        connection = this._factory.createConnection(this._current.connector.connect(), this._current);
        connection.start(this); // TODO
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            this.connectionStartFailed(connection, ex);
        }
        else
        {
            throw ex;
        }
    }
}

var ConnectionsFinished = function(size, complete, cbContext)
{
    this._size = size;
    this._complete = complete;
    this._cbContext = cbContext;
    this._count = 0;
}

ConnectionsFinished.prototype.finished = function()
{
    Debug.assert(this._count < this._size);
    ++this._count;
    if(this._count === this._size)
    {
        this._complete.call(this._cbContext === undefined ? this._complete : this._cbContext);
    }
}
