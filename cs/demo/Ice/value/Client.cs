// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Diagnostics;
using Demo;

public class Client
{
    private static int run(string[] args, Ice.Communicator communicator)
    {
        Ice.Properties properties = communicator.getProperties();
        string refProperty = "Value.Initial";
        string @ref = properties.getProperty(refProperty);
        if(@ref.Length == 0)
        {
            Console.Error.WriteLine("property `" + refProperty + "' not set");
            return 1;
        }
        
        Ice.ObjectPrx @base = communicator.stringToProxy(@ref);
        InitialPrx initial = InitialPrxHelper.checkedCast(@base);
        if(initial == null)
        {
            Console.Error.WriteLine("invalid object reference");
            return 1;
        }
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Let's first transfer a simple object, for a class without");
        Console.Out.WriteLine("operations, and print its contents. No factory is required");
        Console.Out.WriteLine("for this.");
        Console.Out.WriteLine("[press enter]");
	Console.In.ReadLine();
        
        Simple simple = initial.getSimple();
        Console.Out.WriteLine("==> " + simple.message);
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Ok, this worked. Now let's try to transfer an object for a class");
        Console.Out.WriteLine("with operations as type Ice.Object. Because no factory is installed,");
        Console.Out.WriteLine("the class will be sliced to Ice.Object.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        Ice.Object obj = initial.getPrinterAsObject();
        Console.Out.WriteLine("The type ID of the received object is \"" + obj.ice_id() + "\"");
        Debug.Assert(obj.ice_id().Equals("::Ice::Object"));
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Yes, this worked. Now let's try to transfer an object for a class");
        Console.Out.WriteLine("with operations as type Demo.Printer, without installing a factory first.");
        Console.Out.WriteLine("This should give us a `no factory' exception.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        Printer printer;
        PrinterPrx printerProxy;
        bool gotException = false;
        try
        {
            initial.getPrinter(out printer, out printerProxy);
        }
        catch(Ice.NoObjectFactoryException ex)
        {
            Console.Out.WriteLine("==> " + ex);
            gotException = true;
        }
        Debug.Assert(gotException);
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Yep, that's what we expected. Now let's try again, but with");
        Console.Out.WriteLine("installing an appropriate factory first. If successful, we print");
        Console.Out.WriteLine("the object's content.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        Ice.ObjectFactory factory = new ObjectFactory();
        communicator.addObjectFactory(factory, "::Demo::Printer");
        
        initial.getPrinter(out printer, out printerProxy);
        Console.Out.WriteLine("==> " + printer.message);
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Cool, it worked! Let's try calling the printBackwards() method");
        Console.Out.WriteLine("on the object we just received locally.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        Console.Out.Write("==> ");
        printer.printBackwards();
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Now we call the same method, but on the remote object. Watch the");
        Console.Out.WriteLine("server's output.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        printerProxy.printBackwards();
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Next, we transfer a derived object from the server as a base");
        Console.Out.WriteLine("object. Since we haven't yet installed a factory for the derived");
        Console.Out.WriteLine("class, the derived class (Demo.DerivedPrinter) is sliced");
        Console.Out.WriteLine("to its base class (Demo.Printer).");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        Printer derivedAsBase = initial.getDerivedPrinter();
        Console.Out.WriteLine("The type ID of the received object is \"" + derivedAsBase.ice_id() + "\"");
        Debug.Assert(derivedAsBase.ice_id().Equals("::Demo::Printer"));
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Now we install a factory for the derived class, and try again.");
        Console.Out.WriteLine("Because we receive the derived object as a base object,");
        Console.Out.WriteLine("we need to do a class cast to get from the base to the derived object.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        communicator.addObjectFactory(factory, "::Demo::DerivedPrinter");
        
        derivedAsBase = initial.getDerivedPrinter();
        DerivedPrinter derived = (DerivedPrinter)derivedAsBase;
        
        Console.Out.WriteLine("==> class cast to derived object succeded");
        Console.Out.WriteLine("The type ID of the received object is \"" + derived.ice_id() + "\"");
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Let's print the message contained in the derived object, and");
        Console.Out.WriteLine("call the operation printUppercase() on the derived object");
        Console.Out.WriteLine("locally.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        Console.Out.WriteLine("==> " + derived.derivedMessage);
        Console.Out.Write("==> ");
        derived.printUppercase();
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("Finally, we try the same again, but instead of returning the");
        Console.Out.WriteLine("derived object, we throw an exception containing the derived");
        Console.Out.WriteLine("object.");
        Console.Out.WriteLine("[press enter]");
        Console.In.ReadLine();
        
        try
        {
            initial.throwDerivedPrinter();
        }
        catch(DerivedPrinterException ex)
        {
            derived = ex.derived;
            Debug.Assert(derived != null);
        }
        
        Console.Out.WriteLine("==> " + derived.derivedMessage);
        Console.Out.Write("==> ");
        derived.printUppercase();
        
        Console.Out.WriteLine();
        Console.Out.WriteLine("That's it for this demo. Have fun with Ice!");
        
        return 0;
    }
    
    public static void Main(string[] args)
    {
        int status = 0;
        Ice.Communicator communicator = null;
        
        try
        {
            Ice.Properties properties = Ice.Util.createProperties();
            properties.load("config");
            communicator = Ice.Util.initializeWithProperties(ref args, properties);
            status = run(args, communicator);
        }
        catch(System.Exception ex)
        {
            Console.Error.WriteLine(ex);
            status = 1;
        }
        
	if(communicator != null)
	{
	    try
	    {
		communicator.destroy();
	    }
	    catch(System.Exception ex)
	    {
		Console.Error.WriteLine(ex);
		status = 1;
	    }
	}
        
        System.Environment.Exit(status);
    }
}
