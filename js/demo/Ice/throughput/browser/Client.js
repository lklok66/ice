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

var communicator = Ice.initialize();

//
// Run the throughput test.
//
function run()
{
    //
    // Create a proxy to the throughput object.
    //
    var hostname = $("#hostname").val() || $("#hostname").attr("placeholder");
    var proxy = communicator.stringToProxy("throughput:ws -h " + hostname + " -p 10000");
    
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
            
            //
            // Get the proxy and operation to use by this test.
            //
            var test = $("#test").val();
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
            // Invoke the test operation in a loop with the required
            // arguments. 
            //
            // We chain the promises, a test operation is called only
            // once the promise for the previous operation is
            // fulfilled.
            //
            var start = new Date().getTime();
            var args = test != "receive" ? [seq] : [];
            return loop(
                function() 
                {
                    return operation.apply(proxy, args);
                },
                repetitions
            ).then(
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

//
// Asynchronous loop, each call to the given function returns a
// promise that when fulfilled runs the next iteration.
//    
function loop(fn, repetitions)
{
    var i = 0;
    var next = function() 
    {
        if(i++ < repetitions)
        {
            return fn.call().then(next);
        }
    };
    return next();
}

//
// Handle the client state.
//
var State = { Finish:0, Running: 1 };
var state = State.Finish;

function setState(s, ex)
{
    if(s != state)
    {
        switch(s)
        {
            case State.Running:
            {
                $("#console").val("");
                $("#run").addClass("disabled");
                $("#progress").show();
                $("body").addClass("waiting");
                break;
            }
            case State.Finish:
            {
                $("#run").removeClass("disabled");
                $("#progress").hide();
                $("body").removeClass("waiting");
                break;
            }
        }
        state = s;
    }
}

$("#run").click(
    function()
    {
        //
        // Run the throughput loop if not already running.
        //
        if(state !== State.Running)
        {
            setState(State.Running);

            Ice.Promise.try(
                function()
                {
                    return run();
                }
            ).exception(
                function(ex)
                {
                    console.log(ex.stack);
                    $("#console").val(ex.toString());
                }
            ).finally(
                function()
                {
                    setState(State.Finish);
                }
            );
        }
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
