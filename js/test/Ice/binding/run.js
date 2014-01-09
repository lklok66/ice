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
id.properties.setProperty("Ice.Trace.Protocol", "1");
id.properties.setProperty("Ice.Trace.Network", "3");
require("./Client").test.Ice.binding.run({
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
