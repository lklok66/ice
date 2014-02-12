// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){

var communicator;
var flushEnabled = false;
var State = {Idle:0, SendRequest:1, FlushBatchRequests:2};
var state;
var batch = 0;

function sayHello()
{
    Ice.Promise.try(
        function()
        {
            setState(State.SendRequest);
            //
            // Initialize the communicator if it has not yet been initialized.
            //
            if(!communicator)
            {
                communicator = Ice.initialize();
            }
            
            //
            // Create a proxy to the hello object.
            //
            var s = "hello:ws -h " + hostname() + " -p 10000:" +
                    "wss -h " + hostname() + " -p 10001"
            
            var proxy = communicator.stringToProxy(s)
                                    .ice_twoway()
                                    .ice_timeout(-1)
                                    .ice_secure(false);
            
            //
            // Down-cast the proxy the Demo.Hello interface.
            //
            return Demo.HelloPrx.checkedCast(proxy).then(
                function(asyncResult, twoway)
                {
                    //
                    // Create a proxy to use in oneway invocations
                    // by calling ice_oneway.
                    //
                    var oneway = twoway.ice_oneway();
                    
                    //
                    // Create a proxy to use in batch oneway 
                    // invocations by calling ice_batchOneway.
                    //
                    var batchOneway = twoway.ice_batchOneway();
                    
                    //
                    // Set or clear the timeout.
                    //
                    var timeout = $("#timeout").val();
                    if(timeout > 0)
                    {
                        twoway = twoway.ice_timeout(timeout);
                        oneway = oneway.ice_timeout(timeout);
                        batchOneway = batchOneway.ice_timeout(timeout);
                    }
                    else
                    {
                        twoway = twoway.ice_timeout(-1);
                        oneway = oneway.ice_timeout(-1);
                        batchOneway = batchOneway.ice_timeout(-1);
                    }
                    
                    //
                    // Now send the request with the give protocol
                    // and mode.
                    //
                    var mode = $("#mode").val();
                    var delay = $("#delay").val();
                    if(mode == "twoway")
                    {
                        return twoway.sayHello(delay);
                    }
                    else if(mode == "twoway-secure")
                    {
                        return twoway.ice_secure(true).sayHello(delay);
                    }
                    else if(mode == "oneway")
                    {
                        return oneway.sayHello(delay);
                    }
                    else if(mode == "oneway-secure")
                    {
                        return oneway.ice_secure(true).sayHello(delay);
                    }
                    else if(mode == "oneway-batch")
                    {
                        batch++;
                        return batchOneway.sayHello(delay);
                    }
                    else if(mode == "oneway-batch-secure")
                    {
                        batch++;
                        return batchOneway.ice_secure(true).sayHello(delay);
                    }
                });
        }
    ).then(
        function()
        {
            setState(State.Idle);
        },
        function(ex)
        {
            setState(State.Idle, ex);
        });
}

//
// Flush batch requests.
//
function flush()
{
    batch = 0;
    setState(State.FlushBatchRequests);
    communicator.flushBatchRequests().then(
        function()
        {
            setState(State.Idle);
        },
        function(ex)
        {
            setState(State.Idle, ex);
        });
}

//
// Shutdown the server.
//
function shutdown()
{
    Ice.Promise.try(
        function()
        {
            setState(State.SendRequest);
            //
            // Initialize the communicator if it has not yet been initialized.
            //
            if(!communicator)
            {
                communicator = Ice.initialize();
            }
            
            //
            // Create a proxy to the hello object.
            //
            var s = "hello:ws -h " + hostname() + " -p 10000";
            var proxy = communicator.stringToProxy(s);
            
            //
            // Down-cast the proxy the Demo.Hello interface.
            //
            return Demo.HelloPrx.checkedCast(proxy).then(
                function(r, hello)
                {
                    //
                    // Now invoke the shutdown operation in the proxy
                    // to shutdown the server.
                    //
                    return hello.shutdown();
                });
        }
    ).then(
        function()
        {
            setState(State.Idle);
        },
        function(ex)
        {
            setState(State.Idle, ex);
        });
}

//
// Enable/disable flush request
//
function enableFlush(value)
{
    if(value && batch > 0)
    {
        $("#flush").removeClass("disabled")
                   .click(
                        function()
                        {
                            flush();
                            return false;
                        });
    }
    else
    {
        $("#flush").off("click")
                   .addClass("disabled");
    }
}

//
// Client state
//
function setState(newState, ex)
{
    assert(state !== newState);
    switch(newState)
    {
        case State.Idle:
        {
            assert(state === undefined || 
                   state === State.SendRequest ||
                   state === State.FlushBatchRequests);
            
            if(ex !== undefined)
            {
                $("#output").val(ex.toString());
            }
            
            //
            // Hide the progress indicator.
            //
            $("#progress").hide();
            
            //
            // Hello button event handler.
            //
            $("#hello").removeClass("disabled");
            $("#hello").click(
                function()
                {
                    sayHello();
                    return false;
                });

            //
            // Shutdown button event handler.
            //
            $("#shutdown").removeClass("disabled");
            $("#shutdown").click(
                function()
                {
                    shutdown();
                    return false;
                });
            
            enableFlush(true);
            
            break;
        }
        case State.SendRequest:
        case State.FlushBatchRequests:
        {
            assert(state === State.Idle);
            //
            // Disable buttons and show the pogress indicator.
            //
            $("#output").val("");
            $("#hello").off("click");
            $("#hello").addClass("disabled");
            $("#shutdown").off("click");
            $("#shutdown").addClass("disabled");
            enableFlush(false);
            $("#progress-message").text(
                newState === State.SendRequest ? "Sending Request..." : "Flush Batch Requests...");
            $("#progress").show();
            break;
        }
    }
    state = newState;
};

//
// Sets the initial state.
//
setState(State.Idle);

//
// Helper method to get the hostname.
//
function hostname()
{
    return $("#hostname").val() || $("#hostname").attr("placeholder");
}

function assert(v)
{
    if(!v)
    {
        throw new Error("Assertion failed");
    }
}

}());
