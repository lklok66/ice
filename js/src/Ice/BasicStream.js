// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Debug = require("./Debug");
var ExUtil = require("./ExUtil");
var Protocol = require("./Protocol");
var FormatType = require("./FormatType");
var LocalEx = require("./LocalException").Ice;
var Ver = require("./Version").Ice;
var OptionalFormat = require("./OptionalFormat");

var SliceType = {};
SliceType.NoSlice = 0;
SliceType.ObjectSlice = 1;
SliceType.ExceptionSlice = 2;


var OPTIONAL_END_MARKER           = 0xFF;
var FLAG_HAS_TYPE_ID_STRING       = (1<<0);
var FLAG_HAS_TYPE_ID_INDEX        = (1<<1);
var FLAG_HAS_TYPE_ID_COMPACT      = (1<<1 | 1<<0);
var FLAG_HAS_OPTIONAL_MEMBERS     = (1<<2);
var FLAG_HAS_INDIRECTION_TABLE    = (1<<3);
var FLAG_HAS_SLICE_SIZE           = (1<<4);
var FLAG_IS_LAST_SLICE            = (1<<5);

var BasicStream = function(instance, encoding, unlimited)
{
    this._instance = instance;
    this._closure = null;
    this._encoding = encoding;

    this._readEncapsStack = null;
    this._writeEncapsStack = null;
    this._readEncapsCache = null;
    this._writeEncapsCache = null;

    this._sliceObjects = true;

    this._messageSizeMax = this._instance.messageSizeMax(); // Cached for efficiency.
    this._unlimited = unlimited;

    this._startSeq = -1;
    this._sizePos = -1;
    
    Object.defineProperty(this, "instance", {
        get: function() { return this._instance; }
    });
    
    Object.defineProperty(this, "closure", {
        get: function() { return this._type; }
    });
}

//
// This function allows this object to be reused, rather than
// reallocated.
//
BasicStream.prototype.reset = function()
{
    this._buf.reset();
    this.clear();
}

BasicStream.prototype.clear = function()
{
    if(this._readEncapsStack != null)
    {
        Debug.assert(this._readEncapsStack.next === null);
        this._readEncapsStack.next = this._readEncapsCache;
        this._readEncapsCache = this._readEncapsStack;
        this._readEncapsCache.reset();
        this._readEncapsStack = null;
    }

    if(_writeEncapsStack != null)
    {
        Debug.assert(this._writeEncapsStack.next === null);
        this._writeEncapsStack.next = this._writeEncapsCache;
        this._writeEncapsCache = this._writeEncapsStack;
        this._writeEncapsCache.reset();
        this._writeEncapsStack = null;
    }

    this._startSeq = -1;

    this._sliceObjects = true;
}

BasicStream.prototype.swap = function(other)
{
    Debug.assert(this._instance === other._instance);

    var tmpBuf = other._buf;
    other._buf = this._buf;
    this._buf = tmpBuf;

    var tmpClosure = other._closure;
    other._closure = this._closure;
    this._closure = tmpClosure;

    //
    // Swap is never called for BasicStreams that have encapsulations being read/write. However,
    // encapsulations might still be set in case marshalling or un-marshalling failed. We just
    // reset the encapsulations if there are still some set.
    //
    this.resetEncaps();
    other.resetEncaps();

    var tmpUnlimited = other._unlimited;
    other._unlimited = this._unlimited;
    this._unlimited = tmpUnlimited;

    var tmpStartSeq = other._startSeq;
    other._startSeq = this._startSeq;
    this._startSeq = tmpStartSeq;

    var tmpMinSeqSize = other._minSeqSize;
    other._minSeqSize = this._minSeqSize;
    this._minSeqSize = tmpMinSeqSize;

    var tmpSizePos = other._sizePos;
    other._sizePos = this._sizePos;
    this._sizePos = tmpSizePos;
}


BasicStream.prototype.resetEncaps = function()
{
    this._readEncapsStack = null;
    this._writeEncapsStack = null;
}


BasicStream.prototype.resize(sz, reading)
{
    //
    // Check memory limit if stream is not unlimited.
    //
    if(!this._unlimited && sz > this._messageSizeMax)
    {
        ExUtil.throwMemoryLimitException(sz, this._messageSizeMax);
    }

    //TODO
    this._buf.resize(sz, reading);
    this._buf.b.position(sz);
}


BasicStream.prototype.prepareWrite = function()
{
    //TODO
    this._buf.b.limit(this._buf.size());
    this._buf.b.position(0);
    return this._buf;
}

BasicStream.prototype.prepareWrite.getBuffer = function()
{
    return this._buf;
}

BasicStream.prototype.startWriteObject = function(data)
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.startInstance(SliceType.ObjectSlice, data);
}

BasicStream.prototype.endWriteObject = function()
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.endInstance();
}

BasicStream.prototype.startReadObject = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.startInstance(SliceType.ObjectSlice);
}

BasicStream.prototype.endReadObject = function(preserve)
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    return this._readEncapsStack.decoder.endInstance(preserve);
}

