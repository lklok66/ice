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
        //
        // Ice.Object
        //
        // Using IceObject in this file to avoid collisions with the native Object.
        //

        var Ice = global.Ice || {};
        var Slice = global.Slice || {};
        
        var nextAddress = 0;

        var IceObject = function()
        {
            // Fake Address used as the hashCode for this object instance.
            this.__address = nextAddress++;
        };

        IceObject.prototype.hashCode = function()
        {
            return this.__address;
        };

        IceObject.prototype.__notImplemented = function()
        {
            throw new Error("not implemented");
        };

        IceObject.prototype.ice_isA = function(s, current)
        {
            return this.ice_ids().indexOf(s) >= 0;
        };

        IceObject.prototype.ice_ping = function(current)
        {
        };

        IceObject.__ids = ["::Ice::Object"];

        IceObject.prototype.ice_ids = function(current)
        {
            return IceObject.__ids;
        };

        IceObject.ice_staticId = function()
        {
            return IceObject.__ids[0];
        };

        IceObject.prototype.ice_id = function(current)
        {
            return IceObject.__ids[0];
        };
        
        IceObject.prototype.toString = function()
        {
            return "[object " + this.ice_id() + "]";
        };

        IceObject.prototype.ice_preMarshal = function()
        {
        };

        IceObject.prototype.ice_postUnmarshal = function()
        {
        };

        IceObject.prototype.__write = function(os)
        {
            os.startWriteObject(null);
            this.__writeImpl(os);
            os.endWriteObject();
        };

        IceObject.prototype.__read = function(is)
        {
            is.startReadObject();
            this.__readImpl(is);
            is.endReadObject(false);
        };
        
        Slice.defineLocalObject = function(constructor, base)
        {
            var obj = constructor;

            if(base !== undefined)
            {
                obj.prototype = new base;
                obj.__parent = base;
            }
            obj.prototype.constructor = constructor;
            
            return obj;
        };
        
        IceObject.__instanceof = function(T)
        {
            if(T === this)
            {
                return true;
            }

            for(var i in this.__implements)
            {
                if(this.__implements[i].__instanceof(T))
                {
                    return true;
                }
            }

            if(this.__parent)
            {
                return this.__parent.__instanceof(T);
            }
            return false;
        };

        IceObject.prototype.ice_instanceof = function(T)
        {
            if(T)
            {
                if(this instanceof T)
                {
                    return true;
                }
                return this.constructor.__instanceof(T);
            }
            return false;
        };
        
        Slice.defineObject = function(constructor, base, scope, ids, writeImpl, readImpl, write, read, interfaces)
        {
            var obj = constructor;

            obj.prototype = new base;
            obj.__parent = base;
            
            obj.prototype.constructor = obj;
            
            obj.prototype.ice_ids = function(current)
            {
                return ids;
            };
            
            obj.prototype.ice_id = function(current)
            {
                return ids[scope];
            };

            obj.ice_staticId = function()
            {
                return ids[scope];
            };
            
            obj.prototype.__writeImpl = writeImpl;
            obj.prototype.__readImpl = readImpl;
            
            if(write !== undefined)
            {
                obj.prototype.__write = write;
            }
            
            if(read !== undefined)
            {
                obj.prototype.__read = read;
            }
            
            obj.__instanceof = IceObject.__instanceof;
            
            obj.__implements = interfaces;
            
            //
            // Override ice_instanceof so it calls the correct __instanceof,
            // we cannot use this.constructor.__instanceof as the class
            // might be subclass and instanciate by a factory in which case
            // constructor will lack __instanceof method.
            //
            obj.prototype.ice_instanceof = function(T)
            {
                if(T)
                {
                    if(this instanceof T)
                    {
                        return true;
                    }
                    return obj.__instanceof(T);
                }
                return false;
            };
            
            return obj;
        };

        Ice.Object = IceObject;
        global.Slice = Slice;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Object"));
