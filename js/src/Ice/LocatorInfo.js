// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var AsyncResult = require("./AsyncResult");
var Debug = require("./Debug");
var Ex = require("./Exception");
var ExUtil = require("./ExUtil");
var HashMap = require("./HashMap");
var Protocol = require("./Protocol");

var LocalEx = require("./LocalException").Ice;
var LocatorRegistryPrx = require("./Locator").Ice.LocatorRegisterPrx;

var LocatorInfo = function(locator, table, background)
{
    this._locator = locator;
    this._locatorRegistry = null;
    this._table = table;
    this._background = background;

    this._adapterRequests = new HashMap(); // Map<String, Request>
    this._objectRequests = new HashMap(); // Map<Ice.Identity, Request>
    this._objectRequests.comparator = function(i1, i2) { return i1.equals(i2); };
}

LocatorInfo.prototype.destroy = function()
{
    this._locatorRegistry = null;
    this._table.clear();
}

LocatorInfo.prototype.equals = function(rhs)
{
    if(this === rhs)
    {
        return true;
    }

    if(rhs instanceof LocatorInfo)
    {
        return this._locator.equals(rhs._locator);
    }

    return false;
}

LocatorInfo.prototype.hashCode = function()
{
     return this._locator.hashCode();
}

LocatorInfo.prototype.getLocator = function()
{
    return this._locator;
}

LocatorInfo.prototype.getLocatorRegistry = function()
{
    var result = new GetLocatorRegistryAsyncResult(); // TODO

    if(this._locatorRegistry !== null)
    {
        result.__complete(this._locatorRegistry);
        return result;
    }

    var self = this;
    this._locator.getRegistry().whenCompleted(
        function(r, reg)
        {
            //
            // The locator registry can't be located.
            //
            self._locatorRegistry = LocatorRegistryPrx.uncheckedCast(reg.ice_locator(null));
            result.__complete(self._locatorRegistry);
        },
        function(r, ex)
        {
            result.__exception(ex);
        });

    return result;
}

LocatorInfo.prototype.getEndpoints = function(
    ref,
    wellKnownRef,
    ttl,
    successCallback,    // function(endpoints, cached)
    exceptionCallback,  // function(ex)
    cbContext)
{
    Debug.assert(ref.isIndirect());
    var endpoints = null;
    var cached = { value: false };
    if(!ref.isWellKnown())
    {
        endpoints = this._table.getAdapterEndpoints(ref.getAdapterId(), ttl, cached);
        if(!cached.value)
        {
            if(this._background && endpoints !== null)
            {
                this.getAdapterRequest(ref).addCallback(ref, wellKnownRef, ttl, null, null, null);
            }
            else
            {
                this.getAdapterRequest(ref).addCallback(ref, wellKnownRef, ttl, successCallback, exceptionCallback,
                                                        cbContext);
                return;
            }
        }
    }
    else
    {
        var r = this._table.getObjectReference(ref.getIdentity(), ttl, cached);
        if(!cached.value)
        {
            if(this._background && r !== null)
            {
                this.getObjectRequest(ref).addCallback(ref, null, ttl, null, null, null);
            }
            else
            {
                this.getObjectRequest(ref).addCallback(ref, null, ttl, successCallback, exceptionCallback, cbContext);
                return;
            }
        }

        if(!r.isIndirect())
        {
            endpoints = r.getEndpoints();
        }
        else if(!r.isWellKnown())
        {
            this.getEndpoints(r, ref, ttl, successCallback, exceptionCallback, cbContext);
            return;
        }
    }

    Debug.assert(endpoints !== null);
    if(ref.getInstance().traceLevels().location >= 1)
    {
        this.getEndpointsTrace(ref, endpoints, true);
    }
    if(successCallback !== null)
    {
        successCallback.call(cbContext === undefined ? successCallback : cbContext, endpoints, true);
    }
}

LocatorInfo.prototype.clearCache = function(ref)
{
    Debug.assert(ref.isIndirect());

    if(!ref.isWellKnown())
    {
        var endpoints = this._table.removeAdapterEndpoints(ref.getAdapterId());

        if(endpoints !== null && ref.getInstance().traceLevels().location >= 2)
        {
            this.trace("removed endpoints from locator table\n", ref, endpoints);
        }
    }
    else
    {
        var r =  this._table.removeObjectReference(ref.getIdentity());
        if(r !== null)
        {
            if(!r.isIndirect())
            {
                if(ref.getInstance().traceLevels().location >= 2)
                {
                    this.trace("removed endpoints from locator table", ref, r.getEndpoints());
                }
            }
            else if(!r.isWellKnown())
            {
                this.learCache(r);
            }
        }
    }
}

