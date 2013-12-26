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
        //
        // Using a separate module for these constants so that ObjectPrx does
        // not need to include Reference.
        //
        var ReferenceMode = {};

        ReferenceMode.ModeTwoway = 0;
        ReferenceMode.ModeOneway = 1;
        ReferenceMode.ModeBatchOneway = 2;
        ReferenceMode.ModeDatagram = 3;
        ReferenceMode.ModeBatchDatagram = 4;
        ReferenceMode.ModeLast = ReferenceMode.ModeBatchDatagram;

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.ReferenceMode = ReferenceMode;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ReferenceMode"));
