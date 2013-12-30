// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){
        
        var Ice = global.Ice || {};
        
        var HashMap = function(h)
        {
            this._size = 0;
            this._head = null;
            this._initialCapacity = 32;
            this._loadFactor = 0.75;
            this._table = [];
            this._comparator = function(k1, k2) { return k1 === k2; };

            var i, length;
            if(h === undefined || h === null || h._size === 0)
            {
                this._threshold = this._initialCapacity * this._loadFactor;
                for(i = 0; i < this._initialCapacity; i++)
                {
                    this._table[i] = null;
                }
            }
            else
            {
                this._threshold = h._threshold;
                this._comparator = h._comparator;
                length = h._table.length;
                this._table.length = length;
                for(i = 0; i < length; i++)
                {
                    this._table[i] = null;
                }
                this.merge(h);
            }
        };

        Object.defineProperty(HashMap.prototype, "size", {
            get: function() { return this._size; }
        });

        Object.defineProperty(HashMap.prototype, "entries", {
            get: function() { return this._head; }
        });

        Object.defineProperty(HashMap.prototype, "comparator", {
            get: function() { return this._comparator; },
            set: function(fn) { this._comparator = fn; }
        });

        Object.defineProperty(HashMap, "compareEquals", {
            get: function() { return function(k1, k2) { return k1.equals(k2); }; }
        });

        function setInternal(map, key, value, hash, index)
        {
            //
            // Search for an entry with the same key.
            //
            for(var e = map._table[index]; e !== null; e = e._nextInBucket)
            {
                if(e._hash === hash && map.isEqual(key, e._key))
                {
                    //
                    // Found a match, update the value.
                    //
                    e._value = value;
                    return undefined;
                }
            }

            //
            // No match found, add a new entry.
            //
            map.add(key, value, hash, index);
            return undefined;
        }

        HashMap.prototype.set = function(key, value)
        {    
            var hash = this.computeHash(key);

            var index = this.hashIndex(hash, this._table.length);

            return setInternal(this, key, value, hash, index);
        };

        HashMap.prototype.get = function(key)
        {
            var e = this.findEntry(key, this.computeHash(key));
            return e !== undefined ? e._value : undefined;
        };

        HashMap.prototype.has = function(key)
        {
            return this.findEntry(key, this.computeHash(key)) !== undefined;
        };

        HashMap.prototype.delete = function(key)
        {
            var hash = this.computeHash(key);

            var index = this.hashIndex(hash, this._table.length);

            //
            // Search for an entry with the same key.
            //
            var prev = null;
            for(var e = this._table[index]; e !== null; e = e._nextInBucket)
            {
                if(e._hash === hash && this.isEqual(key, e._key))
                {
                    //
                    // Found a match.
                    //
                    this._size--;

                    //
                    // Remove from bucket.
                    //
                    if(prev !== null)
                    {
                        prev._nextInBucket = e._nextInBucket;
                    }
                    else
                    {
                        this._table[index] = e._nextInBucket;
                    }

                    //
                    // Unlink the entry.
                    //
                    if(e._prev !== null)
                    {
                        e._prev._next = e._next;
                    }
                    if(e._next !== null)
                    {
                        e._next._prev = e._prev;
                    }

                    if(this._head === e)
                    {
                        this._head = e._next;
                    }

                    return e._value;
                }

                prev = e;
            }

            return undefined;
        };

        HashMap.prototype.clear = function()
        {
            for(var i = 0; i < this._table.length; ++i)
            {
                this._table[i] = null;
            }
            this._head = null;
            this._size = 0;
        };

        HashMap.prototype.forEach = function(fn, obj)
        {
            obj = obj === undefined ? fn : obj;
            for(var e = this._head; e !== null; e = e._next)
            {
                fn.call(obj, e._key, e._value);
            }
        };

        HashMap.prototype.hashCode = function()
        {
            var hash = 0;

            for(var e = this._head; e !== null; e = e._next)
            {
                hash = hash * 5 + this.computeHash(e._key);
                hash = hash * 5 + this.computeHash(e._value);
            }

            return hash;
        };

        HashMap.prototype.equals = function(other)
        {
            if(other === null || !(other instanceof HashMap) || this._size !== other._size)
            {
                return false;
            }

            for(var e = this._head; e !== null; e = e._next)
            {
                var oe = other.findEntry(e._key, e._hash);
                if(oe === undefined || !this.isEqual(e._value, oe._value))
                {
                    return false;
                }
            }

            return true;
        };

        HashMap.prototype.clone = function()
        {
            return new HashMap(this);
        };

        HashMap.prototype.merge = function(from)
        {
            for(var e = from._head; e !== null; e = e._next)
            {
                setInternal(this, e._key, e._value, e._hash, this.hashIndex(e._hash, this._table.length));
            }
        };

        HashMap.prototype.add = function(key, value, hash, index)
        {
            //
            // Create a new table entry.
            //
            /*
            var e =
            {
                key: key,
                value: value,
                prev: null,
                next: null,
                _hash: hash
            }
            */
            var e = Object.create(null, {
                "key": {
                    enumerable: true,
                    get: function() { return this._key; }
                },
                "value": {
                    enumerable: true,
                    get: function() { return this._value; }
                },
                "next": {
                    enumerable: true,
                    get: function() { return this._next; }
                },
                "_key": {
                    enumerable: false,
                    writable: true,
                    value: key
                },
                "_value": {
                    enumerable: false,
                    writable: true,
                    value: value
                },
                "_prev": {
                    enumerable: false,
                    writable: true,
                    value: null
                },
                "_next": {
                    enumerable: false,
                    writable: true,
                    value: null
                },
                "_nextInBucket": {
                    enumerable: false,
                    writable: true,
                    value: null
                },
                "_hash": {
                    enumerable: false,
                    writable: true,
                    value: hash
                }
            });
            e._nextInBucket = this._table[index];
            this._table[index] = e;

            e._next = this._head;
            if(this._head !== null)
            {
                this._head._prev = e;
            }
            this._head = e;

            this._size++;
            if(this._size >= this._threshold)
            {
                this.resize(this._table.length * 2);
            }
        };

        HashMap.prototype.resize = function(capacity)
        {
            var oldTable = this._table;

            var newTable = [];
            for(var i = 0; i < capacity; i++)
            {
                newTable[i] = null;
            }

            //
            // Re-assign all entries to buckets.
            //
            for(var e = this._head; e !== null; e = e._next)
            {
                var index = this.hashIndex(e._hash, capacity);
                e._nextInBucket = newTable[index];
                newTable[index] = e;
            }

            this._table = newTable;
            this._threshold = (capacity * this._loadFactor);
        };

        HashMap.prototype.findEntry = function(key, hash)
        {
            var index = this.hashIndex(hash, this._table.length);

            //
            // Search for an entry with the same key.
            //
            for(var e = this._table[index]; e !== null; e = e._nextInBucket)
            {
                if(e._hash == hash && this.isEqual(key, e._key))
                {
                    return e;
                }
            }

            return undefined;
        };

        HashMap.prototype.hashIndex = function(hash, len)
        {
            return hash & (len - 1);
        };

        HashMap.prototype.computeHash = function(v)
        {
            if(typeof(v.hashCode) === "function")
            {
                return v.hashCode();
            }

            var hash = 0;
            var type = typeof(v);
            if(type === "string" || v instanceof String)
            {
                hash = this.computeStringHash(v);
            }
            else if(type === "number" || v instanceof Number)
            {
                hash = v.toFixed(0);
            }
            else if(type === "boolean" || v instanceof Boolean)
            {
                hash = v ? 1 : 0;
            }
            else if(v !== null)
            {
                throw "cannot compute hash for value of type " + type;
            }
            return hash;
        };

        HashMap.prototype.computeStringHash = function(s)
        {
            var hash = 0;
            var n = s.length;

            for(var i = 0; i < n; i++)
            {
                hash = 31 * hash + s.charCodeAt(i);
            }

            return hash;
        };

        HashMap.prototype.isEqual = function(k1, k2)
        {
            return this._comparator.call(this._comparator, k1, k2);
        };

        Ice.HashMap = HashMap;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/HashMap"));