LocatorInfo.prototype.trace = function(msg, ref, endpoints)
{
    Debug.assert(ref.isIndirect());

    var s = [];
    s.push(msg);
    s.push("\n");
    if(!ref.isWellKnown())
    {
        s.push("adapter = ");
        s.push(ref.getAdapterId());
        s.push("\n");
    }
    else
    {
        s.push("object = ");
        s.push(ref.getInstance().identityToString(ref.getIdentity()));
        s.push("\n");
    }

    s.push("endpoints = ");
    for(var i = 0; i < endpoints.length; i++)
    {
        s.push(endpoints[i].toString());
        if(i + 1 < endpoints.length)
        {
            s.push(":");
        }
    }

    ref.getInstance().initializationData().logger.trace(ref.getInstance().traceLevels().locationCat, s.join(""));
}

LocatorInfo.prototype.getEndpointsException = function(ref, exc)
{
    Debug.assert(ref.isIndirect());

    var instance = ref.getInstance();

    try
    {
        throw exc;
    }
    catch(ex)
    {
        if(ex instanceof LocalEx.AdapterNotFoundException)
        {
            if(instance.traceLevels().location >= 1)
            {
                var s = [];
                s.push("adapter not found\n");
                s.push("adapter = ");
                s.push(ref.getAdapterId());
                instance.initializationData().logger.trace(instance.traceLevels().locationCat, s.join(""));
            }

            var e = new LocalEx.NotRegisteredException();
            e.kindOfObject = "object adapter";
            e.id = ref.getAdapterId();
            throw e;
        }
        else if(ex instanceof LocalEx.ObjectNotFoundException)
        {
            if(instance.traceLevels().location >= 1)
            {
                var s = [];
                s.push("object not found\n");
                s.push("object = ");
                s.push(instance.identityToString(ref.getIdentity()));
                instance.initializationData().logger.trace(instance.traceLevels().locationCat, s.join(""));
            }

            var e = new LocalEx.NotRegisteredException();
            e.kindOfObject = "object";
            e.id = instance.identityToString(ref.getIdentity());
            throw e;
        }
        else if(ex instanceof LocalEx.NotRegisteredException)
        {
            throw ex;
        }
        else if(ex instanceof LocalEx.LocalException)
        {
            if(instance.traceLevels().location >= 1)
            {
                var s = [];
                s.push("couldn't contact the locator to retrieve adapter endpoints\n");
                if(ref.getAdapterId().length > 0)
                {
                    s.push("adapter = ");
                    s.push(ref.getAdapterId());
                    s.push("\n");
                }
                else
                {
                    s.push("object = ");
                    s.push(instance.identityToString(ref.getIdentity()));
                    s.push("\n");
                }
                s.push("reason = " + ExUtil.toString(ex));
                instance.initializationData().logger.trace(instance.traceLevels().locationCat, s.join(""));
            }
            throw ex;
        }
        else
        {
            Debug.assert(false);
        }
    }
}

LocatorInfo.prototype.getEndpointsTrace = function(ref, endpoints, cached)
{
    if(endpoints !== null && endpoints.length > 0)
    {
        if(cached)
        {
            this.trace("found endpoints in locator table", ref, endpoints);
        }
        else
        {
            this.trace("retrieved endpoints from locator, adding to locator table", ref, endpoints);
        }
    }
    else
    {
        var instance = ref.getInstance();
        var s = [];
        s.push("no endpoints configured for ");
        if(ref.getAdapterId().length > 0)
        {
            s.push("adapter\n");
            s.push("adapter = ");
            s.push(ref.getAdapterId());
            s.push("\n");
        }
        else
        {
            s.push("object\n");
            s.push("object = ");
            s.push(instance.identityToString(ref.getIdentity()));
            s.push("\n");
        }
        instance.initializationData().logger.trace(instance.traceLevels().locationCat, s.join(""));
    }
}

