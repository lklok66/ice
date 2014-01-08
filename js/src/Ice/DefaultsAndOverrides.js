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
        
        require("Ice/FormatType");
        require("Ice/EndpointTypes");
        require("Ice/Protocol");
        
        var Ice = global.Ice || {};
        
        var FormatType = Ice.FormatType;
        var EndpointSelectionType = Ice.EndpointSelectionType;
        var Protocol = Ice.Protocol;

        var DefaultsAndOverrides = function(properties)
        {
            var value;
            
            this.defaultProtocol = properties.getPropertyWithDefault("Ice.Default.Protocol", "tcp");

            value = properties.getProperty("Ice.Default.Host");
            this.defaultHost = value.length > 0 ? value : null;

            value = properties.getProperty("Ice.Override.Timeout");
            if(value.length > 0)
            {
                this.overrideTimeout = true;
                this.overrideTimeoutValue = properties.getPropertyAsInt("Ice.Override.Timeout");
            }
            else
            {
                this.overrideTimeout = false;
                this.overrideTimeoutValue = -1;
            }

            value = properties.getProperty("Ice.Override.ConnectTimeout");
            if(value.length > 0)
            {
                this.overrideConnectTimeout = true;
                this.overrideConnectTimeoutValue = properties.getPropertyAsInt("Ice.Override.ConnectTimeout");
            }
            else
            {
                this.overrideConnectTimeout = false;
                this.overrideConnectTimeoutValue = -1;
            }

            value = properties.getProperty("Ice.Override.CloseTimeout");
            if(value.length > 0)
            {
                this.overrideCloseTimeout = true;
                this.overrideCloseTimeoutValue = properties.getPropertyAsInt("Ice.Override.CloseTimeout");
            }
            else
            {
                this.overrideCloseTimeout = false;
                this.overrideCloseTimeoutValue = -1;
            }

            this.overrideCompress = false;
            this.overrideSecure = false;

            value = properties.getPropertyWithDefault("Ice.Default.EndpointSelection", "Random");
            if(value === "Random")
            {
                this.defaultEndpointSelection = EndpointSelectionType.Random;
            }
            else if(value === "Ordered")
            {
                this.defaultEndpointSelection = EndpointSelectionType.Ordered;
            }
            else
            {
                var ex = new require("./LocalException").Ice.EndpointSelectionTypeParseException();
                ex.str = "illegal value `" + value + "'; expected `Random' or `Ordered'";
                throw ex;
            }

            this.defaultLocatorCacheTimeout = properties.getPropertyAsIntWithDefault("Ice.Default.LocatorCacheTimeout", -1);

            this.defaultPreferSecure = false;

            value = properties.getPropertyWithDefault("Ice.Default.EncodingVersion",
                                                    Protocol.encodingVersionToString(Protocol.currentEncoding));
            this.defaultEncoding = Protocol.stringToEncodingVersion(value);
            Protocol.checkSupportedEncoding(this.defaultEncoding);

            var slicedFormat = properties.getPropertyAsIntWithDefault("Ice.Default.SlicedFormat", 0) > 0;
            this.defaultFormat = slicedFormat ? FormatType.SlicedFormat : FormatType.CompactFormat;
        };

        Ice.DefaultsAndOverrides = DefaultsAndOverrides;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/DefaultsAndOverrides"));