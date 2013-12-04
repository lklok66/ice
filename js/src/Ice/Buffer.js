// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Buffer = function(length)
{
    this._buffers = null;
    this._length = length === undefined ? 256 : length;
    this._size = 0;
    this._capacity = 0;
}

Buffer.prototype.resize = function(n)
{
    if(n == 0)
    {
        this.clear();
    }
    else if(n > this._capacity)
    {
        this.reserve(n); 
    }
    this._size = n;
}

Buffer.prototype.clear = function()
{
    this._buffers = [];
    this._size = 0;
    this._capacity = 0;
}

Buffer.prototype.reserve = function(n)
{
    if(this._capacity === n)
    {
        return;
    }
    
    var c = this._capacity;
    var b = Math.floor(n / this._length); // Number of buffers required to hold at least n bytes
    if(n % this._length !== 0)
    {
        b++;
    }
    this._capacity = b * this._length;
    
    try
    {
        if(this._capacity > c)
        {
            var buffers = this._buffers ? this._buffers.slice(0) : new Array();
            for(var i = buffers.length; i < b; ++i)
            {
                buffers.push(new Uint8Array(this._length));
            }
            this._buffers = buffers;
        }
        else
        {
            var buffers = this._buffers ? this._buffers.slice(0) : new Array();
            for(var i = buffers.length; i > b; ++i)
            {
                buffers.pop();
            }
            this._buffers = buffers;
        }
    }
    catch(e)
    {
        this._capacity = c; // Restore the previous capacity.
        throw e;
    }
}

Buffer.prototype.write = function(value)
{
    this.resize(this._size + 1);
    this._buffers[this._buffers.length - 1][this._size - 1] = value;
}

Buffer.prototype.set = function(offset, value)
{
    this._buffers[Math.floor(offset / this._length)][offset % this._length] = value;
}

Buffer.prototype.read = function(offset, length)
{
    length = (length === undefined || length === null) ? 1 : length;
    if(length === 1)
    {
        return this._buffers[Math.floor(offset / this._length)][offset % this._length];
    }
    else
    {
        var result = new Uint8Array(length);
        for(var i = 0; i < length; ++i)
        {
            result[i] = this._buffers[Math.floor(offset + i / this._length)][offset + i % this._length];
        }
        return result;
    }
}

module.exports = Buffer;
