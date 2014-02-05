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

    var Demo = global.Demo || {};
    var CallbackSenderPrx = Demo.CallbackSenderPrx;

    var CallbackReceiverI = function()
    {
    };

    CallbackReceiverI.prototype = new Demo.CallbackReceiver();
    CallbackReceiverI.prototype.constructor = CallbackReceiverI;

    CallbackReceiverI.prototype.callback = function(num, current)
    {
        console.log("received callback #" + num);
    };

    var run = function()
    {
        var id = new Ice.InitializationData();
        id.properties = Ice.createProperties();
        //
        // Client-side ACM must be disabled for bidirectional connections.
        //
        id.properties.setProperty("Ice.ACM.Client", "0");

        var communicator = Ice.initialize(id);
        
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
                    }
                );
            }
        });

        var ident = new Ice.Identity(Ice.UUID.generateUUID(), "");
        
        CallbackSenderPrx.checkedCast(communicator.stringToProxy("sender:tcp -p 10000")).then(
            function(r, server)
            {
                return communicator.createObjectAdapter("").then(
                    function(r, adapter)
                    {
                        adapter.add(new CallbackReceiverI(), ident);
                        return adapter.activate().then(
                            function(asyncResult)
                            {
                                return server.ice_getConnection();
                            }
                        ).then(
                            function(asyncResult, conn)
                            {
                                conn.setAdapter(adapter);
                                return server.addClient(ident);
                            }
                        );
                    });
            }
        ).exception(
            function(ex)
            {
                console.log(ex.toString());
                communicator.destroy().finally(
                    function()
                    {
                        process.exit(1);
                    });
            });
    };

    run();
}());
