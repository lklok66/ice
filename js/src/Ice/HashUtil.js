// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var StringUtil = require("./StringUtil");

module.exports.addBoolean = function(h, b)
{
    return ((h << 5) + h) ^ (b ? 0 : 1);
};

module.exports.addBooleanArray = function(h, arr)
{
    var i, length;
    if(arr !== undefined && arr !== null)
    {
        for(i = 0, length = arr.length; i < length; ++i)
        {
            h = ((h << 5) + h) ^ (arr[i] ? 0 : 1);
        }
    }
    return h;
};

module.exports.addString = function(h, str)
{
    if(str !== undefined && str !== null)
    {
        h = ((h << 5) + h) ^ StringUtil.hashCode(str);
    }
    return h;
};

module.exports.addStringArray = function(h, arr)
{
    var i, length;
    if(arr !== undefined && arr !== null)
    {
        for(i = 0, length = arr.length; i < length; ++i)
        {
            h = module.exports.addString(h, arr[i]);
        }
    }
    return h;
};

module.exports.addNumber = function(h, num)
{
    return ((h << 5) + h) ^ num;
};

module.exports.addNumberArray = function(h, arr)
{
    var i, length;
    if(arr !== undefined && arr !== null)
    {
        for(i = 0, length = arr.length; i < length; ++i)
        {
            h = module.exports.addNumber(h, arr[i]);
        }
    }
    return h;
};

module.exports.addHashable = function(h, obj)
{
    if(obj !== undefined && obj !== null)
    {
        h = ((h << 5) + h) ^ obj.hashCode();
    }
    return h;
};

module.exports.addHashableArray = function(h, arr)
{
    if(arr !== undefined && arr !== null)
    {
        for(var i = 0; i < arr.length; ++i)
        {
            h = module.exports.addHashable(h, arr[i]);
        }
    }
    return h;
};
