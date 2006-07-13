// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using Demo;

public class Client : Ice.Application
{
    private static void menu()
    {
        Console.WriteLine("usage:\n"
                           + "\n"
                           + "toggle type of data to send:\n"
                           + "1: sequence of bytes (default)\n"
                           + "2: sequence of strings (\"hello\")\n"
                           + "3: sequence of structs with a string (\"hello\") and a double\n"
                           + "4: sequence of structs with two ints and a double\n"
                           + "\n"
                           + "select test to run:\n"
                           + "t: Send sequence as twoway\n"
                           + "o: Send sequence as oneway\n" 
                           + "r: Receive sequence\n"
			   + "e: Echo (send and receive) sequence\n"
                           + "\n"
                           + "other commands\n"
                           + "s: shutdown server\n"
			   + "x: exit\n"
			   + "?: help\n");
    }
    
    public override int
    run(string[] args)
    {
        Ice.Properties properties = communicator().getProperties();
        string refProperty = "Throughput.Throughput";
        string r = properties.getProperty(refProperty);
        if(r.Length == 0)
        {
            Console.Error.WriteLine("property `" + r + "' not set");
            return 1;
        }
        
        Ice.ObjectPrx b = communicator().stringToProxy(r);
        ThroughputPrx throughput = ThroughputPrxHelper.checkedCast(b);
        if(throughput == null)
        {
            Console.Error.WriteLine("invalid proxy");
            return 1;
        }
        ThroughputPrx throughputOneway = ThroughputPrxHelper.uncheckedCast(throughput.ice_oneway());

        byte[] byteSeq = new byte[ByteSeqSize.value];

        string[] stringSeq = new string[StringSeqSize.value];
	for(int i = 0; i < StringSeqSize.value; ++i)
	{
	    stringSeq[i] = "hello";
	}

        StringDouble[] structSeq = new StringDouble[StringDoubleSeqSize.value];
        for(int i = 0; i < StringDoubleSeqSize.value; ++i)
        {
	    structSeq[i] = new StringDouble();
            structSeq[i].s = "hello";
            structSeq[i].d = 3.14;
        }

	Fixed[] fixedSeq = new Fixed[FixedSeqSize.value];
	for(int i = 0; i < FixedSeqSize.value; ++i)
	{
	    fixedSeq[i].i = 0;
	    fixedSeq[i].j = 0;
	    fixedSeq[i].d = 0;
	}
        
        menu();
        
        throughput.ice_ping(); // Initial ping to setup the connection.

        //
        // By default use byte sequence.
        //
        char currentType = '1';
        int seqSize = ByteSeqSize.value;

        string line = null;
        do 
        {
            try
            {
                Console.Write("==> ");
                Console.Out.Flush();
                line = Console.In.ReadLine();
		if(line == null)
		{
		    break;
		}

                long tmsec = System.DateTime.Now.Ticks / 10000;
                const int repetitions = 100;

                if(line.Equals("1") || line.Equals("2") || line.Equals("3") || line.Equals("4"))
                {
                    currentType = line[0];
                    switch(currentType)
                    {
                        case '1':
                        {
                            Console.WriteLine("using byte sequences");
                            seqSize = ByteSeqSize.value;
                            break;
                        }
                        case '2':
                        {
                            Console.WriteLine("using string sequences");
                            seqSize = StringSeqSize.value;
                            break;
                        }
                        case '3':
                        {
                            Console.WriteLine("using variable-length struct sequences");
                            seqSize = StringDoubleSeqSize.value;
                            break;
                        }
                        case '4':
                        {
                            Console.WriteLine("using fixed-length struct sequences");
                            seqSize = FixedSeqSize.value;
                            break;
                        }
                    } 
                }
                else if(line.Equals("t") || line.Equals("o") || line.Equals("r") || line.Equals("e"))
                {
                    char c = line[0];
                    switch (c)
                    {                       
                        case 't': 
                        {
                            Console.Write("sending");
                            break;
                        }                                            
                        case 'o': 
                        {
                            Console.Write("sending");
                            break;
                        }                                            
                        case 'r': 
                        {
                            Console.Write("receiving");
                            break;
                        }                              
                        case 'e': 
                        {
                            Console.Write("sending and receiving");
                            break;
                        }
                    }
                
                    Console.Write(" " + repetitions);
                    switch(currentType)
                    {
                        case '1':
                        {
                            Console.Write(" byte");
                            break;
                        }
                        case '2':
                        {
                            Console.Write(" string");
                            break;
                        }
                        case '3':
                        {
                            Console.Write(" variable-length struct");
                            break;
                        }
                        case '4':
                        {
                            Console.Write(" fixed-length struct");
                            break;
                        }
                    }
                    Console.Write(" sequences of size " + seqSize);

                    if(c == 'o')
                    {
                        Console.Write(" as oneway");
                    }
        
                    Console.WriteLine("...");
        
                    for (int i = 0; i < repetitions; ++i)
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
        
                    double dmsec = System.DateTime.Now.Ticks / 10000 - tmsec;
                    Console.WriteLine("time for " + repetitions + " sequences: " + dmsec.ToString("F") + "ms");
                    Console.WriteLine("time per sequence: " + ((double)(dmsec / repetitions)).ToString("F") + "ms");
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
                            wireSize = stringSeq[0].Length;
                            break;
                        }
                        case '3':
                        {
                            wireSize = structSeq[0].s.Length;
                            wireSize += 8; // Size of double on the wire.
                            break;
                        }
                        case '4':
                        {
                            wireSize = 16; // Sizes of two ints and a double on the wire.
                            break;
                        }
                    }
		    double mbit = repetitions * seqSize * wireSize * 8.0 / dmsec / 1000.0;
		    if(c == 'e')
		    {
			mbit *= 2;
		    }
		    Console.WriteLine("throughput: " + mbit.ToString("#.##") + "Mbps");
		}
		else if(line.Equals("s"))
		{
		    throughput.shutdown();
		}
		else if(line.Equals("x"))
		{
		    // Nothing to do
		}
		else if(line.Equals("?"))
		{
		    menu();
		}
		else
		{
		    Console.WriteLine("unknown command `" + line + "'");
		    menu();
		}
	    }
            catch(System.Exception ex)
            {
                Console.Error.WriteLine(ex);
            }
        }
        while(!line.Equals("x"));
        
        return 0;
    }

    public static void Main(string[] args)
    {
        Client app = new Client();
        int status = app.main(args, "config.client");
        if(status != 0)
        {
            System.Environment.Exit(status);
        }
    }
}
