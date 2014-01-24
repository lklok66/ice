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
        
        var Ice = global.Ice || {};
        
        Ice.OptionalFormat = Slice.defineEnum({'F1':0, 'F2':1, 'F4':2, 'F8':3, 'Size':4, 'VSize':5, 'FSize':6, 'Class':7});
        
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/OptionalFormat"));
