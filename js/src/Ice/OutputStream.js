// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var BasicStream = require("./BasicStream").Ice.BasicStream;
var MarshalException = require("./LocalException").Ice.MarshalException;

var OutputStream = function(communicator, encoding)
{
    this._communicator = communicator;
    encoding = encoding || this._communicator.instance.defaultsAndOverrides().defaultEncoding;
    this._os = new BasicStream(this._communicator.instance, encoding, true);
    this._os.closure = this;
};

Object.defineProperty(OutputStream.prototype, "communicator", {
    get: function() { return this._communicator; }
});


OutputStream.prototype.writeBool = function(v)
{
    this._os.writeBool(v);
};

OutputStream.prototype.writeBoolSeq = function(v)
{
    this._os.writeBoolSeq(v);
};

OutputStream.prototype.writeByte = function(v)
{
    this._os.writeByte(v);
};

OutputStream.prototype.writeByteSeq = function(v)
{
    this._os.writeByteSeq(v);
};

OutputStream.prototype.writeShort = function(v)
{
    this._os.writeShort(v);
};

OutputStream.prototype.writeShortSeq = function(v)
{
    this._os.writeShortSeq(v);
};

OutputStream.prototype.writeInt = function(v)
{
    this._os.writeInt(v);
};

OutputStream.prototype.writeIntSeq = function(v)
{
    this._os.writeIntSeq(v);
};

OutputStream.prototype.writeLong = function(v)
{
    this._os.writeLong(v);
};

OutputStream.prototype.writeLongSeq = function(v)
{
    this._os.writeLongSeq(v);
};

OutputStream.prototype.writeFloat = function(v)
{
    this._os.writeFloat(v);
};

OutputStream.prototype.writeFloatSeq = function(v)
{
    this._os.writeFloatSeq(v);
};

OutputStream.prototype.writeDouble = function(v)
{
    this._os.writeDouble(v);
};

OutputStream.prototype.writeDoubleSeq = function(v)
{
    this._os.writeDoubleSeq(v);
};

OutputStream.prototype.writeString = function(v)
{
    this._os.writeString(v);
};

OutputStream.prototype.writeStringSeq = function(v)
{
    this._os.writeStringSeq(v);
};

OutputStream.prototype.writeSize = function(sz)
{
    if(sz < 0)
    {
        throw new MarshalException();
    }
    this._os.writeSize(sz);
};

OutputStream.prototype.writeProxy = function(v)
{
    this._os.writeProxy(v);
};

OutputStream.prototype.writeObject = function(v)
{
    this._os.writeObject(v);
};

OutputStream.prototype.writeEnum = function(v)
{
    this._os.writeEnum(v);
};

OutputStream.prototype.writeStruct = function(v)
{
    this._os.writeStruct(v);
};

OutputStream.prototype.writeException = function(v)
{
    this._os.writeUserException(v);
};

OutputStream.prototype.startObject = function(slicedData)
{
    this._os.startWriteObject(slicedData);
};

OutputStream.prototype.endObject = function()
{
    this._os.endWriteObject();
};

OutputStream.prototype.startException = function(slicedData)
{
    this._os.startWriteException(slicedData);
};

OutputStream.prototype.endException = function()
{
    this._os.endWriteException();
};

OutputStream.prototype.startSlice = function(typeId, compactId, last)
{
    this._os.startWriteSlice(typeId, compactId, last);
};

OutputStream.prototype.endSlice = function()
{
    this._os.endWriteSlice();
};

OutputStream.prototype.startEncapsulationWithEncoding = function(encoding, format)
{
    this._os.startWriteEncapsWithEncoding(encoding, format);
};

OutputStream.prototype.startEncapsulation = function()
{
    this._os.startWriteEncaps();
};

OutputStream.prototype.endEncapsulation = function()
{
    this._os.endWriteEncapsChecked();
};

OutputStream.prototype.getEncoding = function()
{
    return this._os.getWriteEncoding();
};

OutputStream.prototype.writePendingObjects = function()
{
    this._os.writePendingObjects();
};

/*public boolean
writeOptional(int tag, Ice.OptionalFormat format)
{
    return _os.writeOpt(tag, format);
}*/


Object.defineProperty(OutputStream.prototype, "pos", {
    get: function() { return this._os.pos; }
});

OutputStream.prototype.rewrite = function(sz, pos)
{
    this._os.rewriteInt(sz, pos);
};

OutputStream.prototype.startSize = function()
{
    return this._os.startSize();
};

OutputStream.prototype.endSize = function()
{
    this._os.endSize();
};

OutputStream.prototype.finished = function()
{
    var buf = this._os.prepareWrite();
    return buf.getArray(buf.limit);
};

OutputStream.prototype.reset = function(clearBuffer)
{
    this._os.clear();

    var buf = this._os.getBuffer();
    if(clearBuffer)
    {
        buf.clear();
    }
    else
    {
        buf.reset();
    }
    buf.position = 0;
};

OutputStream.prototype.destroy = function()
{
    if(this._os !== null)
    {
        this._os = null;
    }
};
module.exports.Ice = {};
module.exports.Ice.OutputStream = OutputStream;
