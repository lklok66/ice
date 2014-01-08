// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

require ("Ice/Ice");

var id = new Ice.InitializationData();
id.properties = Ice.createProperties();
id.properties.setProperty("Ice.MessageSizeMax", "10");
require("./Client").test.Ice.exceptions.run({
    write: function(msg) { process.stdout.write(msg); },
    writeLine: function(msg) { this.write(msg + "\n"); }
}, id).exception(function(ex){
    if(ex.stack)
    {
        console.log(ex.stack);
    }
    else
    {
        console.log(ex);
    }
});
