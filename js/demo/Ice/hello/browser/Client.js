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

function sayHello()
{
    Ice.Promise.try(
        function()
        {
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
                        enableFlush(true);
                        return batchOneway.sayHello(delay);
                    }
                    else if(mode == "oneway-batch-secure")
                    {
                        enableFlush(true);
                        return batchOneway.ice_secure(true).sayHello(delay);
                    }
                });
        }
    ).exception(
        function(ex)
        {
            //
            // Handle any exceptions above.
            //
            $("#console").val(ex.toString());
        });
}

//
// Flush batch requests.
//
function flush()
{
    enableFlush(false);
    communicator.flushBatchRequests().exception(
        function(ex)
        {
            //
            // Handle any exceptions above.
            //
            $("#console").val(ex.toString());
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
    ).exception(
        function(ex)
        {
            //
            // Handle any exceptions above.
            //
            $("#console").val(ex.toString());
        });
}

//
// Hello button event handler.
//
$("#hello").click(
    function()
    {
        sayHello();
        return false;
    });

//
// Shutdown button event handler.
//
$("#shutdown").click(
    function()
    {
        shutdown();
        return false;
    });

//
// Enable/disable flush request
//
var flushEnabled = false;

function enableFlush(value)
{
    if(value != flushEnabled)
    {
        if(value)
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
        flushEnabled = value;
    }
}

//
// Helper method to get the hostname.
//
function hostname()
{
    return $("#hostname").val() || $("#hostname").attr("placeholder");
}

}());
