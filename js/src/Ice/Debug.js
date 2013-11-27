// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

module.exports.assert = function(b, msg)
{
    if(!b)
    {
        if(typeof(console.assert) === undefined)
        {
            throw new Error(msg === undefined ? "assertion failed" : msg);
        }
        else
        {
            console.assert(b);
        }
    }
}
