// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    require("Ice/Ice");

    var write = function(msg)
    {
        process.stdout.write(msg); 
    };

    var writeLine = function(msg)
    {
        this.write(msg + "\n");
    };
    
    var run = function(module)
    {
        var id = new Ice.InitializationData();
        id.properties = Ice.createProperties(process.argv);
        
        module.require("./Client");
        var test = global.__test__;
        
        test({write: write, writeLine: writeLine}, id).exception(
            function(ex)
            {
                console.log(ex.toString());
                if(typeof ex.ice_getAsyncResult === "function")
                {
                    console.log("\nexception occurred in call to " + ex.ice_getAsyncResult().operation);
                }
                if(ex.stack)
                {
                    console.log(ex.stack);
                }
                process.exit(1);
            });
    };

    module.exports = run;
}(typeof (global) === "undefined" ? window : global));
