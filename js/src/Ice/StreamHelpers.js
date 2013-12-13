// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

module.exports.writeByte = function(os, v)
{
    os.write(v);
};

module.exports.writeShort = function(os, v)
{
    os.writeShort(v);
};

module.exports.writeInt = function(os, v)
{
    os.writeInt(v);
};

module.exports.writeLong = function(os, v)
{
    os.writeLong(v);
};

module.exports.writeFloat = function(os, v)
{
    os.writeFloat(v);
};

module.exports.writeDouble = function(os, v)
{
    os.writeDouble(v);
};

module.exports.writeString = function(os, v)
{
    os.writeString(v);
};

module.exports.writeEnum = function(os, v)
{
    os.writeEnum(v, v.maxValue);
};

module.exports.writeStruct = function(os, v)
{
    v.__write(os);
};

module.exports.writeObject = function(os, v)
{
    os.writeObject(v);
};

module.exports.writeSeq = function(os, v, callbacks)
{
    if(v === null || v.length === 0)
    {
        os.writeSize(0);
    }
    else
    {
        var valueCB = callbacks[0];
        callbacks = callbacks.slice(1);
        var length = v.length;
        os.writeSize(length);
        for(var i = 0; i < length; ++i)
        {
            valueCB.call(null, os, v[i], callbacks);
        }
    }
};

module.exports.writeMap = function(os, v, callbacks)
{
    if(v === null || v.size === 0)
    {
        os.writeSize();
    }
    else
    {
        var keyCB = callbacks[0][0];
        var valueCB = callbacks[0][1];
        callbacks = callbacks.slice(1);
        os.writeSize(v.size);
        for(var e = v.entries; e !== null; e = e.next)
        {
            keyCB.call(null, os, e.key);
            valueCB.call(null, os, e.value, callbacks);
        }
    }
};
