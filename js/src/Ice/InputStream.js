// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var BasicStream = require("./BasicStream");

var InputStream = function(communicator, data, copyData, encodingVersion)
{
    this._communicator = communicator;
    encodingVersion = encodingVersion || this._communicator.instance.defaultsAndOverrides().defaultEncoding;
    if(copyData)
    {
        this._is = new BasicStream(this._communicator.instance, encodingVersion, true);
        this._is.resize(data.length, true);
        var buf = this._is.buffer;
        buf.position = 0;
        buf.putArray(data);
        buf.position = 0;
    }
    else
    {
        this._is = new BasicStream(this._communicator.instance, encodingVersion, data);
    }
    this._is.closure = this;
};

Object.defineProperty(InputStream.prototype, "communicator", {
    get: function() { return this._communicator; }
});


InputStream.prototype.sliceObjects = function(slice)
{
    this._is.sliceObjects(slice);
};

InputStream.prototype.readBool = function()
{
    return this._is.readBool();
};

InputStream.prototype.readBoolSeq = function()
{
    return this._is.readBoolSeq();
};

InputStream.prototype.readByte = function()
{
    return this._is.readByte();
};

InputStream.prototype.readByteSeq = function()
{
    return this._is.readByteSeq();
};

InputStream.prototype.readShort = function()
{
    return this._is.readShort();
};

InputStream.prototype.readShortSeq = function()
{
    return this._is.readShortSeq();
};

InputStream.prototype.readInt = function()
{
    return this._is.readInt();
};

InputStream.prototype.readIntSeq = function()
{
    return this._is.readIntSeq();
};

InputStream.prototype.readLong = function()
{
    return this._is.readLong();
};

InputStream.prototype.readLongSeq = function()
{
    return this._is.readLongSeq();
};


InputStream.prototype.readFloat = function()
{
    return this._is.readFloat();
};

InputStream.prototype.readFloatSeq = function()
{
    return this._is.readFloatSeq();
};

InputStream.prototype.readDouble = function()
{
    return this._is.readDouble();
};

InputStream.prototype.readDoubleSeq = function()
{
    return this._is.readDoubleSeq();
};

InputStream.prototype.readString = function()
{
    return this._is.readString();
};

InputStream.prototype.readStringSeq = function()
{
    return this._is.readStringSeq();
};

InputStream.prototype.readSize = function()
{
    return this._is.readSize();
};

InputStream.prototype.readAndCheckSeqSize = function(minWire)
{
    return this._is.readAndCheckSeqSize(minWire);
};

InputStream.prototype.readProxy = function()
{
    return this._is.readProxy();
};


InputStream.prototype.readObject = function(cb, type)
{
    return this._is.readObject(cb, type);
};

InputStream.prototype.readEnum = function(type)
{
    return this._is.readEnum(type);
};

InputStream.prototype.throwException = function()
{
    this._is.throwException();
};

InputStream.prototype.startObject = function()
{
    this._is.startReadObject();
};

InputStream.prototype.endObject = function(preserve)
{
    return this._is.endReadObject(preserve);
};

InputStream.prototype.startException = function()
{
    this._is.startReadException();
};

InputStream.prototype.endException = function(preserve)
{
    return this._is.endReadException(preserve);
};

InputStream.prototype.startSlice = function()
{
    return this._is.startReadSlice();
};

InputStream.prototype.endSlice = function()
{
    this._is.endReadSlice();
};

InputStream.prototype.skipSlice = function()
{
    this._is.skipSlice();
};

InputStream.prototype.startEncapsulation = function()
{
    return this._is.startReadEncaps();
};

InputStream.prototype.endEncapsulation = function()
{
    this._is.endReadEncapsChecked();
};

InputStream.prototype.skipEncapsulation = function()
{
    return this._is.skipEncaps();
};

InputStream.prototype.getEncoding = function()
{
    return this._is.getReadEncoding();
};


InputStream.prototype.readPendingObjects = function()
{
    this._is.readPendingObjects();
};

InputStream.prototype.rewind = function()
{
    this._is.clear();
    this._is.getBuffer().position = 0;
};

InputStream.prototype.skip = function(sz)
{
    this._is.skip(sz);
};

InputStream.prototype.skipSize = function()
{
    this._is.skipSize();
};

/*public boolean
readOptional(int tag, OptionalFormat format)
{
    return _is.readOpt(tag, format);
}*/

Object.defineProperty(InputStream.prototype, "pos",{
    get: function() { return this._is.pos; }
});

InputStream.prototype.destroy = function()
{
    if(this._is !== null)
    {
        this._is = null;
    }
};

module.exports = InputStream;