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
        var HashMap = require("Ice/HashMap").Ice.HashMap;

        var StreamHelpers = {};

        StreamHelpers.BoolHelper = {};

        StreamHelpers.BoolHelper.write = function(os, v)
        {
            os.writeBool(v);
        };

        StreamHelpers.BoolHelper.read = function(os)
        {
            return os.readBool();
        };

        Object.defineProperty(StreamHelpers.BoolHelper, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.ByteHelper= {};

        StreamHelpers.ByteHelper.write = function(os, v)
        {
            os.writeByte(v);
        };

        StreamHelpers.ByteHelper.read = function(os)
        {
            return os.readByte();
        };

        Object.defineProperty(StreamHelpers.ByteHelper, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.ShortHelper = {};

        StreamHelpers.ShortHelper.write = function(os, v)
        {
            os.writeShort(v);
        };

        StreamHelpers.ShortHelper.read = function(os)
        {
            return os.readShort();
        };

        Object.defineProperty(StreamHelpers.ShortHelper, "minWireSize", {
            get: function(){ return 2; }
        });


        StreamHelpers.IntHelper = {};

        StreamHelpers.IntHelper.write = function(os, v)
        {
            os.writeInt(v);
        };

        StreamHelpers.IntHelper.read = function(os)
        {
            return os.readInt();
        };

        Object.defineProperty(StreamHelpers.IntHelper, "minWireSize", {
            get: function(){ return 4; }
        });

        StreamHelpers.LongHelper = {};

        StreamHelpers.LongHelper.write = function(os, v)
        {
            os.writeLong(v);
        };

        StreamHelpers.LongHelper.read = function(os)
        {
            return os.readLong();
        };

        Object.defineProperty(StreamHelpers.LongHelper, "minWireSize", {
            get: function(){ return 8; }
        });

        StreamHelpers.FloatHelper = {};

        StreamHelpers.FloatHelper.write = function(os, v)
        {
            os.writeFloat(v);
        };

        StreamHelpers.FloatHelper.read = function(os)
        {
            return os.readFloat();
        };

        Object.defineProperty(StreamHelpers.FloatHelper, "minWireSize", {
            get: function(){ return 4; }
        });

        StreamHelpers.DoubleHelper = {};

        StreamHelpers.DoubleHelper.write = function(os, v)
        {
            os.writeDouble(v);
        };

        StreamHelpers.DoubleHelper.read = function(os)
        {
            return os.readDouble();
        };

        Object.defineProperty(StreamHelpers.DoubleHelper, "minWireSize", {
            get: function(){ return 8; }
        });

        StreamHelpers.StringHelper = {};

        StreamHelpers.StringHelper.write = function(os, v)
        {
            os.writeString(v);
        };

        StreamHelpers.StringHelper.read = function(os)
        {
            return os.readString();
        };

        Object.defineProperty(StreamHelpers.StringHelper, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.ProxyHelper = {};

        StreamHelpers.ProxyHelper.write = function(os, v)
        {
            os.writeProxy(v);
        };

        StreamHelpers.ProxyHelper.read = function(os)
        {
            return os.readProxy();
        };

        Object.defineProperty(StreamHelpers.ProxyHelper, "minWireSize", {
            get: function(){ return 2; }
        });

        var EnumHelper = function(){};

        EnumHelper.prototype.read = function(os)
        {
            return os.readEnum(this.type);
        };

        EnumHelper.prototype.write = function(os, v)
        {
            os.writeEnum(v);
        };

        StreamHelpers.generateEnumHelper = function(type, minWireSize)
        {
            //
            // If we have already generated a helper for this
            // type return that.
            //
            if(type.__helper !== undefined)
            {
                return type.__helper;
            }
            
            var Helper = new EnumHelper();
            
            Object.defineProperty(Helper, "minWireSize", {
                get: function(){ return 1; }
            });
            
            Object.defineProperty(Helper, "type", {
                get: function(){ return type; }
            });
            
            //
            // Cache the helper with the type.
            //
            type.__helper = Helper;
            return Helper;
        };

        var StructHelper = function(){};

        StructHelper.prototype.read = function(os)
        {
            return os.readStruct(this.type);
        };

        StructHelper.prototype.write = function(os, v)
        {
            os.writeStruct(v);
        };

        StreamHelpers.generateStructHelper = function(type, minWireSize)
        {
            //
            // If we have already generated a helper for this
            // type return that.
            //
            if(type.__helper !== undefined)
            {
                return type.__helper;
            }
            
            var Helper = new StructHelper();
            
            Object.defineProperty(Helper, "minWireSize", {
                get: function(){ return minWireSize; }
            });
            
            Object.defineProperty(Helper, "type", {
                get: function(){ return type; }
            });
            
            //
            // Cache the helper with the type.
            //
            type.__helper = Helper;
            return Helper;
        };

        var SequenceHelper = function(){};

        SequenceHelper.prototype.read = function(os)
        {
            // Cache the element helper.
            var helper = this.elementHelper;
            var sz = os.readAndCheckSeqSize(helper.minWireSize);
            var v = [];
            v.length = sz;
            for(var i = 0; i < sz; ++i)
            {
                v[i] = helper.read(os);
            }
            return v;
        };

        SequenceHelper.prototype.write = function(os, v)
        {
            if(v === null || v.length === 0)
            {
                os.writeSize(0);
            }
            else
            {
                var helper = this.elementHelper;
                os.writeSize(v.length);
                for(var i = 0; i < v.length; ++i)
                {
                    helper.write(os, v[i]);
                }
            }
        };

        Object.defineProperty(SequenceHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.generateSeqHelper = function(elementHelper)
        {
            var Helper = new SequenceHelper();
            
            Object.defineProperty(Helper, "elementHelper", {
                get: function(){ return elementHelper; }
            });
            
            return Helper;
        };

        var ObjectSequenceHelper = function(){};

        ObjectSequenceHelper.prototype.read = function(os)
        {
            var sz = os.readAndCheckSeqSize(1);
            var v = [];
            v.length = sz;
            var type = this.type;
            var readObjectAtIndex = function(idx)
            {
                os.readObject(function(obj) { v[idx] = obj; }, type);
            };
            
            for(var i = 0; i < sz; ++i)
            {
                readObjectAtIndex(i);
            }
            return v;
        };

        ObjectSequenceHelper.prototype.write = function(os, v)
        {
            if(v === null || v.size === 0)
            {
                os.writeSize(0);
            }
            else
            {
                var sz = v.length;
                os.writeSize(sz);
                for(var i = 0; i < sz; ++i)
                {
                    os.writeObject(v[i]);
                }
            }
        };

        Object.defineProperty(ObjectSequenceHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.generateObjectSeqHelper = function(type)
        {
            var Helper = new ObjectSequenceHelper();
            
            Object.defineProperty(Helper, "type", {
                get: function(){ return type; }
            });
            
            return Helper;
        };

        var DictionaryHelper = function(){};

        DictionaryHelper.prototype.read = function(os)
        {
            var v = new HashMap();
            var sz = os.readSize();
            var keyHelper = this.keyHelper;
            var valueHelper = this.valueHelper;
            for(var i = 0; i < sz; ++i)
            {
                v.set(keyHelper.read(os), valueHelper.read(os));
            }
            return v;
        };

        DictionaryHelper.prototype.write = function(os, v)
        {
            if(v === null || v.size === 0)
            {
                os.writeSize();
            }
            else
            {
                var keyHelper = this.keyHelper;
                var valueHelper = this.valueHelper;
                os.writeSize(v.size);
                for(var e = v.entries; e !== null; e = e.next)
                {
                    keyHelper.write(os, e.key);
                    valueHelper.write(os, e.value);
                }
            }
        };

        Object.defineProperty(DictionaryHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.generateDictHelper = function(keyHelper, valueHelper)
        {
            var Helper = new DictionaryHelper();
            
            Object.defineProperty(Helper, "keyHelper", {
                get: function(){ return keyHelper; }
            });
            
            Object.defineProperty(Helper, "valueHelper", {
                get: function(){ return valueHelper; }
            });
            
            return Helper;
        };

        var ObjectDictionaryHelper = function(){};

        ObjectDictionaryHelper.prototype.read = function(os)
        {
            var sz = os.readSize();
            var v = new HashMap();

            var type = this.type;
            
            var readObjectForKey = function(key)
            {
                os.readObject(function(obj) { v.set(key, obj); }, type);
            };
            
            var keyHelper = this.keyHelper;
            for(var i = 0; i < sz; ++i)
            {
                readObjectForKey(keyHelper.read(os));
            }
            return v;
        };

        ObjectDictionaryHelper.prototype.write = function(os, v)
        {
            if(v === null || v.size === 0)
            {
                os.writeSize(0);
            }
            else
            {
                os.writeSize(v.size);
                var keyHelper = this.keyHelper;
                for(var e = v.entries; e !== null; e = e.next)
                {
                    keyHelper.write(os, e.key);
                    os.writeObject(e.value);
                }
            }
        };

        Object.defineProperty(ObjectDictionaryHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.generateObjectDictHelper = function(keyHelper, type)
        {
            var Helper = new ObjectDictionaryHelper();
            
            Object.defineProperty(Helper, "keyHelper", {
                get: function(){ return keyHelper; }
            });
            
            Object.defineProperty(Helper, "type", {
                get: function(){ return type; }
            });
            
            return Helper;
        };

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.StreamHelpers = StreamHelpers;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/StreamHelpers"));
