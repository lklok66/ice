// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

$(document).ready(function(){

var communicator;

$("#hello").click(
    function()
    {
        Ice.Promise.try(
            function()
            {
                //
                // Initialize the communicator if has not  yet been 
                // initialized.
                //
                if(!communicator)
                {
                    communicator = Ice.initialize();
                }
                var s = "hello:ws -h " + hostname() + " -p 10000:" +
                        "wss -h " + hostname() + " -p 10001"
                
                var proxy = communicator.stringToProxy(s).ice_twoway().
                                            ice_timeout(-1).ice_secure(false);
                
                return Demo.HelloPrx.checkedCast(proxy).then(
                    function(asyncResult, twoway)
                    {
                        var oneway = twoway.ice_oneway();
                        var batchOneway = twoway.ice_batchOneway();
                        
                        var timeout = $("#timeout").val();
                        var delay = $("#delay").val();
                        var mode = $("#mode").val();
                        
                        //
                        // Set or clear the timeout.
                        //
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
                        if(mode == "twoway")
                        {
                            return twoway.sayHello(delay);
                        }
                        else if(mode == "twoway-secure")
                        {
                            twoway = twoway.ice_secure(true);
                            return twoway.sayHello(delay);
                        }
                        else if(mode == "oneway")
                        {
                            return oneway.sayHello(delay);
                        }
                        else if(mode == "oneway-secure")
                        {
                            oneway = oneway.ice_secure(true);
                            return oneway.sayHello(delay);
                        }
                        else if(mode == "oneway-batch")
                        {
                            $("#flush").removeClass("disabled");
                            return batchOneway.sayHello(delay);
                        }
                        else if(mode == "oneway-batch-secure")
                        {
                            $("#flush").removeClass("disabled");
                            batchOneway = batchOneway.ice_secure(true);
                            return batchOneway.sayHello(delay);
                        }
                    });
            }
        ).exception(
            function(ex)
            {
                write(ex.toString());
            });
        return false;
    });

$("#flush").click(
    function()
    {
        if(!$(this).hasClass("disabled"))
        {
            $(this).addClass("disabled");
            communicator.flushBatchRequests().exception(
                function(ex)
                {
                    write(ex.toString());
                });
        }
        return false;
    });

$("#shutdown").click(
    function()
    {
        Ice.Promise.try(
            function()
            {
                //
                // Initialize the communicator if has not  yet been 
                // initialized.
                //
                if(!communicator)
                {
                    communicator = Ice.initialize();
                }
                var proxy = communicator.stringToProxy(
                                "hello:ws -h " + hostname() + " -p 10000");
                return Demo.HelloPrx.checkedCast(proxy).then(
                    function(asyncResult, twoway)
                    {
                        return twoway.shutdown();
                    });
            }
        ).exception(
            function(ex)
            {
                write(ex.toString());
            });
        return false;
    });


$("#hostname").attr("placeholder", document.location.hostname || "127.0.0.1");
$("#timeout").noUiSlider({range: [0, 2500], start: 0, handles: 1});
$("#delay").noUiSlider({range: [0, 2500], start: 0, handles: 1});

var write = function(msg)
{
    $("#console").val(msg);
    $("#console").scrollTop($("#console").get(0).scrollHeight);
};

var hostname = function()
{
    return $("#hostname").val() || $("#hostname").attr("placeholder");
};

});


