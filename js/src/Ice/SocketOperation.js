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
        var SocketOperation = {};

        SocketOperation.None = 0;
        SocketOperation.Read = 1;
        SocketOperation.Write = 2;
        SocketOperation.Connect = 2; // Same as Write

        global.Ice = global.Ice || {};
        global.Ice.SocketOperation = SocketOperation;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/SocketOperation"));
