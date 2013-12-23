// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


//
// The Long type represents a signed 64-bit integer as two 32-bit values
// corresponding to the high and low words.
//

var Long = function(high, low)
{
    this.high = high;
    this.low = low;
};

Long.prototype.hashCode = function()
{
    return this.low;
};

Long.prototype.equals = function(rhs)
{
    if(this === rhs)
    {
        return true;
    }
    if(!(rhs instanceof Long))
    {
        return false;
    }
    
    return this.high === rhs.high && this.low == rhs.low;
};

Long.prototype.toString = function()
{
    return this.high + ":" + this.low;
};

module.exports.Ice = {};
module.exports.Ice.Long = Long;
