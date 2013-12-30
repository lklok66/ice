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
        var Ice = global.Ice || {};
        var HashMap = Ice.HashMap;

        function TypeRegistry()
        {
            var __TypeRegistry__ = new HashMap();
            
            var register = function(typeId, type)
            {
                __TypeRegistry__.set(typeId, type);
            };
            
            var find = function(typeId)
            {
                return __TypeRegistry__.get(typeId);
            };
            
            return {register: register, find: find};
        }
        
        Ice.CompactIdRegistry = TypeRegistry();
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/TypeRegistry"));
