// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var HashMap = require("./HashMap");
var RouterInfo = require("./RouterInfo");

var RouterPrx = require("./Router").Ice.RouterPrx;

var RouterManager = function()
{
    this._table = new HashMap(); // Map<Ice.RouterPrx, RouterInfo>
    this._table.comparator = function(p1, p2) { return p1.equals(p2); };
}

RouterManager.prototype.destroy = function()
{
    for(var e = this._table.entries; e != null; e = e.next)
    {
        e.destroy();
    }
    this._table.clear();
}

//
// Returns router info for a given router. Automatically creates
// the router info if it doesn't exist yet.
//
RouterManager.prototype.find = function(rtr)
{
    if(rtr === null)
    {
        return null;
    }

    //
    // The router cannot be routed.
    //
    var router = RouterPrx.uncheckedCast(rtr.ice_router(null));

    var info = this._table.get(router);
    if(info === undefined)
    {
        info = new RouterInfo(router);
        this._table.put(router, info);
    }
    else
    {
        info = null;
    }

    return info;
}

RouterManager.prototype.erase = function(rtr)
{
    var info = null;
    if(rtr !== null)
    {
        // The router cannot be routed.
        var router = RouterPrx.uncheckedCast(rtr.ice_router(null));

        info = this._table.get(router);
        this._table.delete(router);
    }
    return info;
}

module.exports = RouterManager;
