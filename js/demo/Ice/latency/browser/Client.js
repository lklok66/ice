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
// Run the latency test.
//
function run()
{
    if(!isRunning())
    {
        var communicator;
        
        Promise.try(
            function()
            {
                setSate(State.Running);
                //
                // Initialize the communicator and create a proxy to the 
                // ping object.
                //
                communicator = Ice.initialize();
                var s = "ping:ws -h " + hostname() + " -p 10000:wss -h " + 
                        hostname() + " -p 10001";
                
                var proxy = communicator.stringToProxy(s)
                                        .ice_twoway()
                                        .ice_timeout(-1)
                                        .ice_secure(secure());
                
                var start, total;
                var repetitions = 10000;
                
                //
                // Down-cast the proxy to the Demo.Ping interface.
                //
                return Demo.PingPrx.checkedCast(proxy).then(
                    function(r, obj)
                    {
                        //
                        // Promise used to wait for the completion of
                        // the ping calls.
                        //
                        var p = new Ice.Promise();
                        
                        writeLine("pinging server " + repetitions + 
                                  " times (this may take a while)");
                        
                        var j = 0;
                        //
                        // The success callback waits until all calls
                        // completed and then call succeed on the 
                        // promise.
                        //
                        var succeedCB = function()
                        {
                            if(++j == repetitions)
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
                        setTimeout(
                            function()
                            {
                                start = new Date().getTime();
                                for(var i = 0; i < repetitions; ++i)
                                {
                                    obj.ice_ping().then(succeedCB)
                                                  .exception(exceptionCB);
                                }
                            });
                        return p;
                    }
                ).then(
                    function()
                    {
                        //
                        // Write the results.
                        //
                        total = new Date().getTime() - start;
                        writeLine("time for " + repetitions + " pings: " + 
                                  total + "ms");
                        writeLine("time per ping: " + (total / repetitions) + "ms");
                        setSate(State.Finish);
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
                    c = communicator;
                    communicator = null;
                    return c.destroy();
                }
            }
        ).exception(
            function(ex)
            {
                //
                // Handle any exceptions above.
                //
                setSate(State.Finish, ex);
            });
    }
}


//
// Handle the client state.
//
var State = {Finish:0, Running: 1};

var state = State.Finish;

function isRunning()
{
    return state == State.Running;
}

function setSate(s, ex)
{
    if(s != state)
    {
        switch(s)
        {
            case State.Running:
            {
                $("#console").val("");
                $("#run").addClass("disabled");
                break;
            }
            case State.Finish:
            {
                if(ex)
                {
                    $("#console").val(ex.toString());
                }
                $("#run").removeClass("disabled");
                break;
            }
        }
        state = s;
    }
}

//
// Run buttton envent handler.
//
$("#run").click(
    function(){
        run();
        return false;
    });

//
// Helper function to retrieve the hostname.
//
function hostname()
{
    return $("#hostname").val() || $("#hostname").attr("placeholder");
}

//
// Helper function to retrieve the "wss" checkbox that indicate if 
// we should use a secure endpoint.
//
function secure()
{
    return $("#wss").is(":checked");
}

//
// Helper function to write the output.
//
function writeLine(msg)
{
    $("#console").val($("#console").val() + msg + "\n");
    $("#console").scrollTop($("#console").get(0).scrollHeight);
}

}());
