// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Debug = require("./Debug");
var Ex = require("./Exception");
var HashMap = require("./HashMap");

var RouterInfo = function(router)
{
    this._router = router;

    Debug.assert(this._router != null);

    this._clientEndpoints = null;
    this._serverEndpoints = null;
    this._adapter = null;
    this._identities = new HashMap(); // Set<Identity> = Map<Identity, 1>
    this._identities.comparator = HashMap.compareEquals;
    this._evictedIdentities = [];
}

RouterInfo.prototype.destroy = function()
{
    this._clientEndpoints = [];
    this._serverEndpoints = [];
    this._adapter = null;
    this._identities.clear();
}

RouterInfo.prototype.equals = function(rhs)
{
    if(this === rhs)
    {
        return true;
    }

    if(rhs instanceof RouterInfo)
    {
        return this._router.equals(rhs._router);
    }

    return false;
}

RouterInfo.prototype.hashCode = function()
{
    return this._router.hashCode();
}

RouterInfo.prototype.getRouter = function()
{
    //
    // No mutex lock necessary, _router is immutable.
    //
    return this._router;
}

RouterInfo.prototype.getClientEndpoints = function(
    successCallback,    // function(endpoints)
    exceptionCallback,  // function(ex)
    cbContext
)
{
    if(this._clientEndpoints !== null)
    {
        successCallback.call(cbContext === undefined ? successCallback : cbContext, this._clientEndpoints);
        return;
    }

    var self = this;
    this._router.getClientProxy().whenCompleted(
        function(r, clientProxy)
        {
            self.setClientEndpoints(clientProxy, successCallback, exceptionCallback, cbContext);
        },
        function(r, ex)
        {
            Debug.assert(ex instanceof Ex.LocalException);
            exceptionCallback.call(cbContext === undefined ? exceptionCallback : cbContext, ex);
        });
}

RouterInfo.prototype.getServerEndpoints = function(
    successCallback,    // function(endpoints)
    exceptionCallback,  // function(ex)
    cbContext
)
{
    if(this._serverEndpoints !== null) // Lazy initialization.
    {
        successCallback.call(cbContext === undefined ? successCallback : cbContext, this._clientEndpoints);
        return;
    }

    var self = this;
    this._router.getServerProxy().whenCompleted(
        function(r, proxy)
        {
            successCallback.call(cbContext === undefined ? successCallback : cbContext,
                                 self.setServerEndpoints(proxy));
        },
        function(r, ex)
        {
            Debug.assert(ex instanceof Ex.LocalException);
            exceptionCallback.call(cbContext === undefined ? exceptionCallback : cbContext, ex);
        });
}

RouterInfo.prototype.addProxy = function(
    proxy,
    successCallback,    // function()
    exceptionCallback,  // function(ex)
    cbContext
)
{
    Debug.assert(proxy != null);

    if(this._identities.has(proxy.ice_getIdentity()))
    {
        //
        // Only add the proxy to the router if it's not already in our local map.
        //
        return true;
    }

    var self = this;
    this._router.addProxies([ proxy ]).whenCompleted(
        function(r, evictedProxies)
        {
            self.addAndEvictProxies(proxy, evictedProxies);
            successCallback.call(cbContext === undefined ? successCallback : cbContext);
        },
        function(r, ex)
        {
            Debug.assert(ex instanceof Ex.LocalException);
            exceptionCallback.call(cbContext === undefined ? exceptionCallback : cbContext, ex);
        });

    return false;
}

RouterInfo.prototype.setAdapter = function(adapter)
{
    this._adapter = adapter;
}

RouterInfo.prototype.getAdapter = function()
{
    return this._adapter;
}

RouterInfo.prototype.clearCache = function(ref)
{
    this._identities.delete(ref.getIdentity());
}

RouterInfo.prototype.setClientEndpoints = function(
    clientProxy,
    successCallback,    // function(endpoints)
    exceptionCallback,  // function(ex)
    cbContext
)
{
    if(this._clientEndpoints === null)
    {
        if(clientProxy === null)
        {
            //
            // If getClientProxy() return nil, use router endpoints.
            //
            this._clientEndpoints = this._router.__reference().getEndpoints();
            successCallback.call(cbContext === undefined ? successCallback : cbContext, this._clientEndpoints);
        }
        else
        {
            clientProxy = clientProxy.ice_router(null); // The client proxy cannot be routed.

            //
            // In order to avoid creating a new connection to the
            // router, we must use the same timeout as the already
            // existing connection.
            //
            var self = this;
            this._router.ice_getConnection().whenCompleted(
                function(r, con)
                {
                    var proxy = clientProxy.ice_timeout(con.timeout());
                    self._clientEndpoints = proxy.__reference().getEndpoints();
                    successCallback.call(cbContext === undefined ? successCallback : cbContext, self._clientEndpoints);
                },
                function(r, ex)
                {
                    Debug.assert(ex instanceof Ex.LocalException);
                    exceptionCallback.call(cbContext === undefined ? exceptionCallback : cbContext, ex);
                });
        }
    }
}

RouterInfo.prototype.setServerEndpoints = function(serverProxy)
{
    if(serverProxy === null)
    {
        throw new LocalEx.NoEndpointException();
    }

    serverProxy = serverProxy.ice_router(null); // The server proxy cannot be routed.
    this._serverEndpoints = serverProxy.__reference().getEndpoints();
    return this._serverEndpoints;
}

RouterInfo.prototype.addAndEvictProxies = function(proxy, evictedProxies)
{
    //
    // Check if the proxy hasn't already been evicted by a
    // concurrent addProxies call. If it's the case, don't
    // add it to our local map.
    //
    var index = ArrayUtil.indexOf(this._evictedIdentities, proxy.ice_getIdentity(),
                                  function(i1, i2) { return i1.equals(i2); });
    if(index >= 0)
    {
        this._evictedIdentities.splice(index, 1);
    }
    else
    {
        //
        // If we successfully added the proxy to the router,
        // we add it to our local map.
        //
        this._identities.set(proxy.ice_getIdentity(), 1);
    }

    //
    // We also must remove whatever proxies the router evicted.
    //
    for(var i = 0; i < evictedProxies.length; ++i)
    {
        this._identities.delete(evictedProxies[i].ice_getIdentity());
    }
}

module.exports = RouterInfo;
