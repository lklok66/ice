// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var TimeUtil = {};

TimeUtil.now = function()
{
    return new Date().getTime();
};

module.exports.Ice = {};
module.exports.Ice.TimeUtil = TimeUtil;