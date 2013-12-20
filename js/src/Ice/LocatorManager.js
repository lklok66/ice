// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var HashMap = require("./HashMap").Ice.HashMap;
var LocatorInfo = require("./LocatorInfo").Ice.LocatorInfo;
var LocatorTable = require("./LocatorTable").Ice.LocatorTable;
var LocatorPrx = require("./Locator").Ice.LocatorPrx;

var LocatorManager = function(properties)
{
    this._background = properties.getPropertyAsInt("Ice.BackgroundLocatorCacheUpdates") > 0;

    this._table = new HashMap(); // Map<Ice.LocatorPrx, LocatorInfo>
    this._table.comparator = HashMap.compareEquals;
    this._locatorTables = new HashMap(); // Map<Ice.Identity, LocatorTable>
    this._locatorTables.comparator = HashMap.compareEquals;
};

LocatorManager.prototype.destroy = function()
{
    for(var e = this._table.entries; e !== null; e = e.next)
    {
        e.value.destroy();
    }
    this._table.clear();
    this._locatorTables.clear();
};

//
// Returns locator info for a given locator. Automatically creates
// the locator info if it doesn't exist yet.
//
LocatorManager.prototype.find = function(loc)
{
    if(loc === null)
    {
        return null;
    }

    //
    // The locator can't be located.
    //
    var locator = LocatorPrx.uncheckedCast(loc.ice_locator(null));

    //
    // TODO: reap unused locator info objects?
    //

    var info = this._table.get(locator);
    if(info === undefined)
    {
        //
        // Rely on locator identity for the adapter table. We want to
        // have only one table per locator (not one per locator
        // proxy).
        //
        var table = this._locatorTables.get(locator.ice_getIdentity());
        if(table === undefined)
        {
            table = new LocatorTable();
            this._locatorTables.set(locator.ice_getIdentity(), table);
        }

        info = new LocatorInfo(locator, table, this._background);
        this._table.set(locator, info);
    }

    return info;
};

module.exports.Ice = {};
module.exports.Ice.LocatorManager = LocatorManager;
