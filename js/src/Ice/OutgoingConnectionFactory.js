// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var ConnectionI = require("./ConnectionI");
var Debug = require("./Debug");
var Ex = require("./Exception");
var HashMap = require("./HashMap");

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

    // TODO
    this._connections = new ConnectionListMap(); // map<Connector, Vector<Ice.ConnectionI>>
    this._connectionsByEndpoint = new ConnectionListMap(); // map<EndpointI, Vector<Ice.ConnectionI>>
    this._pending = new HashMap(); // map<Connector, IdentitySet<ConnectCallback>>
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
            self.gotClientEndpoints(endpoints, successCallback, cbContext);
        },
        routerInfo, exceptionCallback, cbContext);
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

        // TODO
        for each(var v:Object in this._connections)
        {
            var connectionList:Vector.<Ice.ConnectionI> = v as Vector.<Ice.ConnectionI>;
            for each(var connection:Ice.ConnectionI in connectionList)
            {
                if(connection.endpoint().equals(endpoint))
                {
                    connection.setAdapter(adapter);
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

    for(var e:Ice.HashMapEntry = this._connections.entries; e != null; e = e.next)
    {
        const connectionList:Vector.<Ice.ConnectionI> = e.value as Vector.<Ice.ConnectionI>;
        for each(var connection:Ice.ConnectionI in connectionList)
        {
            if(connection.getAdapter() == adapter)
            {
                connection.setAdapter(null);
            }
        }
    }
}

public function flushAsyncBatchRequests(outAsync:CommunicatorBatchOutgoingAsync):void
{
    var c:Vector.<Ice.ConnectionI> = new Vector.<Ice.ConnectionI>();

    if(!this._destroyed)
    {
        for(var e:Ice.HashMapEntry = this._connections.entries; e != null; e = e.next)
        {
            const connectionList:Vector.<Ice.ConnectionI> = e.value as Vector.<Ice.ConnectionI>;
            for each(var connection:Ice.ConnectionI in connectionList)
            {
                if(connection.isActiveOrHolding())
                {
                    c.push(connection);
                }
            }
        }
    }

    for each(var conn:Ice.ConnectionI in c)
    {
        try
        {
            outAsync.flushConnection(conn);
        }
        catch(ex:Ice.LocalException)
        {
            // Ignore.
        }
    }
}

private function applyOverrides(endpts:Vector.<EndpointI>):Vector.<EndpointI>
{
    const defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    var endpoints:Vector.<EndpointI> = new Vector.<EndpointI>();
    for each(var endpoint:EndpointI in endpts)
    {
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

private function findConnectionByEndpoint(endpoints:Vector.<EndpointI>, compress:Ice.BooleanHolder):Ice.ConnectionI
{
    if(this._destroyed)
    {
        throw new LocalEx.CommunicatorDestroyedException();
    }

    const defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    CONFIG::debug { Debug.assert(endpoints.length > 0); }

    for each(var endpoint:EndpointI in endpoints)
    {
        const connectionList:Vector.<Ice.ConnectionI> =
            this._connectionsByEndpoint.find(endpoint) as Vector.<Ice.ConnectionI>;
        if(connectionList == null)
        {
            continue;
        }

        for each(var connection:Ice.ConnectionI in connectionList)
        {
            if(connection.isActiveOrHolding()) // Don't return destroyed or un-validated connections
            {
                if(defaultsAndOverrides.overrideCompress)
                {
                    compress.value = defaultsAndOverrides.overrideCompressValue;
                }
                else
                {
                    compress.value = endpoint.compress();
                }
                return connection;
            }
        }
    }

    return null;
}

private function findConnection(connectors:Vector.<ConnectorInfo>, compress:Ice.BooleanHolder):Ice.ConnectionI
{
    const defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    for each(var ci:ConnectorInfo in connectors)
    {
        if(this._pending.find(ci.connector) != null)
        {
            continue;
        }

        const connectionList:Vector.<Ice.ConnectionI> = this._connections.find(ci.connector) as Vector.<Ice.ConnectionI>;
        if(connectionList == null)
        {
            continue;
        }

        for each(var connection:Ice.ConnectionI in connectionList)
        {
            if(connection.isActiveOrHolding()) // Don't return destroyed or un-validated connections
            {
                if(defaultsAndOverrides.overrideCompress)
                {
                    compress.value = defaultsAndOverrides.overrideCompressValue;
                }
                else
                {
                    compress.value = ci.endpoint.compress();
                }
                return connection;
            }
        }
    }

    return null;
}

public function incPendingConnectCount():void
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

public function decPendingConnectCount():void
{
    --this._pendingConnectCount;
    CONFIG::debug { Debug.assert(this._pendingConnectCount >= 0); }
    if(this._destroyed && this._pendingConnectCount == 0)
    {
        checkFinished();
    }
}

public function getConnection(connectors:Vector.<ConnectorInfo>, cb:ConnectCallback,
                              compress:Ice.BooleanHolder):Ice.ConnectionI
{
    if(this._destroyed)
    {
        throw new LocalEx.CommunicatorDestroyedException();
    }

    //
    // Reap closed connections
    //
    const cons:Vector.<Ice.ConnectionI> = this._reaper.swapConnections();
    if(cons != null)
    {
        for each(var c:Ice.ConnectionI in cons)
        {
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
        const connection:Ice.ConnectionI = findConnection(connectors, compress);
        if(connection != null)
        {
            return connection;
        }

        if(addToPending(cb, connectors))
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

public function createConnection(transceiver:Transceiver, ci:ConnectorInfo):Ice.ConnectionI
{
    CONFIG::debug { Debug.assert(this._pending.find(ci.connector) != null && transceiver != null); }

    //
    // Create and add the connection to the connection map. Adding the connection to the map
    // is necessary to support the interruption of the connection initialization and validation
    // in case the communicator is destroyed.
    //
    var connection:Ice.ConnectionI = null;
    try
    {
        if(this._destroyed)
        {
            throw new LocalEx.CommunicatorDestroyedException();
        }

        connection = new Ice.ConnectionI(this._communicator, this._instance, this._reaper, transceiver, ci.connector,
                                         ci.endpoint.changeCompress(false), null);
    }
    catch(ex:Ice.LocalException)
    {
        try
        {
            transceiver.close();
        }
        catch(exc:Ice.LocalException)
        {
            // Ignore
        }
        throw ex;
    }

    this._connections.put(ci.connector, connection);
    this._connectionsByEndpoint.put(connection.endpoint(), connection);
    this._connectionsByEndpoint.put(connection.endpoint().changeCompress(true), connection);
    return connection;
}

public function finishGetConnection(connectors:Vector.<ConnectorInfo>, ci:ConnectorInfo,
                                    connection:Ice.ConnectionI, cb:ConnectCallback):void
{
    var connectionCallbacks:IdentitySet = new IdentitySet();
    if(cb != null)
    {
        connectionCallbacks.add(cb);
    }

    var callbacks:IdentitySet = new IdentitySet();
    var key:Object;
    var cc:ConnectCallback;
    for each(var c:ConnectorInfo in connectors)
    {
        const cbs:IdentitySet = this._pending.remove(c.connector) as IdentitySet;
        if(cbs != null)
        {
            for(key in cbs.entries)
            {
                cc = key as ConnectCallback;
                if(cc.hasConnector(ci))
                {
                    connectionCallbacks.add(cc);
                }
                else
                {
                    callbacks.add(cc);
                }
            }
        }
    }

    for(key in connectionCallbacks.entries)
    {
        cc = key as ConnectCallback;
        cc.removeFromPending();
        callbacks.remove(cc);
    }
    for(key in callbacks.entries)
    {
        cc = key as ConnectCallback;
        cc.removeFromPending();
    }

    var compress:Boolean;
    const defaultsAndOverrides:DefaultsAndOverrides = this._instance.defaultsAndOverrides();
    if(defaultsAndOverrides.overrideCompress)
    {
        compress = defaultsAndOverrides.overrideCompressValue;
    }
    else
    {
        compress = ci.endpoint.compress();
    }

    for(key in callbacks.entries)
    {
        cc = key as ConnectCallback;
        cc.getConnection();
    }
    for(key in connectionCallbacks.entries)
    {
        cc = key as ConnectCallback;
        cc.setConnection(connection, compress);
    }

    checkFinished();
}

public function finishGetConnectionEx(connectors:Vector.<ConnectorInfo>, ex:Ice.LocalException,
                                      cb:ConnectCallback):void
{
    var failedCallbacks:IdentitySet = new IdentitySet();
    if(cb != null)
    {
        failedCallbacks.add(cb);
    }

    var callbacks:IdentitySet = new IdentitySet();
    var key:Object;
    var cc:ConnectCallback;
    for each(var c:ConnectorInfo in connectors)
    {
        const cbs:IdentitySet = this._pending.remove(c.connector) as IdentitySet;
        if(cbs != null)
        {
            for(key in cbs.entries)
            {
                cc = key as ConnectCallback;
                if(cc.removeConnectors(connectors))
                {
                    failedCallbacks.add(cc);
                }
                else
                {
                    callbacks.add(cc);
                }
            }
        }
    }

    for(key in callbacks.entries)
    {
        cc = key as ConnectCallback;
        CONFIG::debug { Debug.assert(!failedCallbacks.contains(cc)); }
        cc.removeFromPending();
    }
    checkFinished();

    for(key in callbacks.entries)
    {
        cc = key as ConnectCallback;
        cc.getConnection();
    }
    for(key in failedCallbacks.entries)
    {
        cc = key as ConnectCallback;
        cc.setException(ex);
    }
}

private function addToPending(cb:ConnectCallback, connectors:Vector.<ConnectorInfo>):Boolean
{
    //
    // Add the callback to each connector pending list.
    //
    var found:Boolean = false;
    var p:ConnectorInfo;
    for each(p in connectors)
    {
        const cbs:IdentitySet = this._pending.find(p.connector) as IdentitySet;
        if(cbs != null)
        {
            found = true;
            if(cb != null)
            {
                cbs.add(cb); // Add the callback to each pending connector.
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
    for each(p in connectors)
    {
        if(this._pending.find(p.connector) == null)
        {
            this._pending.put(p.connector, new IdentitySet());
        }
    }

    return false;
}

public function removeFromPending(cb:ConnectCallback, connectors:Vector.<ConnectorInfo>):void
{
    for each(var p:ConnectorInfo in connectors)
    {
        const cbs:IdentitySet = this._pending.find(p.connector) as IdentitySet;
        if(cbs != null)
        {
            cbs.remove(cb);
        }
    }
}

public function handleConnectionException(ex:Ice.LocalException, hasMore:Boolean):void
{
    const traceLevels:TraceLevels = this._instance.traceLevels();
    if(traceLevels.retry >= 2)
    {
        var s:Vector.<String> = new Vector.<String>();
        s.push("connection to endpoint failed");
        if(ex is LocalEx.CommunicatorDestroyedException)
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
        s.push(Ex.toString(ex));
        this._instance.initializationData().logger._trace(traceLevels.retryCat, s.join(""));
    }
}

public function handleException(ex:Ice.LocalException, hasMore:Boolean):void
{
    const traceLevels:TraceLevels = this._instance.traceLevels();
    if(traceLevels.retry >= 2)
    {
        var s:Vector.<String> = new Vector.<String>();
        s.push("couldn't resolve endpoint host");
        if(ex is LocalEx.CommunicatorDestroyedException)
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
        s.push(Ex.toString(ex));
        this._instance.initializationData().logger._trace(traceLevels.retryCat, s.join(""));
    }
}

private function checkFinished():void
{
    //
    // Can't continue until the factory is destroyed and there are no pending connections.
    //
    if(this._waitComplete == null || !this._destroyed || this._pending.length > 0 || this._pendingConnectCount > 0)
    {
        return;
    }

    //
    // Count the number of connections.
    //
    var size:int = 0;
    var connectionList:Vector.<Ice.ConnectionI>;
    var e:Ice.HashMapEntry;
    for(e = this._connections.entries; e != null; e = e.next)
    {
        connectionList = e.value as Vector.<Ice.ConnectionI>;
        size += connectionList.length;
    }

    //
    // Now we wait until the destruction of each connection is finished.
    //
    if(size > 0)
    {
        var cb:ConnectionsFinished = new ConnectionsFinished(size, connectionsFinished);
        for(e = this._connections.entries; e != null; e = e.next)
        {
            connectionList = e.value as Vector.<Ice.ConnectionI>;
            for each(var connection:Ice.ConnectionI in connectionList)
            {
                connection.waitUntilFinished(cb.finished);
            }
        }
    }
    else
    {
        connectionsFinished();
    }
}

private function connectionsFinished():void
{
    // Ensure all the connections are finished and reapable at this point.
    var cons:Vector.<Ice.ConnectionI> = this._reaper.swapConnections();
    if(cons != null)
    {
        var size:int = 0;
        for(var e:Ice.HashMapEntry = this._connections.entries; e != null; e = e.next)
        {
            const connectionList:Vector.<Ice.ConnectionI> = e.value as Vector.<Ice.ConnectionI>;
            size += connectionList.length;
        }
        CONFIG::debug { Debug.assert(cons.length == size); }
        this._connections.clear();
        this._connectionsByEndpoint.clear();
    }
    else
    {
        CONFIG::debug { Debug.assert(this._connections.length == 0); }
        CONFIG::debug { Debug.assert(this._connectionsByEndpoint.length == 0); }
    }

    CONFIG::debug { Debug.assert(this._waitComplete != null); }
    this._waitComplete(this._waitResult);
}

modules.export = OutgoingConnectionFactory;

import Ice.ConnectionI;
import Ice.HashMap;
import IceInternal.Debug;

//
// Value is a Vector.<Ice.ConnectionI>
//
class ConnectionListMap extends Ice.HashMap
{
    public override function put(key:Object, value:Object):Object
    {
        var prev:Object = null;
        var list:Vector.<Ice.ConnectionI> = find(key) as Vector.<Ice.ConnectionI>;
        if(list == null)
        {
            list = new Vector.<Ice.ConnectionI>();
            super.put(key, list);
        }
        else
        {
            prev = list;
        }
        CONFIG::debug { IceInternal.Debug.assert(value is Ice.ConnectionI); }
        list.push(value);
        return prev;
    }

    public function removeConnection(key:Object, value:Ice.ConnectionI):void
    {
        const list:Vector.<Ice.ConnectionI> = find(key) as Vector.<Ice.ConnectionI>;
        CONFIG::debug { IceInternal.Debug.assert(list != null); }
        const idx:int = list.indexOf(value);
        CONFIG::debug { IceInternal.Debug.assert(idx != -1); }
        list.splice(idx, 1);
        if(list.length == 0)
        {
            remove(key);
        }
    }
}

import Ice.ConnectionI_StartCallback;
import LocalEx.CommunicatorDestroyedException;
import Ice.EndpointSelectionType;
import Ice.LocalException;
import IceInternal.Connector;
import IceInternal.EndpointI;
import IceInternal.EndpointI_connectors;
import IceInternal.OutgoingConnectionFactory;
import IceInternal.OutgoingConnectionFactory_CreateConnectionCallback;
import IceInternal.VectorUtil;

class ConnectCallback implements Ice.ConnectionI_StartCallback, IceInternal.EndpointI_connectors
{
    public function ConnectCallback(f:IceInternal.OutgoingConnectionFactory, endpoints:Vector.<IceInternal.EndpointI>,
                                    more:Boolean,
                                    selType:Ice.EndpointSelectionType,
                                    cb:IceInternal.OutgoingConnectionFactory_CreateConnectionCallback)
    {
        _factory = f;
        _endpoints = endpoints;
        _hasMore = more;
        _callback = cb;
        _selType = selType;
        _endpointsIndex = 0;
        _index = 0;
    }

    //
    // Methods from ConnectionI_StartCallback
    //
    public function connectionStartCompleted(connection:Ice.ConnectionI):void
    {
        connection.activate();
        _factory.finishGetConnection(_connectors, _current, connection, this);
    }

    public function connectionStartFailed(connection:Ice.ConnectionI, ex:Ice.LocalException):void
    {
        CONFIG::debug { Debug.assert(_current != null); }

        _factory.handleConnectionException(ex, _hasMore || _index < _connectors.length);
        if(ex is LocalEx.CommunicatorDestroyedException) // No need to continue.
        {
            _factory.finishGetConnectionEx(_connectors, ex, this);
        }
        else if(_index < _connectors.length) // Try the next connector.
        {
            nextConnector();
        }
        else
        {
            _factory.finishGetConnectionEx(_connectors, ex, this);
        }
    }

    //
    // Methods from EndpointI_connectors
    //
    public function connectors(cons:Vector.<IceInternal.Connector>):void
    {
        //
        // Shuffle connectors if endpoint selection type is Random.
        //
        if(_selType == Ice.EndpointSelectionType.Random)
        {
            VectorUtil.shuffle(cons);
        }

        for each(var p:IceInternal.Connector in cons)
        {
            _connectors.push(new ConnectorInfo(p, _currentEndpoint));
        }

        if(_endpointsIndex < _endpoints.length)
        {
            nextEndpoint();
        }
        else
        {
            CONFIG::debug { Debug.assert(_connectors.length > 0); }

            //
            // We now have all the connectors for the given endpoints. We can try to obtain the
            // connection.
            //
            _index = 0;
            getConnection();
        }
    }

    public function exception(ex:Ice.LocalException):void
    {
        _factory.handleException(ex, _hasMore || _endpointsIndex < _endpoints.length);
        if(_endpointsIndex < _endpoints.length)
        {
            nextEndpoint();
        }
        else if(_connectors.length > 0)
        {
            //
            // We now have all the connectors for the given endpoints. We can try to obtain the
            // connection.
            //
            _index = 0;
            getConnection();
        }
        else
        {
            _callback.setException(ex);
            _factory.decPendingConnectCount(); // Must be called last.
        }
    }

    public function setConnection(connection:Ice.ConnectionI, compress:Boolean):void
    {
        //
        // Callback from the factory: the connection to one of the callback
        // connectors has been established.
        //
        _callback.setConnection(connection, compress);
        _factory.decPendingConnectCount(); // Must be called last.
    }

    public function setException(ex:Ice.LocalException):void
    {
        //
        // Callback from the factory: connection establishment failed.
        //
        _callback.setException(ex);
        _factory.decPendingConnectCount(); // Must be called last.
    }

    public function hasConnector(ci:ConnectorInfo):Boolean
    {
        return findConnectorInfo(ci) != -1;
    }

    private function findConnectorInfo(ci:ConnectorInfo):int
    {
        var index:int = -1;
        _connectors.some(
            function(item:ConnectorInfo, idx:int, v:Vector.<ConnectorInfo>):Boolean
            {
                if(ci.equals(item))
                {
                    index = idx;
                    return true;
                }
                return false;
            });
        return index;
    }

    public function removeConnectors(connectors:Vector.<ConnectorInfo>):Boolean
    {
        var len:int = _connectors.length;
        for each(var ci:ConnectorInfo in connectors)
        {
            const i:int = findConnectorInfo(ci);
            if(i != -1)
            {
                _connectors.splice(i, 1);
            }
        }
        _index = 0;
        return _connectors.length == 0;
    }

    public function removeFromPending():void
    {
        _factory.removeFromPending(this, _connectors);
    }

    public function getConnectors():void
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
        catch(ex:Ice.LocalException)
        {
            _callback.setException(ex);
            return;
        }

        nextEndpoint();
    }

    public function nextEndpoint():void
    {
        try
        {
            CONFIG::debug { Debug.assert(_endpointsIndex < _endpoints.length); }
            _currentEndpoint = _endpoints[_endpointsIndex++];
            _currentEndpoint.connectors_async(this);
        }
        catch(ex:Ice.LocalException)
        {
            exception(ex);
        }
    }

    public function getConnection():void
    {
        try
        {
            //
            // If all the connectors have been created, we ask the factory to get a
            // connection.
            //
            const compress:Ice.BooleanHolder = new Ice.BooleanHolder();
            const connection:Ice.ConnectionI = _factory.getConnection(_connectors, this, compress);
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

            _callback.setConnection(connection, compress.value);
            _factory.decPendingConnectCount(); // Must be called last.
        }
        catch(ex:Ice.LocalException)
        {
            _callback.setException(ex);
            _factory.decPendingConnectCount(); // Must be called last.
        }
    }

    public function nextConnector():void
    {
        var connection:Ice.ConnectionI = null;
        try
        {
            CONFIG::debug { Debug.assert(_index < _connectors.length); }
            _current = _connectors[_index++];
            connection = _factory.createConnection(_current.connector.connect(), _current);
            connection.start(this);
        }
        catch(ex:Ice.LocalException)
        {
            connectionStartFailed(connection, ex);
        }
    }

    private var _factory:OutgoingConnectionFactory;
    private var _hasMore:Boolean;
    private var _callback:OutgoingConnectionFactory_CreateConnectionCallback;
    private var _endpoints:Vector.<IceInternal.EndpointI>;
    private var _selType:Ice.EndpointSelectionType;
    private var _endpointsIndex:int; // Current index of _endpoints
    private var _currentEndpoint:EndpointI;
    private var _connectors:Vector.<ConnectorInfo> = new Vector.<ConnectorInfo>();
    private var _index:int; // Current index of _connectors
    private var _current:ConnectorInfo;
}

class ConnectorInfo
{
    public function ConnectorInfo(c:IceInternal.Connector, e:IceInternal.EndpointI)
    {
        connector = c;
        endpoint = e;
    }

    public function equals(obj:Object):Boolean
    {
        const r:ConnectorInfo = obj as ConnectorInfo;
        return connector.equals(r.connector);
    }

    public var connector:IceInternal.Connector;
    public var endpoint:IceInternal.EndpointI;
}

class ConnectionsFinished
{
    public function ConnectionsFinished(size:int, complete:Function)
    {
        _size = size;
        _complete = complete;
        _count = 0;
    }

    public function finished():void
    {
        CONFIG::debug { Debug.assert(_count < _size); }
        ++_count;
        if(_count == _size)
        {
            _complete();
        }
    }

    private var _size:int;
    private var _complete:Function;
    private var _count:int;
}
