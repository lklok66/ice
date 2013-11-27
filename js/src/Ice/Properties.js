// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var StringUtil = require("./StringUtil");
var HashMap = require("./HashMap");
var LocalEx = require("./LocalException").Ice;

//
// Ice.Properties
//
var Properties = function(args, defaults)
{
    this._properties = new HashMap();

    if(defaults !== undefined && defaults !== null)
    {
        //
        // NOTE: we can't just do a shallow copy of the map as the map values
        // would otherwise be shared between the two PropertiesI object.
        //
        //_properties = new HashMap(pi._properties);
        for(var e = defaults._properties.entries; e != null; e = e.next)
        {
            this._properties.set(e.key, { 'value': e.value.value, 'used': false });
        }
    }

    if(args !== undefined && args !== null)
    {
        var v = this.parseIceCommandLineOptions(args);
        args.length = 0;
        for(var i = 0; i < v.length; ++i)
        {
            args.push(v[i]);
        }
    }
}

Properties.prototype.getProperty = function(key)
{
    return this.getPropertyWithDefault(key, "");
}

Properties.prototype.getPropertyWithDefault = function(key, value)
{
    var pv = this._properties.get(key);
    if(pv !== undefined)
    {
        pv.used = true;
        return pv.value;
    }
    else
    {
        return value;
    }
}

Properties.prototype.getPropertyAsInt = function(key)
{
    return this.getPropertyAsIntWithDefault(key, 0);
}

Properties.prototype.getPropertyAsIntWithDefault = function(key, value)
{
    var pv = this._properties.get(key);
    if(pv !== undefined)
    {
        pv.used = true;
        return parseInt(pv.value);
    }
    else
    {
        return value;
    }
}

Properties.prototype.getPropertyAsList = function(key)
{
    return this.getPropertyAsListWithDefault(key, 0);
}

Properties.prototype.getPropertyAsListWithDefault = function(key, value)
{
    if(value === undefined || value === null)
    {
        value = [];
    }

    var pv = this._properties.get(key);
    if(pv !== undefined)
    {
        pv.used = true;

        var result = StringUtil.splitString(pv.value, ", \t\r\n");
        if(result == null)
        {
            // TODO
            //Ice.Util.getProcessLogger().warning("mismatched quotes in property " + key
            //                                    + "'s value, returning default value");
            return value;
        }
        if(result.length == 0)
        {
            result = value;
        }
        return result;
    }
    else
    {
        return value;
    }
}

Properties.prototype.getPropertiesForPrefix = function(prefix)
{
    var result = new HashMap();
    for(var e = this._properties.entries; e != null; e = e.next)
    {
        if(prefix === undefined || prefix === null || e.key.indexOf(prefix) === 0)
        {
            e.value.used = true;
            result.set(e.key, e.value.value);
        }
    }
    return result;
}

Properties.prototype.setProperty = function(key, value)
{
    //
    // Trim whitespace
    //
    if(key !== null)
    {
        key = key.trim();
    }

    //
    // Check if the property is legal.
    //
    //var logger = Util.getProcessLogger();
    var logger = null; // TODO
    if(key === undefined || key === null || key.length === 0)
    {
        throw new LocalEx.InitializationException("Attempt to set property with empty key");
    }

    /* TODO
    var dotPos = key.indexOf(".");
    if(dotPos !== -1)
    {
        var prefix = key.substr(0, dotPos);
        for(var i:int = 0; IceInternal.PropertyNames.validProps[i] != null; ++i)
        {
            var pattern:String = IceInternal.PropertyNames.validProps[i][0].pattern();
            dotPos = pattern.indexOf(".");
            //
            // Each top level prefix describes a non-empty namespace. Having a string without a
            // prefix followed by a dot is an error.
            //
            CONFIG::debug { Debug.assert(dotPos != -1); }
            var propPrefix:String = pattern.substring(0, dotPos - 1);
            if(propPrefix != prefix)
            {
                continue;
            }

            var found:Boolean = false;
            for(var j:int = 0; IceInternal.PropertyNames.validProps[i][j] != null && !found; ++j)
            {
                pattern = IceInternal.PropertyNames.validProps[i][j].pattern();
                var pComp:RegExp = new RegExp(pattern);
                found = pComp.test(key);

                if(found && IceInternal.PropertyNames.validProps[i][j].deprecated())
                {
                    logger.warning("deprecated property: " + key);
                    if(IceInternal.PropertyNames.validProps[i][j].deprecatedBy() != null)
                    {
                        key = IceInternal.PropertyNames.validProps[i][j].deprecatedBy();
                    }
                }
            }

            if(!found)
            {
                logger.warning("unknown property: " + key);
            }
            else
            {
                break;
            }
        }
    }
    */

    //
    // Set or clear the property.
    //
    if(value !== undefined && value !== null && value.length > 0)
    {
        var pv = this._properties.get(key);
        if(pv !== undefined)
        {
            pv.value = value;
        }
        else
        {
            this._properties.set(key, { 'value': value, 'used': false });
        }
    }
    else
    {
        this._properties.delete(key);
    }
}

Properties.prototype.getCommandLineOptions = function()
{
    // TODO
}

Properties.prototype.parseCommandLineOptions = function(pfx, options)
{
    // TODO
}

Properties.prototype.parseIceCommandLineOptions = function(options)
{
    // TODO
}

Properties.prototype.load = function(file)
{
    // TODO
}

Properties.prototype.clone = function(file)
{
    // TODO
}

Properties.prototype.getUnusedProperties = function()
{
    // TODO
}

Properties.createProperties = function(args, defaults)
{
    return new Properties(args, defaults);
}

module.exports = Properties;
