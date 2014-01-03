// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Ice.Buffer implementation to be used by Node.js, it uses node Buffer 
// as the store.
//

//
// Define Node.Buffer as an alias to NodeJS global Buffer type,
// that allow us to refer to Ice.Buffer as Buffer in this file.
//
(function(module, name){
    var __m = function(global, module, exports, require){
        var Node = { Buffer: global.Buffer };

        require("Ice/Long");
        
        var Ice = global.Ice || {};
        var Long = Ice.Long;

        var Buffer = function(buffer)
        {
            if(buffer !== undefined)
            {
                this.b = buffer;
            }
            else
            {
                this.b = null;
            }
            this._position = 0;
            this._limit = 0;
            this._shrinkCounter = 0;
        };

        //
        // Create a native buffer from an array of bytes.
        //
        Buffer.createNative = function(data)
        {
            if(data === undefined)
            {
                return new Node.Buffer(0);
            }
            else
            {
                return new Node.Buffer(data);
            }
        };

        Buffer.nativeSize = function(data)
        {
            return data.length;
        };

        Object.defineProperty(Buffer.prototype, "position", {
            get: function() { return this._position; },
            set: function(position){
                if(position >= 0 && position <= this._limit)
                {
                    this._position = position;
                }
            }
        });

        Object.defineProperty(Buffer.prototype, "limit", {
            get: function() { return this._limit; },
            set: function(limit){
                if(limit <= this.capacity)
                {
                    this._limit = limit;
                    if(this._position > limit)
                    {
                        this._position = limit;
                    }
                }
            }
        });

        Object.defineProperty(Buffer.prototype, "capacity", {
            get: function() { return this.b === null ? 0 : this.b.length; }
        });

        Object.defineProperty(Buffer.prototype, "remaining", {
            get: function() { return this._limit - this._position; }
        });

        Buffer.prototype.empty = function()
        {
            return this._limit === 0;
        };

        Buffer.prototype.resize = function(n)
        {
            if(n === 0)
            {
                this.clear();
            }
            else if(n > this.capacity)
            {
                this.reserve(n); 
            }
            this._limit = n;
        };

        Buffer.prototype.clear = function()
        {
            this.b = null;
            this._position = 0;
            this._limit = 0;
        };

        //
        // Call expand(n) to add room for n additional bytes. Note that expand()
        // examines the current position of the buffer first; we don't want to
        // expand the buffer if the caller is writing to a location that is
        // already in the buffer.
        //
        Buffer.prototype.expand = function(n)
        {
            var sz = this.capacity === 0 ? n : this._position + n;
            if(sz > this._limit)
            {
                this.resize(sz);
            }
        };

        Buffer.prototype.reset = function()
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
        };

        Buffer.prototype.reserve = function(n)
        {
            var b, capacity;
            if(n > this.capacity)
            {
                capacity = Math.max(n, 2 * this.capacity);
                capacity = Math.max(1024, capacity);
                if(this.b === null)
                {
                    this.b = new Node.Buffer(capacity);
                }
                else
                {
                    b = new Node.Buffer(capacity);
                    this.b.copy(b);
                    this.b = b;
                }
            }
            else if(n < this.capacity)
            {
                this.b = this.b.slice(0, this.capacity);
            }
            else
            {
                return;
            }
        };

        Buffer.prototype.put = function(v)
        {
            if(this._position === this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.b.writeUInt8(v, this._position, true);
            this._position++;
        };

        Buffer.prototype.putAt = function(i, v)
        {
            if(i >= this._limit)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.b.writeUInt8(v, i, true);
        };

        Buffer.prototype.putArray = function(v)
        {
            //Expects a Nodejs Buffer
            if(this._position + v.length > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            v.copy(this.b, this._position);
            this._position += v.length;
        };

        Buffer.prototype.putShort = function(v)
        {
            if(this._position + 2 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.b.writeInt16LE(v, this._position, true);
            this._position += 2;
        };

        Buffer.prototype.putShortAt = function(i, v)
        {
            if(i + 2 > this._limit || i < 0)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.b.writeInt16LE(v, i, true);
        };

        Buffer.prototype.putInt = function(v)
        {
            if(this._position + 4 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.b.writeInt32LE(v, this._position, true);
            this._position += 4;
        };

        Buffer.prototype.putIntAt = function(i, v)
        {
            if(i + 4 > this._limit || i < 0)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.b.writeInt32LE(v, i, true);
        };

        Buffer.prototype.putFloat = function(v)
        {
            if(this._position + 4 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.b.writeFloatLE(v, this._position, true);
            this._position += 4;
        };

        Buffer.prototype.putFloatAt = function(i, v)
        {
            if(i + 4 > this._limit || i < 0)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.b.writeFloatLE(v, i, true);
        };

        Buffer.prototype.putDouble = function(v)
        {
            if(this._position + 8 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.b.writeDoubleLE(v, this._position, true);
            this._position += 8;
        };

        Buffer.prototype.putDoubleAt = function(i, v)
        {
            if(i + 8 > this._limit || i < 0)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.b.writeDoubleLE(v, i, true);
        };

        Buffer.prototype.putLong = function(v)
        {
            if(this._position + 8 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.b.writeUInt32LE(v.low, this._position, true);
            this._position += 4;
            this.b.writeUInt32LE(v.high, this._position, true);
            this._position += 4;
        };

        Buffer.prototype.putLongAt = function(i, v)
        {
            if(i + 8 > this._limit || i < 0)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.b.writeUInt32LE(v.low, i, true);
            this.b.writeUInt32LE(v.high, i + 4, true);
        };

        Buffer.prototype.writeString = function(stream, v)
        {
            var sz = Node.Buffer.byteLength(v);
            stream.writeSize(sz);
            stream.expand(sz);
            this.putString(v, sz);
        };

        Buffer.prototype.putString = function(v, sz)
        {
            if(this._position + sz > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            var bytes = this.b.write(v, this._position);
            //
            // Check all bytes were written
            //
            if(bytes < sz)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this._position += sz;
        };

        Buffer.prototype.get = function()
        {
            if(this._position >= this._limit)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.b.readUInt8(this._position, true);
            this._position++;
            return v;
        };

        Buffer.prototype.getAt = function(i)
        {
            if(i < 0 || i >= this._limit)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            return this.b.readUInt8(i, true);
        };

        Buffer.prototype.getArray = function(length)
        {
            if(this._position + length > this._limit)
            {
                throw new Error("BufferUnderflowException");
            }
            var buffer = new Node.Buffer(length);
            this.b.slice(this._position, this._position + length).copy(buffer);
            this._position += length;
            return buffer;
        };
        
        Buffer.prototype.getArrayAt = function(position, length)
        {
            if(position + length > this._limit)
            {
                throw new Error("BufferUnderflowException");
            }
            var buffer = new Node.Buffer(length);
            length = length === undefined ? (this.b.length - position) : length;
            this.b.slice(position, position + length).copy(buffer);
            return buffer;
        };

        Buffer.prototype.getShort = function()
        {
            var v;
            if(this._limit - this._position < 2)
            {
                throw new Error("BufferUnderflowException");
            }
            v = this.b.readInt16LE(this._position, true);
            this._position += 2;
            return v;
        };

        Buffer.prototype.getInt = function()
        {
            var v;
            if(this._limit - this._position < 4)
            {
                throw new Error("BufferUnderflowException");
            }
            v = this.b.readInt32LE(this._position, true);
            this._position += 4;
            return v;
        };

        Buffer.prototype.getFloat = function()
        {
            if(this._limit - this._position < 4)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.b.readFloatLE(this._position, true);
            this._position += 4;
            return v;
        };

        Buffer.prototype.getDouble = function()
        {
            if(this._limit - this._position < 8)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.b.readDoubleLE(this._position, true);
            this._position += 8;
            return v;
        };

        Buffer.prototype.getLong = function()
        {
            if(this._limit - this._position < 8)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = new Long();
            v.low = this.b.readUInt32LE(this._position, true);
            this._position += 4;
            v.high = this.b.readUInt32LE(this._position, true);
            this._position += 4;
            return v;
        };

        Buffer.prototype.getString = function(length)
        {
            if(this._position + length > this._limit)
            {
                throw new Error("BufferUnderflowException");
            }
            var s =this.b.toString("utf8", this._position, this._position + length);
            this._position += length;
            return s;
        };

        Ice.Buffer = Buffer;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Buffer"));
