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
    require("Ice/Class");
    require("Ice/Exception");
    require("Ice/ExUtil");
    require("Ice/FormatType");
    require("Ice/StreamHelpers");
    require("Ice/OptionalFormat");

    var Ice = global.Ice || {};
    var Slice = global.Slice || {};

    var ExUtil = Ice.ExUtil;
    
    var defineClass = Ice.__defineClass;
    
    var nextAddress = 0;
    
    var IceObject = defineClass({
        __init__: function()
        {
            // Fake Address used as the hashCode for this object instance.
            this.__address = nextAddress++;
        },
        hashCode: function()
        {
            return this.__address;
        },
        ice_isA: function(s, current)
        {
            return this.ice_ids().indexOf(s) >= 0;
        },
        ice_ping: function(current)
        {
        },
        ice_ids: function(current)
        {
            return this.__mostDerivedType().__ids;
        },
        ice_id: function(current)
        {
            return this.__mostDerivedType().__id;
        },
        toString: function()
        {
            return "[object " + this.ice_id() + "]";
        },
        ice_preMarshal: function()
        {
        },
        ice_postUnmarshal: function()
        {
        },
        __write: function(os)
        {
            os.startWriteObject(null);
            __writeImpl(this, os, this.__mostDerivedType());
            os.endWriteObject();
        },
        __read: function(is)
        {
            is.startReadObject();
            __readImpl(this, is, this.__mostDerivedType());
            is.endReadObject(false);
        },
        ice_instanceof: function(T)
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
        },
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
        __mostDerivedType: function()
        {
            return IceObject;
        }
    });
    
    IceObject.ice_staticId = function()
    {
        return IceObject.__id;
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
    
    IceObject.__ids = ["::Ice::Object"];
    IceObject.__id = IceObject.__ids[0];
    IceObject.__compactId = -1;
    IceObject.__preserved = false;

    var stringSeqHelper = Ice.StreamHelpers.generateSeqHelper(Ice.StreamHelpers.StringHelper, false);

    IceObject.__ops =
    {
        "ice_ping":
        {
        },
        "ice_isA":
        {
            u: function(__is, __r)
            {
                var id;
                id = __is.readString();
                __r.push(id);
            },
            m: function(__os, __ret)
            {
                __os.writeBool(__ret);
            }
        },
        "ice_id":
        {
            m: function(__os, __ret)
            {
                __os.writeString(__ret);
            }
        },
        "ice_ids":
        {
            m: function(__os, __ret)
            {
                stringSeqHelper.write(__os, __ret);
            }
        }
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

    var AMDCallback = defineClass({
        __init__: function(incomingAsync, exceptions, format, marshalFn)
        {
            this.incomingAsync = incomingAsync;
            this.exceptions = exceptions;
            this.format = format;
            this.marshalFn = marshalFn;
        },
        ice_response: function()
        {
            var args = arguments;

            try
            {
                if(this.marshalFn === undefined)
                {
                    if(args.length > 0)
                    {
                        //
                        // No results expected.
                        //
                        this.incomingAsync.__exception(
                            new Ice.UnknownException("ice_response called with invalid arguments"));
                        return;
                    }
                    else
                    {
                        this.incomingAsync.__writeEmptyParams();
                    }
                }
                else
                {
                    var __os = this.incomingAsync.__startWriteParams(this.format);
                    args.splice(0, 0, __os);
                    this.marshalFn.apply(this.marshalFn, args);
                    this.incomingAsync.__endWriteParams(true);
                }
            }
            catch(ex)
            {
                this.incomingAsync.__exception(ex);
            }
        },
        ice_exception: function(ex)
        {
            if(this.exceptions !== undefined)
            {
                //
                // Make sure the given exception is an instance of one of the declared user exceptions
                // for this operation.
                //
                for(var i = 0; i < this.exceptions.length; ++i)
                {
                    if(ex instanceof this.exceptions[i])
                    {
                        //
                        // User exception is valid, now marshal it.
                        //
                        this.incomingAsync.__exception(ex);
                        return;
                    }
                }
            }
            else if(ex instanceof Ice.UserException)
            {
                this.incomingAsync.__exception(new Ice.UnknownUserException(ExUtil.toString(ex)));
            }
            else if(ex instanceof Ice.LocalException)
            {
                this.incomingAsync.__exception(new Ice.UnknownLocalException(ExUtil.toString(ex)));
            }
            else
            {
                this.incomingAsync.__exception(new Ice.UnknownException(ExUtil.toString(ex)));
            }
        }
    });
    
    var __dispatchImpl = function(type, servant, incomingAsync, current)
    {
        var op;
        var o = type;
        while(o !== undefined && op === undefined)
        {
            op = o.__ops[current.operation];
            if(op === undefined)
            {
                o = o.__parent;
            }
        }

        if(op === undefined)
        {
            throw new Ice.OperationNotExistException(current.id, current.facet, current.operation);
        }

        //
        // The "n" property contains the native method name in case the operation's Slice name is a keyword.
        // Check to make sure the servant implements the operation.
        //
        var methodName = op.n !== undefined ? op.n : current.operation;
        if(servant[methodName] === undefined || typeof(servant[methodName]) !== "function")
        {
            var comm = current.adapter.getCommunicator();
            var msg = "servant for identity " + comm.identityToString(current.id) +
                " does not define operation `" + methodName + "'";
            console.log(msg);
            throw new Ice.UnknownException(msg);
        }

        //
        // The "m" property defines a function to marshal the results.
        //
        var marshalOutParamsFn = op.m;

        //
        // The "e" property defines the user exceptions the operation has declared (if any).
        //
        var userExceptions = op.e;

        //
        // The "f" property defines the operation format (if non-default).
        //
        var format = op.f === undefined ? Ice.FormatType.DefaultFormat : Ice.FormatType.valueOf(op.f);

        //
        // The "a" property indicates whether the operation uses AMD.
        //
        var amd = op.a;

        var cb = new AMDCallback(incomingAsync, userExceptions, format, marshalOutParamsFn);

        //
        // Unmarshal the in params (if any). The "u" property defines the unmarshaling function.
        //
        var inParams = amd ? [cb] : [];
        var unmarshalInParamsFn = op.u;
        if(unmarshalInParamsFn === undefined)
        {
            incomingAsync.readEmptyParams();
        }
        else
        {
            var __is = incomingAsync.startReadParams();
            unmarshalInParamsFn(__is, inParams);
            incomingAsync.endReadParams();
        }

        inParams.push(current);
        try
        {
            var results = servant[methodName].apply(servant, inParams);

            if(!amd)
            {
                cb.ice_response.apply(cb, results);
            }
        }
        catch(ex)
        {
            cb.ice_exception(ex);
        }

        //
        // Not necessary
        //
        //return DispatchStatus.DispatchAsync;
    };

    Slice.defineLocalObject = function(constructor, base)
    {
        var obj = constructor || function(){};

        if(base !== undefined)
        {
            obj.prototype = new base();
            obj.__parent = base;
            obj.prototype.constructor = constructor;
        }
        
        return obj;
    };
    
    Slice.defineObject = function(constructor, base, intfs, scope, ids, compactId, writeImpl, readImpl, preserved, ops)
    {
        var obj = constructor || function(){};

        obj.prototype = new base();
        obj.__parent = base;
        obj.__ids = ids;
        obj.__id = ids[scope];
        obj.__compactId = compactId;
        obj.__instanceof = IceObject.__instanceof;
        obj.__implements = intfs;
        obj.__ops = ops;
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

        obj.prototype.__dispatch = function(incomingAsync, current)
        {
            __dispatchImpl(obj, this, incomingAsync, current);
        };

        return obj;
    };
    
    Ice.Object = IceObject;
    global.Slice = Slice;
    global.Ice = Ice;
}());
