// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){

var Promise = Ice.Promise;
var ThroughputPrx = Demo.ThroughputPrx;

//
// Initialize sequences.
//
var byteSeq = new ArrayBuffer();
for(var i = 0; i < Demo.ByteSeqSize; ++i)
{
    byteSeq[i] = 0;
}

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

//
// Run the throughput test.
//
function run()
{
    if(!isRunning())
    {
        setSate(State.Running);
        var communicator;
        
        Promise.try(
            function()
            {
                //
                // Initialize the communicator and create a proxy
                // to the throughput object.
                //
                communicator = Ice.initialize();
                var s = "throughput:ws -h " + hostname() + " -p 10000";
                var proxy = communicator.stringToProxy(s);
                
                //
                // Down-cast the proxy to the Demo.Throughput interface.
                //
                return ThroughputPrx.checkedCast(proxy).then(
                    function(r, twoway)
                    {
                        oneway = twoway.ice_oneway();

                        var seq;
                        var seqSize
                        var wireSize;
                        var proxy;
                        var operation;
                        var repetitions = 100;
                        
                        var data = $("#data").val();
                        //
                        // Get the sequence data to use by
                        // this test.
                        //
                        if(data == "byte-seq")
                        {
                            seq = byteSeq;
                            seqSize = Demo.ByteSeqSize;
                            seq = byteSeq;
                            wireSize = 1;
                        }
                        else if(data == "string-seq")
                        {
                            seq = stringSeq;
                            seqSize = Demo.StringSeqSize;
                            seq = stringSeq;
                            wireSize = seq[0].length;
                        }
                        else if(data == "struct-seq")
                        {
                            seq = structSeq;
                            seqSize = Demo.StringDoubleSeqSize;
                            seq = structSeq;
                            wireSize = seq[0].s.length;
                            //
                            // Size of double on the wire.
                            //
                            wireSize += 8;
                        }
                        else if(data == "fixed-seq")
                        {
                            seq = fixedSeq;
                            seqSize = Demo.FixedSeqSize;
                            seq = fixedSeq;
                            //
                            // Size of two ints and a double on the wire.
                            //
                            wireSize = 16;
                        }
                        
                        var test = $("#test").val();
                        //
                        // Get the proxy and operation to use by
                        // this test.
                        //
                        if(test == "twoway" || test == "oneway")
                        {
                            proxy = test == "twoway" ? twoway : oneway;
                            if(data == "byte-seq")
                            {
                                operation = proxy.sendByteSeq;
                            }
                            else if(data == "string-seq")
                            {
                                operation = proxy.sendStringSeq;
                            }
                            else if(data == "struct-seq")
                            {
                                operation = proxy.sendStructSeq;
                            }
                            else if(data == "fixed-seq")
                            {
                                operation = proxy.sendFixedSeq;
                            }
                            write("sending");
                        }
                        else if(test == "receive")
                        {
                            proxy = twoway;
                            if(data == "byte-seq")
                            {
                                operation = proxy.recvByteSeq;
                            }
                            else if(data == "string-seq")
                            {
                                operation = proxy.recvStringSeq;
                            }
                            else if(data == "struct-seq")
                            {
                                operation = proxy.recvStructSeq;
                            }
                            else if(data == "fixed-seq")
                            {
                                operation = proxy.recvFixedSeq;
                            }
                            write("receiving");
                        }
                        else if(test == "echo")
                        {
                            proxy = twoway;
                            if(data == "byte-seq")
                            {
                                operation = proxy.echoByteSeq;
                            }
                            else if(data == "string-seq")
                            {
                                operation = proxy.echoStringSeq;
                            }
                            else if(data == "struct-seq")
                            {
                                operation = proxy.echoStructSeq;
                            }
                            else if(data == "fixed-seq")
                            {
                                operation = proxy.echoFixedSeq;
                            }
                            write("sending and receiving");
                        }
                        
                        write(" " + repetitions);
                        if(data == "byte-seq")
                        {
                            write(" byte");
                        }
                        else if(data == "string-seq")
                        {
                            write(" string");
                        }
                        else if(data == "struct-seq")
                        {
                            write(" variable-length struct");
                        }
                        else if(data == "fixed-seq")
                        {
                            write(" fixed-length struct");
                        }
                        write(" sequences of size " + seqSize);
                        if(test == "oneway")
                        {
                            write(" as oneway");
                        }
                        writeLine("...");
                        
                        //
                        // Promise used to wait for the completion of all
                        // operations.
                        //
                        var p = new Promise();
                        var count = 0;
                        var start;
                        
                        //
                        // The success callback wait for the completion of
                        // all operations and then call the succeed method.
                        //
                        var successCB = function(r)
                        {
                            if(++count == repetitions)
                            {
                                p.succeed();
                            }
                        };
                        
                        //
                        // If there is an exception we call fail and we are
                        // done.
                        //
                        var exceptionCB = function(ex)
                        {
                            p.fail(ex);
                        };
                        
                        var args = test != "receive" ? [seq] : [];
                        
                        setTimeout(
                            function()
                            {
                                start = new Date().getTime();
                                //
                                // Invoke the test operation in a loop with the 
                                // required arguments.
                                //
                                for(var i = 0; i < repetitions; ++i)
                                {
                                    operation.apply(proxy, args)
                                             .then(successCB)
                                             .exception(exceptionCB);
                                }
                            });
                        
                        return p.then(
                            function()
                            {
                                //
                                // Write the results.
                                //
                                var total = new Date().getTime() - start;
                                writeLine("time for " + repetitions + " sequences: " + total  + " ms");
                                writeLine("time per sequence: " + total / repetitions + " ms");
                                
                                var mbit = repetitions * seqSize * wireSize * 8.0 / total / 1000.0;
                                if(test == "echo")
                                {
                                    mbit *= 2;
                                }
                                mbit = Math.round(mbit * 100) / 100;
                                writeLine("throughput: " + mbit + " Mbps");
                            });
                    });
            }
        ).finally(
            function()
            {
                //
                // Destroy the communicator if required.
                //
                if(communicator)
                {
                    return communicator.destroy();
                }
            }
        ).then(
            function()
            {
                //
                // Switch to finish state.
                //
                setSate(State.Finish);
            },
            function(ex)
            {
                console.log(ex.stack);
                //
                // Handle any exceptions above and switch
                // to finish state.
                //
                setSate(State.Finish, ex);
            });
    }
    return false;
}

//
// Handle the client state.
//
var State = {Finish:0, Running: 1};
var state = State.Finish;

function isRunning()
{
    return state == State.Running;
}

function setSate(s, ex)
{
    if(s != state)
    {
        switch(s)
        {
            case State.Running:
            {
                $("#console").val("");
                $("#run").addClass("disabled");
                break;
            }
            case State.Finish:
            {
                if(ex)
                {
                    $("#console").val(ex.toString());
                }
                $("#run").removeClass("disabled");
                break;
            }
        }
        state = s;
    }
}

//
// Helper function to retrieve the hostname.
//
function hostname()
{
    return $("#hostname").val() || $("#hostname").attr("placeholder");
}

$("#run").click(
    function()
    {
        run();
        return false;
    });

//
// Helper functions to write the output.
//
function write(msg)
{
    $("#console").val($("#console").val() + msg);
}

function writeLine(msg)
{
    write(msg + "\n");
    $("#console").scrollTop($("#console").get(0).scrollHeight);
}

}());
