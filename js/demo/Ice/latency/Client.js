
(function(){
console.log("Client.js");
    
        require("Ice/Ice");
        require("Latency");
        
        var Demo = global.Demo || {};
        
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
                    p.fail(ex);
                }
            }
            
            try
            {
                var id = new Ice.InitializationData();
                id.properties = Ice.createProperties();
                id.properties.setProperty("Ping.Proxy", "ping:default -p 10000");
                
                communicator = Ice.initialize(id);
                
                var start, total;
                var repetitions = 100000;
                
                Demo.PingPrx.checkedCast(communicator.propertyToProxy("Ping.Proxy")).then(
                    function(asyncResult, obj)
                    {
                        start = new Date().getTime();
                        console.log("pinging server " + repetitions + " times (this may take a while)");
                        
                        var p = new Ice.Promise();
                        var j = 0;
                        var succeedCB = function()
                        {
                            j++;
                            if(j == repetitions)
                            {
                                p.succeed();
                            }
                        };
                        
                        for(var i = 0; i < repetitions; ++i)
                        {
                            obj.ice_ping().then(succeedCB).exception(function(ex){ p.fail(ex); });
                        }
                        return p;
                    },
                    function(ex)
                    {
                        console.log("invalid proxy: ");
                        throw ex;
                    }
                ).then(
                    function()
                    {
                        total = new Date().getTime() - start;
                        var perPing = total / repetitions;

                        console.log("time for " + repetitions + " pings: " + total + "ms");
                        console.log("time per ping: " + perPing + "ms");
                        cleanup();
                    },
                    function(ex)
                    {
                        throw ex;
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
        client.run();
}());
