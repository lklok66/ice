
(function(){
    
        require("Ice/Ice");
        require("Throughput");
        
        var Demo = global.Demo || {};
        
        var menu = function()
        {
            process.stdout.write(
                "usage:\n" +
                "\n" +
                "toggle type of data to send:\n" +
                "1: sequence of bytes (default)\n" +
                "2: sequence of strings (\"hello\")\n" +
                "3: sequence of structs with a string (\"hello\") and a double\n" +
                "4: sequence of structs with two ints and a double\n" +
                "\n" +
                "select test to run:\n" +
                "t: Send sequence as twoway\n" +
                "o: Send sequence as oneway\n" +
                "r: Receive sequence\n" +
                "e: Echo (send and receive) sequence\n" +
                "\n" +
                "other commands:\n" +
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
                var byteSeq = [];
                for(var i = 0; i < Demo.ByteSeqSize; ++i)
                {
                    byteSeq[i] = 0;
                }
                byteSeq = Ice.Buffer.createNative(byteSeq);
                
                var stringSeq = [];
                for(var i = 0; i < Demo.StringSeqSize; ++i)
                {
                    stringSeq[i] = "hello";
                }

                var structSeq = [];
                for(var i = 0; i < Demo.StringDoubleSeqSize; ++i)
                {
                    structSeq[i] = new Demo.StringDouble();
                    structSeq[i].s = "hello";
                    structSeq[i].d = 3.14;
                }

                var fixedSeq = [];
                for(var i = 0; i < Demo.FixedSeqSize; ++i)
                {
                    fixedSeq[i] = new Demo.Fixed();
                    fixedSeq[i].i = 0;
                    fixedSeq[i].j = 0;
                    fixedSeq[i].d = 0;
                }
                
                var id = new Ice.InitializationData();
                id.properties = Ice.createProperties();
                id.properties.setProperty("Throughput.Proxy", "throughput:default -p 10000");
                
                communicator = Ice.initialize(id);
                
                var currentType = "1";
                var repetitions = 100;
                
                var seqSize = Demo.ByteSeqSize;
                var seq = byteSeq;
                var wireSize = 1;
                
                var printEx = function(ex){ console.log(ex.stack); };
                Demo.ThroughputPrx.checkedCast(communicator.propertyToProxy("Throughput.Proxy")).then(
                    function(asyncResult, twoway)
                    {
                        process.stdin.resume();
                        
                        console.log("checkedCast");
                        var oneway = twoway.ice_oneway();
                        
                        menu();
                        
                        var data = [];
                        process.stdin.on("data", 
                            function(buffer)
                            {
                                data = buffer.toString("utf-8").trim().replace("\n","").split("");
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
                                        
                                        var proxy;
                                        var operation;
                                        
                                        if(key == "1" || key == "2" || key == "3" || key == "4")
                                        {
                                            currentType = key;
                                            switch(currentType)
                                            {
                                                case "1":
                                                {
                                                    console.log("using byte sequences");
                                                    seqSize = Demo.ByteSeqSize;
                                                    seq = byteSeq;
                                                    wireSize = 1;
                                                    break;
                                                }

                                                case "2":
                                                {
                                                    console.log("using string sequences");
                                                    seqSize = Demo.StringSeqSize;
                                                    seq = stringSeq;
                                                    wireSize = seq[0].length;
                                                    break;
                                                }

                                                case "3":
                                                {
                                                    console.log("using variable-length struct sequences");
                                                    seqSize = Demo.StringDoubleSeqSize;
                                                    seq = structSeq;
                                                    wireSize = seq[0].s.length;
                                                    wireSize += 8; // Size of double on the wire.
                                                    break;
                                                }

                                                case "4":
                                                {
                                                    console.log("using fixed-length struct sequences");
                                                    seqSize = Demo.FixedSeqSize;
                                                    seq = fixedSeq;
                                                    wireSize = 16; // Size of two ints and a double on the wire.
                                                    break;
                                                }
                                            }
                                            process.stdout.write("==> ");
                                        }
                                        else if(key == "t" || key == "o" || key == "r" || key == "e")
                                        {
                                            process.stdin.pause();
                                            switch(key)
                                            {
                                                case "t":
                                                case "o":
                                                {
                                                    proxy = key == "o" ? oneway : twoway;
                                                    if(currentType == 1)
                                                    {
                                                        operation = proxy.sendByteSeq;
                                                    }
                                                    else if(currentType == 2)
                                                    {
                                                        operation = proxy.sendStringSeq;
                                                    }
                                                    else if(currentType == 3)
                                                    {
                                                        operation = proxy.sendStructSeq;
                                                    }
                                                    else if(currentType == 4)
                                                    {
                                                        operation = proxy.sendFixedSeq;
                                                    }
                                                    process.stdout.write("sending");
                                                    break;
                                                }
                                                
                                                case "r":
                                                {
                                                    proxy = twoway;
                                                    if(currentType == 1)
                                                    {
                                                        operation = proxy.recvByteSeq;
                                                    }
                                                    else if(currentType == 2)
                                                    {
                                                        operation = proxy.recvStringSeq;
                                                    }
                                                    else if(currentType == 3)
                                                    {
                                                        operation = proxy.recvStructSeq;
                                                    }
                                                    else if(currentType == 4)
                                                    {
                                                        operation = proxy.recvFixedSeq;
                                                    }
                                                    process.stdout.write("receiving");
                                                    break;
                                                }
                                                
                                                case "e":
                                                {
                                                    proxy = twoway;
                                                    if(currentType == 1)
                                                    {
                                                        operation = proxy.echoByteSeq;
                                                    }
                                                    else if(currentType == 2)
                                                    {
                                                        operation = proxy.echoStringSeq;
                                                    }
                                                    else if(currentType == 3)
                                                    {
                                                        operation = proxy.echoStructSeq;
                                                    }
                                                    else if(currentType == 4)
                                                    {
                                                        operation = proxy.echoFixedSeq;
                                                    }
                                                    process.stdout.write("sending and receiving");
                                                    break;
                                                }
                                            }
                                            
                                            process.stdout.write(" " + repetitions);
                                            switch(currentType)
                                            {
                                                case "1":
                                                {
                                                    process.stdout.write(" byte");
                                                    break;
                                                }
                                                case "2":
                                                {
                                                    process.stdout.write(" string");
                                                    break;
                                                }
                                                case "3":
                                                {
                                                    process.stdout.write(" variable-length struct");
                                                    break;
                                                }

                                                case "4":
                                                {
                                                    process.stdout.write(" fixed-length struct");
                                                    break;
                                                }
                                            }
                                            
                                            process.stdout.write(" sequences of size " + seqSize);
                    
                                            if(key == "o")
                                            {
                                                process.stdout.write(" as oneway");
                                            }
                                            console.log("...");
                                            
                                            var p1 = new Ice.Promise();
                                            var count = 0;
                                            var start = new Date().getTime();
                                            for(var i = 0; i < repetitions; ++i)
                                            {
                                                var args = [];
                                                if(key != "r")
                                                {
                                                    args.push(seq);
                                                }
                                                operation.apply(proxy, args).then(
                                                    function(asyncResult)
                                                    {
                                                        if(++count == repetitions)
                                                        {
                                                            p1.succeed();
                                                        }
                                                    },
                                                    function(ex)
                                                    {
                                                        console.log(ex);
                                                        p1.fail(ex);
                                                    });
                                            }
                                            
                                            p1.then(
                                                function()
                                                {
                                                    var total = new Date().getTime() - start;
                                                    console.log("time for " + repetitions + " sequences: " + total  + "ms");
                                                    console.log("time per sequence: " + total / repetitions + "ms");
                                                    
                                                    var mbit = repetitions * seqSize * wireSize * 8.0 / total / 1000.0;
                                                    if(key == "e")
                                                    {
                                                        mbit *= 2;
                                                    }
                                                    mbit = Math.round(mbit * 100) / 100;
                                                    console.log("throughput: " + mbit + " Mbps");
                                                    
                                                    process.stdin.resume();
                                                    process.stdout.write("==> ");
                                                },
                                                function(ex)
                                                {
                                                    cleanup(ex);
                                                }
                                            );
                                        }
                                        else if(key == "s")
                                        {
                                            twoway.shutdown().exception(printEx);
                                            process.stdout.write("==> ");
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
                        console.log("invalid proxy: " + twoway);
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
