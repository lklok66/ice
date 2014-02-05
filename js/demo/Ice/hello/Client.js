
(function(){
    
        require("Ice/Ice");
        require("Hello");
        
        var Demo = global.Demo || {};
        
        var menu = function()
        {
            process.stdout.write(
                "usage:\n" +
                "t: send greeting as twoway\n" +
                "o: send greeting as oneway\n" +
                "O: send greeting as batch oneway\n" +
                "f: flush all batch requests\n" +
                "T: set a timeout\n" +
                "P: set a server delay\n" +
                //"S: switch secure mode on/off\n" +
                "s: shutdown server\n" +
                "x: exit\n" +
                "?: help\n" +
                "\n" +
                "==> ");
        };
        
        var writeException = function(ex)
        {
            console.log(ex.toString());
            if(ex._asyncResult)
            {
                console.log("\nexception occurred in call to " + ex._asyncResult.operation);
            }
            if(ex.stack)
            {
                console.log(ex.stack);
            }
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
            }
            
            try
            {
                var id = new Ice.InitializationData();
                id.properties = Ice.createProperties();
                id.properties.setProperty("Hello.Proxy", "hello:default -p 10000");
                
                communicator = Ice.initialize(id);
                var proxy = communicator.propertyToProxy("Hello.Proxy").ice_twoway().ice_timeout(-1).ice_secure(false);
                
                var secure = false;
                var timeout = -1;
                var delay = 0;
                
                var printEx = function(ex){ console.log(ex.stack); };
                Demo.HelloPrx.checkedCast(proxy).then(
                    function(asyncResult, twoway)
                    {
                        var oneway = twoway.ice_oneway();
                        var batchOneway = twoway.ice_batchOneway();
                        menu();
                        process.stdin.resume();
                        var data = [];
                        process.stdin.on("data", 
                            function(buffer)
                            {
                                data = buffer.toString("utf-8").trim().split("");
                                process.stdout.write("\n");
                                data.forEach(
                                    function(key)
                                    {
                                        if(key == "x")
                                        {
                                            process.stdin.pause();
                                            if(communicator)
                                            {
                                                communicator.destroy().then(cleanup).exception(cleanup);
                                            }
                                            else
                                            {
                                                cleanup();
                                            }
                                            return;
                                        }
                                        process.stdout.write("==> ");
                                        
                                        if(key == "t")
                                        {
                                            twoway.sayHello(delay).exception(printEx);
                                        }
                                        else if(key == "o")
                                        {
                                            oneway.sayHello(delay).exception(printEx);
                                        }
                                        else if(key == "O")
                                        {
                                            batchOneway.sayHello(delay).exception(printEx);
                                        }
                                        else if(key == "f")
                                        {
                                            communicator.flushBatchRequests().exception(printEx);
                                        }
                                        else if(key == "T")
                                        {
                                            if(timeout == -1)
                                            {
                                                timeout = 2000;
                                            }
                                            else
                                            {
                                                timeout = -1;
                                            }

                                            twoway = twoway.ice_timeout(timeout);
                                            oneway = oneway.ice_timeout(timeout);
                                            batchOneway = batchOneway.ice_timeout(timeout);

                                            if(timeout == -1)
                                            {
                                                console.log("timeout is now switched off");
                                            }
                                            else
                                            {
                                                console.log("timeout is now set to 2000ms");
                                            }
                                            process.stdout.write("==> ");
                                        }
                                        else if(key == "P")
                                        {
                                            if(delay == 0)
                                            {
                                                delay = 2500;
                                            }
                                            else
                                            {
                                                delay = 0;
                                            }

                                            if(delay == 0)
                                            {
                                                console.log("server delay is now deactivated");
                                            }
                                            else
                                            {
                                                console.log("server delay is now set to 2500ms");
                                            }
                                            process.stdout.write("==> ");
                                        }
                                        else if(key == "s")
                                        {
                                            twoway.shutdown().exception(printEx);
                                        }
                                        else if(key == "?")
                                        {
                                            process.stdout.write("\n");
                                            menu();
                                        }
                                        else
                                        {
                                            console.log("unknown command `" + key + "'");
                                            process.stdout.write("\n");
                                            menu();
                                        }
                                    });
                                data = [];
                            });
                    },
                    function(ex)
                    {
                        console.log("invalid proxy: " + proxy);
                        cleanup(ex);
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
