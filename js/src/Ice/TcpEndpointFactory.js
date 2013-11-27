// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var TcpEndpointI = require("./TcpEndpointI");

var Endp = require("./Endpoint").Ice;

var TcpEndpointFactory = function(instance)
{
    this._instance = instance;
}

TcpEndpointFactory.prototype.type = function()
{
    return Endp.TCPEndpointType;
}

TcpEndpointFactory.prototype.protocol = function()
{
    return "tcp";
}

TcpEndpointFactory.prototype.create = function(str, oaEndpoint)
{
    return TcpEndpointI.fromString(this._instance, str, oaEndpoint);
}

TcpEndpointFactory.prototype.read = function(s)
{
    return TcpEndpointI.fromStream(s);
}

TcpEndpointFactory.prototype.destroy = function()
{
    this._instance = null;
}

module.exports = TcpEndpointFactory;