BasicStream.prototype.startWriteException = function(data)
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.startInstance(SliceType.ExceptionSlice, data);
}

BasicStream.prototype.endWriteException = function()
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.endInstance();
}

BasicStream.prototype.startReadException = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.startInstance(SliceType.ExceptionSlice);
}

BasicStream.prototype.endReadException = function(preserve)
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    return this._readEncapsStack.decoder.endInstance(preserve);
}

BasicStream.prototype.startWriteEncaps = function()
{
    //
    // If no encoding version is specified, use the current write
    // encapsulation encoding version if there's a current write
    // encapsulation, otherwise, use the stream encoding version.
    //

    if(this._writeEncapsStack !== null)
    {
        this.startWriteEncapsWithEncoding(this._writeEncapsStack.encoding, this._writeEncapsStack.format);
    }
    else
    {
        this.startWriteEncapsWithEncoding(this._encoding, FormatType.DefaultFormat);
    }
}

BasicStream.prototype.startWriteEncapsWithEncoding(encoding, format)
{
    Protocol.checkSupportedEncoding(encoding);

    var curr = this._writeEncapsCache;
    if(curr !== null)
    {
        curr.reset();
        this._writeEncapsCache = this._writeEncapsCache.next;
    }
    else
    {
        curr = new WriteEncaps();
    }
    curr.next = this._writeEncapsStack;
    this._writeEncapsStack = curr;

    this._writeEncapsStack.format = format;
    this._writeEncapsStack.setEncoding(encoding);
    this._writeEncapsStack.start = _buf.size();

    this.writeInt(0); // Placeholder for the encapsulation length.
    this._writeEncapsStack.encoding.__write(this);
}

BasicStream.prototype.endWriteEncaps = function()
{
    Debug.assert(this._writeEncapsStack !== null);

    // Size includes size and version.
    var start = this._writeEncapsStack.start;
    var sz = this_buf.size() - start;
    this._buf.b.putInt(start, sz);

    var curr = this._writeEncapsStack;
    this._writeEncapsStack = curr.next;
    curr.next = this._writeEncapsCache;
    this._writeEncapsCache = curr;
    this._writeEncapsCache.reset();
}

BasicStream.prototype.endWriteEncapsChecked = function() // Used by public stream API.
{
    if(this._writeEncapsStack === null)
    {
        throw new LocalEx.EncapsulationException("not in an encapsulation");
    }
    this.endWriteEncaps();
}

BasicStream.prototype.writeEmptyEncaps = function(encoding)
{
    Protocol.checkSupportedEncoding(encoding);
    this.writeInt(6); // Size
    encoding.__write(this);
}

BasicStream.prototype.writeEncaps = function(v)
{
    if(v.length < 6)
    {
        throw new LocalEx.EncapsulationException();
    }
    this.expand(v.length);
    this._buf.b.put(v);
}


BasicStream.prototype.getWriteEncoding = function()
{
    return this._writeEncapsStack !== null ? this._writeEncapsStack.encoding : _encoding;
}

BasicStream.prototype.startReadEncaps = function()
{
    var curr = this._readEncapsCache;
    if(curr !== null)
    {
        curr.reset();
        this._readEncapsCache = this._readEncapsCache.next;
    }
    else
    {
        curr = new ReadEncaps();
    }
    curr.next = this._readEncapsStack;
    this._readEncapsStack = curr;

    this._readEncapsStack.start = this._buf.b.position();

    //
    // I don't use readSize() and writeSize() for encapsulations,
    // because when creating an encapsulation, I must know in advance
    // how many bytes the size information will require in the data
    // stream. If I use an Int, it is always 4 bytes. For
    // readSize()/writeSize(), it could be 1 or 5 bytes.
    //
    var sz = this.readInt();
    if(sz < 6)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    if(sz - 4 > this._buf.b.remaining())
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    this._readEncapsStack.sz = sz;

    var encoding = new Ver.EncodingVersion();
    encoding.__read(this);
    Protocol.checkSupportedEncoding(encoding); // Make sure the encoding is supported.
    this._readEncapsStack.setEncoding(encoding);

    return encoding;
}

BasicStream.prototype.endReadEncaps = function()
{
    Debug.assert(this._readEncapsStack !== null);

    if(!this._readEncapsStack.encoding_1_0)
    {
        this.skipOpts();
        if(this._buf.b.position() !== this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            throw new LocalEx.EncapsulationException();
        }
    }
    else if(this._buf.b.position() !== this._readEncapsStack.start + this._readEncapsStack.sz)
    {
        if(this._buf.b.position() + 1 != this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            throw new LocalEx.EncapsulationException();
        }
        
        //
        // Ice version < 3.3 had a bug where user exceptions with
        // class members could be encoded with a trailing byte
        // when dispatched with AMD. So we tolerate an extra byte
        // in the encapsulation.
        //
        
        // TODO
        /*try
        {
            _buf.b.get();
        }
        catch(java.nio.BufferUnderflowException ex)
        {
            throw new Ice.UnmarshalOutOfBoundsException();
        }*/
    }

    var curr = this._readEncapsStack;
    this._readEncapsStack = curr.next;
    curr.next = this._readEncapsCache;
    this._readEncapsCache = curr;
    this._readEncapsCache.reset();
}

