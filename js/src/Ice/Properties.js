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
var Promise = require("./Promise");
var PropertyNames = require("./PropertyNames");
var Debug = require("./Debug");

var fs = require("fs");

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
        for(var e = defaults._properties.entries; e !== null; e = e.next)
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
};

Properties.prototype.getProperty = function(key)
{
    return this.getPropertyWithDefault(key, "");
};

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
};

Properties.prototype.getPropertyAsInt = function(key)
{
    return this.getPropertyAsIntWithDefault(key, 0);
};

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
};

Properties.prototype.getPropertyAsList = function(key)
{
    return this.getPropertyAsListWithDefault(key, 0);
};

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
        if(!result)
        {
            // TODO
            //Ice.Util.getProcessLogger().warning("mismatched quotes in property " + key
            //                                    + "'s value, returning default value");
            return value;
        }
        if(result.length === 0)
        {
            result = value;
        }
        return result;
    }
    else
    {
        return value;
    }
};

Properties.prototype.getPropertiesForPrefix = function(prefix)
{
    var result = new HashMap();
    for(var e = this._properties.entries; e !== null; e = e.next)
    {
        if(prefix === undefined || prefix === null || e.key.indexOf(prefix) === 0)
        {
            e.value.used = true;
            result.set(e.key, e.value.value);
        }
    }
    return result;
};

Properties.prototype.setProperty = function(key, value)
{
    //
    // Trim whitespace
    //
    if(key)
    {
        key = key.trim();
    }

    //
    // Check if the property is legal.
    //
    //TODO
    //var logger = Util.getProcessLogger(); 
    if(!key || key.length === 0)
    {
        throw new LocalEx.InitializationException("Attempt to set property with empty key");
    }

    var dotPos = key.indexOf(".");
    if(dotPos !== -1)
    {
        var prefix = key.substr(0, dotPos);
        for(var i = 0; i < PropertyNames.validProps.length; ++i)
        {
            var pattern = PropertyNames.validProps[i][0].pattern();
            dotPos = pattern.indexOf(".");
            //
            // Each top level prefix describes a non-empty namespace. Having a string without a
            // prefix followed by a dot is an error.
            //
            //TODO is possible to assert only in Debug mode?
            Debug.assert(dotPos != -1);
            var propPrefix = pattern.substring(0, dotPos - 1);
            if(propPrefix != prefix)
            {
                continue;
            }
            var found = false;
            for(var j = 0; j < PropertyNames.validProps[i][j].length && !found; ++j)
            {
                pattern = PropertyNames.validProps[i][j].pattern();
                var pComp = new RegExp(pattern);
                found = pComp.test(key);

                if(found && PropertyNames.validProps[i][j].deprecated())
                {
                    //TODO
                    console.log("deprecated property: " + key);
                    //logger.warning("deprecated property: " + key);
                    if(PropertyNames.validProps[i][j].deprecatedBy())
                    {
                        key = PropertyNames.validProps[i][j].deprecatedBy();
                    }
                }
            }

            //
            // TODO Add mismatchCase checks as we do in Java/C++/.NET
            if(!found)
            {
                console.log("unknown property: " + key);
                //logger.warning("unknown property: " + key);
            }
            else
            {
                break;
            }
        }
    }

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
};

Properties.prototype.getCommandLineOptions = function()
{
    var result = [];
    for(var e = this._properties.entries; e !== null; e = e.next)
    {
        result.push("--" + e.key + "=" + e.pv.value);
    }
    return result;
};

Properties.prototype.parseCommandLineOptions = function(pfx, options)
{
    if(pfx.length > 0 && pfx.charAt(pfx.length - 1) != ".")
    {
        pfx += ".";
    }
    pfx = "--" + pfx;

    var result = [];
    
    var self = this;
    options.forEach(
        function(opt)
        {
            if(opt.indexOf(pfx) === 0)
            {
                if(opt.indexOf('=') === -1)
                {
                    opt += "=1";
                }

                self.parseLine(opt.substring(2));
            }
            else
            {
                result.push(opt);
            }
        });
    return result;
};

Properties.prototype.parseIceCommandLineOptions = function(options)
{
    var args = options.slice();
    for(var i = 0; i < PropertyNames.clPropNames.length; ++i)
    {
        args = this.parseCommandLineOptions(PropertyNames.clPropNames[i], args);
    }
    return args;
};

