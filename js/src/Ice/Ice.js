// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Communicator = require("./Communicator");
var Debug = require("./Debug");
var Ex = require("./Exception");
var HashMap = require("./HashMap");
var IdentityUtil = require("./IdentityUtil");
var IceObject = require("./Object");
var Logger = require("./Logger");
var ProcessLogger = require("./ProcessLogger");
var ObjectPrx = require("./ObjectPrx");
var Properties = require("./Properties");

var Identity = require("./Identity").Ice.Identity;
var LocalEx = require("./LocalException").Ice;

var Ice = (function(my, undefined)
{
    my.Communicator = Communicator;
    my.HashMap = HashMap;
    my.Identity = Identity;
    my.LocalException = Ex.LocalException;
    my.Logger = Logger;
    my.Object = IceObject;
    my.ObjectPrx = ObjectPrx;
    my.Properties = Properties;
    my.UserException = Ex.UserException;

    for(var s in LocalEx)
    {
        my[s] = LocalEx[s];
    }

    my.stringToIdentity = IdentityUtil.stringToIdentity;
    my.identityToString = IdentityUtil.identityToString;
    my.proxyIdentityCompare = IdentityUtil.proxyIdentityCompare;
    my.proxyIdentityAndFacetCompare = IdentityUtil.proxyIdentityAndFacetCompare;

    //
    // Ice.InitializationData
    //
    my.InitializationData = function()
    {
        this.properties = null;
        this.logger = null;
    };

    my.InitializationData.prototype.clone = function()
    {
        var r = new my.InitializationData();
        r.properties = this.properties;
        r.logger = this.logger;
        return r;
    };

    //
    // Ice.initialize()
    //
    my.initialize = function(arg1, arg2)
    {
        var args = null;
        var initData = null;

        if(arg1 instanceof Array)
        {
            args = arg1;
        }
        else if(arg1 instanceof my.InitializationData)
        {
            initData = arg1;
        }
        else if(arg1 !== undefined && arg1 !== null)
        {
            throw new LocalEx.InitializationException("invalid argument to initialize");
        }

        if(arg2 !== undefined && arg2 !== null)
        {
            if(arg2 instanceof my.InitializationData && initData === null)
            {
                initData = arg2;
            }
            else
            {
                throw new LocalEx.InitializationException("invalid argument to initialize");
            }

        }

        if(initData === null)
        {
            initData = new my.InitializationData();
        }
        else
        {
            initData = initData.clone();
        }
        initData.properties = my.createProperties(args, initData.properties);

        var result = new Communicator(initData);
        result.finishSetup(null);
        return result;
    };

    //
    // Ice.createProperties()
    //
    my.createProperties = function(args, defaults)
    {
        return new Properties(args, defaults);
    };
    
    my.getProcessLogger = ProcessLogger.getProcessLogger;
    my.setProcessLogger = ProcessLogger.setProcessLogger;

    return my;
}(Ice || {}));

module.exports = Ice;
