// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var HashMap = require("./HashMap");

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
    get: function(){ return 1; }
});

StreamHelpers.generateEnumHelper = function(Type)
{
    var EnumHelper = {};

    EnumHelper.write = function(os, v)
    {
        os.writeEnum(v, Type.MaxValue);
    };

    EnumHelper.read = function(os)
    {
        return os.readEnum(Type);
    };

    Object.defineProperty(EnumHelper, "minWireSize", {
        get: function(){ return 1; }
    });
    
    return EnumHelper;
};

StreamHelpers.generateStructHelper = function(Type)
{
    var StructHelper = {};

    StructHelper.write = function(os, v)
    {
        v.__write(os);
    };

    StructHelper.read = function(os)
    {
        var v = new Type();
        v.__read(os);
        return v;
    };

    Object.defineProperty(StructHelper, "minWireSize", {
        get: function(){ return 1; }
    });
    
    return StructHelper;
};

StreamHelpers.SequenceHelper = {};

StreamHelpers.SequenceHelper.read = function(os, helpers)
{
    var helper = helpers.shift();
    var sz = this.readAndCheckSeqSize(helper.minWireSize);
    var v = [];
    v.length = sz;
    for(var i = 0; i < sz; ++i)
    {
        v[i] = helper.read(os, helpers);
    }
    return v;
};

StreamHelpers.SequenceHelper.write = function(os, v, helpers)
{
    if(v === null || v.size === 0)
    {
        os.writeSize(0);
    }
    else
    {
        os.writeSize(v.length);
        var helper = helpers.shift();
        for(var i = 0; i < v.length; ++i)
        {
            helper.write(os, v[i], helpers);
        }
    }
};

Object.defineProperty(StreamHelpers.SequenceHelper, "minWireSize", {
    get: function(){ return 1; }
});

StreamHelpers.generateObjectSequenceHelper = function(Type)
{
    var ObjectSequenceHelper = {};

    ObjectSequenceHelper.read = function(os)
    {
        var sz = os.readSize();
        var v = [];
        v.length = sz;
        
        var readObjectAtIndex = function(idx)
        {
            os.readObject(function(obj) { v[idx] = obj; }, Type);
        };
        
        for(var i = 0; i < sz; ++i)
        {
            readObjectAtIndex(i);
        }
        return v;
    };
    
    ObjectSequenceHelper.write = function(os, v)
    {
        if(v === null || v.size === 0)
        {
            os.writeSize(0);
        }
        else
        {
            os.writeSize(v.length);
            for(var i = 0; i < v.length; ++i)
            {
                os.writeObject(v[i]);
            }
        }
    };
    
    Object.defineProperty(ObjectSequenceHelper, "minWireSize", {
        get: function(){ return 1; }
    });
    
    return ObjectSequenceHelper;
};

StreamHelpers.generateKeyValueHelper = function(key, value)
{
    var KeyValueTypeHelper = {};

    Object.defineProperty(KeyValueTypeHelper, "key", {
        get: function(){ return key; }
    });

    Object.defineProperty(KeyValueTypeHelper, "value", {
        get: function(){ return value; }
    });
    
    return KeyValueTypeHelper;
};

StreamHelpers.DictionaryHelper = {};

StreamHelpers.DictionaryHelper.read = function(os, helpers)
{
    var v = new HashMap();
    var sz = this.readSize(1);
    var helper = helpers.shift();
    for(var i = 0; i < sz; ++i)
    {
        v.set(helper.key.read(os), helper.value.read(os, helpers));
    }
    return v;
};

StreamHelpers.DictionaryHelper.write = function(os, v, helpers)
{
    if(v === null || v.size === 0)
    {
        os.writeSize();
    }
    else
    {
        var helper = helpers.shift();
        os.writeSize(v.size);
        for(var e = v.entries; e !== null; e = e.next)
        {
            helper.key.write(os, e.key);
            helper.value.write(os, e.value, helpers);
        }
    }
};

Object.defineProperty(StreamHelpers.DictionaryHelper, "minWireSize", {
    get: function(){ return 1; }
});
 

StreamHelpers.generateObjectDictionaryHelper = function(keyHelper, ObjectType)
{
    var ObjectDictionaryHelper = {};

    ObjectDictionaryHelper.read = function(os)
    {
        var sz = os.readSize();
        var v = new HashMap();
        v.length = sz;
        
        var readObjectForKey = function(key)
        {
            os.readObject(function(obj) { v.set(key, obj); }, ObjectType);
        };
        
        var key;
        for(var i = 0; i < sz; ++i)
        {
            key = keyHelper.read(os);
            readObjectForKey(key);
        }
        return v;
    };
    
    ObjectDictionaryHelper.write = function(os, v)
    {
        if(v === null || v.size === 0)
        {
            os.writeSize(0);
        }
        else
        {
            os.writeSize(v.size);
            for(var e = v.entries; e !== null; e = e.next)
            {
                keyHelper.write(os, e.key);
                os.writeObject(e.value);
            }
        }
    };
    
    Object.defineProperty(ObjectDictionaryHelper, "minWireSize", {
        get: function(){ return 1; }
    });
    
    return ObjectDictionaryHelper;
};

module.exports = StreamHelpers;
