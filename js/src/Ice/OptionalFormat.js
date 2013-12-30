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
        
        require("Ice/EnumBase");
        var EnumBase = Ice.EnumBase;

        var OptionalFormat = function(_n, _v)
        {
            EnumBase.call(this, _n, _v);
        };

        OptionalFormat.prototype = new EnumBase();
        OptionalFormat.prototype.constructor = OptionalFormat;

        EnumBase.defineEnum(OptionalFormat, {'F1':0, 'F2':1, 'F4':2, 'F8':3, 'Size':4, 'VSize':5, 'FSize':6, 'Class':7});

        global.Ice = global.Ice || {};
        global.Ice.OptionalFormat = OptionalFormat;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/OptionalFormat"));
