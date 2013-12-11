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
