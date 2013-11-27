// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Debug = require("./Debug");
var OpaqueEndpointI = require("./OpaqueEndpointI");

var LocalEx = require("./LocalException").Ice;

var EndpointFactoryManager = function(instance)
{
    this._instance = instance;
    this._factories = [];
}

EndpointFactoryManager.prototype.add = function(factory)
{
    for(var i = 0; i < this._factories.length; ++i)
    {
        Debug.assert(this._factories[i].type() != factory.type());
    }

    this._factories.push(factory);
}

EndpointFactoryManager.prototype.get = function(type)
{
    for(var i = 0; i < this._factories.length; ++i)
    {
        if(this._factories[i].type() === type)
        {
            return this._factories[i];
        }
    }
    return null;
}

EndpointFactoryManager.prototype.create = function(str, oaEndpoint)
{
    var s = str.trim();
    if(s.length === 0)
    {
        throw new LocalEx.EndpointParseException("value has no non-whitespace characters");
    }

    var protocol;
    var rest = "";

    var pos = s.search(/[ \t\n\r]+/);
    if(pos === -1)
    {
        protocol = s;
    }
    else
    {
        protocol = s.substring(0, pos);
        if(pos < s.length)
        {
            rest = s.substring(pos);
        }
    }

    if(protocol === "default")
    {
        protocol = this._instance.defaultsAndOverrides().defaultProtocol;
    }

    var f;
    for(var i = 0; i < this._factories.length; ++i)
    {
        if(this._factories[i].protocol() === protocol)
        {
            return this._factories[i].create(rest, oaEndpoint);
        }
    }

    //
    // If the stringified endpoint is opaque, create an unknown endpoint,
    // then see whether the type matches one of the known endpoints.
    //
    if(protocol === "opaque")
    {
        var ue = OpaqueEndpointI.fromString(rest);
        for(var i = 0; i < this._factories.length; ++i)
        {
            if(this._factories[i].type() == ue.type())
            {
                //
                // Make a temporary stream, write the opaque endpoint data into the stream,
                // and ask the factory to read the endpoint data from that stream to create
                // the actual endpoint.
                //
                var bs = new BasicStream(this._instance, true);
                ue.streamWrite(bs);
                bs.pos = 0;
                bs.readShort(); // type
                return this._factories[i].read(bs);
            }
        }
        return ue; // Endpoint is opaque, but we don't have a factory for its type.
    }

    return null;
}

EndpointFactoryManager.prototype.read = function(s)
{
    var type = s.readShort();

    for(var i = 0; i < this._factories.length; ++i)
    {
        if(this._factories[i].type() == type)
        {
            return this._factories[i].read(s);
        }
    }
    return OpaqueEndpointI.fromStream(type, s);
}

EndpointFactoryManager.prototype.destroy = function()
{
    for(var i = 0; i < this._factories.length; ++i)
    {
        this._factories[i].destroy();
    }
    _factories = [];
}

module.exports = EndpointFactoryManager;
