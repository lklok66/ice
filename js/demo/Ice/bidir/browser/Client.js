(function(){

var Promise = Ice.Promise;
var CallbackSenderPrx = Demo.CallbackSenderPrx;

//
// Define a servant class that implements Demo.CallbackReceiver
// interface.
//
var CallbackReceiverI = Ice.Class(Demo.CallbackReceiver, {
    callback: function(num, current)
    {
        writeLine("received callback #" + num);
    }
});

var id = new Ice.InitializationData();
id.properties = Ice.createProperties();
//
// Client-side ACM must be disabled for bidirectional connections.
//
id.properties.setProperty("Ice.ACM.Client", "0");

var communicator;

var start = function()
{
    if(!isConnected())
    {
        setState(State.Connecting);
        Promise.try(
            function()
            {
                //
                // Initialize the communicator and create a proxy to the sender object.
                //
                communicator = Ice.initialize(id);
                var proxy = communicator.stringToProxy("sender:ws -p 10000 -h " + hostname());
                
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
                                var ident = new Ice.Identity(Ice.generateUUID(), "");
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
                                    }
                                ).then(
                                    function()
                                    {
                                        //
                                        // Now switch to the connected state.
                                        //
                                        setState(State.Connected);
                                    });
                            });
                    });
            }
        ).exception(
            function(ex)
            {
                setState(State.Disconnecting, ex);
            });
    }
    return false;
};

var stop = function()
{
    if(isConnected())
    {
        setState(State.Disconnecting);
    }
};

$("#start").click(start);
$("#stop").click(stop);

var State = {Disconnected: 0, Connecting: 1, Connected: 2, Disconnecting: 3};
var state = State.Disconnected;

var isConnected = function()
{
    return state == State.Connected;
};

var setState = function(s, ex)
{
    if(state == s)
    {
        return;
    }
    state = s;
    switch(s)
    {
        case State.Disconnected:
        {
            if(ex)
            {
                $("#console").val(ex.toString());
            }
            $("#start").removeClass("disabled");
            break;
        }
        case State.Connecting:
        {
            $("#console").val("");
            $("#start").addClass("disabled");
            break;
        }
        case State.Connected:
        {
            $("#stop").removeClass("disabled");
            break;
        }
        case State.Disconnecting:
        {
            $("#stop").addClass("disabled");
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
                    setState(State.Disconnected, ex);
                });
            break;
        }
        default:
        {
            break;
        }
    }
};

var hostname = function()
{
    return $("#hostname").val() || $("#hostname").attr("placeholder");
}

var writeLine = function(msg)
{
    $("#console").val($("#console").val() + msg + "\n");
    $("#console").scrollTop($("#console").get(0).scrollHeight);
}

}());
