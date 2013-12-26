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
        module.exports.test = module.exports.test || {};
        
        var tests = {};
        
        var TestSuite = {};
        
        TestSuite.add = function(name, cb)
        {
            tests[name] = cb;
        };
        
        TestSuite.get = function(name)
        {
            return tests[name];
        };
        
        TestSuite.names = function()
        {
            return Object.keys(tests);
        };
        
        module.exports.test.Common = module.exports.test.Common || {};
        module.exports.test.Common.TestSuite = TestSuite;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : __m(module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Common/TestSuite"));