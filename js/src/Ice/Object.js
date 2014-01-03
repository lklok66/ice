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
        
        IceObject.prototype.ice_instanceof = function(T)
        {
            if(T)
            {
                if(this instanceof T)
                {
                    return true;
                }
                var p = Object.getPrototypeOf(this);
                if(p !== null && p !== undefined && p.__implements)
                {
                    return p.__implements.indexOf(T) !== -1;
                }
            }
            return false;
        };

        Ice.Object = IceObject;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Object"));
