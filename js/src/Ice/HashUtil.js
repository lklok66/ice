// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){
        
        require("Ice/StringUtil");
        
        var StringUtil = Ice.StringUtil;

        var HashUtil = {};

        HashUtil.addBoolean = function(h, b)
        {
            return ((h << 5) + h) ^ (b ? 0 : 1);
        };

        HashUtil.addBooleanArray = function(h, arr)
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

        HashUtil.addString = function(h, str)
        {
            if(str !== undefined && str !== null)
            {
                h = ((h << 5) + h) ^ StringUtil.hashCode(str);
            }
            return h;
        };

        HashUtil.addStringArray = function(h, arr)
        {
            var i, length;
            if(arr !== undefined && arr !== null)
            {
                for(i = 0, length = arr.length; i < length; ++i)
                {
                    h = HashUtil.addString(h, arr[i]);
                }
            }
            return h;
        };

        HashUtil.addNumber = function(h, num)
        {
            return ((h << 5) + h) ^ num;
        };

        HashUtil.addNumberArray = function(h, arr)
        {
            var i, length;
            if(arr !== undefined && arr !== null)
            {
                for(i = 0, length = arr.length; i < length; ++i)
                {
                    h = HashUtil.addNumber(h, arr[i]);
                }
            }
            return h;
        };

        HashUtil.addHashable = function(h, obj)
        {
            if(obj !== undefined && obj !== null)
            {
                h = ((h << 5) + h) ^ obj.hashCode();
            }
            return h;
        };

        HashUtil.addHashableArray = function(h, arr)
        {
            if(arr !== undefined && arr !== null)
            {
                for(var i = 0; i < arr.length; ++i)
                {
                    h = HashUtil.addHashable(h, arr[i]);
                }
            }
            return h;
        };

        global.Ice = global.Ice || {};
        global.Ice.HashUtil = HashUtil;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/HashUtil"));
