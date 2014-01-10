// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

require("Ice/Ice");

var write = function(msg)
{
    process.stdout.write(msg); 
};

var writeLine = function(msg)
{
    this.write(msg + "\n");
};


var run = function(test)
{
    var id = new Ice.InitializationData();
    id.properties = Ice.createProperties(process.argv);
    
    test({write: write, writeLine: writeLine}, id).exception(
        function(ex)
        {
            console.log(ex.stack);
            process.exit(1);
        }
    );
};

module.exports = run;