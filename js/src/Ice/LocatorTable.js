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
    
    require("Ice/Class");
    require("Ice/HashMap");
    require("Ice/TimeUtil");
    require("Ice/Debug");
    
    var Ice = global.Ice || {};
    
    var HashMap = Ice.HashMap;
    var TimeUtil = Ice.TimeUtil;
    var Debug = Ice.Debug;

    var LocatorTable = Ice.__defineClass({
        __init__: function()
        {
            this._adapterEndpointsTable = new HashMap(); // Map<String, EndpointTableEntry>
            this._objectTable = new HashMap(); // Map<Ice.Identity, ReferenceTableEntry>
            this._objectTable.keyComparator = HashMap.compareEquals;
        },
        clear: function()
        {
            this._adapterEndpointsTable.clear();
            this._objectTable.clear();
        },
        getAdapterEndpoints: function(adapter, ttl, cached)
        {
            if(ttl === 0) // Locator cache disabled.
            {
                cached.value = false;
                return null;
            }

            var entry = this._adapterEndpointsTable.get(adapter);
            if(entry !== undefined)
            {
                cached.value = this.checkTTL(entry.time, ttl);
                return entry.endpoints;
            }
            cached.value = false;
            return null;
        },
        addAdapterEndpoints: function(adapter, endpoints)
        {
            this._adapterEndpointsTable.set(adapter, new EndpointTableEntry(TimeUtil.now(), endpoints));
        },
        removeAdapterEndpoints: function(adapter)
        {
            var entry = this._adapterEndpointsTable.get(adapter);
            this._adapterEndpointsTable.delete(adapter);
            return entry !== undefined ? entry.endpoints : null;
        },
        getObjectReference: function(id, ttl, cached)
        {
            if(ttl === 0) // Locator cache disabled.
            {
                cached.value = false;
                return null;
            }

            var entry = this._objectTable.get(id);
            if(entry !== undefined)
            {
                cached.value = this.checkTTL(entry.time, ttl);
                return entry.reference;
            }
            cached.value = false;
            return null;
        },
        addObjectReference: function(id, ref)
        {
            this._objectTable.set(id, new ReferenceTableEntry(TimeUtil.now(), ref));
        },
        removeObjectReference: function(id)
        {
            var entry = this._objectTable.get(id);
            this._objectTable.delete(id);
            return entry !== undefined ? entry.reference : null;
        },
        checkTTL: function(time, ttl)
        {
            Debug.assert(ttl !== 0);
            if(ttl < 0) // TTL = infinite
            {
                return true;
            }
            else
            {
                return TimeUtil.now() - time <= (ttl * 1000);
            }
        }
    });
    
    Ice.LocatorTable = LocatorTable;
    global.Ice = Ice;
    
    var EndpointTableEntry = function(time, endpoints)
    {
        this.time = time;
        this.endpoints = endpoints;
    };

    var ReferenceTableEntry = function(time, reference)
    {
        this.time = time;
        this.reference = reference;
    };
}());
