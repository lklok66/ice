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
        
        require("Ice/Debug");
        require("Ice/ExUtil");
        require("Ice/Network");
        require("Ice/SocketOperation");
        require("Ice/Connection");
        require("Ice/Exception");
        require("Ice/LocalException");
        require("IceWS/ConnectionInfo");
        
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "IceWS/Transceiver"));
