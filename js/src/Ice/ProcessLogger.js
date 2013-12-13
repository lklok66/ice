
// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Logger = require("./Logger");

var processLogger = null;

module.exports.getProcessLogger = function()
{
    if(processLogger === null)
    {
        //
        // TODO: Would be nice to be able to use process name as prefix by default.
        //
        processLogger = new Logger("", "");
    }

    return processLogger;
};

module.exports.setProcessLogger = function(logger)
{
    processLogger = logger;
};