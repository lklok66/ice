// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

module.exports.clone = function(arr)
{
    if(arr === undefined)
    {
        return arr;
    }
    else if(arr === null)
    {
        return [];
    }
    else
    {
        return arr.slice();
    }
}

module.exports.equals = function(v1, v2, equalFn)
{
    if(v1.length != v2.length)
    {
        return false;
    }

    if(equalFn !== undefined && equalFn !== null)
    {
        for(var i = 0; i < v1.length; ++i)
        {
            if(!equalFn.call(equalFn, v1[i], v2[i]))
            {
                return false;
            }
        }
    }
    else
    {
        for(var i = 0; i < v1.length; ++i)
        {
            if(v1[i] != v2[i])
            {
                return false;
            }
        }
    }

    return true;
}

module.exports.shuffle = function(arr)
{
    for(var i = arr.length; i > 1; --i)
    {
        var e = arr[i - 1];
        var rand = Math.floor(Math.random() * i);
        arr[i - 1] = arr[rand];
        arr[rand] = e;
    }
}

module.exports.indexOf = function(arr, elem, equalFn)
{
    if(equalFn !== undefined && equalFn !== null)
    {
        // TODO
        //CONFIG::debug { Debug.assert(elem is Ice.Hashable); }
        for(var i = 0; i < arr.length; ++i)
        {
            //CONFIG::debug { Debug.assert(arr[i] is Ice.Hashable); }
            if(equalFn.call(equalFn, arr[i], elem))
            {
                return i;
            }
        }
    }
    else
    {
        return arr.indexOf(elem);
    }

    return -1;
}

module.exports.filter = function(arr, includeFn, obj)
{
    obj = obj === undefined ? includeFn : obj;
    var result = [];
    for(var i = 0; i < arr.length; ++i)
    {
        if(includeFn.call(obj, arr[i], i, arr))
        {
            result.push(arr[i]);
        }
    }
    return result;
}
