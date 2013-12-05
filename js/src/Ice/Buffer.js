// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Buffer = function()
{
    this.b = null;
    this.size = 0;
    this.capacity = 0;
}

Buffer.prototype.resize = function(n)
{
    if(n == 0)
    {
        this.clear();
    }
    else if(n > this.capacity)
    {
        this.reserve(n); 
    }
    this.size = n;
}

Buffer.prototype.clear = function()
{
    this.b = null;
    this.size = 0;
    this.capacity = 0;
}

Buffer.prototype.reserve = function(n)
{
    var c = this.capacity;
    
    try
    {
        if(n > this.capacity)
        {
            this.capacity = Math.max(n, 2 * this.capacity);
            this.capacity = Math.max(1024, this.capacity);
            
            if(!this.b)
            {
                this.b = new Uint8Array(this.capacity);
            }
            else
            {
                var b = new Uint8Array(this.capacity);
                b.set(this.b);
                this.b = b;
            }
        }
        else if(n < this.capacity)
        {
            this.capacity = n;
            this.b  = new Uint8Array(this.b.buffer.slice(0, this.capacity - 1));
        }
        else
        {
            return;
        }
    }
    catch(ex)
    {
        this.capacity = c; // Restore the previous capacity.
        throw ex;
    }
}

Buffer.prototype.write = function(value)
{
    this.resize(this.size + 1);
    this.b[this.size - 1] = value;
}

Buffer.prototype.set = function(offset, value)
{
    this.b[offset] = value;
}

Buffer.prototype.read = function(offset, length)
{
    length = (length === undefined || length === null) ? 1 : length;
    if(length === 1)
    {
        return this.b[offset];
    }
    else
    {
        return this.b.subarray(offset, offset + length);
    }
}

module.exports = Buffer;
