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
        var ObjectFactory = function()
        {
        };

        ObjectFactory.prototype.create = function(type)
        {
            throw new Error("not implemented");
        };

        ObjectFactory.prototype.destroy = function()
        {
            throw new Error("not implemented");
        };

        global.Ice = global.Ice || {};
        global.Ice.ObjectFactory = ObjectFactory;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ObjectFactory"));
