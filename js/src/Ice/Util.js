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
        // Helper function to merge definitions
        //
        var merge = function(target, source)
        {
            if(target == source)
            {
                return;
            }
            for(var s in source)
            {
                if(target[s] === undefined)
                {
                    target[s] = source[s];
                }
                else
                {
                    merge(target[s], source[s]);
                }
            }
        };

        module.exports.merge = merge;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Util"));
