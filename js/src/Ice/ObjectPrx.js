// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var ArrayUtil = require("./ArrayUtil");
var AsyncResult = require("./AsyncResult");
var ConnectRequestHandler = require("./ConnectRequestHandler");
var Debug = require("./Debug");
var Ex = require("./Exception");
var FormatType = require("./FormatType");
var HashMap = require("./HashMap");
var OutgoingAsync = require("./OutgoingAsync");
var RefMode = require("./ReferenceMode");

var Curr = require("./Current").Ice;
var LocalEx = require("./LocalException").Ice;

//
// Ice.ObjectPrx
//
var ObjectPrx = function()
{
    this._reference = null;
    this._handler = null;
};

ObjectPrx.prototype.hashCode = function(r)
{
    return this._reference.hashCode();
};

ObjectPrx.prototype.ice_getCommunicator = function()
{
    return this._reference.getCommunicator();
};

ObjectPrx.prototype.toString = function()
{
    return this._reference.toString();
};

ObjectPrx.prototype.ice_isA = function(__id, __context)
{
    var __ice_isA_name = "ice_isA";
    this.__checkAsyncTwowayOnly(__ice_isA_name);
    var __promise = new OutgoingAsync(this, __ice_isA_name, ObjectPrx.__completed_bool);
    try
    {
        __promise.__prepare(__ice_isA_name, Curr.OperationMode.Nonmutating, __context);
        var __os = __promise.__startWriteParams(FormatType.DefaultFormat);
        __os.writeString(__id);
        __os.endWriteEncaps();
        __promise.__send();
    }
    catch(__ex)
    {
        this.__handleLocalException(__promise, __ex);
    }
    return __promise;
};

ObjectPrx.prototype.ice_ping = function(__context)
{
    var __ice_ping_name = "ice_ping";
    var __promise = new OutgoingAsync(this, __ice_ping_name, ObjectPrx.__completed);
    try
    {
        __promise.__prepare(__ice_ping_name, Curr.OperationMode.Nonmutating, __context);
        __promise.__writeEmptyParams();
        __promise.__send();
    }
    catch(__ex)
    {
        this.__handleLocalException(__promise, __ex);
    }
    return __promise;
};

ObjectPrx.prototype.ice_ids = function(__context)
{
    var __ice_ids_name = "ice_ids";
    this.__checkAsyncTwowayOnly(__ice_ids_name);
    var __promise = new OutgoingAsync(this, __ice_ids_name, this._ice_ids_completed);
    try
    {
        __promise.__prepare(__ice_ids_name, Curr.OperationMode.Nonmutating, __context);
        __promise.__writeEmptyParams();
        __promise.__send();
    }
    catch(__ex)
    {
        this.__handleLocalException(__promise, __ex);
    }
    return __promise;
};

ObjectPrx.prototype._ice_ids_completed = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readStringSeq();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

ObjectPrx.prototype.ice_id = function(__context)
{
    var __ice_id_name = "ice_id";
    this.__checkAsyncTwowayOnly(__ice_id_name);
    var __promise = new OutgoingAsync(this, __ice_id_name, ObjectPrx.__completed_string);
    try
    {
        __promise.__prepare(__ice_id_name, Curr.OperationMode.Nonmutating, __context);
        __promise.__writeEmptyParams();
        __promise.__send();
    }
    catch(__ex)
    {
        this.__handleLocalException(__promise, __ex);
    }
    return __promise;
};

ObjectPrx.prototype.ice_getIdentity = function()
{
    return this._reference.getIdentity().clone();
};

ObjectPrx.prototype.ice_identity = function(newIdentity)
{
    if(newIdentity === undefined || newIdentity === null || newIdentity.name.length === 0)
    {
        throw new LocalEx.IllegalIdentityException();
    }
    if(newIdentity.equals(this._reference.getIdentity()))
    {
        return this;
    }
    else
    {
        var proxy = new ObjectPrx();
        proxy.__setup(this._reference.changeIdentity(newIdentity));
        return proxy;
    }
};

ObjectPrx.prototype.ice_getContext = function()
{
    return new HashMap(this._reference.getContext());
};

ObjectPrx.prototype.ice_context = function(newContext)
{
    return this.__newInstance(this._reference.changeContext(newContext));
};

ObjectPrx.prototype.ice_getFacet = function()
{
    return this._reference.getFacet();
};

ObjectPrx.prototype.ice_facet = function(newFacet)
{
    if(newFacet === undefined || newFacet === null)
    {
        newFacet = "";
    }

    if(newFacet === this._reference.getFacet())
    {
        return this;
    }
    else
    {
        var proxy = new ObjectPrx();
        proxy.__setup(this._reference.changeFacet(newFacet));
        return proxy;
    }
};