LocatorInfo.prototype.getAdapterRequest = function(ref)
{
    if(ref.getInstance().traceLevels().location >= 1)
    {
        var instance = ref.getInstance();
        var s = [];
        s.push("searching for adapter by id\n");
        s.push("adapter = ");
        s.push(ref.getAdapterId());
        instance.initializationData().logger.trace(instance.traceLevels().locationCat, s.join(""));
    }

    var request = this._adapterRequests.get(ref.getAdapterId());
    if(request !== undefined)
    {
        return request;
    }
    request = new AdapterRequest(this, ref);
    this._adapterRequests.set(ref.getAdapterId(), request);
    return request;
}

LocatorInfo.prototype.getObjectRequest = function(ref)
{
    if(ref.getInstance().traceLevels().location >= 1)
    {
        var instance = ref.getInstance();
        var s = [];
        s.push("searching for object by id\n");
        s.push("object = ");
        s.push(instance.identityToString(ref.getIdentity()));
        instance.initializationData().logger.trace(instance.traceLevels().locationCat, s.join(""));
    }

    var request = this._objectRequests.get(ref.getIdentity());
    if(request !== undefined)
    {
        return request;
    }
    request = new ObjectRequest(this, ref);
    this._objectRequests.set(ref.getIdentity(), request);
    return request;
}

LocatorInfo.prototype.finishRequest = function(ref, wellKnownRefs, proxy, notRegistered)
{
    if(proxy === null || proxy.__reference().isIndirect())
    {
        //
        // Remove the cached references of well-known objects for which we tried
        // to resolved the endpoints if these endpoints are empty.
        //
        for(var i = 0; i < wellKnownRefs.length; ++i)
        {
            this._table.removeObjectReference(wellKnownRefs[i].getIdentity());
        }
    }

    if(!ref.isWellKnown())
    {
        if(proxy !== null && !proxy.__reference().isIndirect())
        {
            // Cache the adapter endpoints.
            this._table.addAdapterEndpoints(ref.getAdapterId(), proxy.__reference().getEndpoints());
        }
        else if(notRegistered) // If the adapter isn't registered anymore, remove it from the cache.
        {
            this._table.removeAdapterEndpoints(ref.getAdapterId());
        }

        Debug.assert(this._adapterRequests.has(ref.getAdapterId()));
        this._adapterRequests.delete(ref.getAdapterId());
    }
    else
    {
        if(proxy !== null && !proxy.__reference().isWellKnown())
        {
            // Cache the well-known object reference.
            this._table.addObjectReference(ref.getIdentity(), proxy.__reference());
        }
        else if(notRegistered) // If the well-known object isn't registered anymore, remove it from the cache.
        {
            this._table.removeObjectReference(ref.getIdentity());
        }

        Debug.assert(this._objectRequests.has(ref.getIdentity()));
        this._objectRequests.delete(ref.getIdentity());
    }
}

module.exports = LocatorInfo;

var RequestCallback = function(ref, ttl, successCallback, exceptionCallback, cbContext)
{
    this._ref = ref;
    this._ttl = ttl;
    this._successCallback = successCallback;
    this._exceptionCallback = exceptionCallback;
    this._cbContext = cbContext;
}

RequestCallback.prototype.response = function(locatorInfo, proxy)
{
    var endpoints = null;
    if(proxy !== null)
    {
        var r = proxy.__reference();
        if(this._ref.isWellKnown() && !Protocol.isSupported(this._ref.getEncoding(), r.getEncoding()))
        {
            //
            // If a well-known proxy and the returned proxy
            // encoding isn't supported, we're done: there's
            // no compatible endpoint we can use.
            //
        }
        else if(!r.isIndirect())
        {
            endpoints = r.getEndpoints();
        }
        else if(this._ref.isWellKnown() && !r.isWellKnown())
        {
            //
            // We're resolving the endpoints of a well-known object and the proxy returned
            // by the locator is an indirect proxy. We now need to resolve the endpoints
            // of this indirect proxy.
            //
            locatorInfo.getEndpoints(r, this._ref, this._ttl, this._successCallback, this._exceptionCallback,
                                     this._cbContext);
            return;
        }
    }

    if(this._ref.getInstance().traceLevels().location >= 1)
    {
        locatorInfo.getEndpointsTrace(this._ref, endpoints, false);
    }
    if(this._successCallback !== null)
    {
        this._successCallback.call(this._cbContext === undefined ? this._successCallback : this._cbContext,
                                   endpoints === null ? [] : endpoints, false);
    }
}

