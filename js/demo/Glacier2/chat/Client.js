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
    require("Glacier2/Glacier2");
    require("Chat");
    
    //
    // Servant that implements the ChatCallback interface,
    // the message operation just writes the received data
    // to stdout.
    //
    var ChatCallbackI = Ice.Class(Demo.ChatCallback, {
        message: function(data)
        {
            console.log(data);
        }
    });
    
    var run = function(communicator)
    {
        var createSession = function()
        {
            return Ice.Promise.try(
                function()
                {
                    return Glacier2.RouterPrx.checkedCast(communicator.getDefaultRouter()).then(
                        function(r, router)
                        {
                            console.log("This demo accepts any user-id / password combination.\n");
                            process.stdout.write("user id: ");
                            return getline().then(
                                function(id)
                                {
                                    process.stdout.write("password: ");
                                    return getline().then(
                                        function(pw)
                                        {
                                            return router.createSession(id, pw);
                                        });
                                }
                            ).then(
                                function(r, session)
                                {
                                    return runWithSession(router, Demo.ChatSessionPrx.uncheckedCast(session));
                                },
                                function(ex)
                                {
                                    if(ex instanceof Glacier2.PermissionDeniedExceptionx)
                                    {
                                        console.log("permission denied:\n" << ex.reason);
                                        createSession();
                                    }
                                    else if(ex instanceof Glacier2.CannotCreateSessionException)
                                    {
                                        console.log("cannot create session:\n" << ex.reason);
                                        createSession();
                                    }
                                    else
                                    {
                                        throw ex;
                                    }
                                });
                        });
                });
        };
        
        var runWithSession = function(router, session)
        {
            var p = new Ice.Promise();
            var refreshSession;
            //
            // Get the session timeout, the router client category and
            // create the client object adapter.
            //
            // Use Ice.Promise.all to wait for the completion of all the
            // calls.
            //
            Ice.Promise.all(
                router.getSessionTimeout(),
                router.getCategoryForClient(),
                communicator.createObjectAdapterWithRouter("", router)
            ).then(
                function()
                {
                    var timeout = arguments[0][1];
                    var category = arguments[1][1];
                    var adapter = arguments[2][1];
                    
                    //
                    // Setup an interval call to refreshSession to keep
                    // the session alive.
                    //
                    refreshSession = setInterval(
                        function(){
                            router.refreshSession().exception(
                                function(ex){
                                    p.fail(ex);
                                });
                        }, (timeout.low * 500));
                    
                    //
                    // Create the ChatCallback servant and add it to the ObjectAdapter.
                    //
                    var callback = Demo.ChatCallbackPrx.uncheckedCast(
                        adapter.add(new ChatCallbackI(), new Ice.Identity("callback", category)));
                    
                    //
                    // Activate the client object adater before set the session callback.
                    //
                    adapter.activate();
                    
                    //
                    // Set the chat session callback.
                    //
                    return session.setCallback(callback);
                }
            ).then(
                function()
                {
                    //
                    // The chat function sequantially reads stdin messages
                    // and send it to server using the session say method.
                    //
                    var chat = function()
                    {
                        process.stdout.write("==> ");
                        return getline().then(
                            function(msg)
                            {
                                if(msg == "/quit")
                                {
                                    p.succeed();
                                }
                                else if(msg.indexOf("/") == 0)
                                {
                                    console.log("enter /quit to exit.");
                                }
                                else
                                {
                                    return session.say(msg);
                                }
                            }
                        ).then(
                            function()
                            {
                                if(!p.completed())
                                {
                                    return chat();
                                }
                            }
                        ).exception(
                            function(ex)
                            {
                                p.fail(ex);
                            });
                    };
                    
                    //
                    // Start the chat loop
                    //
                    return chat();
                }
            ).exception(
                function(ex)
                {
                    p.fail(ex);
                }
            ).finally(
                function()
                {
                    clearInterval(refreshSession);
                });
            
            return p;
        };
        
        return createSession();
    }
    
    var communicator;
    Ice.Promise.try(
        function() 
        {
            var id = new Ice.InitializationData();
            id.properties = Ice.createProperties();
            id.properties.setProperty("Ice.Default.Router", 
                                      "DemoGlacier2/router:tcp -p 4063 -h localhost");
            communicator = Ice.initialize(id);
            return run(communicator);
        }
    ).finally(
        function()
        {
            if(communicator)
            {
                return communicator.destroy();
            }
        }
    ).then(
        function()
        {
            process.exit(0);
        },
        function(ex)
        {
            console.log(ex.toString());
            process.exit(1);
        });
    
    //
    // Destroy communicator on SIGINT so application
    // exit cleanly.
    //
    process.once("SIGINT", function() {
        if(communicator)
        {
            communicator.destroy().finally(
                function()
                {
                    process.exit(0);
                });
        }
    });
    
    //
    // Asynchonously process stdin lines using a promise
    //
    var getline = function()
    {
        var p = new Ice.Promise();
        process.stdin.resume();
        process.stdin.once("data", 
            function(buffer)
            {
                process.stdin.pause();
                p.succeed(buffer.toString("utf-8").trim());
            });
        return p;
    };
}());
