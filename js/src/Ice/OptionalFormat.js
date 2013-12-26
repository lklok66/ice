// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(module, exports, require){
        var EnumBase = require("Ice/EnumBase").Ice.EnumBase;

        var OptionalFormat = function(_n, _v)
        {
            EnumBase.call(this, _n, _v);
        };

        OptionalFormat.prototype = new EnumBase();
        OptionalFormat.prototype.constructor = OptionalFormat;

        EnumBase.defineEnum(OptionalFormat, {'F1':0, 'F2':1, 'F4':2, 'F8':3, 'Size':4, 'VSize':5, 'FSize':6, 'Class':7});

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.OptionalFormat = OptionalFormat;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/OptionalFormat"));
