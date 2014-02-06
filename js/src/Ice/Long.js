// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    require("Ice/Class");
    
    //
    // The Long type represents a signed 64-bit integer as two 32-bit values
    // corresponding to the high and low words.
    //
    var Ice = global.Ice || {};
    
    var Long = Ice.Class({
        __init__: function(high, low)
        {
            this.high = high;
            this.low = low;
        },
        hashCode: function()
        {
            return this.low;
        },
        equals: function(rhs)
        {
            if(this === rhs)
            {
                return true;
            }
            if(!(rhs instanceof Long))
            {
                return false;
            }
            
            return this.high === rhs.high && this.low === rhs.low;
        },
        toString: function()
        {
            return this.high + ":" + this.low;
        }
    });
    
    Ice.Long = Long;
    global.Ice = Ice;
}(typeof (global) === "undefined" ? window : global));
