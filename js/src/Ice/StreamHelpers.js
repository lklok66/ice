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
        
        require("Ice/HashMap");
        require("Ice/OptionalFormat");
        
        var Ice = global.Ice || {};
        
        var HashMap = Ice.HashMap;
        var OptionalFormat = Ice.OptionalFormat;

        var StreamHelpers = {};

        StreamHelpers.BoolHelper = {};

        StreamHelpers.BoolHelper.write = function(os, v)
        {
            os.writeBool(v);
        };

        StreamHelpers.BoolHelper.read = function(is)
        {
            return is.readBool();
        };

        Object.defineProperty(StreamHelpers.BoolHelper, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.ByteHelper= {};

        StreamHelpers.ByteHelper.write = function(os, v)
        {
            os.writeByte(v);
        };

        StreamHelpers.ByteHelper.read = function(is)
        {
            return is.readByte();
        };

        Object.defineProperty(StreamHelpers.ByteHelper, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.ShortHelper = {};

        StreamHelpers.ShortHelper.write = function(os, v)
        {
            os.writeShort(v);
        };

        StreamHelpers.ShortHelper.read = function(is)
        {
            return is.readShort();
        };

        Object.defineProperty(StreamHelpers.ShortHelper, "minWireSize", {
            get: function(){ return 2; }
        });
        
        StreamHelpers.IntHelper = {};

        StreamHelpers.IntHelper.write = function(os, v)
        {
            os.writeInt(v);
        };

        StreamHelpers.IntHelper.read = function(is)
        {
            return is.readInt();
        };

        Object.defineProperty(StreamHelpers.IntHelper, "minWireSize", {
            get: function(){ return 4; }
        });

        StreamHelpers.LongHelper = {};

        StreamHelpers.LongHelper.write = function(os, v)
        {
            os.writeLong(v);
        };

        StreamHelpers.LongHelper.read = function(is)
        {
            return is.readLong();
        };

        Object.defineProperty(StreamHelpers.LongHelper, "minWireSize", {
            get: function(){ return 8; }
        });

        StreamHelpers.FloatHelper = {};

        StreamHelpers.FloatHelper.write = function(os, v)
        {
            os.writeFloat(v);
        };

        StreamHelpers.FloatHelper.read = function(is)
        {
            return is.readFloat();
        };

        Object.defineProperty(StreamHelpers.FloatHelper, "minWireSize", {
            get: function(){ return 4; }
        });

        StreamHelpers.DoubleHelper = {};

        StreamHelpers.DoubleHelper.write = function(os, v)
        {
            os.writeDouble(v);
        };

        StreamHelpers.DoubleHelper.read = function(is)
        {
            return is.readDouble();
        };

        Object.defineProperty(StreamHelpers.DoubleHelper, "minWireSize", {
            get: function(){ return 8; }
        });

        StreamHelpers.StringHelper = {};

        StreamHelpers.StringHelper.write = function(os, v)
        {
            os.writeString(v);
        };

        StreamHelpers.StringHelper.read = function(is)
        {
            return is.readString();
        };

        Object.defineProperty(StreamHelpers.StringHelper, "minWireSize", {
            get: function(){ return 1; }
        });
        
        var FSizeOptHelper = function()
        {
            this.writeOpt = function(os, tag, v)
            {
                if(v !== undefined && os.writeOpt(tag, OptionalFormat.FSize))
                {
                    var pos = os.startSize();
                    this.write(os, v);
                    os.endSize(pos);
                }
            };
            
            this.readOpt = function(is, tag)
            {
                var v;
                if(is.readOpt(tag, OptionalFormat.FSize))
                {
                    is.skip(4);
                    v = this.read(is);
                }
                return v;
            };
        };
        
        var VSizeOptHelper = function()
        {
            this.writeOpt = function(os, tag, v)
            {
                if(v !== undefined && os.writeOpt(tag, OptionalFormat.VSize))
                {
                    var sz = this.size(v);
                    os.writeSize(sz > 254 ? sz * this.minWireSize + 5 : sz * this.minWireSize + 1);
                    this.write(os, v);
                }
            };
            
            this.readOpt = function(is, tag)
            {
                var v;
                if(is.readOpt(tag, OptionalFormat.VSize))
                {
                    is.skipSize();
                    v = this.read(is);
                }
                return v;
            };
        };
        
        var VSize1OptHelper = function()
        {
            this.writeOpt = function(os, tag, v)
            {
                if(v !== undefined && os.writeOpt(tag, OptionalFormat.VSize))
                {
                    this.write(os, v);
                }
            };
            
            this.readOpt = function(is, tag)
            {
                var v;
                if(is.readOpt(tag, OptionalFormat.VSize))
                {
                    v = this.read(is);
                }
                return v;
            };
        };
        
        var VSizeHelper = function(minWireSize)
        {
            this.writeOpt = function(os, tag, v)
            {
                if(v !== undefined && os.writeOpt(tag, OptionalFormat.VSize))
                {
                    os.writeSize(minWireSize);
                    this.write(os, v);
                }
            };
            
            this.readOpt = function(is, tag)
            {
                var v;
                if(is.readOpt(tag, OptionalFormat.VSize))
                {
                    is.skipSize();
                    v = this.read(is);
                }
                return v;
            };
        };
        
        //
        // Functional mixin to add __readOpt/__writeOpt to structs.
        //
        StreamHelpers.StructHelper = function(T, minWireSize, optionalFormat)
        {
            T.write = function(os, v)
            {
                os.writeStruct(v);
            };
            
            T.read = function(is)
            {
                return is.readStruct(T);
            };
            
            Object.defineProperty(T, "minWireSize", {
                get: function(){ return minWireSize; }
            });
            
            if(optionalFormat === OptionalFormat.FSize)
            {
                T.__writeOpt = function(os, tag, v)
                {
                    if(v !== undefined && os.writeOpt(tag, OptionalFormat.FSize))
                    {
                        var pos = os.startSize();
                        os.writeStruct(v);
                        os.endSize(pos);
                    }
                };
                
                T.__readOpt = function(is, tag)
                {
                    var v;
                    if(is.readOpt(tag, OptionalFormat.FSize))
                    {
                        is.skip(4);
                        v = is.readStruct(T);
                    }
                    return v;
                }
            }
            else if(optionalFormat === OptionalFormat.VSize)
            {
                T.__writeOpt = function(os, tag, v)
                {
                    if(v !== undefined && os.writeOpt(tag, OptionalFormat.VSize))
                    {
                        os.writeSize(minWireSize);
                        os.writeStruct(v);
                    }
                };
                
                T.__readOpt = function(is, tag)
                {
                    var v;
                    if(is.readOpt(tag, OptionalFormat.VSize))
                    {
                        is.skipSize();
                        v = is.readStruct(T);
                    }
                    return v;
                };
            }
        };
        
        var SequenceHelper = function(){};

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
        
        SequenceHelper.prototype.read = function(is)
        {
            // Cache the element helper.
            var helper = this.elementHelper;
            var sz = is.readAndCheckSeqSize(helper.minWireSize);
            var v = [];
            v.length = sz;
            for(var i = 0; i < sz; ++i)
            {
                v[i] = helper.read(is);
            }
            return v;
        };
        
        SequenceHelper.prototype.size = function(v)
        {
            return (v === null || v === undefined) ? 0 : v.length; 
        };

        Object.defineProperty(SequenceHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.generateSeqHelper = function(elementHelper, optionalFormat)
        {
            var Helper = new SequenceHelper();
            
            if(optionalFormat == OptionalFormat.FSize)
            {
                FSizeOptHelper.call(Helper);
            }
            if(optionalFormat == OptionalFormat.VSize)
            {
                if(elementHelper === StreamHelpers.BoolHelper || elementHelper === StreamHelpers.ByteHelper)
                {
                    VSize1OptHelper.call(Helper);
                }
                else
                {
                    VSizeOptHelper.call(Helper);
                }
            }
            
            Object.defineProperty(Helper, "elementHelper", {
                get: function(){ return elementHelper; }
            });
            
            return Helper;
        };

        var ObjectSequenceHelper = function(){};
        
        ObjectSequenceHelper.prototype.write = function(os, v)
        {
            if(v === null || v.length === 0)
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
        
        ObjectSequenceHelper.prototype.read = function(is)
        {
            var sz = is.readAndCheckSeqSize(1);
            var v = [];
            v.length = sz;
            var T = this.T;
            var readObjectAtIndex = function(idx)
            {
                is.readObject(function(obj) { v[idx] = obj; }, T);
            };
            
            for(var i = 0; i < sz; ++i)
            {
                readObjectAtIndex(i);
            }
            return v;
        };

        Object.defineProperty(ObjectSequenceHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });
        
        FSizeOptHelper.call(ObjectSequenceHelper.prototype);

        StreamHelpers.generateObjectSeqHelper = function(T)
        {
            var Helper = new ObjectSequenceHelper();
            Object.defineProperty(Helper, "T", {
                get: function(){ return T; }
            });
            return Helper;
        };

        var DictionaryHelper = function(){};

        DictionaryHelper.prototype.write = function(os, v)
        {
            if(v === null || v.size === 0)
            {
                os.writeSize(0);
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
        
        DictionaryHelper.prototype.read = function(is)
        {
            var v = new HashMap();
            var sz = is.readSize();
            var keyHelper = this.keyHelper;
            var valueHelper = this.valueHelper;
            for(var i = 0; i < sz; ++i)
            {
                v.set(keyHelper.read(is), valueHelper.read(is));
            }
            return v;
        };
        
        DictionaryHelper.prototype.size = function(v)
        {
            return (v === null || v === undefined) ? 0 : v.size; 
        };

        Object.defineProperty(DictionaryHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });

        StreamHelpers.generateDictHelper = function(keyHelper, valueHelper, optionalFormat)
        {
            var Helper = new DictionaryHelper();
            
            if(optionalFormat == OptionalFormat.FSize)
            {
                FSizeOptHelper.call(Helper);
            }
            if(optionalFormat == OptionalFormat.VSize)
            {
                VSizeOptHelper.call(Helper);
            }
            
            Object.defineProperty(Helper, "keyHelper", {
                get: function(){ return keyHelper; }
            });
            
            Object.defineProperty(Helper, "valueHelper", {
                get: function(){ return valueHelper; }
            });
            
            return Helper;
        };

        var ObjectDictionaryHelper = function(){};

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
        
        ObjectDictionaryHelper.prototype.read = function(is)
        {
            var sz = is.readSize();
            var v = new HashMap();

            var T = this.T;
            
            var readObjectForKey = function(key)
            {
                is.readObject(function(obj) { v.set(key, obj); }, T);
            };
            
            var keyHelper = this.keyHelper;
            for(var i = 0; i < sz; ++i)
            {
                readObjectForKey(keyHelper.read(is));
            }
            return v;
        };

        Object.defineProperty(ObjectDictionaryHelper.prototype, "minWireSize", {
            get: function(){ return 1; }
        });
        
        FSizeOptHelper.call(ObjectDictionaryHelper.prototype);

        StreamHelpers.generateObjectDictHelper = function(keyHelper, T)
        {
            var Helper = new ObjectDictionaryHelper();
            
            Object.defineProperty(Helper, "keyHelper", {
                get: function(){ return keyHelper; }
            });
            
            Object.defineProperty(Helper, "T", {
                get: function(){ return T; }
            });
            
            return Helper;
        };

        Ice.StreamHelpers = StreamHelpers;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/StreamHelpers"));
