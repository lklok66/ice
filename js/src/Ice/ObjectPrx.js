// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var global = this;
    require("Ice/ArrayUtil");
    require("Ice/AsyncResult");
    require("Ice/ConnectRequestHandler");
    require("Ice/Debug");
    require("Ice/FormatType");
    require("Ice/HashMap");
    require("Ice/OutgoingAsync");
    require("Ice/ProxyBatchOutgoingAsync");
    require("Ice/ReferenceMode");
    require("Ice/Current");
    require("Ice/Exception");
    require("Ice/BuiltinSequences");
    require("Ice/LocalException");
    require("Ice/Object");

    var Ice = global.Ice || {};
    var Slice = global.Slice || {};
    
    var ArrayUtil = Ice.ArrayUtil;
    var AsyncResultBase = Ice.AsyncResultBase;
    var AsyncResult = Ice.AsyncResult;
    var ConnectRequestHandler = Ice.ConnectRequestHandler;
    var Debug = Ice.Debug;
    var FormatType = Ice.FormatType;
    var HashMap = Ice.HashMap;
    var OutgoingAsync = Ice.OutgoingAsync;
    var ProxyBatchOutgoingAsync = Ice.ProxyBatchOutgoingAsync;
    var RefMode = Ice.ReferenceMode;
    var OperationMode = Ice.OperationMode;

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

    ObjectPrx.prototype.ice_isA = function(__id, __ctx)
    {
        return ObjectPrx.__invoke(this, "ice_isA", 1, 0, __ctx,
            function(__os)
            {
                __os.writeString(__id);
            },
            ObjectPrx.__returns_bool, null);
    };

    ObjectPrx.prototype.ice_ping = function(__ctx)
    {
        return ObjectPrx.__invoke(this, "ice_ping", 1, 0, __ctx, null, null, null);
    };

    ObjectPrx.prototype.ice_ids = function(__ctx)
    {
        return ObjectPrx.__invoke(this, "ice_ids", 1, 0, __ctx, null,
            function(__is, __results)
            {
                __results.push(Ice.StringSeqHelper.read(__is));
            }, null);
    };

    ObjectPrx.prototype.ice_id = function(__ctx)
    {
        return ObjectPrx.__invoke(this, "ice_id", 1, 0, __ctx, null, ObjectPrx.__returns_string, null);
    };

    ObjectPrx.prototype.ice_getIdentity = function()
    {
        return this._reference.getIdentity().clone();
    };

    ObjectPrx.prototype.ice_identity = function(newIdentity)
    {
        if(newIdentity === undefined || newIdentity === null || newIdentity.name.length === 0)
        {
            throw new Ice.IllegalIdentityException();
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
        var __r = new AsyncResultBase(this._reference.getCommunicator(), "ice_getConnection", null, this, null);
        this.__getRequestHandler().onConnection(__r);
        return __r;
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
        var __r = new ProxyBatchOutgoingAsync(this, "ice_flushBatchRequests");
        try
        {
            __r.__send();
        }
        catch(__ex)
        {
            this.__handleLocalException(__r, __ex);
        }
        return __r;
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
        if(this._handler !== null && handler._connection === this._handler._connection)
        {
            this._handler = null;
        }

        if(cnt == -1) // Don't retry if the retry count is -1.
        {
            throw ex;
        }

        try
        {
            return this._reference.getInstance().proxyFactory().checkRetryAfterException(ex, this._reference,
                                                                                            interval, cnt);
        }
        catch(e)
        {
            if(e instanceof Ice.CommunicatorDestroyedException)
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
        if(this._handler !== null && handler._connection === this._handler._connection)
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
            if(this._handler !== null && handler._connection === this._handler._connection)
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
    // Generic invocation for operations that have input parameters.
    //
    ObjectPrx.__invoke = function(p, name, modeNum, fmtNum, ctx, marshalFn, unmarshalFn, userEx)
    {
        var mode = OperationMode.valueOf(modeNum);
        var fmt = FormatType.valueOf(fmtNum);

        if(unmarshalFn !== null || userEx !== null)
        {
            p.__checkAsyncTwowayOnly(name);
        }

        var __r = new OutgoingAsync(p, name,
            function(__res)
            {
                ObjectPrx.__completed(__res, unmarshalFn, userEx);
            });

        try
        {
            __r.__prepare(name, mode, ctx);
            if(marshalFn === null)
            {
                __r.__writeEmptyParams();
            }
            else
            {
                var __os = __r.__startWriteParams(fmt);
                marshalFn(__os);
                __r.__endWriteParams();
            }
            __r.__send();
        }
        catch(ex)
        {
            p.__handleLocalException(__r, ex);
        }
        return __r;
    };
    
    ObjectPrx.prototype.ice_invoke = function(operation, mode, inParams, ctx, explicitCtx)
    {
        if(explicitCtx && ctx === null)
        {
            ctx = new Ice.HashMap();
        }
        
        var self = this;
        
        var completedFn = function(__res)
            {
                try
                {
                    var results = [__res];
                    if((__r._state & AsyncResult.OK) === 0)
                    {
                        results.push(false);
                    }
                    else
                    {
                        results.push(true);
                    }
                    if(self._reference.getMode() === Ice.ReferenceMode.ModeTwoway)
                    {
                        results.push(__res._is.readEncaps(null));
                    }
                    __res.succeed.apply(__res, results);
                }
                catch(ex)
                {
                    ObjectPrx.__dispatchLocalException(__res, ex);
                    return;
                }
            };
            
        var __r = new OutgoingAsync(this, operation, completedFn, completedFn);
        
        try
        {
            __r.__prepare(operation, mode, ctx);
            __r.__writeParamEncaps(inParams);
            __r.__send();
        }
        catch(ex)
        {
            this.__handleLocalException(__r, ex);
        }
        return __r;
    };

    //
    // Handles the completion of an invocation.
    //
    ObjectPrx.__completed = function(__r, unmarshalFn, userEx)
    {
        if(!ObjectPrx.__check(__r, userEx))
        {
            return;
        }

        try
        {
            if(unmarshalFn === null)
            {
                __r.__readEmptyParams();
                __r.succeed(__r);
            }
            else
            {
                var __is = __r.__startReadParams();
                var results = [__r];
                unmarshalFn(__is, results);
                __r.__endReadParams();
                __r.succeed.apply(__r, results);
            }
        }
        catch(ex)
        {
            ObjectPrx.__dispatchLocalException(__r, ex);
            return;
        }
    };

    //
    // Unmarshal callback for operations that return a bool as the only result.
    //
    ObjectPrx.__returns_bool = function(__is, __results)
    {
        __results.push(__is.readBool());
    };

    //
    // Unmarshal callback for operations that return a byte as the only result.
    //
    ObjectPrx.__returns_byte = function(__is, __results)
    {
        __results.push(__is.readByte());
    };

    //
    // Unmarshal callback for operations that return a short as the only result.
    //
    ObjectPrx.__returns_short = function(__is, __results)
    {
        __results.push(__is.readShort());
    };

    //
    // Unmarshal callback for operations that return an int as the only result.
    //
    ObjectPrx.__returns_int = function(__is, __results)
    {
        __results.push(__is.readInt());
    };

    //
    // Unmarshal callback for operations that return a long as the only result.
    //
    ObjectPrx.__returns_long = function(__is, __results)
    {
        __results.push(__is.readLong());
    };

    //
    // Unmarshal callback for operations that return a float as the only result.
    //
    ObjectPrx.__returns_float = function(__is, __results)
    {
        __results.push(__is.readFloat());
    };

    //
    // Unmarshal callback for operations that return a double as the only result.
    //
    ObjectPrx.__returns_double = function(__is, __results)
    {
        __results.push(__is.readDouble());
    };

    //
    // Unmarshal callback for operations that return a string as the only result.
    //
    ObjectPrx.__returns_string = function(__is, __results)
    {
        __results.push(__is.readString());
    };

    //
    // Unmarshal callback for operations that return a proxy as the only result.
    //
    ObjectPrx.__returns_ObjectPrx = function(__is, __results)
    {
        __results.push(__is.readProxy());
    };

    //
    // Unmarshal callback for operations that return an object as the only result.
    //
    ObjectPrx.__returns_Object = function(__is, __results)
    {
        __is.readObject(function(obj){ __results.push(obj); }, Ice.Object);
        __is.readPendingObjects();
    };

    //
    // Handles user exceptions.
    //
    ObjectPrx.__check = function(__r, __uex)
    {
        //
        // If __uex is non-null, it must be an array of exception types.
        //
        try
        {
            __r.__throwUserException();
        }
        catch(ex)
        {
            if(ex instanceof Ice.UserException)
            {
                if(__uex !== null)
                {
                    for(var i = 0; i < __uex.length; ++i)
                    {
                        if(ex instanceof __uex[i])
                        {
                            __r.fail(ex);
                            return false;
                        }
                    }
                }
                __r.fail(new Ice.UnknownUserException(ex.ice_name()));
                return false;
            }
            else
            {
                __r.fail(ex);
                return false;
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

        this._reference = ref;
    };

    ObjectPrx.prototype.__newInstance = function(ref)
    {
        var proxy = new this.constructor();
        proxy.__setup(ref);
        return proxy;
    };

    ObjectPrx.prototype.__handleLocalException = function(__r, __ex)
    {
        if(__ex instanceof Ice.LocalException)
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
        __r.fail(__ex);
    };

    //
    // NOT a prototype function
    //
    ObjectPrx.checkedCastImpl = function(type, id, prx, facet, ctx)
    {
        var __r = null;

        if(prx === undefined || prx === null)
        {
            __r = new AsyncResultBase(null, "checkedCast", null, null, null);
            __r.succeed(__r, null);
        }
        else
        {
            if(facet !== undefined)
            {
                prx = prx.ice_facet(facet);
            }

            var __h = new type();
            __h.__copyFrom(prx);
            __r = new AsyncResultBase(prx.ice_getCommunicator(), "checkedCast", null, __h, null);
            prx.ice_isA(id, ctx).then(
                function(__res, __ret)
                {
                    __r.succeed(__r, __ret ? __h : null);
                }).exception(
                    function(__ex)
                    {
                        if(__ex instanceof Ice.FacetNotExistException)
                        {
                            __r.succeed(__r, null);
                        }
                        else
                        {
                            __r.fail(__ex);
                        }
                    });
        }

        return __r;
    };

    ObjectPrx.ice_staticId = Ice.Object.ice_staticId;

    //
    // NOT a prototype function
    //
    ObjectPrx.checkedCast = function(prx, facet, ctx)
    {
        return ObjectPrx.checkedCastImpl(this, this.ice_staticId(), prx, facet, ctx);
    };

    //
    // NOT a prototype function
    //
    ObjectPrx.uncheckedCast = function(prx, facet)
    {
        return ObjectPrx.uncheckedCastImpl(this, prx, facet);
    };

    //
    // NOT a prototype function
    //
    ObjectPrx.uncheckedCastImpl = function(type, prx, facet)
    {
        var r = null;
        if(prx !== undefined && prx !== null)
        {
            r = new type();
            if(facet !== undefined)
            {
                prx = prx.ice_facet(facet);
            }
            r.__copyFrom(prx);
        }
        return r;
    };

    Object.defineProperty(ObjectPrx, "minWireSize", {
        get: function(){ return 2; }
    });

    ObjectPrx.write = function(os, v)
    {
        os.writeProxy(v);
    };

    ObjectPrx.read = function(is)
    {
        var v = is.readProxy();
        if(v !== null)
        {
            v = this.uncheckedCast(v);
        }
        return v;
    };

    ObjectPrx.writeOpt = function(os, tag, v)
    {
        os.writeOptProxy(tag, v);
    };

    ObjectPrx.readOpt = function(is, tag)
    {
        var v = is.readOptProxy(tag);
        if(v !== null && v !== undefined)
        {
            v = this.uncheckedCast(v);
        }
        return v;
    };
    
    ObjectPrx.__instanceof = function(T)
    {
        if(T === this)
        {
            return true;
        }

        for(var i in this.__implements)
        {
            if(this.__implements[i].__instanceof(T))
            {
                return true;
            }
        }

        if(this.__parent)
        {
            return this.__parent.__instanceof(T);
        }
        return false;
    };

    ObjectPrx.prototype.ice_instanceof = function(T)
    {
        if(T)
        {
            if(this instanceof T)
            {
                return true;
            }
            return this.constructor.__instanceof(T);
        }
        return false;
    };

    Slice.defineProxy = function(base, staticId, prxInterfaces)
    {
        var prx = function()
        {
            base.call(this);
        };
        prx.__parent = base;

        // All generated proxies inherit from ObjectPrx
        prx.prototype = new base();
        prx.prototype.constructor = prx;

        if(prxInterfaces !== undefined)
        {
            // Copy methods from super-interfaces
            prx.__implements = prxInterfaces;
            for(var intf in prxInterfaces)
            {
                var prxInterface = prxInterfaces[intf].prototype;
                for(var f in prxInterface)
                {
                    if(typeof prxInterface[f] == "function" && prx.prototype[f] === undefined)
                    {
                        prx.prototype[f] = prxInterface[f];
                    }
                }
            }
        }

        // Static methods
        prx.ice_staticId = staticId;

        // Copy static methods inherited from ObjectPrx
        prx.checkedCast = ObjectPrx.checkedCast;
        prx.uncheckedCast = ObjectPrx.uncheckedCast;
        prx.write = ObjectPrx.write;
        prx.writeOpt = ObjectPrx.writeOpt;
        prx.read = ObjectPrx.read;
        prx.readOpt = ObjectPrx.readOpt;
        
        prx.__instanceof = ObjectPrx.__instanceof;

        //static properties
        Object.defineProperty(prx, "minWireSize", {
            get: function(){ return 2; }
        });
        return prx;
    };

    Ice.ObjectPrx = ObjectPrx;
    
    global.Slice = Slice;
    global.Ice = Ice;
}());
