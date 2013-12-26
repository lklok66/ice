// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(module, exports, require){
        
        var _merge = require("Ice/Util").merge;

        var Ice = module.exports.Ice || {};
        
        _merge(Ice, require("Ice/Communicator").Ice);
        _merge(Ice, require("Ice/HashMap").Ice);
        _merge(Ice, require("Ice/Object").Ice);
        _merge(Ice, require("Ice/Long").Ice);
        _merge(Ice, require("Ice/Logger").Ice);
        _merge(Ice, require("Ice/ObjectPrx").Ice);
        _merge(Ice, require("Ice/Properties").Ice);
        _merge(Ice, require("Ice/IdentityUtil").Ice);
        _merge(Ice, require("Ice/ProcessLogger").Ice);
        _merge(Ice, require("Ice/Protocol").Ice);
        _merge(Ice, require("Ice/Identity").Ice);
        _merge(Ice, require("Ice/Exception").Ice);
        _merge(Ice, require("Ice/LocalException").Ice);
        _merge(Ice, require("Ice/BuiltinSequences").Ice);
        _merge(Ice, require("Ice/StreamHelpers").Ice);
        _merge(Ice, require("Ice/Promise").Ice);
        _merge(Ice, require("Ice/EndpointTypes").Ice);
        _merge(Ice, require("Ice/Locator").Ice);
        _merge(Ice, require("Ice/Router").Ice);
        _merge(Ice, require("Ice/Version").Ice);
        _merge(Ice, require("Ice/ObjectFactory").Ice);
        _merge(Ice, require("Ice/Buffer").Ice);
        _merge(Ice, require("Ice/TypeRegistry").Ice);

        var Protocol = require("Ice/Protocol").Ice.Protocol;

        //
        // Ice.InitializationData
        //
        Ice.InitializationData = function()
        {
            this.properties = null;
            this.logger = null;
        };

        Ice.InitializationData.prototype.clone = function()
        {
            var r = new Ice.InitializationData();
            r.properties = this.properties;
            r.logger = this.logger;
            return r;
        };

        //
        // Ice.initialize()
        //
        Ice.initialize = function(arg1, arg2)
        {
            var args = null;
            var initData = null;

            if(arg1 instanceof Array)
            {
                args = arg1;
            }
            else if(arg1 instanceof Ice.InitializationData)
            {
                initData = arg1;
            }
            else if(arg1 !== undefined && arg1 !== null)
            {
                throw new Ice.InitializationException("invalid argument to initialize");
            }

            if(arg2 !== undefined && arg2 !== null)
            {
                if(arg2 instanceof Ice.InitializationData && initData === null)
                {
                    initData = arg2;
                }
                else
                {
                    throw new Ice.InitializationException("invalid argument to initialize");
                }

            }

            if(initData === null)
            {
                initData = new Ice.InitializationData();
            }
            else
            {
                initData = initData.clone();
            }
            initData.properties = Ice.createProperties(args, initData.properties);

            var result = new Ice.Communicator(initData);
            result.finishSetup(null);
            return result;
        };

        //
        // Ice.createProperties()
        //
        Ice.createProperties = function(args, defaults)
        {
            return new Ice.Properties(args, defaults);
        };

        Ice.stringToProtocolVersion = Protocol.stringToProtocolVersion;
        Ice.stringToEncodingVersion = Protocol.stringToEncodingVersion;
        Ice.protocolVersionToString = Protocol.protocolVersionToString;
        Ice.encodingVersionToString = Protocol.encodingVersionToString;

        Ice.currentProtocol = function()
        {
            return Protocol.currentProtocol.clone();
        };

        Ice.currentEncoding = function()
        {
            return Protocol.currentEncoding.clone();
        };

        Ice.Protocol_1_0 = Protocol.Protocol_1_0;
        Ice.Encoding_1_0 = Protocol.Encoding_1_0;
        Ice.Encoding_1_1 = Protocol.Encoding_1_1;

        module.exports = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Ice"));
