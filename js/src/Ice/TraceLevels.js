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
        
        var Ice = global.Ice || {};
        
        var TraceLevels = function(properties)
        {
            var networkCat = "Network";
            var protocolCat = "Protocol";
            var retryCat = "Retry";
            var locationCat = "Locator";
            var slicingCat = "Slicing";

            var keyBase = "Ice.Trace.";

            var network = properties.getPropertyAsInt(keyBase + networkCat);
            var protocol = properties.getPropertyAsInt(keyBase + protocolCat);
            var retry = properties.getPropertyAsInt(keyBase + retryCat);
            var location = properties.getPropertyAsInt(keyBase + locationCat);
            var slicing = properties.getPropertyAsInt(keyBase + slicingCat);
            properties.getPropertyAsInt(keyBase + "ThreadPool"); // Avoid an "unused property" warning.

            return {
                'network': {
                    get: function() { return network; }
                },
                'networkCat': {
                    get: function() { return networkCat; }
                },
                'protocol': {
                    get: function() { return protocol; }
                },
                'protocolCat': {
                    get: function() { return protocolCat; }
                },
                'retry': {
                    get: function() { return retry; }
                },
                'retryCat': {
                    get: function() { return retryCat; }
                },
                'location': {
                    get: function() { return location; }
                },
                'locationCat': {
                    get: function() { return locationCat; }
                },
                'slicing': {
                    get: function() { return slicing; }
                },
                'slicingCat': {
                    get: function() { return slicingCat; }
                }
            };
        };

        Ice.TraceLevels = TraceLevels;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/TraceLevels"));