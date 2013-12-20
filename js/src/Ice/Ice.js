// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var _merge = require("./Util").merge;

var Ice = {};
_merge(Ice, require("./Communicator").Ice);
_merge(Ice, require("./HashMap").Ice);
_merge(Ice, require("./Object").Ice);
_merge(Ice, require("./Long").Ice);
_merge(Ice, require("./Logger").Ice);
_merge(Ice, require("./ObjectPrx").Ice);
_merge(Ice, require("./Properties").Ice);
_merge(Ice, require("./IdentityUtil").Ice);
_merge(Ice, require("./ProcessLogger").Ice);
_merge(Ice, require("./Protocol").Ice);
_merge(Ice, require("./Identity").Ice);
_merge(Ice, require("./Exception").Ice);
_merge(Ice, require("./LocalException").Ice);
_merge(Ice, require("./BuiltinSequences").Ice);
_merge(Ice, require("./StreamHelpers").Ice);
_merge(Ice, require("./Promise").Ice);


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

module.exports = Ice;
