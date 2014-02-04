// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    require("Ice/Class");
    
    var Slice = global.Slice || {};
    var Ice = global.Ice || {};
    //
    // Ice.EnumBase
    //
    var EnumBase = Ice.__defineClass({
        __init__: function(name, value)
        {
            this._name = name;
            this._value = value;
        },
        equals: function(rhs)
        {
            if(this === rhs)
            {
                return true;
            }

            var proto = Object.getPrototypeOf(this);
            if(!(rhs instanceof proto.constructor))
            {
                return false;
            }

            return this._value == rhs._value;
        },
        hashCode: function()
        {
            return this._value;
        },
        toString: function()
        {
            return this._name;
        }
    });
    
    var prototype = EnumBase.prototype;
    
    Object.defineProperty(prototype, 'name', {
        enumerable: true,
        get: function() { return this._name; }
    });

    Object.defineProperty(prototype, 'value', {
        enumerable: true,
        get: function() { return this._value; }
    });
    
    var write = function(os, v)
    {
        os.writeEnum(v);
    };
    var writeOpt = function(os, tag, v)
    {
        os.writeOptValue(tag, Ice.OptionalFormat.Size, Ice.BasicStream.prototype.writeEnum, v);
    };

    Slice.defineEnum = function(enumerators)
    {
        var type = function(n, v)
        {
            EnumBase.call(this, n, v);
        };
        
        type.prototype = new EnumBase();
        type.prototype.constructor = type;
    
        var enums = [];
        var maxValue = 0;
        for(var e in enumerators)
        {
            var value = enumerators[e];
            var enumerator = new type(e, value);
            enums[value] = enumerator;
            Object.defineProperty(type, e, {
                enumerable: true,
                value: enumerator
            });
            if(value > maxValue)
            {
                maxValue = value;
            }
        }
        
        Object.defineProperty(type, "minWireSize", {
            get: function(){ return 1; }
        });
        
        type.write = write;
        type.read = function(is)
        {
            return is.readEnum(type);
        };
        type.writeOpt = writeOpt;
        type.readOpt = function(is, tag)
        {
            return is.readOptEnum(tag, type);
        };

        Object.defineProperty(type, 'valueOf', {
            value: function(v) {
                if(v === undefined)
                {
                    return type;
                }
                return enums[v]; }
        });
        
        Object.defineProperty(type, 'maxValue', {
            value: maxValue
        });
        
        Object.defineProperty(type.prototype, 'maxValue', {
            value: maxValue
        });
        
        return type;
    };
    
    Ice.EnumBase = EnumBase;
    
    global.Slice = Slice;
    global.Ice = Ice;
}(typeof (global) === "undefined" ? window : global));
