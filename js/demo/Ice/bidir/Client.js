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
require("Callback");

var Demo = global.Demo;
var Promise = Ice.Promise;
var CallbackSenderPrx = Demo.CallbackSenderPrx;

//
// Define a servant class that implements Demo.CallbackReceiver
// interface.
//
var CallbackReceiverI = Ice.Class(Demo.CallbackReceiver, {
    callback: function(num, current)
    {
        console.log("received callback #" + num);
    }
});

var id = new Ice.InitializationData();
id.properties = Ice.createProperties();
//
// Client-side ACM must be disabled for bidirectional connections.
//
id.properties.setProperty("Ice.ACM.Client", "0");

var communicator;

//
// Exit on SIGINT
//
process.on("SIGINT", function() {
    if(communicator)
    {
        communicator.destroy().finally(
            function()
            {
                process.exit(0);
            });
    }
});

var ident = new Ice.Identity(Ice.generateUUID(), "");
Promise.try(
    function()
    {
        //
        // Initialize the communicator and create a proxy to the sender object.
        //
        communicator = Ice.initialize(id);
        var proxy = communicator.stringToProxy("sender:tcp -p 10000");
        
        //
        // Down-cast the proxy to the Demo.CallbackSender interface.
        //
        return CallbackSenderPrx.checkedCast(proxy).then(
            function(r, server)
            {
                //
                // Create the client object adapter.
                //
                return communicator.createObjectAdapter("").then(
                    function(r, adapter)
                    {
                        //
                        // Create a callback receiver servant and add it to
                        // the object adapter.
                        //
                        adapter.add(new CallbackReceiverI(), ident);
                        
                        //
                        // Activate the object adapter.
                        //
                        return adapter.activate().then(
                            function(r)
                            {
                                //
                                // Retrive the proxy connection to use with our
                                // object adapter.
                                //
                                return server.ice_getConnection();
                            }
                        ).then(
                            function(r, conn)
                            {
                                //
                                // Set the connection adapter.
                                //
                                conn.setAdapter(adapter);
                                return server.addClient(ident);
                            });
                    });
            });
    }
).exception(
    function(ex)
    {
        console.log(ex.toString());
        Promise.try(
            function()
            {
                if(communicator)
                {
                    return communicator.destroy();
                }
            }
        ).finally(
            function()
            {
                process.exit(1);
            });
    });

}());
