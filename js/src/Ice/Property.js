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
        var Property = function Property(pattern, deprecated, deprecatedBy)
        {
            this._pattern = pattern;
            this._deprecated = deprecated;
            this._deprecatedBy = deprecatedBy;
        };

        Object.defineProperty(Property.prototype, "pattern",{
            get: function() { return this._pattern; }
        });

        Object.defineProperty(Property.prototype, "deprecated",{
            get: function() { return this._deprecated; }
        });

        Object.defineProperty(Property.prototype, "deprecatedBy",{
            get: function() { return this._deprecatedBy; }
        });

        module.exports.Ice = module.exports.Ice || {};
        module.exports.Ice.Property = Property;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Property"));
