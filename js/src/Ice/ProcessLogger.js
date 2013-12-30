
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
        
        require("Ice/Logger");
        var Logger = Ice.Logger;
        
        var processLogger = null;
        
        var getProcessLogger = function()
        {
            if(processLogger === null)
            {
                //
                // TODO: Would be nice to be able to use process name as prefix by default.
                //
                processLogger = new Logger("", "");
            }

            return processLogger;
        };

        var setProcessLogger = function(logger)
        {
            processLogger = logger;
        };

        global.Ice = global.Ice || {};
        global.Ice.getProcessLogger = getProcessLogger;
        global.Ice.setProcessLogger = setProcessLogger;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ProcessLogger"));
