
// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var processLogger = null;

var getProcessLogger = function()
{
    if(processLogger === null)
    {
        //
        // TODO: Would be nice to be able to use process name as prefix by default.
        //
        processLogger = new require("./Logger").Ice.Logger("", "");
    }

    return processLogger;
};

var setProcessLogger = function(logger)
{
    processLogger = logger;
};

module.exports.Ice = {};
module.exports.Ice.getProcessLogger = getProcessLogger;
module.exports.Ice.setProcessLogger = setProcessLogger;