ObjectPrx.prototype.ice_getAdapterId = function()
{
    return this._reference.getAdapterId();
};

ObjectPrx.prototype.ice_adapterId = function(newAdapterId)
{
    if(newAdapterId === undefined || newAdapterId === null)
    {
        newAdapterId = "";
    }

    if(newAdapterId === this._reference.getAdapterId())
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeAdapterId(newAdapterId));
    }
};

ObjectPrx.prototype.ice_getEndpoints = function()
{
    return ArrayUtil.clone(this._reference.getEndpoints());
};

ObjectPrx.prototype.ice_endpoints = function(newEndpoints)
{
    if(newEndpoints === undefined || newEndpoints === null)
    {
        newEndpoints = [];
    }

    if(ArrayUtil.equals(newEndpoints, this._reference.getEndpoints()))
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeEndpoints(newEndpoints));
    }
};

ObjectPrx.prototype.ice_getLocatorCacheTimeout = function()
{
    return this._reference.getLocatorCacheTimeout();
};

ObjectPrx.prototype.ice_locatorCacheTimeout = function(newTimeout)
{
    if(newTimeout === this._reference.getLocatorCacheTimeout())
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeLocatorCacheTimeout(newTimeout));
    }
};

ObjectPrx.prototype.ice_isConnectionCached = function()
{
    return this._reference.getCacheConnection();
};

ObjectPrx.prototype.ice_connectionCached = function(newCache)
{
    if(newCache === this._reference.getCacheConnection())
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeCacheConnection(newCache));
    }
};

ObjectPrx.prototype.ice_getEndpointSelection = function()
{
    return this._reference.getEndpointSelection();
};

ObjectPrx.prototype.ice_endpointSelection = function(newType)
{
    if(newType === this._reference.getEndpointSelection())
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeEndpointSelection(newType));
    }
};

ObjectPrx.prototype.ice_isSecure = function()
{
    return this._reference.getSecure();
};

ObjectPrx.prototype.ice_secure = function(b)
{
    if(b === this._reference.getSecure())
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeSecure(b));
    }
};

ObjectPrx.prototype.ice_getEncodingVersion = function()
{
    return this._reference.getEncoding().clone();
};

ObjectPrx.prototype.ice_encodingVersion = function(e)
{
    if(e.equals(this._reference.getEncoding()))
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeEncoding(e));
    }
};

ObjectPrx.prototype.ice_isPreferSecure = function()
{
    return this._reference.getPreferSecure();
};

ObjectPrx.prototype.ice_preferSecure = function(b)
{
    if(b === this._reference.getPreferSecure())
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changePreferSecure(b));
    }
};

ObjectPrx.prototype.ice_getRouter = function()
{
    var ri = this._reference.getRouterInfo();
    return ri !== null ? ri.getRouter() : null;
};

ObjectPrx.prototype.ice_router = function(router)
{
    var ref = this._reference.changeRouter(router);
    if(ref.equals(this._reference))
    {
        return this;
    }
    else
    {
        return this.__newInstance(ref);
    }
};

ObjectPrx.prototype.ice_getLocator = function()
{
    var ri = this._reference.getLocatorInfo();
    return ri !== null ? ri.getLocator() : null;
};

ObjectPrx.prototype.ice_locator = function(locator)
{
    var ref = this._reference.changeLocator(locator);
    if(ref.equals(this._reference))
    {
        return this;
    }
    else
    {
        return this.__newInstance(ref);
    }
};

ObjectPrx.prototype.ice_isTwoway = function()
{
    return this._reference.getMode() === RefMode.ModeTwoway;
};

ObjectPrx.prototype.ice_twoway = function()
{
    if(this._reference.getMode() === RefMode.ModeTwoway)
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeMode(RefMode.ModeTwoway));
    }
};

ObjectPrx.prototype.ice_isOneway = function()
{
    return this._reference.getMode() === RefMode.ModeOneway;
};

ObjectPrx.prototype.ice_oneway = function()
{
    if(this._reference.getMode() === RefMode.ModeOneway)
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeMode(RefMode.ModeOneway));
    }
};

ObjectPrx.prototype.ice_isBatchOneway = function()
{
    return this._reference.getMode() === RefMode.ModeBatchOneway;
};

ObjectPrx.prototype.ice_batchOneway = function()
{
    if(this._reference.getMode() === RefMode.ModeBatchOneway)
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeMode(RefMode.ModeBatchOneway));
    }
};

