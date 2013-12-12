// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var HashMap = require("./HashMap");

var LocalEx = require("./LocalException").Ice;

//
// The base class for all ImplicitContext implementations
//
var ImplicitContextI = function()
{
    this._context = new HashMap();
};

ImplicitContextI.create = function(kind)
{
    if(kind.length === 0 || kind === "None")
    {
        return null;
    }
    else if(kind === "Shared")
    {
        return new ImplicitContextI();
    }
    else
    {
        throw new LocalEx.InitializationException("'" + kind + "' is not a valid value for Ice.ImplicitContext");
    }
};

ImplicitContextI.prototype.getContext = function()
{
    return new HashMap(this._context);
};

ImplicitContextI.prototype.setContext = function(context)
{
    if(context !== null && context.size > 0)
    {
        this._context = new HashMap(context);
    }
    else
    {
        this._context.clear();
    }
};

ImplicitContextI.prototype.containsKey = function(key)
{
    if(key === null)
    {
        key = "";
    }

    return this._context.has(key);
};

ImplicitContextI.prototype.get = function(key)
{
    if(key === null)
    {
        key = "";
    }

    var val = this._context.get(key);
    if(val === null)
    {
        val = "";
    }

    return val;
};

ImplicitContextI.prototype.put = function(key, value)
{
    if(key === null)
    {
        key = "";
    }
    if(value === null)
    {
        value = "";
    }

    var oldVal = this._context.get(key);
    if(oldVal === null)
    {
        oldVal = "";
    }

    this._context.set(key, value);

    return oldVal;
};

ImplicitContextI.prototype.remove = function(key)
{
    if(key === null)
    {
        key = "";
    }

    var val = this._context.get(key);
    this._context.delete(key);

    if(val === null)
    {
        val = "";
    }
    return val;
};

ImplicitContextI.prototype.write = function(prxContext, os)
{
    if(prxContext.size === 0)
    {
        ContextHelper.write(os, this._context);
    }
    else
    {
        var ctx = null;
        if(this._context.size === 0)
        {
            ctx = prxContext;
        }
        else
        {
            ctx = new HashMap(this._context);
            ctx.merge(prxContext);
        }
        ContextHelper.write(os, ctx);
    }
};

module.exports = ImplicitContextI;
