// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// ByteBuffer implementation to be used by web browsers, it uses ArrayBuffer as
// the store.
//

var ByteBuffer = function()
{
    this.b = null; // ArrayBuffer
    this.v = null; // DataView
    this._position = 0;
    this._limit = 0;
    this._shrinkCounter = 0;
}

Object.defineProperty(ByteBuffer.prototype, "position", {
    get: function() { return this._position; },
    set: function(position){
        if(position >= 0 && position < this._limit)
        {
            this._position = position;
        }
    }
});

Object.defineProperty(ByteBuffer.prototype, "limit", {
    get: function() { return this._limit; },
    set: function(limit){
        if(limit < this.capacity)
        {
            this._limit = limit;
            if(this._position >= limit)
            {
                this._position = limit - 1;
            }
        }
    }
});

Object.defineProperty(ByteBuffer.prototype, "capacity", {
    get: function() { return this.b === null ? 0 : this.b.byteLength; }
});

Object.defineProperty(ByteBuffer.prototype, "remaining", {
    get: function() { return this._limit - this._position; }
});

ByteBuffer.prototype.empty = function()
{
    return this._limit === 0;
}

ByteBuffer.prototype.resize = function(n)
{
    if(n == 0)
    {
        this.clear();
    }
    else if(n > this.capacity)
    {
        this.reserve(n); 
    }
    this._limit = n;
}

ByteBuffer.prototype.clear = function()
{
    this.b = null;
    this._position = 0;
    this._limit = 0;
}

//
// Call expand(n) to add room for n additional bytes. Note that expand()
// examines the current position of the buffer first; we don't want to
// expand the buffer if the caller is writing to a location that is
// already in the buffer.
//
ByteBuffer.prototype.expand = function(n)
{
    var sz = this.capacity === 0 ? n : this._position + n;
    if(sz > this._limit)
    {
        this.resize(sz);
    }
}

ByteBuffer.prototype.reset = function()
{
    if(this._limit > 0 && this._limit * 2 < this.capacity)
    {
        //
        // If the current buffer size is smaller than the
        // buffer capacity, we shrink the buffer memory to the
        // current size. This is to avoid holding on to too much
        // memory if it's not needed anymore.
        //
        if(++this._shrinkCounter > 2)
        {
            this.reserve(this._limit);
            this._shrinkCounter = 0;
        }
    }
    else
    {
        this._shrinkCounter = 0;
    }
    this._limit = 0;
    this._position = 0;
}

ByteBuffer.prototype.reserve = function(n)
{
    if(n > this.capacity)
    {
        var capacity = Math.max(n, 2 * this.capacity);
        capacity = Math.max(1024, capacity);
        if(!this.b)
        {
            this.b = new ArrayBuffer(capacity);
        }
        else
        {
            var b = new Uint8Array(capacity);
            b.set(new Uint8Array(this.b));
            this.b = b.buffer;
        }
        this.v = new DataView(this.b);
    }
    else if(n < this.capacity)
    {
        this.b = this.b.slice(0, this.capacity);
        this.v = new DataView(this.b);
    }
    else
    {
        return;
    }
}

ByteBuffer.prototype.put = function(v)
{
    this.resize(this._limit + 1);
    this.v.setUint8(this._position, v);
    this._position++;
}

ByteBuffer.prototype.putAt = function(i, v)
{
    if(i >= this._limit)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    this.v.setUint8(i, v, true);
}

ByteBuffer.prototype.putArray = function(v)
{
    //Expects an ArrayBuffer
    this.resize(this._limit + v.bytesLength);
    new Uint8Array(this.b).set(new Uint8Array(v), this._position);
    this._position += v.byteLength;
}

ByteBuffer.prototype.putShort = function(v)
{
    this.resize(this._limit + 2);
    this.v.setInt16(this._position, v, true);
    this._position += 2;
}

ByteBuffer.prototype.putShortAt = function(i, v)
{
    if(i + 2 >= this._limit)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    this.v.setInt16(i, v, true);
}

ByteBuffer.prototype.putShortArray = function(v)
{
    this.resize(this._limit + (v.length * 2));
    for(var i = 0; i < v.length; ++i)
    {
        this.v.setInt16(this._position, v[i], true);
        this._position += 2;
    }
}

