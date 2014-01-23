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

        var Slice = global.Slice || {};
        
        Slice.defineStruct = function(constructor, clone, equals, hashCode, write, read)
        {
            var obj = constructor;

            obj.prototype.clone = clone;
            
            obj.prototype.equals = equals;

            obj.prototype.hashCode = hashCode;
            
            if(write !== undefined)
            {
                obj.prototype.__write = write;
            }
            
            if(read !== undefined)
            {
                obj.prototype.__read = read;
            }
            
            return obj;
        };
        
        global.Slice = Slice;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Struct"));
