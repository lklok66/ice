// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

require("./Client").test.Ice.proxy.run({
    write: function(msg) { process.stdout.write(msg); },
    writeLine: function(msg) { this.write(msg + "\n"); }
}).exception(
    function(ex)
    {
        if(ex.stack)
        {
            console.log(ex.stack);
        }
        else
        {
            console.log(ex);
        }
    });