ByteBuffer.prototype.putInt = function(v)
{
    this.resize(this._limit + 4);
    this.v.setInt32(this._position, v, true);
    this._position += 4;
}

ByteBuffer.prototype.putIntAt = function(i, v)
{
    if(i + 4 >= this._limit)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    this.v.setInt32(i, v, true);
}

ByteBuffer.prototype.putIntArray = function(v)
{
    this.resize(this._limit + (v.length * 4));
    for(var i = 0; i < v.length; ++i)
    {
        this.v.setInt32(this._position, v[i], true);
        this._position += 4;
    }
}

ByteBuffer.prototype.putFloat = function(v)
{
    this.resize(this._limit + 4);
    this.v.setFloat32(this._position, v, true);
    this._position += 4;
}

ByteBuffer.prototype.putFloatAt = function(i, v)
{
    if(i + 4 >= this._limit)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    this.v.setFloat32(i, v, true);
}

ByteBuffer.prototype.putFloatArray = function(v)
{
    this.resize(this._limit + (v.length * 4));
    for(var i = 0; i < v.length; ++i)
    {
        this.v.setFloat32(this._position, v[i], true);
        this._position += 4;
    }
}

ByteBuffer.prototype.putDouble = function(v)
{
    this.resize(this._limit + 8);
    this.v.setFloat64(this._position, v, true);
    this._position += 8;
}

ByteBuffer.prototype.putDoubleAt = function(i, v)
{
    if(i + 8 >= this._limit)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    this.v.setFloat64(i, v, true);
}

ByteBuffer.prototype.putDoubleArray = function(v)
{
    this.resize(this._limit + (v.length * 8));
    for(var i = 0; i < v.length; ++i)
    {
        this.v.setFloat32(this._position, v[i], true);
        this._position += 8;
    }
}

ByteBuffer.prototype.get = function()
{
    if(this._position >= this._limit)
    {
        throw new Error("BufferUnderflowException");
    }
    var v = this.v.getUint8(this._position);
    this._position++;
    return v;
}

ByteBuffer.prototype.getAt = function(i)
{
    if(i < 0 || i >= this._limit)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    return this.v.getUint8(i);
}

ByteBuffer.prototype.getArray = function(length)
{
    if(this._position + length > this._limit)
    {
        throw new Error("BufferUnderflowException");
    }
    return this.b.slice(this._position, this._position + length);
}

ByteBuffer.prototype.getShort = function()
{
    if(this._limit - this._position < 2)
    {
        throw new Error("BufferUnderflowException");
    }
    var v = this.v.getInt16(this._position, true);
    this._position += 2;
    return v;
}

ByteBuffer.prototype.getShortAt = function(i)
{
    if(this._limit - i < 2 || i < 0)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    return this.v.getInt16(i, true);
}

ByteBuffer.prototype.getInt = function()
{
    if(this._limit - this._position < 4)
    {
        throw new Error("BufferUnderflowException");
    }
    var v = this.v.getInt32(this._position, true);
    this._position += 4;
    return v;
}

ByteBuffer.prototype.getIntAt = function(i)
{
    if(this._limit - i < 4 || i < 0)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    return this.v.getInt32(i, true);
}

ByteBuffer.prototype.getFloat = function()
{
    if(this._limit - this._position < 4)
    {
        throw new Error("BufferUnderflowException");
    }
    var v = this.v.getFloat32(this._position, true);
    this._position += 4;
    return v;
}

ByteBuffer.prototype.getFloatAt = function(i)
{
    if(this._limit - i < 4 || i < 0)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    return this.v.getFloat32(i, true);
}

ByteBuffer.prototype.getDouble = function()
{
    if(this._limit - this._position < 8)
    {
        throw new Error("BufferUnderflowException");
    }
    var v = this.v.getFloat64(this._position, true);
    this._position += 8;
    return v;
}

ByteBuffer.prototype.getDoubleAt = function(i)
{
    if(this._limit - i < 8 || i < 0)
    {
        throw new Error("IndexOutOfBoundsException");
    }
    return this.v.getFloat64(i, true);
}

module.exports = Buffer;
