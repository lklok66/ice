// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){
        
        require("Ice/WsEndpointI");
        require("Ice/Endpoint");
        
        var Ice = global.Ice || {};
        
        var WsEndpointI = Ice.WsEndpointI;
        var WSEndpointType = Ice.WSEndpointType;

        var WsEndpointFactory = function(instance)
        {
            this._instance = instance;
        };

        WsEndpointFactory.prototype.type = function()
        {
            return WSEndpointType;
        };

        WsEndpointFactory.prototype.protocol = function()
        {
            return "ws";
        };

        WsEndpointFactory.prototype.create = function(str, oaEndpoint)
        {
            return WsEndpointI.fromString(this._instance, str, oaEndpoint);
        };

        WsEndpointFactory.prototype.read = function(s)
        {
            return WsEndpointI.fromStream(s);
        };

        WsEndpointFactory.prototype.destroy = function()
        {
            this._instance = null;
        };

        Ice.WsEndpointFactory = WsEndpointFactory;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/WsEndpointFactory"));