ObjectPrx.prototype.ice_isDatagram = function()
{
    return this._reference.getMode() === RefMode.ModeDatagram;
};

ObjectPrx.prototype.ice_datagram = function()
{
    if(this._reference.getMode() === RefMode.ModeDatagram)
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeMode(RefMode.ModeDatagram));
    }
};

ObjectPrx.prototype.ice_isBatchDatagram = function()
{
    return this._reference.getMode() === RefMode.ModeBatchDatagram;
};

ObjectPrx.prototype.ice_batchDatagram = function()
{
    if(this._reference.getMode() === RefMode.ModeBatchDatagram)
    {
        return this;
    }
    else
    {
        return this.__newInstance(this._reference.changeMode(RefMode.ModeBatchDatagram));
    }
};

ObjectPrx.prototype.ice_compress = function(co)
{
    var ref = this._reference.changeCompress(co);
    if(ref.equals(this._reference))
    {
        return this;
    }
    else
    {
        return this.__newInstance(ref);
    }
};

ObjectPrx.prototype.ice_timeout = function(t)
{
    var ref = this._reference.changeTimeout(t);
    if(ref.equals(this._reference))
    {
        return this;
    }
    else
    {
        return this.__newInstance(ref);
    }
};

ObjectPrx.prototype.ice_getConnectionId = function()
{
    return this._reference.getConnectionId();
};

ObjectPrx.prototype.ice_connectionId = function(id)
{
    var ref = this._reference.changeConnectionId(id);
    if(ref.equals(this._reference))
    {
        return this;
    }
    else
    {
        return this.__newInstance(ref);
    }
};

ObjectPrx.prototype.ice_getConnection = function()
{
    return this.__getRequestHandler().onConnection();
};

ObjectPrx.prototype.ice_getCachedConnection = function()
{
    if(this._handler !== null)
    {
        return this._handler.getConnection();
    }
    return null;
};

ObjectPrx.prototype.ice_flushBatchRequests = function()
{
    // TODO
    return null;
};

ObjectPrx.prototype.equals = function(r)
{
    if(this === r)
    {
        return true;
    }

    if(r instanceof ObjectPrx)
    {
        return this._reference.equals(r._reference);
    }

    return false;
};

ObjectPrx.prototype.__reference = function()
{
    return this._reference;
};

ObjectPrx.prototype.__copyFrom = function(from)
{
    Debug.assert(this._reference === null);
    Debug.assert(this._handler === null);

    this._reference = from._reference;

    if(this._reference.getCacheConnection())
    {
        this._handler = from._handler;
    }
};

ObjectPrx.prototype.__handleException = function(handler, ex, interval, cnt)
{
    if(handler === this._handler)
    {
        this._handler = null;
    }

    if(cnt == -1) // Don't retry if the retry count is -1.
    {
        throw ex;
    }

    try
    {
        return this._reference.getInstance().proxyFactory().checkRetryAfterException(ex, this._reference, interval,
                                                                                     cnt);
    }
    catch(e)
    {
        if(e instanceof LocalEx.CommunicatorDestroyedException)
        {
            //
            // The communicator is already destroyed, so we cannot
            // retry.
            //
            throw ex;
        }
        else
        {
            throw e;
        }
    }
};

ObjectPrx.prototype.__handleExceptionWrapper = function(handler, ex)
{
    if(handler === this._handler)
    {
        this._handler = null;
    }

    if(!ex.retry)
    {
        throw ex.inner;
    }
};

ObjectPrx.prototype.__handleExceptionWrapperRelaxed = function(handler, ex, interval, cnt)
{
    if(!ex.retry)
    {
        return this.__handleException(handler, ex.inner, interval, cnt);
    }
    else
    {
        if(handler === this._handler)
        {
            this._handler = null;
        }

        return cnt;
    }
};

ObjectPrx.prototype.__checkAsyncTwowayOnly = function(name)
{
    if(!this.ice_isTwoway())
    {
        throw new Error("`" + name + "' can only be called with a twoway proxy");
    }
};

//
// Completed callback for operations that have no return value or out parameters.
//
ObjectPrx.__completed = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    __r.__readEmptyParams();
    __r.succeed(__r);
};