BasicStream.prototype.skipEmptyEncaps = function(encoding)
{
    var sz = readInt();
    if(sz !== 6)
    {
        throw new LocalEx.EncapsulationException();
    }

    var pos = this._buf.b.position();
    if(pos + 2 > this._buf.size())
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    if(encoding !== null)
    {
        encoding.__read(this);
    }
    else
    {
        this._buf.b.position(pos + 2);
    }
}

BasicStream.prototype.endReadEncapsChecked = function() // Used by public stream API.
{
    if(this._readEncapsStack === null)
    {
        throw new LocalEx.EncapsulationException("not in an encapsulation");
    }
    this.endReadEncaps();
}

BasicStream.prototype.readEncaps = function(encoding)
{
    var sz = this.readInt();
    if(sz < 6)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    if(sz - 4 > this._buf.b.remaining())
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    if(encoding !== null)
    {
        encoding.__read(this);
        this._buf.b.position(this._buf.b.position() - 6);
    }
    else
    {
        this._buf.b.position(_buf.b.position() - 4);
    }

    //TODO
    /*
    var v = new byte[sz];
    try
    {
        v =_buf.b.get(v);
        return v;
    }
    catch(java.nio.BufferUnderflowException ex)
    {
        throw new Ice.UnmarshalOutOfBoundsException();
    }*/
}

BasicStream.prototype.getReadEncoding = function()
{
    return this._readEncapsStack !== null ? this._readEncapsStack.encoding : _encoding;
}

BasicStream.prototype.getReadEncapsSize = function()
{
    Debug.assert(this._readEncapsStack !== null);
    return this._readEncapsStack.sz - 6;
}

BasicStream.prototype.skipEncaps = function()
{
    var sz = this.readInt();
    if(sz < 6)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    var encoding = new Ver.EncodingVersion();
    encoding.__read(this);
    try
    {
        _buf.b.position(_buf.b.position() + sz - 6);
    }
    catch(ex)
    {
        Debug.assert(ex instanceof LocalEx.IllegalArgumentException);
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    return encoding;
}

BasicStream.prototype.startWriteSlice = function(typeId, compactId, last)
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.startSlice(typeId, compactId, last);
}

BasicStream.prototype.endWriteSlice = function()
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.endSlice();
}

BasicStream.prototype.startReadSlice = function() // Returns type ID of next slice
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    return this._readEncapsStack.decoder.startSlice();
}

BasicStream.prototype.endReadSlice = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.endSlice();
}

BasicStream.prototype.skipSlice = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.skipSlice();
}

BasicStream.prototype.readPendingObjects = function()
{
    if(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null)
    {
        this._readEncapsStack.decoder.readPendingObjects();
    }
    else if(this._readEncapsStack !== null ? this._readEncapsStack.encoding_1_0 : this._encoding.equals(Protocol.Encoding_1_0))
    {
        //
        // If using the 1.0 encoding and no objects were read, we
        // still read an empty sequence of pending objects if
        // requested (i.e.: if this is called).
        //
        // This is required by the 1.0 encoding, even if no objects
        // are written we do marshal an empty sequence if marshaled
        // data types use classes.
        //
        this.skipSize();
    }
}

BasicStream.prototype.writePendingObjects = function()
{
    if(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null)
    {
        this._writeEncapsStack.encoder.writePendingObjects();
    }
    else if(this._writeEncapsStack !== null ? this._writeEncapsStack.encoding_1_0 : this._encoding.equals(Protocol.Encoding_1_0))
    {
        //
        // If using the 1.0 encoding and no objects were written, we
        // still write an empty sequence for pending objects if
        // requested (i.e.: if this is called).
        // 
        // This is required by the 1.0 encoding, even if no objects
        // are written we do marshal an empty sequence if marshaled
        // data types use classes.
        //
        this.writeSize(0);
    }
}


BasicStream.prototype.writeSize = function(v)
{
    if(v > 254)
    {
        this.expand(5);
        this._buf.put(-1);
        this._buf.putInt(v);
    }
    else
    {
        this.expand(1);
        this._buf.put(v);
    }
}

