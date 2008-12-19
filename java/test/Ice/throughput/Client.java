// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.throughput;

import java.io.PrintWriter;

import test.Ice.throughput.Demo.ByteSeqSize;
import test.Ice.throughput.Demo.Fixed;
import test.Ice.throughput.Demo.FixedSeqSize;
import test.Ice.throughput.Demo.StringDouble;
import test.Ice.throughput.Demo.StringDoubleSeqSize;
import test.Ice.throughput.Demo.StringSeqSize;
import test.Ice.throughput.Demo.ThroughputPrx;
import test.Ice.throughput.Demo.ThroughputPrxHelper;

public class Client extends test.Util.Application
{
    class ShutdownHook extends Thread
    {
        public void
        run()
        {
            try
            {
                communicator().destroy();
            }
            catch(Ice.LocalException ex)
            {
                ex.printStackTrace();
            }
        }
    }

    public int
    run(String[] args)
    {
        PrintWriter out = getWriter();
        PrintWriter err = out;
        if(args.length > 0)
        {
            err.println(appName() + ": too many arguments");
            return 1;
        }

        //
        // Since this is an interactive demo we want to clear the
        // Application installed interrupt callback and install our
        // own shutdown hook.
        //
        ThroughputPrx throughput = ThroughputPrxHelper.checkedCast(communicator().propertyToProxy("Throughput.Proxy"));
        if(throughput == null)
        {
            err.println("invalid proxy");
            return 1;
        }
        ThroughputPrx throughputOneway = ThroughputPrxHelper.uncheckedCast(throughput.ice_oneway());

        int factor = 1;
        if(communicator().getProperties().getPropertyWithDefault("Ice.Default.Protocol", "tcp").equals("ssl"))
        {
            factor = 10;
        }

        byte[] byteSeq = new byte[ByteSeqSize.value / factor];

        String[] stringSeq = new String[StringSeqSize.value / factor];
        for(int i = 0; i < StringSeqSize.value / factor; ++i)
        {
            stringSeq[i] = "hello";
        }

        StringDouble[] structSeq = new StringDouble[StringDoubleSeqSize.value / factor];
        for(int i = 0; i < StringDoubleSeqSize.value / factor; ++i)
        {
            structSeq[i] = new StringDouble();
            structSeq[i].s = "hello";
            structSeq[i].d = 3.14;
        }

        Fixed[] fixedSeq = new Fixed[FixedSeqSize.value / factor];
        for(int i = 0; i < FixedSeqSize.value / factor; ++i)
        {
            fixedSeq[i] = new Fixed();
            fixedSeq[i].i = 0;
            fixedSeq[i].j = 0;
            fixedSeq[i].d = 0;
        }

        // No warm up for Android.

        char currentType = '1';
        int seqSize = ByteSeqSize.value / factor;

        // Initial ping to setup the connection.
        throughput.ice_ping();
        
        String[] input = { "t", "o", "r", "e", "s", "x", null };
        int inputIndex = 0;
        String line = null;
        do
        {
            try
            {
                line = input[inputIndex++];

                long tmsec = System.currentTimeMillis();
                final int repetitions = 100;

                if(line.equals("1") || line.equals("2") || line.equals("3") || line.equals("4"))
                {
                    currentType = line.charAt(0);

                    switch(currentType)
                    {
                        case '1':
                        {
                            out.println("using byte sequences");
                            seqSize = ByteSeqSize.value / factor;
                            break;
                        }

                        case '2':
                        {
                            out.println("using string sequences");
                            seqSize = StringSeqSize.value / factor;
                            break;
                        }

                        case '3':
                        {
                            out.println("using variable-length struct sequences");
                            seqSize = StringDoubleSeqSize.value / factor;
                            break;
                        }

                        case '4':
                        {
                            out.println("using fixed-length struct sequences");
                            seqSize = FixedSeqSize.value / factor;
                            break;
                        }
                    }
                }
                else if(line.equals("t") || line.equals("o") || line.equals("r") || line.equals("e"))
                {
                    char c = line.charAt(0);

                    switch(c)
                    {
                        case 't':
                        case 'o':
                        {
                            out.print("sending");
                            break;
                        }
                        
                        case 'r':
                        {
                            out.print("receiving");
                            break;
                        }
                        
                        case 'e':
                        {
                            out.print("sending and receiving");
                            break;
                        }
                    }
                    
                    out.print(" " + repetitions);
                    switch(currentType)
                    {
                        case '1':
                        {
                            out.print(" byte");
                            break;
                        }

                        case '2':
                        {
                            out.print(" string");
                            break;
                        }

                        case '3':
                        {
                            out.print(" variable-length struct");
                            break;
                        }

                        case '4':
                        {
                            out.print(" fixed-length struct");
                            break;
                        }
                    }
                    
                    out.print(" sequences of size " + seqSize);
                    
                    if(c == 'o')
                    {
                        out.print(" as oneway");
                    }
                    
                    out.println("...");
                    out.flush();
                    
                    for(int i = 0; i < repetitions; ++i)
                    {
                        switch(currentType)
                        {
                            case '1':
                            {
                                switch(c)
                                {
                                    case 't':
                                    {
                                        throughput.sendByteSeq(byteSeq);
                                        break;
                                    }
                            
                                    case 'o':
                                    {
                                        throughputOneway.sendByteSeq(byteSeq);
                                        break;
                                    }
                            
                                    case 'r':
                                    {
                                        throughput.recvByteSeq();
                                        break;
                                    }
                            
                                    case 'e':
                                    {
                                        throughput.echoByteSeq(byteSeq);
                                        break;
                                    }
                                }
                                break;
                            }

                            case '2':
                            {
                                switch(c)
                                {
                                    case 't':
                                    {
                                        throughput.sendStringSeq(stringSeq);
                                        break;
                                    }
                            
                                    case 'o':
                                    {
                                        throughputOneway.sendStringSeq(stringSeq);
                                        break;
                                    }
                            
                                    case 'r':
                                    {
                                        throughput.recvStringSeq();
                                        break;
                                    }
                            
                                    case 'e':
                                    {
                                        throughput.echoStringSeq(stringSeq);
                                        break;
                                    }
                                }
                                break;
                            }

                            case '3':
                            {
                                switch(c)
                                {
                                    case 't':
                                    {
                                        throughput.sendStructSeq(structSeq);
                                        break;
                                    }
                            
                                    case 'o':
                                    {
                                        throughputOneway.sendStructSeq(structSeq);
                                        break;
                                    }
                            
                                    case 'r':
                                    {
                                        throughput.recvStructSeq();
                                        break;
                                    }
                            
                                    case 'e':
                                    {
                                        throughput.echoStructSeq(structSeq);
                                        break;
                                    }
                                }
                                break;
                            }

                            case '4':
                            {
                                switch(c)
                                {
                                    case 't':
                                    {
                                        throughput.sendFixedSeq(fixedSeq);
                                        break;
                                    }
                            
                                    case 'o':
                                    {
                                        throughputOneway.sendFixedSeq(fixedSeq);
                                        break;
                                    }
                            
                                    case 'r':
                                    {
                                        throughput.recvFixedSeq();
                                        break;
                                    }
                            
                                    case 'e':
                                    {
                                        throughput.echoFixedSeq(fixedSeq);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }

                    double dmsec = System.currentTimeMillis() - tmsec;
                    out.println("time for " + repetitions + " sequences: " + dmsec  + "ms");
                    out.println("time per sequence: " + dmsec / repetitions + "ms");
                    out.flush();
                    int wireSize = 0;
                    switch(currentType)
                    {
                        case '1':
                        {
                            wireSize = 1;
                            break;
                        }

                        case '2':
                        {
                            wireSize = stringSeq[0].length();
                            break;
                        }

                        case '3':
                        {
                            wireSize = structSeq[0].s.length();
                            wireSize += 8; // Size of double on the wire.
                            break;
                        }

                        case '4':
                        {
                            wireSize = 16; // Size of two ints and a double on the wire.
                            break;
                        }
                    }
                    double mbit = repetitions * seqSize * wireSize * 8.0 / dmsec / 1000.0;
                    if(c == 'e')
                    {
                        mbit *= 2;
                    }
                    out.println("throughput: " + new java.text.DecimalFormat("#.##").format(mbit) + "Mbps");
                    out.flush();
                }
                else if(line.equals("s"))
                {
                    throughput.shutdown();
                }
                else if(line.equals("x"))
                {
                    // Nothing to do
                }
                else if(line.equals("?"))
                {
                }
                else
                {
                    out.println("unknown command `" + line + "'");
                }
            }
            catch(Ice.LocalException ex)
            {
                ex.printStackTrace();
            }
        }
        while(!line.equals("x"));

        return 0;
    }
    
    protected Ice.InitializationData getInitData(Ice.StringSeqHolder argsH)
    {
        Ice.InitializationData initData = new Ice.InitializationData();
        initData.properties = Ice.Util.createProperties(argsH);
        initData.properties.setProperty("Ice.Package.Demo", "test.Ice.throughput");
        initData.properties.setProperty("Throughput.Proxy", "throughput:default -p 10000 -h 127.0.0.1");
        initData.properties.setProperty("Ice.ACM.Client", "0");
        return initData;
    }

    public static void main(String[] args)
    {
        Client app = new Client();
        int result = app.main("Client", args);
        System.gc();
        System.exit(result);
    }
}