RequestCallback.prototype.exception = function(locatorInfo, exc)
{
    try
    {
        locatorInfo.getEndpointsException(this._ref, exc); // This throws.
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            if(this._exceptionCallback != null)
            {
                this._exceptionCallback.call(this._cbContext === undefined ? this._exceptionCallback : this._cbContext,
                                             ex);
            }
        }
        else
        {
            throw ex;
        }
    }
}

var Request = function(locatorInfo, ref)
{
    this._locatorInfo = locatorInfo;
    this._ref = ref;

    this._callbacks = []; // Array<RequestCallback>
    this._wellKnownRefs = []; // Array<Reference>
    this._sent = false;
    this._response = false;
    this._proxy = null;
    this._exception = null;
}

Request.prototype.addCallback = function(ref, wellKnownRef, ttl, successCallback, exceptionCallback, cbContext)
{
    var callback = new RequestCallback(ref, ttl, successCallback, exceptionCallback, cbContext);
    if(this._response)
    {
        callback.response(this._locatorInfo, this._proxy);
    }
    else if(this._exception != null)
    {
        callback.exception(this._locatorInfo, this._exception);
    }
    else
    {
        this._callbacks.push(callback);
        if(wellKnownRef !== null) // This request is to resolve the endpoints of a cached well-known object ref
        {
            this._wellKnownRefs.push(wellKnownRef);
        }
        if(!this._sent)
        {
            this._sent = true;
            this.send();
        }
    }
}

Request.prototype.response = function(proxy)
{
    this._locatorInfo.finishRequest(this._ref, this._wellKnownRefs, proxy, false);
    this._response = true;
    this._proxy = proxy;
    for(var i = 0; i < this._callbacks.length; ++i)
    {
        this._callbacks[i].response(this._locatorInfo, proxy);
    }
}

Request.prototype.exception = function(ex)
{
    this._locatorInfo.finishRequest(this._ref, this._wellKnownRefs, null, ex instanceof Ex.UserException);
    this._exception = ex;
    for(var i = 0; i < this._callbacks.length; ++i)
    {
        this._callbacks[i].exception(this._locatorInfo, ex);
    }
}

Request.prototype.send = function()
{
    //
    // Abstract
    //
    Debug.assert(false);
}

var ObjectRequest = function(locatorInfo, reference)
{
    Request.call(this, locatorInfo, reference);
    Debug.assert(reference.isWellKnown());
}

ObjectRequest.prototype = new Request();
ObjectRequest.prototype.constructor = ObjectRequest;

ObjectRequest.prototype.send = function()
{
    try
    {
        var self = this;
        var r = this._locatorInfo.getLocator().findObjectById(this._ref.getIdentity());
        r.whenCompleted(
            function(ar, proxy)
            {
                self.response(proxy);
            },
            function(ar, ex)
            {
                self.exception(ex);
            });
    }
    catch(ex)
    {
        this.exception(ex);
    }
}

var AdapterRequest = function(locatorInfo, reference)
{
    Request.call(this, locatorInfo, reference);
    Debug.assert(reference.isIndirect());
}

AdapterRequest.prototype = new Request();
AdapterRequest.prototype.constructor = AdapterRequest;

AdapterRequest.prototype.send = function()
{
    try
    {
        var self = this;
        var r = this._locatorInfo.getLocator().findAdapterById(this._ref.getAdapterId());
        r.whenCompleted(
            function(ar, proxy)
            {
                self.response(proxy);
            },
            function(ar, ex)
            {
                self.exception(ex);
            });
    }
    catch(ex)
    {
        this.exception(ex);
    }
}

var GetLocatorRegistryAsyncResult = function()
{
    AsyncResult.call(this, null, null, null, null, "getLocatoryRegistry");
    this._registry = null;
}

GetLocatorRegistryAsyncResult.prototype = new AsyncResult();
GetLocatorRegistryAsyncResult.prototype.constructor = GetLocatorRegistryAsyncResult;

GetLocatorRegistryAsyncResult.prototype.__complete = function(registry)
{
    this._registry = registry;
    this.completed = true;
    this.__finished();
}

GetLocatorRegistryAsyncResult.prototype.__response = function()
{
    this.responseCallback.call(this.cbContext === null ? this.responseCallback : this.cbContext, this._registry);
}
