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
        
        require("Ice/Endpoint");
        require("IceWS/EndpointInfo");
        require("IceWS/EndpointI");
        
        var Ice = global.Ice || {};
        var IceWS = global.IceWS || {};
        
        var EndpointI = IceWS.EndpointI;
        var WSEndpointType = Ice.WSEndpointType;

        var EndpointFactory = function(instance, secure)
        {
            this._instance = instance;
            this._secure = secure;
        };

        EndpointFactory.prototype.type = function()
        {
            return this._secure ? IceWS.WSSEndpointType : IceWS.WSEndpointType;
        };

        EndpointFactory.prototype.protocol = function()
        {
            return this._secure ? "wss" : "ws";
        };

        EndpointFactory.prototype.create = function(str, oaEndpoint)
        {
            return EndpointI.fromString(this._instance, this._secure, str, oaEndpoint);
        };

        EndpointFactory.prototype.read = function(s)
        {
            return EndpointI.fromStream(s, this._secure);
        };

        EndpointFactory.prototype.destroy = function()
        {
            this._instance = null;
        };

        IceWS.EndpointFactory = EndpointFactory;
        global.IceWS = IceWS;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/WsEndpointFactory"));
