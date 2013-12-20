// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Exception utilities
//

var ExUtil = {};

ExUtil.toString = function(ex)
{
    // TODO: Best way to stringify exception?
    return ex.toString();
};

ExUtil.throwUOE = function(expectedType, actualType)
{
    throw new require("./LocalException").Ice.UnexpectedObjectException("expected element of type `" + expectedType + "' but received '" +
                                         actualType, actualType, expectedType);
};

ExUtil.throwMemoryLimitException = function(requested, maximum)
{
    throw new require("./LocalException").Ice.UnexpectedObjectException("requested " + requested + " bytes, maximum allowed is " + maximum +
                                   " bytes (see Ice.MessageSizeMax)");
};

module.exports.Ice = {};
module.exports.Ice.ExUtil = ExUtil;