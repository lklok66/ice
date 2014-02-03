// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var global = this;
    require("Ice/HashMap");
    require("Ice/OptionalFormat");
    
    var Ice = global.Ice || {};
    
    var HashMap = Ice.HashMap;
    var OptionalFormat = Ice.OptionalFormat;

    var StreamHelpers = {};

    StreamHelpers.FSizeOptHelper = function()
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
    
    StreamHelpers.VSizeOptHelper = function()
    {
        this.writeOpt = function(os, tag, v)
        {
            if(v !== undefined && os.writeOpt(tag, OptionalFormat.VSize))
            {
                os.writeSize(this.minWireSize);
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

    StreamHelpers.VSizeContainerOptHelper = function(elementSize)
    {
        this.writeOpt = function(os, tag, v)
        {
            if(v !== undefined && os.writeOpt(tag, OptionalFormat.VSize))
            {
                var sz = this.size(v);
                os.writeSize(sz > 254 ? sz * elementSize + 5 : sz * elementSize + 1);
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
    
    StreamHelpers.VSizeContainer1OptHelper = function()
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

    //
    // Sequence helper to write sequences
    //
    var SequenceHelper = function() {};
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
        var helper = this.elementHelper; // Cache the element helper.
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
    
    // Speacialization optimized for ByteSeq
    var byteSeqHelper = new SequenceHelper();
    byteSeqHelper.write = function(os, v) { return os.writeByteSeq(v); }
    byteSeqHelper.read = function(is) { return is.readByteSeq(); }
    Object.defineProperty(byteSeqHelper, "elementHelper", {
        get: function(){ return Ice.ByteHelper; }
    });
    StreamHelpers.VSizeContainer1OptHelper.call(byteSeqHelper);

    // Read method for object sequences
    var objectSequenceHelperRead = function(is)
    {
        var sz = is.readAndCheckSeqSize(1);
        var v = [];
        v.length = sz;
        var elementType = this.elementType;
        var readObjectAtIndex = function(idx)
        {
            is.readObject(function(obj) { v[idx] = obj; }, elementType);
        };
        
        for(var i = 0; i < sz; ++i)
        {
            readObjectAtIndex(i);
        }
        return v;
    };

    StreamHelpers.generateSeqHelper = function(elementHelper, fixed, elementType)
    {
        if(elementHelper === Ice.ByteHelper)
        {
            return byteSeqHelper;
        }

        var helper = new SequenceHelper();
        if(fixed)
        {
            if(elementHelper.minWireSize === 1)
            {
                StreamHelpers.VSizeContainer1OptHelper.call(helper);
            }
            else
            {
                StreamHelpers.VSizeContainerOptHelper.call(helper, elementHelper.minWireSize);
            }
        }
        else
        {
            StreamHelpers.FSizeOptHelper.call(helper);
        }
        
        Object.defineProperty(helper, "elementHelper", {
            get: function(){ return elementHelper; }
        });

        if(elementHelper == Ice.ObjectHelper)
        {
            Object.defineProperty(helper, "elementType", {
                get: function(){ return elementType; }
            });
            helper.read = objectSequenceHelperRead;
        }
        
        return helper;
    };

    //
    // Dictionary helper to write dictionaries
    //
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

    // Read method for dictionaries of objects
    var objectDictionaryHelperRead = function(is)
    {
        var sz = is.readSize();
        var v = new HashMap();
        var valueType = this.valueType;
        
        var readObjectForKey = function(key)
        {
            is.readObject(function(obj) { v.set(key, obj); }, valueType);
        };
        
        var keyHelper = this.keyHelper;
        for(var i = 0; i < sz; ++i)
        {
            readObjectForKey(keyHelper.read(is));
        }
        return v;
    };

    StreamHelpers.generateDictHelper = function(keyHelper, valueHelper, fixed, valueType)
    {
        var helper = new DictionaryHelper();
        if(fixed)
        {
            StreamHelpers.VSizeContainerOptHelper.call(helper, keyHelper.minWireSize + valueHelper.minWireSize);
        }
        else
        {
            StreamHelpers.FSizeOptHelper.call(helper);
        }
        Object.defineProperty(helper, "keyHelper", {
            get: function(){ return keyHelper; }
        });        
        Object.defineProperty(helper, "valueHelper", {
            get: function(){ return valueHelper; }
        });

        if(valueHelper == Ice.ObjectHelper)
        {
            Object.defineProperty(helper, "valueType", {
                get: function(){ return valueType; }
            });
            helper.read = objectDictionaryHelperRead;
        }

        return helper;
    };

    Ice.StreamHelpers = StreamHelpers;

    global.Ice = Ice;
}());
