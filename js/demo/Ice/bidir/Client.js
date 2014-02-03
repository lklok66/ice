
(function(){

    require("Ice/Ice");
    require("Callback");

    var Demo = global.Demo || {};

    var writeException = function(ex)
    {
        if(ex && ex._asyncResult)
        {
            console.log("exception occurred in call to " + ex._asyncResult.operation);
        }
        if(ex.stack)
        {
            console.log(ex.stack);
        }
        else
        {
            console.log(ex);
        }
    };

    var CallbackReceiverI = function()
    {
    };

    CallbackReceiverI.prototype = new Demo.CallbackReceiver();
    CallbackReceiverI.prototype.constructor = CallbackReceiverI;

    CallbackReceiverI.prototype.callback = function(num, current)
    {
        console.log("received callback #" + num);
    };

    var Client = function()
    {
    };

    Client.prototype.run = function()
    {
        var communicator;
        var p = new Ice.Promise();

        var cleanup = function(ex)
        {
            if(communicator !== null)
            {
                communicator.destroy().then(
                    function(asyncResult)
                    {
                        if(ex && ex instanceof Error)
                        {
                            writeException(ex);
                            p.fail(ex);
                        }
                        else
                        {
                            p.succeed();
                        }
                    }
                ).exception(
                    function(ex)
                    {
                        p.fail(ex);
                    }
                );
            }
            else
            {
                writeException(ex);
                p.fail(ex);
            }
        };
        
        //
        // Exit on SIGINT
        //
        process.on("SIGINT", function() { cleanup(); });

        try
        {
            var id = new Ice.InitializationData();
            id.properties = Ice.createProperties();
            id.properties.setProperty("CallbackSender.Proxy", "sender:tcp -p 10000");
            //
            // Client-side ACM must be disabled for bidirectional connections.
            //
            id.properties.setProperty("Ice.ACM.Client", "0");

            communicator = Ice.initialize(id);
            var adapter = communicator.createObjectAdapter("");
            var ident = new Ice.Identity();
            ident.name = Ice.UUID.generateUUID();
            ident.category = "";
            adapter.add(new CallbackReceiverI(), ident);

            var server;

            Demo.CallbackSenderPrx.checkedCast(communicator.propertyToProxy("CallbackSender.Proxy")
                ).then(
                    function(asyncResult, o)
                    {
                        server = o;
                        return adapter.activate();
                    }
                ).then(
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
                ).exception(cleanup);
        }
        catch(ex)
        {
            cleanup(ex);
        }
        return p;
    };

    var client = new Client();
    client.run().then(
        function()
        {
            process.exit(0)
        }
    ).exception(
        function(ex)
        {
            process.exit(1);
        }
    );
}());
