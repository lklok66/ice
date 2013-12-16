// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var HashMap = require("./HashMap");

module.exports.writeByte = function(os, v)
{
    os.writeByte(v);
};

module.exports.writeByteSeq = function(os, v)
{
    os.writeByteSeq(v);
};

module.exports.writeShort = function(os, v)
{
    os.writeShort(v);
};

module.exports.writeShortSeq = function(os, v)
{
    os.writeShortSeq(v);
};

module.exports.writeInt = function(os, v)
{
    os.writeInt(v);
};

module.exports.writeIntSeq = function(os, v)
{
    os.writeIntSeq(v);
};

module.exports.writeLong = function(os, v)
{
    os.writeLong(v);
};

module.exports.writeLongSeq = function(os, v)
{
    os.writeLongSeq(v);
};

module.exports.writeFloat = function(os, v)
{
    os.writeFloat(v);
};

module.exports.writeFloatSeq = function(os, v)
{
    os.writeFloatSeq(v);
};

module.exports.writeDouble = function(os, v)
{
    os.writeDouble(v);
};

module.exports.writeDoubleSeq = function(os, v)
{
    os.writeDoubleSeq(v);
};

module.exports.writeString = function(os, v)
{
    os.writeString(v);
};

module.exports.writeStringSeq = function(os, v)
{
    os.writeStringSeq(v);
};

module.exports.generateWriteEnum = function(Type)
{
    return function(os, v)
        {
            return os.writeEnum(v, Type.maxValue);
        };
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
        var valueCB = callbacks.shift();
        var sz = v.length;
        os.writeSize(sz);
        for(var i = 0; i < sz; ++i)
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
        callbacks.shift();
        os.writeSize(v.size);
        for(var e = v.entries; e !== null; e = e.next)
        {
            keyCB.call(null, os, e.key);
            valueCB.call(null, os, e.value, callbacks);
        }
    }
};

module.exports.readByte = function(os)
{
    return os.readByte();
};

module.exports.readByteSeq = function(os)
{
    return os.readByteSeq();
};

module.exports.readShort = function(os)
{
    return os.readShort();
};

module.exports.readShortSeq = function(os)
{
    return os.readShortSeq();
};

module.exports.readInt = function(os)
{
    return os.readInt();
};

module.exports.readIntSeq = function(os)
{
    return os.readIntSeq();
};

module.exports.readLong = function(os)
{
    return os.readLong();
};

module.exports.readLongSeq = function(os)
{
    return os.readLongSeq();
};

module.exports.readFloat = function(os)
{
    return os.readFloat();
};

module.exports.readFloatSeq = function(os)
{
    return os.readFloatSeq();
};

module.exports.readDouble = function(os)
{
    return os.readDouble();
};

module.exports.readDoubleSeq = function(os)
{
    return os.readDoubleSeq();
};

module.exports.readString = function(os)
{
    return os.readString();
};

module.exports.readStringSeq = function(os)
{
    return os.readStringSeq();
};

module.exports.generateReadEnum = function(Type)
{
    return function(os)
        {
            return os.readEnum(Type);
        };
};

module.exports.generateReaStruct = function(Type)
{
    return function(os)
        {
            var v = new Type();
            v.__read(os);
            return v;
        };
};

module.exports.generateReadObject = function(Type)
{
    return function(os)
        {
            var v = null;
            os.reaObject(
                function(obj){
                    if(obj !== null && !(obj instanceof Type))
                    {
                        throw new TypeError("expected element of type " + Type.__ids[0] + " but received " + obj);
                    }
                    v = obj;
                });
            return v;
        };
};

module.exports.readSeq = function(os, callbacks)
{
    var sz = this.readAndCheckSeqSize(1);
    var v = [];
    v.length = sz;
    var valueCB = callbacks.shift()
    for(var i = 0; i < sz; ++i)
    {
        v[i] = valueCB.call(null, os, callbacks);
    }
    return v;
};

module.exports.readMap = function(os, v, callbacks)
{
    var v = new HashMap();
    var sz = this.readAndCheckSeqSize(1);
    
    var keyCB = callbacks[0][0];
    var valueCB = callbacks[0][1];
    callbacks.shift();
    for(var i = 0; i < sz; ++i)
    {
        v.set(keyCB.call(null, os), valueCB.call(null, os, callbacks));
    }
    return v;
};