Properties.prototype.load = function(file)
{
    var promise = new Promise();
    var self = this;
    fs.readFile(file, {encoding: "utf8"},
        function(err, data)
        {
            if(err)
            {
                promise.fail(err);
            }
            else
            {
                try
                {
                    self.parse(data);
                    promise.succeed();
                }
                catch(ex)
                {
                    promise.fail(ex);
                }
            }
        });
    return promise;
};

Properties.prototype.parse = function(data)
{
    var lines = data.match(/[^\r\n]+/g);
    
    var line;
    
    while((line = lines.shift()))
    {
        this.parseLine(line);
    }
};

Properties.ParseStateKey = 0;
Properties.ParseStateValue = 1;
    
Properties.prototype.parseLine = function(line)
{
    var key = "";
    var value = "";

    var state = Properties.ParseStateKey;

    var whitespace = "";
    var escapedspace = "";
    var finished = false;
    
    for(var i = 0; i < line.length; ++i)
    {
        var c = line.charAt(i);
        switch(state)
        {
            case Properties.ParseStateKey:
            {
                switch(c)
                {
                    case '\\':
                        if(i < line.length - 1)
                        {
                            c = line.charAt(++i);
                            switch(c)
                            {
                                case '\\':
                                case '#':
                                case '=':
                                    key += whitespace;
                                    whitespace = "";
                                    key += c;
                                    break;

                                case ' ':
                                    if(key.length !== 0)
                                    {
                                        whitespace += c;
                                    }
                                    break;

                                default:
                                    key += whitespace;
                                    whitespace = "";
                                    key += '\\';
                                    key += c;
                                    break;
                            }
                        }
                        else
                        {
                            key += whitespace;
                            key += c;
                        }
                        break;

                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                        if(key.length !== 0)
                        {
                            whitespace += c;
                        }
                        break;

                    case '=':
                        whitespace = "";
                        state = Properties.ParseStateValue;
                        break;

                    case '#':
                        finished = true;
                        break;

                    default:
                        key += whitespace;
                        whitespace = "";
                        key += c;
                        break;
                }
                break;
            }

            case Properties.ParseStateValue:
            {
                switch(c)
                {
                    case '\\':
                        if(i < line.length - 1)
                        {
                            c = line.charAt(++i);
                            switch(c)
                            {
                                case '\\':
                                case '#':
                                case '=':
                                    value += value.length === 0 ? escapedspace : whitespace;
                                    whitespace = "";
                                    escapedspace = "";
                                    value += c;
                                    break;

                                case ' ':
                                    whitespace += c;
                                    escapedspace += c;
                                    break;

                                default:
                                    value += value.length === 0 ? escapedspace : whitespace;
                                    whitespace = "";
                                    escapedspace = "";
                                    value += '\\';
                                    value += c;
                                    break;
                            }
                        }
                        else
                        {
                            value += value.length === 0 ? escapedspace : whitespace;
                            value += c;
                        }
                        break;

                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                        if(value.length !== 0)
                        {
                            whitespace += c;
                        }
                        break;

                    case '#':
                        value += escapedspace;
                        finished = true;
                        break;

                    default:
                        value += value.length === 0 ? escapedspace : whitespace;
                        whitespace = "";
                        escapedspace = "";
                        value += c;
                        break;
                }
                break;
            }
        }
        if(finished)
        {
            break;
        }
    }
    value += escapedspace;

    if((state === Properties.ParseStateKey && key.length !== 0) || (state == Properties.ParseStateValue && key.length === 0))
    {
        console.log("invalid config file entry: \"" + line + "\"");
        // TODO
        //Util.getProcessLogger().warning("invalid config file entry: \"" + line + "\"");
        return;
    }
    else if(key.length === 0)
    {
        return;
    }
    
    this.setProperty(key, value);
};

Properties.prototype.clone = function()
{
    return new Properties(null, this);
};

Properties.prototype.getUnusedProperties = function()
{
    var unused = [];
    for(var e = this._properties.entries; e !== null; e = e.next)
    {
        if(!e.pv.used)
        {
            unused.push(e.key);
        }
    }
    return unused;
};

Properties.createProperties = function(args, defaults)
{
    return new Properties(args, defaults);
};

module.exports = Properties;
