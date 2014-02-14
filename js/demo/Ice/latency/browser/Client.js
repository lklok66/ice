// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){

var Promise = Ice.Promise;

//
// Initialize the communicator
//
var communicator = Ice.initialize();

//
// Run the latency test.
//
function run()
{
    //
    // Create a proxy to the  ping object.
    //
    var hostname = $("#hostname").val() || $("#hostname").attr("placeholder");
    var proxy = communicator.stringToProxy("ping:ws -h " + hostname + " -p 10000:wss -h " + hostname + " -p 10001")
    if($("#wss").is(":checked"))
    {
        proxy = proxy.ice_secure(true);
    }

    var repetitions = 1000;
    
    //
    // Down-cast the proxy to the Demo.Ping interface.
    //
    return Demo.PingPrx.checkedCast(proxy).then(
        function(obj)
        {
            writeLine("pinging server " + repetitions + " times (this may take a while)");
            start = new Date().getTime();
            return loop(
                function() 
                {
                    return obj.ice_ping();
                },
                repetitions
            ).then(
                function()
                {
                    //
                    // Write the results.
                    //
                    total = new Date().getTime() - start;
                    writeLine("time for " + repetitions + " pings: " + total + "ms");
                    writeLine("time per ping: " + (total / repetitions) + "ms");
                    setState(State.Idle);
                });
        });
}

//
// Run buttton event handler.
//
$("#run").click(
    function()
    {
        //
        // Run the latency loop if not already running.
        //
        if(state !== State.Running)
        {
            setState(State.Running);

            Ice.Promise.try(
                function()
                {
                    return run();
                }
            ).exception(
                function(ex)
                {
                    console.log(ex.stack);
                    $("#console").val(ex.toString());
                }
            ).finally(
                function()
                {
                    setState(State.Idle);
                }
            );
        }
        return false;
    });

//
// Asynchronous loop, each call to the given function returns a
// promise that when fulfilled runs the next iteration.
//    
function loop(fn, repetitions)
{
    var i = 0;
    var next = function() 
    {
        if(i++ < repetitions)
        {
            return fn.call().then(next);
        }
    };
    return next();
}

//
// Helper function to write the output.
//
function writeLine(msg)
{
    $("#console").val($("#console").val() + msg + "\n");
    $("#console").scrollTop($("#console").get(0).scrollHeight);
}

//
// Handle the client state.
//
var State = { 
    Idle:0, 
    Running: 1 
};

var state;

function setState(s, ex)
{
    if(s != state)
    {
        switch(s)
        {
            case State.Running:
            {
                $("#console").val("");
                $("#run").addClass("disabled");
                $("#progress").show();
                $("body").addClass("waiting");
                break;
            }
            case State.Idle:
            {
                $("#run").removeClass("disabled");
                $("#progress").hide();
                $("body").removeClass("waiting");
                break;
            }
        }
        state = s;
    }
}

setState(State.Idle);

}());
