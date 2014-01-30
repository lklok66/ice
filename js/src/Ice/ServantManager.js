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
    require("Ice/Debug");
    require("Ice/ExUtil");
    require("Ice/HashMap");
    require("Ice/LocalException");
    require("Ice/StringUtil");

    var Ice = global.Ice || {};

    var Debug = Ice.Debug;
    var ExUtil = Ice.ExUtil;
    var HashMap = Ice.HashMap;
    var StringUtil = Ice.StringUtil;

    //
    // Only for use by Ice.ObjectAdatperI.
    //
    var ServantManager = function(instance, adapterName)
    {
        this._instance = instance;
        this._adapterName = adapterName;
        this._servantMapMap = new HashMap();        // Map<Ice.Identity, Map<String, Ice.Object> >
        this._servantMapMap.keyComparator = HashMap.compareEquals;
        this._defaultServantMap = new HashMap();    // Map<String, Ice.Object>
        this._locatorMap = new HashMap();           // Map<String, Ice.ServantLocator>
    };

    ServantManager.prototype.addServant = function(servant, ident, facet)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        if(facet === null)
        {
            facet = "";
        }

        var m = this._servantMapMap.get(ident);
        if(m === undefined)
        {
            m = new HashMap();
            this._servantMapMap.set(ident, m);
        }
        else
        {
            if(m.has(facet))
            {
                var ex = new Ice.AlreadyRegisteredException();
                ex.id = this._instance.identityToString(ident);
                ex.kindOfObject = "servant";
                if(facet.length > 0)
                {
                    ex.id += " -f " + StringUtil.escapeString(facet, "");
                }
                throw ex;
            }
        }

        m.set(facet, servant);
    };

    ServantManager.prototype.addDefaultServant = function(servant, category)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction

        var obj = this._defaultServantMap.get(category);
        if(obj !== undefined)
        {
            var ex = new Ice.AlreadyRegisteredException();
            ex.kindOfObject = "default servant";
            ex.id = category;
            throw ex;
        }

        this._defaultServantMap.set(category, servant);
    };

    ServantManager.prototype.removeServant = function(ident, facet)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        if(facet === null)
        {
            facet = "";
        }

        var m = this._servantMapMap.get(ident);
        if(m === undefined || !m.has(facet))
        {
            var ex = new Ice.NotRegisteredException();
            ex.id = this._instance.identityToString(ident);
            ex.kindOfObject = "servant";
            if(facet.length > 0)
            {
                ex.id += " -f " + StringUtil.escapeString(facet, "");
            }
            throw ex;
        }

        var obj = m.get(facet);
        m.delete(facet);

        if(m.size === 0)
        {
            this._servantMapMap.delete(ident);
        }

        return obj;
    };

    ServantManager.prototype.removeDefaultServant = function(category)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        var obj = this._defaultServantMap.get(category);
        if(obj === undefined)
        {
            var ex = new Ice.NotRegisteredException();
            ex.kindOfObject = "default servant";
            ex.id = category;
            throw ex;
        }

        this._defaultServantMap.delete(category);
        return obj;
    };

    ServantManager.prototype.removeAllFacets = function(ident)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        var m = this._servantMapMap.get(ident);
        if(m === undefined)
        {
            var ex = new Ice.NotRegisteredException();
            ex.id = this._instance.identityToString(ident);
            ex.kindOfObject = "servant";
            throw ex;
        }

        this._servantMapMap.delete(ident);

        return m;
    }

    ServantManager.prototype.findServant = function(ident, facet)
    {
        //
        // This assert is not valid if the adapter dispatch incoming
        // requests from bidir connections. This method might be called if
        // requests are received over the bidir connection after the
        // adapter was deactivated.
        //
        //Debug.assert(this._instance !== null); // Must not be called after destruction.

        if(facet === null)
        {
            facet = "";
        }

        var m = this._servantMapMap.get(ident);
        var obj = null;
        if(m === undefined)
        {
            obj = this._defaultServantMap.get(ident.category);
            if(obj === undefined)
            {
                obj = this._defaultServantMap.get("");
            }
        }
        else
        {
            obj = m.get(facet);
        }

        return obj === undefined ? null : obj;
    };

    ServantManager.prototype.findDefaultServant = function(category)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        var ds = this._defaultServantMap.get(category);
        return ds === undefined ? null : ds;
    };

    ServantManager.prototype.findAllFacets = function(ident)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        var m = this._servantMapMap.get(ident);
        if(m !== undefined)
        {
            return m.clone();
        }

        return new HashMap();
    };

    ServantManager.prototype.hasServant = function(ident)
    {
        //
        // This assert is not valid if the adapter dispatch incoming
        // requests from bidir connections. This method might be called if
        // requests are received over the bidir connection after the
        // adapter was deactivated.
        //
        //Debug.assert(this._instance !== null); // Must not be called after destruction.

        var m = this._servantMapMap.get(ident);
        if(m === undefined)
        {
            return false;
        }
        else
        {
            Debug.assert(m.size > 0);
            return true;
        }
    };

    ServantManager.prototype.addServantLocator = function(locator, category)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        var l = this._locatorMap.get(category);
        if(l !== undefined)
        {
            var ex = new Ice.AlreadyRegisteredException();
            ex.id = StringUtil.escapeString(category, "");
            ex.kindOfObject = "servant locator";
            throw ex;
        }

        this._locatorMap.set(category, locator);
    }

    ServantManager.prototype.removeServantLocator = function(category)
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.

        var l = this._locatorMap.get(category);
        if(l === undefined)
        {
            var ex = new Ice.NotRegisteredException();
            ex.id = StringUtil.escapeString(category, "");
            ex.kindOfObject = "servant locator";
            throw ex;
        }
        this._locatorMap.delete(category);
        return l;
    };

    ServantManager.prototype.findServantLocator = function(category)
    {
        //
        // This assert is not valid if the adapter dispatch incoming
        // requests from bidir connections. This method might be called if
        // requests are received over the bidir connection after the
        // adapter was deactivated.
        //
        //Debug.assert(this._instance !== null); // Must not be called after destruction.

        var l = this._locatorMap.get(category);
        return l === undefined ? null : l;
    };

    //
    // Only for use by Ice.ObjectAdapterI.
    //
    ServantManager.prototype.destroy = function()
    {
        Debug.assert(this._instance !== null); // Must not be called after destruction.
        var logger = this._instance.initializationData().logger;
        this._servantMapMap.clear();

        var locatorMap = this._locatorMap.clone();
        this._locatorMap.clear();
        this._instance = null;

        for(var e = locatorMap.entries; e !== null; e = e.next)
        {
            var locator = e.value;
            try
            {
                locator.deactivate(e.key);
            }
            catch(ex)
            {
                var s = "exception during locator deactivation:\n" + "object adapter: `" + this._adapterName +
                    "'\n" + "locator category: `" + e.key + "'\n" + ExUtil.toString(ex);
                logger.error(s);
            }
        }
    };

    Ice.ServantManager = ServantManager;
    global.Ice = Ice;
}());