//
// Completed callback for operations that return a bool as the only result.
//
ObjectPrx.__completed_bool = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readBool();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return a byte as the only result.
//
ObjectPrx.__completed_byte = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readByte();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return a short as the only result.
//
ObjectPrx.__completed_short = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        var __ret = __is.readShort();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return an int as the only result.
//
ObjectPrx.__completed_int = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readInt();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return a long as the only result.
//
ObjectPrx.__completed_long = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readLong();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return a float as the only result.
//
ObjectPrx.__completed_float = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readFloat();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return a double as the only result.
//
ObjectPrx.__completed_double = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readDouble();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return a string as the only result.
//
ObjectPrx.__completed_string = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readString();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return a proxy as the only result.
//
ObjectPrx.__completed_ObjectPrx = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret;
    try
    {
        __ret = __is.readProxy();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret);
};

//
// Completed callback for operations that return an object as the only result.
//
ObjectPrx.__completed_Object = function(__r)
{
    if(!ObjectPrx.__check(__r))
    {
        return;
    }

    var __is = __r.__startReadParams();
    var __ret = { value: null };
    try
    {
        __ret = __is.readObject(__ret);
        __is.readPendingObjects();
        __r.__endReadParams();
    }
    catch(__ex)
    {
        ObjectPrx.__dispatchLocalException(__r, __ex);
        return;
    }
    __r.succeed(__r, __ret.value);
};

//
// Check function for operations that declare no user exceptions.
//
ObjectPrx.__check = function(__r)
{
    try
    {
        __r.__throwUserException();
    }
    catch(ex)
    {
        if(ex instanceof Ex.UserException)
        {
            __r.fail(new LocalEx.UnknownUserException(ex.ice_name()));
            return false;
        }
        else if(ex instanceof Ex.LocalException)
        {
            __r.fail(ex);
            return false;
        }
        else
        {
            throw ex;
        }
    }

    return true;
};

ObjectPrx.prototype.__getRequestHandler = function()
{
    if(this._reference.getCacheConnection())
    {
        if(this._handler !== null)
        {
            return this._handler;
        }
        this._handler = this.__createRequestHandler();
        return this._handler;
    }
    else
    {
        return this.__createRequestHandler();
    }
};

ObjectPrx.prototype.__setRequestHandler = function(handler)
{
    if(this._reference.getCacheConnection())
    {
        this._handler = handler;
    }
};

ObjectPrx.prototype.__createRequestHandler = function()
{
    var handler = new ConnectRequestHandler(this._reference, this);
    return handler.connect();
};

//
// Only for use by IceInternal.ProxyFactory
//
ObjectPrx.prototype.__setup = function(ref)
{
    Debug.assert(this._reference === null);
    //Debug.assert(this._delegate === null); // TODO

    this._reference = ref;
};

ObjectPrx.prototype.__newInstance = function(ref)
{
    var proxy = new (Object.getPrototypeOf(this)).constructor();
    proxy.__setup(ref);
    return proxy;
};

ObjectPrx.prototype.__handleLocalException = function(__r, __ex)
{
    if(__ex instanceof Ex.LocalException)
    {
        __r.__exception(__ex);
    }
    else
    {
        throw __ex;
    }
};

ObjectPrx.__dispatchLocalException = function(__r, __ex)
{
    if(__ex instanceof Ex.LocalException)
    {
        __r.fail(__ex);
    }
    else
    {
        throw __ex;
    }
};

//
// NOT a prototype function
//
ObjectPrx.checkedCast = function(prx, facet, ctx)
{
    var __promise = null;
    if(prx === undefined || prx === null)
    {
        __promise = new AsyncResult(null, "checkedCast", null, null, null, null);
        __promise.succeed(__promise, null);
    }
    else
    {
        if(facet === undefined)
        {
            __promise = new AsyncResult(null, "checkedCast", null, prx, null, null);
            __promise.succeed(__promise, prx);
        }
        else
        {
            var __bb = prx.ice_facet(facet);
            var __h = new ObjectPrx();
            __h.__copyFrom(__bb);
            __promise = new AsyncResult(prx.ice_getCommunicator(), "checkedCast", null, __h, null, null);
            __bb.ice_isA("::Ice::Object", ctx).then(
                function(__r, __ret)
                {
                    __promise.succeed(__promise, __ret ? __h : null);
                },
                function(__r, __ex)
                {
                    if(__ex instanceof LocalEx.FacetNotExistException)
                    {
                        __promise.succeed(__promise, null);
                    }
                    else
                    {
                        __promise.fail(__promise, __ex);
                    }
                });
        }
    }
    return __promise;
};

//
// NOT a prototype function
//
ObjectPrx.uncheckedCast = function(prx, facet)
{
    var r = null;
    if(prx !== undefined && prx !== null)
    {
        var bb = prx.ice_facet(facet);
        var h = new ObjectPrx();
        h.__copyFrom(bb);
        r = h;
    }
    return r;
};

module.exports = ObjectPrx;
