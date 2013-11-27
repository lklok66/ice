// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
//
// Ice version 3.5.1
//
// <auto-generated>
//
// Generated from file `Endpoint.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

var __ice_Object = require("./Object");

var Ice = (function(_mod_Ice, undefined)
{

    Object.defineProperty(_mod_Ice, 'TCPEndpointType', {
        value: 1
    });

    Object.defineProperty(_mod_Ice, 'UDPEndpointType', {
        value: 3
    });

    /// <summary>
    /// Base class providing access to the endpoint details.
    /// </summary>
    
    _mod_Ice.EndpointInfo = function(timeout, compress)
    {
        __ice_Object.call(this);
        this.timeout = timeout !== undefined ? timeout : 0;
        this.compress = compress !== undefined ? compress : false;
    }
    _mod_Ice.EndpointInfo.prototype = new __ice_Object();
    _mod_Ice.EndpointInfo.prototype.constructor = _mod_Ice.EndpointInfo;
    _mod_Ice.EndpointInfo.prototype.type = __ice_Object.prototype.__notImplemented;
    _mod_Ice.EndpointInfo.prototype.datagram = __ice_Object.prototype.__notImplemented;
    _mod_Ice.EndpointInfo.prototype.secure = __ice_Object.prototype.__notImplemented;

    _mod_Ice.EndpointInfo.prototype.toString = function()
    {
        return "[object Ice::EndpointInfo]";
    }

    /// <summary>
    /// The user-level interface to an endpoint.
    /// </summary>
    
    _mod_Ice.Endpoint = function()
    {
        __ice_Object.call(this);
    }
    _mod_Ice.Endpoint.prototype = new __ice_Object();
    _mod_Ice.Endpoint.prototype.constructor = _mod_Ice.Endpoint;
    _mod_Ice.Endpoint.prototype.toString = __ice_Object.prototype.__notImplemented;
    _mod_Ice.Endpoint.prototype.getInfo = __ice_Object.prototype.__notImplemented;

    _mod_Ice.Endpoint.prototype.toString = function()
    {
        return "[object Ice::Endpoint]";
    }

    /// <summary>
    /// Provides access to the address details of a IP endpoint.
    /// </summary>
    
    _mod_Ice.IPEndpointInfo = function(timeout, compress, host, port)
    {
        _mod_Ice.EndpointInfo.call(this, timeout, compress);
        this.host = host !== undefined ? host : null;
        this.port = port !== undefined ? port : 0;
    }
    _mod_Ice.IPEndpointInfo.prototype = new _mod_Ice.EndpointInfo();
    _mod_Ice.IPEndpointInfo.prototype.constructor = _mod_Ice.IPEndpointInfo;

    _mod_Ice.IPEndpointInfo.prototype.toString = function()
    {
        return "[object Ice::IPEndpointInfo]";
    }

    /// <summary>
    /// Provides access to a TCP endpoint information.
    /// </summary>
    
    _mod_Ice.TCPEndpointInfo = function(timeout, compress, host, port)
    {
        _mod_Ice.IPEndpointInfo.call(this, timeout, compress, host, port);
    }
    _mod_Ice.TCPEndpointInfo.prototype = new _mod_Ice.IPEndpointInfo();
    _mod_Ice.TCPEndpointInfo.prototype.constructor = _mod_Ice.TCPEndpointInfo;

    _mod_Ice.TCPEndpointInfo.prototype.toString = function()
    {
        return "[object Ice::TCPEndpointInfo]";
    }

    /// <summary>
    /// Provides access to an UDP endpoint information.
    /// </summary>
    
    _mod_Ice.UDPEndpointInfo = function(timeout, compress, host, port, mcastInterface, mcastTtl)
    {
        _mod_Ice.IPEndpointInfo.call(this, timeout, compress, host, port);
        this.mcastInterface = mcastInterface !== undefined ? mcastInterface : null;
        this.mcastTtl = mcastTtl !== undefined ? mcastTtl : 0;
    }
    _mod_Ice.UDPEndpointInfo.prototype = new _mod_Ice.IPEndpointInfo();
    _mod_Ice.UDPEndpointInfo.prototype.constructor = _mod_Ice.UDPEndpointInfo;

    _mod_Ice.UDPEndpointInfo.prototype.toString = function()
    {
        return "[object Ice::UDPEndpointInfo]";
    }

    /// <summary>
    /// Provides access to the details of an opaque endpoint.
    /// </summary>
    
    _mod_Ice.OpaqueEndpointInfo = function(timeout, compress, rawEncoding, rawBytes)
    {
        _mod_Ice.EndpointInfo.call(this, timeout, compress);
        this.rawEncoding = rawEncoding !== undefined ? rawEncoding : null;
        this.rawBytes = rawBytes !== undefined ? rawBytes : null;
    }
    _mod_Ice.OpaqueEndpointInfo.prototype = new _mod_Ice.EndpointInfo();
    _mod_Ice.OpaqueEndpointInfo.prototype.constructor = _mod_Ice.OpaqueEndpointInfo;

    _mod_Ice.OpaqueEndpointInfo.prototype.toString = function()
    {
        return "[object Ice::OpaqueEndpointInfo]";
    }

    return _mod_Ice;
}(Ice || {}));

module.exports = Ice;
