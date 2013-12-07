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

var LocalEx = require("./LocalException").Ice;

module.exports.toString = function(ex)
{
    // TODO: Best way to stringify exception?
    return ex.toString();
}

module.exports.throwUOE = function(expectedType, actualType)
{
    throw new LocalEx.UnexpectedObjectException("expected element of type `" + expectedType + "' but received '" +
                                                actualType, actualType, expectedType);
}

module.exports.throwMemoryLimitException = function(requested, maximum)
{
    throw new LocalEx.MemoryLimitException("requested " + requested + " bytes, maximum allowed is " + maximum +
                                           " bytes (see Ice.MessageSizeMax)");
}
