// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){

require("Ice/Ice");
require("Latency");

var communicator;

Promise.try(
    function()
    {
        //
        // Initialize the communicator and create a proxy to the 
        // ping object.
        //
        communicator = Ice.initialize();
        var start, total, repetitions = 100000;
        var proxy = communicator.stringToProxy("ping:default -p 10000");
        
        //
        // Down-cast the proxy to the Demo.Ping interface.
        //
        Demo.PingPrx.checkedCast(proxy).then(
            function(r, obj)
            {
                //
                // Promise used to wait for the completion of
                // the ping calls.
                //
                var p = new Ice.Promise();
                console.log("pinging server " + repetitions + " times (this may take a while)");
                var j = 0;
                //
                // The success callback waits until all calls
                // completed and then call succeed on the 
                // promise.
                //
                var succeedCB = function()
                {
                    j++;
                    if(j == repetitions)
                    {
                        p.succeed();
                    }
                };
                
                //
                // If an invocation fails, report that error 
                // and we are done.
                //
                var exceptionCB = function(ex)
                {
                    p.fail(ex);
                };
                
                //
                // Invoke ice_ping repetitions times.
                //
                start = new Date().getTime();
                for(var i = 0; i < repetitions; ++i)
                {
                    obj.ice_ping().then(succeedCB).exception(exceptionCB);
                }
                return p;
            }
        ).then(
            function()
            {
                //
                // Write the results.
                //
                total = new Date().getTime() - start;
                console.log("time for " + repetitions + " pings: " + total + "ms");
                console.log("time per ping: " + (total / repetitions) + "ms");
            });
    }
).finally(
    function()
    {
        //
        // Destroy the communicator if required.
        //
        if(communicator)
        {
            return communicator.destroy();
        }
    }
).exception(
    function(ex)
    {
        //
        // Handle any exceptions above.
        //
        console.log(ex.toString());
        process.exit(1);
    });

}());
