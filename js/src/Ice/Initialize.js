// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    require("Ice/Protocol");
    require("Ice/LocalException");
    require("Ice/Communicator");
    require("Ice/Properties");

    var Ice = global.Ice || {};
    var Protocol = Ice.Protocol;
        
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
    
    global.Ice = Ice;
}(typeof (global) === "undefined" ? window : global));
