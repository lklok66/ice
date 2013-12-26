// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(module, exports, require){
        //
        // Buffer implementation to be used by web browsers, it uses ArrayBuffer as
        // the store.
        //

        var Long = require("Ice/Long").Ice.Long;

        var Buffer = function(buffer)
        {
            if(buffer !== undefined)
            {
                this.b = buffer;
                this.v = new DataView(this.b);
            }
            else
            {
                this.b = null; // ArrayBuffer
                this.v = null; // DataView
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
                return new Uint8Array(0).buffer;
            }
            else
            {
                return new Uint8Array(data).buffer;
            }
        };

        Buffer.nativeSize = function(data)
        {
            return data.byteLength;
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
            get: function() { return this.b === null ? 0 : this.b.byteLength; }
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
            this.v = null;
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
        };

        Buffer.prototype.put = function(v)
        {
            if(this._position === this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.v.setUint8(this._position, v);
            this._position++;
        };

        Buffer.prototype.putAt = function(i, v)
        {
            if(i >= this._limit)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.v.setUint8(i, v);
        };

        Buffer.prototype.putArray = function(v)
        {
            if(v.byteLength > 0)
            {
                //Expects an ArrayBuffer
                if(this._position + v.length > this._limit)
                {
                    throw new Error("BufferOverflowException");
                }
                new Uint8Array(this.b).set(new Uint8Array(v), this._position);
                this._position += v.byteLength;
            }
        };

        Buffer.prototype.putShort = function(v)
        {
            if(this._position + 2 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.v.setInt16(this._position, v, true);
            this._position += 2;
        };

        Buffer.prototype.putInt = function(v)
        {
            if(this._position + 4 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.v.setInt32(this._position, v, true);
            this._position += 4;
        };

        Buffer.prototype.putIntAt = function(i, v)
        {
            if(i + 4 > this._limit || i < 0)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            this.v.setInt32(i, v, true);
        };

        Buffer.prototype.putFloat = function(v)
        {
            if(this._position + 4 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.v.setFloat32(this._position, v, true);
            this._position += 4;
        };

        Buffer.prototype.putDouble = function(v)
        {
            if(this._position + 8 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.v.setFloat64(this._position, v, true);
            this._position += 8;
        };

        Buffer.prototype.putLong = function(v)
        {
            if(this._position + 8 > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            this.v.setInt32(this._position, v.low, true);
            this._position += 4;
            this.v.setInt32(this._position, v.high, true);
            this._position += 4;
        };

        Buffer.prototype.writeString = function(stream, v)
        {
            //
            // Encode the string as utf8
            //
            var encoded = unescape(encodeURIComponent(v));
            
            stream.writeSize(encoded.length);
            stream.expand(encoded.length);
            this.putString(encoded, encoded.length);
        };

        Buffer.prototype.putString = function(v, sz)
        {
            if(this._position + sz > this._limit)
            {
                throw new Error("BufferOverflowException");
            }
            for(var i = 0; i < sz; ++i)
            {
                this.v.setUint8(this._position, v.charCodeAt(i));
                this._position++;
            }
        };

        Buffer.prototype.get = function()
        {
            if(this._position >= this._limit)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.v.getUint8(this._position);
            this._position++;
            return v;
        };

        Buffer.prototype.getAt = function(i)
        {
            if(i < 0 || i >= this._limit)
            {
                throw new Error("IndexOutOfBoundsException");
            }
            return this.v.getUint8(i);
        };

        Buffer.prototype.getArray = function(length)
        {
            if(this._position + length > this._limit)
            {
                throw new Error("BufferUnderflowException");
            }
            return this.b.slice(this._position, this._position + length);
        };

        Buffer.prototype.getShort = function()
        {
            if(this._limit - this._position < 2)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.v.getInt16(this._position, true);
            this._position += 2;
            return v;
        };

        Buffer.prototype.getInt = function()
        {
            if(this._limit - this._position < 4)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.v.getInt32(this._position, true);
            this._position += 4;
            return v;
        };

        Buffer.prototype.getFloat = function()
        {
            if(this._limit - this._position < 4)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.v.getFloat32(this._position, true);
            this._position += 4;
            return v;
        };

        Buffer.prototype.getDouble = function()
        {
            if(this._limit - this._position < 8)
            {
                throw new Error("BufferUnderflowException");
            }
            var v = this.v.getFloat64(this._position, true);
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
            v.low = this.v.getInt32(this._position, true);
            this._position += 4;
            v.high = this.v.getInt32(this._position, true);
            this._position += 4;
            return v;
        };

        Buffer.prototype.getString = function(length)
        {
            if(this._position + length > this._limit)
            {
                throw new Error("BufferUnderflowException");
            }
            
            var data = new DataView(this.b, this._position, length);
            var s = "";
            
            for(var i = 0; i < length; ++i)
            {
                s += String.fromCharCode(data.getUint8(i));
            }
            this._position += length;
            s = decodeURIComponent(escape(s));
            return s;
        };

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.Buffer = Buffer;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Buffer"));