BasicStream.prototype.readSize = function()
{
    try
    {
        var b = this._buf.get();
        if(b === -1)
        {
            var v = this._buf.getInt();
            if(v < 0)
            {
                throw new LocalEx.UnmarshalOutOfBoundsException();
            }
            return v;
        }
        else
        {
            return b < 0 ? b + 256 : b;
        }
    }
    catch(ex)
    {
        //TODO assert is BufferUnderflowException?
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

BasicStream.prototype.readAndCheckSeqSize = function(minSize)
{
    var sz = this.readSize();

    if(sz === 0)
    {
        return sz;
    }

    //
    // The _startSeq variable points to the start of the sequence for which
    // we expect to read at least _minSeqSize bytes from the stream.
    //
    // If not initialized or if we already read more data than _minSeqSize,
    // we reset _startSeq and _minSeqSize for this sequence (possibly a
    // top-level sequence or enclosed sequence it doesn't really matter).
    //
    // Otherwise, we are reading an enclosed sequence and we have to bump
    // _minSeqSize by the minimum size that this sequence will  require on
    // the stream.
    //
    // The goal of this check is to ensure that when we start un-marshalling
    // a new sequence, we check the minimal size of this new sequence against
    // the estimated remaining buffer size. This estimatation is based on
    // the minimum size of the enclosing sequences, it's _minSeqSize.
    //
    if(this._startSeq === -1 || this._buf.position > (this._startSeq + this._minSeqSize))
    {
        this._startSeq = _buf.position;
        this._minSeqSize = sz * minSize;
    }
    else
    {
        this._minSeqSize += sz * minSize;
    }

    //
    // If there isn't enough data to read on the stream for the sequence (and
    // possibly enclosed sequences), something is wrong with the marshalled
    // data: it's claiming having more data that what is possible to read.
    //
    if(this._startSeq + _minSeqSize > this._buf.limit)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    return sz;
}

BasicStream.prototype.startSize = function()
{
    this._sizePos = this._buf.position;
    this.writeInt(0); // Placeholder for 32-bit size
}

BasicStream.prototype.endSize = function()
{
    Debug.assert(this._sizePos >= 0);
    this.rewriteInt(this._buf.position - this._sizePos - 4, this._sizePos);
    this._sizePos = -1;
}

BasicStream.prototype.writeBlob = function(v)
{
    if(v === null)
    {
        return;
    }
    this._buf.putArray(v);
}

public void
writeBlob(byte[] v, int off, int len)
{
    if(v == null)
    {
        return;
    }
    expand(len);
    _buf.b.put(v, off, len);
}

public byte[]
readBlob(int sz)
{
    if(_buf.b.remaining() < sz)
    {
        throw new Ice.UnmarshalOutOfBoundsException();
    }
    byte[] v = new byte[sz];
    try
    {
        _buf.b.get(v);
        return v;
    }
    catch(java.nio.BufferUnderflowException ex)
    {
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}

// Read/write format and tag for optionals
BasicStream.prototype.writeOpt = function(tag, format)
{
    Debug.assert(this._writeEncapsStack !== null);
    if(this._writeEncapsStack.encoder !== null)
    {
        return this._writeEncapsStack.encoder.writeOpt(tag, format);
    }
    else
    {
        return this.writeOptImpl(tag, format);
    }
}

BasicStream.prototype.readOpt = function(tag, expectedFormat)
{
    Debug.assert(this._readEncapsStack !== null);
    if(this._readEncapsStack.decoder !== null)
    {
        return this._readEncapsStack.decoder.readOpt(tag, expectedFormat);
    }
    else
    {
        return this.readOptImpl(tag, expectedFormat);
    }
}

BasicStream.prototype.writeByte = function(byte v)
{
    this._buf.put(v);
}

BasicStream.prototype.writeOptionalByte = function(tag, v)
{
    if(v !== null && v.isSet())
    {
        this.writeByte(tag, v.get());
    }
}

public void
writeByte(int tag, byte v)
{
    if(writeOpt(tag, Ice.OptionalFormat.F1))
    {
        writeByte(v);
    }
}

BasicStream.prototype.rewriteByte = function(v, dest)
{
    this._buf.putAt(dest, v);
}

BasicStream.prototype.writeByteSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this._buf.putArray(v);
    }
}

public void
writeByteSeq(int tag, Ice.Optional<byte[]> v)
{
    if(v != null && v.isSet())
    {
        writeByteSeq(tag, v.get());
    }
}

public void
writeByteSeq(int tag, byte[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeByteSeq(v);
    }
}

BasicStream.prototype.readByte = function()
{
    try
    {
        return _buf.get();
    }
    catch(ex)
    {
        //TODO assert BufferUnderflowException?
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}

public void
readByte(int tag, Ice.ByteOptional v)
{
    if(readOpt(tag, Ice.OptionalFormat.F1))
    {
        v.set(readByte());
    }
    else
    {
        v.clear();
    }
}

BasicStream.prototype.readByteSeq = function()
{
    try
    {
        var sz = this.readAndCheckSeqSize(1);
        return _buf.getArray(sz);
    }
    catch(java.nio.BufferUnderflowException ex)
    {
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}

public void
readByteSeq(int tag, Ice.Optional<byte[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        v.set(readByteSeq());
    }
    else
    {
        v.clear();
    }
}

public java.io.Serializable
readSerializable()
{
    int sz = readAndCheckSeqSize(1);
    if (sz == 0)
    {
        return null;
    }
    try
    {
        InputStreamWrapper w = new InputStreamWrapper(sz, this);
        ObjectInputStream in = new ObjectInputStream(_instance, w);
        return (java.io.Serializable)in.readObject();
    }
    catch(java.lang.Exception ex)
    {
        throw new Ice.MarshalException("cannot deserialize object", ex);
    }
}

BasicStream.prototype.writeBool = function(v)
{
    this._buf.put(v ? 1 : 0);
}

//TODO Optionals
/*public void
writeBool(int tag, Ice.BooleanOptional v)
{
    if(v != null && v.isSet())
    {
        writeBool(tag, v.get());
    }
}

public void
writeBool(int tag, boolean v)
{
    if(writeOpt(tag, Ice.OptionalFormat.F1))
    {
        writeBool(v);
    }
}*/

BasicStream.prototype.rewriteBool = function(v, dest)
{
    this._buf.putAt(dest, v ? 1 : 0);
}

BasicStream.prototype.writeBoolSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        for(var i = 0; i < v.length; ++i)
        {
            this._buf.put(b ? 1 : 0);
        }
    }
}

/*public void
writeBoolSeq(int tag, Ice.Optional<boolean[]> v)
{
    if(v != null && v.isSet())
    {
        writeBoolSeq(tag, v.get());
    }
}

public void
writeBoolSeq(int tag, boolean[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeBoolSeq(v);
    }
}*/

BasicStream.prototype.readBool = prototype()
{
    try
    {
        return this._buf.get() === 1;
    }
    catch(ex)
    {
        // TODO assert BufferUnderflowException
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

//TODO Optionals
/*public void
readBool(int tag, Ice.BooleanOptional v)
{
    if(readOpt(tag, Ice.OptionalFormat.F1))
    {
        v.set(readBool());
    }
    else
    {
        v.clear();
    }
}*/


BasicStream.prototype.readBoolSeq = function()
{
    try
    {
        var sz = readAndCheckSeqSize(1);
        var v = new Array(sz);
        for(var i = 0; i < sz; ++i)
        {
            v[i] = this._buf.get() === 1;
        }
        return v;
    }
    catch(ex)
    {
        //TODO assert BufferUnderflowException
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

//TODO Optionals
/*public void
readBoolSeq(int tag, Ice.Optional<boolean[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        v.set(readBoolSeq());
    }
    else
    {
        v.clear();
    }
}*/


BasicStream.prototype.writeShort = function(v)
{
    this._buf.putShort(v);
}

// TODO Optionals
/*public void
writeShort(int tag, Ice.ShortOptional v)
{
    if(v != null && v.isSet())
    {
        writeShort(tag, v.get());
    }
}

public void
writeShort(int tag, short v)
{
    if(writeOpt(tag, Ice.OptionalFormat.F2))
    {
        writeShort(v);
    }
}*/

BasicStream.prototype.writeShortSeq(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this._buf.putShortArray(v);
    }
}

/*public void
writeShortSeq(int tag, Ice.Optional<short[]> v)
{
    if(v != null && v.isSet())
    {
        writeShortSeq(tag, v.get());
    }
}

public void
writeShortSeq(int tag, short[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeSize(v == null || v.length == 0 ? 1 : v.length * 2 + (v.length > 254 ? 5 : 1));
        writeShortSeq(v);
    }
}*/


BasicStream.prototype.readShort = function()
{
    try
    {
        return this._buf.getShort();
    }
    catch(ex)
    {
        // TODO assert BufferUnderflowException?
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}

// TODO Optionals
/*public void
readShort(int tag, Ice.ShortOptional v)
{
    if(readOpt(tag, Ice.OptionalFormat.F2))
    {
        v.set(readShort());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.readShortSeq = function()
{
    try
    {
        var sz = this.readAndCheckSeqSize(2);
        return this._buf.getShortArray(sz);
    }
    catch(java.nio.BufferUnderflowException ex)
    {
        // TODO assert BufferUnderflowException ?
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

//TODO Optionals
/*public void
readShortSeq(int tag, Ice.Optional<short[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        skipSize();
        v.set(readShortSeq());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.writeInt = function(v)
{
    this._buf.putInt(v);
}

//TODO Optionals
/*public void
writeInt(int tag, Ice.IntOptional v)
{
    if(v != null && v.isSet())
    {
        writeInt(tag, v.get());
    }
}

public void
writeInt(int tag, int v)
{
    if(writeOpt(tag, Ice.OptionalFormat.F4))
    {
        writeInt(v);
    }
}*/

BasicStream.prototype.rewriteInt = function(v, dest)
{
    this._buf.b.putIntAt(dest, v);
}


BasicStream.prototype.writeIntSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this._buf.putIntArray(v);
    }
}

//TODO Optionals
/*public void
writeIntSeq(int tag, Ice.Optional<int[]> v)
{
    if(v != null && v.isSet())
    {
        writeIntSeq(tag, v.get());
    }
}

public void
writeIntSeq(int tag, int[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeSize(v == null || v.length == 0 ? 1 : v.length * 4 + (v.length > 254 ? 5 : 1));
        writeIntSeq(v);
    }
}*/

BasicStream.prototype.readInt = function()
{
    try
    {
        return this._buf.getInt();
    }
    catch(ex)
    {
        //TODO assert BufferUnderflowException 
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

//TODO Optionals
/*public void
readInt(int tag, Ice.IntOptional v)
{
    if(readOpt(tag, Ice.OptionalFormat.F4))
    {
        v.set(readInt());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.readIntSeq = function()
{
    try
    {
        var sz = readAndCheckSeqSize(4);
        return this._buf.getIntArray(sz);
    }
    catch(ex)
    {
        // TODO assert BufferUnderflowException?
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}

//TODO Optionals
/*public void
readIntSeq(int tag, Ice.Optional<int[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        skipSize();
        v.set(readIntSeq());
    }
    else
    {
        v.clear();
    }
}*/

//TODO Long mapping
/*public void
writeLong(long v)
{
    expand(8);
    _buf.b.putLong(v);
}

public void
writeLong(int tag, Ice.LongOptional v)
{
    if(v != null && v.isSet())
    {
        writeLong(tag, v.get());
    }
}

public void
writeLong(int tag, long v)
{
    if(writeOpt(tag, Ice.OptionalFormat.F8))
    {
        writeLong(v);
    }
}

public void
writeLongSeq(long[] v)
{
    if(v == null)
    {
        writeSize(0);
    }
    else
    {
        writeSize(v.length);
        expand(v.length * 8);
        java.nio.LongBuffer longBuf = _buf.b.asLongBuffer();
        longBuf.put(v);
        _buf.b.position(_buf.b.position() + v.length * 8);
    }
}

public void
writeLongSeq(int tag, Ice.Optional<long[]> v)
{
    if(v != null && v.isSet())
    {
        writeLongSeq(tag, v.get());
    }
}

public void
writeLongSeq(int tag, long[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeSize(v == null || v.length == 0 ? 1 : v.length * 8 + (v.length > 254 ? 5 : 1));
        writeLongSeq(v);
    }
}

public long
readLong()
{
    try
    {
        return _buf.b.getLong();
    }
    catch(java.nio.BufferUnderflowException ex)
    {
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}

public void
readLong(int tag, Ice.LongOptional v)
{
    if(readOpt(tag, Ice.OptionalFormat.F8))
    {
        v.set(readLong());
    }
    else
    {
        v.clear();
    }
}

BasicStream.prototype.readLongSeq = function()
{
    try
    {
        var sz = this.readAndCheckSeqSize(8);
        return _buf.getLongArray(sz);
    }
    catch(java.nio.BufferUnderflowException ex)
    {
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}*/

//TODO Optionals
/*public void
readLongSeq(int tag, Ice.Optional<long[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        skipSize();
        v.set(readLongSeq());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.writeFloat = function(v)
{
    this._buf.putFloat(v);
}

/*public void
writeFloat(int tag, Ice.FloatOptional v)
{
    if(v != null && v.isSet())
    {
        writeFloat(tag, v.get());
    }
}

public void
writeFloat(int tag, float v)
{
    if(writeOpt(tag, Ice.OptionalFormat.F4))
    {
        writeFloat(v);
    }
}*/

public void
BasicStream.prototype.writeFloatSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this._buf.putFloatArray(v);
    }
}

//TODO Optionals
/*public void
writeFloatSeq(int tag, Ice.Optional<float[]> v)
{
    if(v != null && v.isSet())
    {
        writeFloatSeq(tag, v.get());
    }
}

public void
writeFloatSeq(int tag, float[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeSize(v == null || v.length == 0 ? 1 : v.length * 4 + (v.length > 254 ? 5 : 1));
        writeFloatSeq(v);
    }
}*/

BasicStream.prototype.readFloat = function()
{
    try
    {
        return _buf.getFloat();
    }
    catch(ex)
    {
        //TODO assert BufferUnderflowException?
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

//TODO Optionals
/*public void
readFloat(int tag, Ice.FloatOptional v)
{
    if(readOpt(tag, Ice.OptionalFormat.F4))
    {
        v.set(readFloat());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.readFloatSeq = function()
{
    try
    {
        var sz = this.readAndCheckSeqSize(4);
        return this._buf.readFloatArray(sz);
    }
    catch(ex)
    {
        // TODO BufferUnderflowException
        throw new Ice.UnmarshalOutOfBoundsException();
    }
}

//TODO Optionals
/*public void
readFloatSeq(int tag, Ice.Optional<float[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        skipSize();
        v.set(readFloatSeq());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.writeDouble = function(v)
{
    this._buf.putDouble(v);
}

//TODO Optionals
/*public void
writeDouble(int tag, Ice.DoubleOptional v)
{
    if(v != null && v.isSet())
    {
        writeDouble(tag, v.get());
    }
}

public void
writeDouble(int tag, double v)
{
    if(writeOpt(tag, Ice.OptionalFormat.F8))
    {
        writeDouble(v);
    }
}*/

BasicStream.prototype.writeDoubleSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this._buf.putDoubleArray(v);
    }
}

//TODO Optionals
/*public void
writeDoubleSeq(int tag, Ice.Optional<double[]> v)
{
    if(v != null && v.isSet())
    {
        writeDoubleSeq(tag, v.get());
    }
}

public void
writeDoubleSeq(int tag, double[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeSize(v == null || v.length == 0 ? 1 : v.length * 8 + (v.length > 254 ? 5 : 1));
        writeDoubleSeq(v);
    }
}*/

BasicStream.prototype.readDouble = function()
{
    try
    {
        return this._buf.getDouble();
    }
    catch(ex)
    {
        //TODO assert BufferUnderflowException
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

// TODO Optionals
/*public void
readDouble(int tag, Ice.DoubleOptional v)
{
    if(readOpt(tag, Ice.OptionalFormat.F8))
    {
        v.set(readDouble());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.readDoubleSeq = function()
{
    try
    {
        var sz = this.readAndCheckSeqSize(8);
        return this._buf.getDoubleArray(sz);
    }
    catch(ex)
    {
        //TODO assert BufferUnderflowException?
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
}

/*public void
readDoubleSeq(int tag, Ice.Optional<double[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        skipSize();
        v.set(readDoubleSeq());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.writeString = function(v)
{
    if(v === null || v.length === 0)
    {
        this.writeSize(0);
    }
    else
    {
        this.startSize();
        this._buf.putString(v);
        this.endSize();
    }
}

//TODO Optionals
/*public void
writeString(int tag, Ice.Optional<String> v)
{
    if(v != null && v.isSet())
    {
        writeString(tag, v.get());
    }
}

public void
writeString(int tag, String v)
{
    if(writeOpt(tag, Ice.OptionalFormat.VSize))
    {
        writeString(v);
    }
}*/


BasicStream.prototype.writeStringSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        for(var i = 0; i < v.length; ++i)
        {
            this.writeString(v[i]);
        }
    }
}

// TODO Optionals
/*public void
writeStringSeq(int tag, Ice.Optional<String[]> v)
{
    if(v != null && v.isSet())
    {
        writeStringSeq(tag, v.get());
    }
}

public void
writeStringSeq(int tag, String[] v)
{
    if(writeOpt(tag, Ice.OptionalFormat.FSize))
    {
        startSize();
        writeStringSeq(v);
        endSize();
    }
}*/

BasicStream.prototype.readString = function()
{
    var len = this.readSize();

    if(len == 0)
    {
        return "";
    }
    else
    {
        //
        // Check the buffer has enough bytes to read.
        //
        if(this._buf.b.remaining < len)
        {
            throw new LocalEx.UnmarshalOutOfBoundsException();
        }

        try
        {
            return this._buf.getString(len);
        }
        catch(ex)
        {
            // TODO assert BufferUnderflowException?
            throw new LocalEx.UnmarshalOutOfBoundsException();
        }
    }
}

//TODO Optionals
/*public void
readString(int tag, Ice.Optional<String> v)
{
    if(readOpt(tag, Ice.OptionalFormat.VSize))
    {
        v.set(readString());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.readStringSeq = function()
{
    var sz = this.readAndCheckSeqSize(1);
    var v = new Array(sz);
    for(var i = 0; i < sz; ++i)
    {
        v[i] = this.readString();
    }
    return v;
}

//TODO Optionals
/*public void
readStringSeq(int tag, Ice.Optional<String[]> v)
{
    if(readOpt(tag, Ice.OptionalFormat.FSize))
    {
        skip(4);
        v.set(readStringSeq());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.writeProxy = function(v)
{
    this._instance.proxyFactory().proxyToStream(v, this);
}

//TODO Optionals
/*public void
writeProxy(int tag, Ice.Optional<Ice.ObjectPrx> v)
{
    if(v != null && v.isSet())
    {
        writeProxy(tag, v.get());
    }
}

public void
writeProxy(int tag, Ice.ObjectPrx v)
{
    if(writeOpt(tag, Ice.OptionalFormat.FSize))
    {
        startSize();
        writeProxy(v);
        endSize();
    }
}*/

BasicStream.prototype.readProxy = function()
{
    return this._instance.proxyFactory().streamToProxy(this);
}

/*TODO Optionals
BasicStream.prototype.readOptionalProxy = function(tag, v)
{
    if(this.readOpt(tag, Ice.OptionalFormat.FSize))
    {
        this.skip(4);
        v.set(this.readProxy());
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.writeEnum = function(v, maxValue)
{
    if(this.isWriteEncoding_1_0())
    {
        if(this.maxValue < 127)
        {
            this.writeByte(v);
        }
        else if(maxValue < 32767)
        {
            this.writeShort((v);
        }
        else
        {
            this.writeInt(v);
        }
    }
    else
    {
        this.writeSize(v);
    }
}


BasicStream.prototype.readEnum = function(maxValue)
{
    if(this.getReadEncoding().equals(Protocol.Encoding_1_0))
    {
        if(maxValue < 127)
        {
            return this.readByte();
        }
        else if(maxValue < 32767)
        {
            return this.readShort();
        }
        else
        {
            return this.readInt();
        }
    }
    else
    {
        return this.readSize();
    }
}


BasicStream.prototype.writeObject = function(v)
{
    this.initWriteEncaps();
    this._writeEncapsStack.encoder.writeObject(v);
}

//TODO Optionals
/*public <T extends Ice.Object> void
writeObject(int tag, Ice.Optional<T> v)
{
    if(v != null && v.isSet())
    {
        writeObject(tag, v.get());
    }
}

BasicStream.prototype.writeOptionalObject(tag, v)
{
    if(this.writeOpt(tag, Ice.OptionalFormat.Class))
    {
        this.writeObject(v);
    }
}*/

BasicStream.prototype.readObject = function(patcher)
{
    this.initReadEncaps();
    this._readEncapsStack.decoder.readObject(patcher);
}

//TODO Optionals
/*BasicStream.prototype.readOptionalObject = function(tag, v)
{
    if(this.readOpt(tag, OptionalFormat.Class))
    {
        //TODO OptionalObject mapping?
        var opt = new Ice.OptionalObject(v, Ice.Object.class, Ice.ObjectImpl.ice_staticId());
        this.readObject(opt);
    }
    else
    {
        v.clear();
    }
}*/

BasicStream.prototype.writeUserException = function(e)
{
    this.initWriteEncaps();
    this._writeEncapsStack.encoder.writeUserException(e);
}

BasicStream.prototype.throwException = function(factory)
{
    this.initReadEncaps();
    this._readEncapsStack.decoder.throwException(factory);
}

BasicStream.prototype.sliceObjects = function(b)
{
    this._sliceObjects = b;
}

BasicStream.prototype.readOptImpl = function(readTag, expectedFormat)
{
    if(this.isReadEncoding_1_0())
    {
        return false; // Optional members aren't supported with the 1.0 encoding.
    }

    while(true)
    {
        if(this._buf.position >= this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            return false; // End of encapsulation also indicates end of optionals.
        }

        var b = this.readByte();
        var v = b < 0 ? b + 256 : b;
        if(v === OPTIONAL_END_MARKER)
        {
            this._buf.position -= 1; // Rewind.
            return false;
        }

        var format = OptionalFormat.valueOf(v & 0x07); // First 3 bits.
        var tag = v >> 3;
        if(tag == 30)
        {
            tag = this.readSize();
        }

        if(tag > readTag)
        {
            var offset = tag < 30 ? 1 : (tag < 255 ? 2 : 6); // Rewind
            this._buf.position -= offset;
            return false; // No optional data members with the requested tag.
        }
        else if(tag < readTag)
        {
            this.skipOpt(format); // Skip optional data members
        }
        else
        {
            if(format != expectedFormat)
            {
                throw new LocalEx.MarshalException("invalid optional data member `" + tag + "': unexpected format");
            }
            return true;
        }
    }
}

BasicStream.prototype.writeOptImpl = function(tag, format)
{
    if(this.isWriteEncoding_1_0())
    {
        return false; // Optional members aren't supported with the 1.0 encoding.
    }

    var v = format.value;
    if(tag < 30)
    {
        v |= tag << 3;
        this.writeByte(v);
    }
    else
    {
        v |= 0x0F0; // tag = 30
        this.writeByte(v);
        this.writeSize(tag);
    }
    return true;
}

BasicStream.prototype.skipOpt = function(format)
{
    switch(format)
    {
    case F1:
    {
        this.skip(1);
        break;
    }
    case F2:
    {
        this.skip(2);
        break;
    }
    case F4:
    {
        this.skip(4);
        break;
    }
    case F8:
    {
        this.skip(8);
        break;
    }
    case Size:
    {
        this.skipSize();
        break;
    }
    case VSize:
    {
        this.skip(this.readSize());
        break;
    }
    case FSize:
    {
        this.skip(this.readInt());
        break;
    }
    case Class:
    {
        this.readObject(null);
        break;
    }
    }
}

BasicStream.prototype.skipOpts = function()
{
    //
    // Skip remaining un-read optional members.
    //
    while(true)
    {
        if(this._buf.position >= this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            return; // End of encapsulation also indicates end of optionals.
        }

        var b = this.readByte();
        var v = b < 0 ? b + 256 : b;
        if(v === OPTIONAL_END_MARKER)
        {
            return;
        }
        
        var format = OptionalFormat.valueOf(v & 0x07); // Read first 3 bits.
        if((v >> 3) == 30)
        {
            this.skipSize();
        }
        this.skipOpt(format);
    }
}

BasicStream.prototype.skip = function(size)
{
    if(size > this._buf.remaining)
    {
        throw new Ice.UnmarshalOutOfBoundsException();
    }
    this._buf.position += size;
}

BasicStream.prototype.skipSize = function()
{
    var b = this.readByte();
    if(b === -1)
    {
        this.skip(4);
    }
}

Object.defineProperty(BasicStream.prototype, "pos", {
    get: fucntion() { return this._buf.position; },
    set: fucntion(n) { this._buf.position = n; }
});

Object.defineProperty(BasicStream.prototype, "size", {
    get: fucntion() { return this._buf.limit; }
}

BasicStream.prototype.isEmpty = function()
{
    return this._buf.empty();
}