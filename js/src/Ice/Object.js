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

    IceObject.prototype.ice_isA = function(s, current)
    {
        return this.ice_ids().indexOf(s) >= 0;
    };

    IceObject.prototype.ice_ping = function(current)
    {
    };

    IceObject.ice_staticId = function()
    {
        return IceObject.__id;
    };

    IceObject.prototype.ice_ids = function(current)
    {
        return this.__mostDerivedType().__ids;
    };

    IceObject.prototype.ice_id = function(current)
    {
        return this.__mostDerivedType().__id;
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
        __writeImpl(this, os, this.__mostDerivedType());
        os.endWriteObject();
    };

    IceObject.prototype.__read = function(is)
    {
        is.startReadObject();
        __readImpl(this, is, this.__mostDerivedType());
        is.endReadObject(false);
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
            return this.__mostDerivedType().__instanceof(T);
        }
        return false;
    };

    IceObject.__ids = ["::Ice::Object"];
    IceObject.__id = IceObject.__ids[0];
    IceObject.__compactId = -1;
    IceObject.__preserved = false;

    //
    // __mostDerivedType returns the the most derived Ice generated class. This is
    // necessary because the user might extend Slice generated classes. The user 
    // class extensions don't have __id, __ids, __instanceof etc static members so
    // the implementation of ice_id, ice_ids and ice_instanceof would fail trying 
    // to access those members of the user defined class. Instead, ice_id, ice_ids
    // and ice_instanceof call __mostDerivedType to get the most derived Ice class.
    //
    // The __mostDerivedType is overriden by each Slice generated class, see the 
    // Slice.defineObject method implementation for details.
    //
    IceObject.prototype.__mostDerivedType = function()
    {
        return IceObject;
    };

    //
    // Private methods
    //

    var __writeImpl = function(obj, os, type)
    {
        //
        // The __writeImpl method is a recursive method that goes down the
        // class hierarchy to marshal each slice of the class using the 
        // generated __writeMemberImpl method.
        //

        if(type === undefined || type === IceObject)
        {
            return; // Don't marshal anything for IceObject
        }

        os.startWriteSlice(type.__id, type.__compactId, type.__parent === IceObject);
        if(type.prototype.__writeMemberImpl)
        {
            type.prototype.__writeMemberImpl.call(obj, os);
        }
        os.endWriteSlice();
        __writeImpl(obj, os, type.__parent);
    };

    var __readImpl = function(obj, is, type)
    {
        //
        // The __readImpl method is a recursive method that goes down the
        // class hierarchy to marshal each slice of the class using the 
        // generated __readMemberImpl method.
        //

        if(type === undefined || type === IceObject)
        {
            return; // Don't marshal anything for IceObject
        }

        is.startReadSlice();
        if(type.prototype.__readMemberImpl)
        {
            type.prototype.__readMemberImpl.call(obj, is);
        }
        is.endReadSlice();
        __readImpl(obj, is, type.__parent);
    };

    var __writePreserved = function(os)
    {
        //
        // For Slice classes which are marked "preserved", the implementation of this method
        // replaces the Ice.Object.prototype.__write method.
        //
        os.startWriteObject(this.__slicedData);
        __writeImpl(this, os, this.__mostDerivedType());
        os.endWriteObject();
    };

    var __readPreserved = function(is)
    {
        //
        // For Slice classes which are marked "preserved", the implementation of this method
        // replaces the Ice.Object.prototype.__read method.
        //
        is.startReadObject();
        __readImpl(this, is, this.__mostDerivedType());
        this.__slicedData = is.endReadObject(true);
    };

    Slice.defineLocalObject = function(constructor, base)
    {
        var obj = constructor;

        if(base !== undefined)
        {
            obj.prototype = new base();
            obj.__parent = base;
        }
        obj.prototype.constructor = constructor;
        
        return obj;
    };
    
    Slice.defineObject = function(constructor, base, intfs, scope, ids, compactId, writeImpl, readImpl, preserved)
    {
        var obj = constructor;

        obj.prototype = new base();
        obj.__parent = base;
        obj.__ids = ids;
        obj.__id = ids[scope];
        obj.__compactId = compactId;
        obj.__instanceof = IceObject.__instanceof;
        obj.__implements = intfs;
        obj.ice_staticId = function()
        {
            return ids[scope];
        };

        obj.prototype.constructor = obj;
        obj.prototype.__mostDerivedType = function() { return obj; };
        if(preserved)
        {
            obj.prototype.__write = __writePreserved;
            obj.prototype.__read = __readPreserved;
        }
        obj.prototype.__writeMemberImpl = writeImpl;
        obj.prototype.__readMemberImpl = readImpl;
        return obj;
    };
    
    Slice.defineObjectSequence = function(module, name, valueType, optionalFormat)
    {
        var helper = null;
        Object.defineProperty(module, name, 
        {
            get: function()
                {
                    if(helper === null)
                    {
                        /*jshint -W061 */
                        helper = Ice.StreamHelpers.generateObjectSeqHelper(eval(valueType), optionalFormat);
                        /*jshint +W061 */
                    }
                    return helper;
                }
        });
    };
    
    Slice.defineObjectDictionary = function(module, name, keyHelper, valueType, optionalFormat)
    {
        var helper = null;
        Object.defineProperty(module, name, 
        {
            get: function()
                {
                    if(helper === null)
                    {
                        /*jshint -W061 */
                        helper = Ice.StreamHelpers.generateObjectDictHelper(eval(keyHelper), eval(valueType), 
                                                                            optionalFormat);
                        /*jshint +W061 */
                    }
                    return helper;
                }
        });
    };

    Ice.Object = IceObject;
    global.Slice = Slice;
    global.Ice = Ice;
}());
