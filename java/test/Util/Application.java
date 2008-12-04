// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Util;

import Ice.Communicator;
import Ice.InitializationData;
import Ice.LocalException;
import Ice.LoggerI;
import Ice.StringSeqHolder;
import Ice.Util;

public abstract class Application
{
    public
    Application()
    {
    }

    //
    // This main() must be called by the global main(). main()
    // initializes the Communicator, calls run(), and destroys
    // the Communicator upon return from run(). It thereby handles
    // all exceptions properly, i.e., error messages are printed
    // if exceptions propagate to main(), and the Communicator is
    // always destroyed, regardless of exceptions.
    //
    public final int
    main(String appName, String[] args)
    {
    	Ice.StringSeqHolder argsH = new Ice.StringSeqHolder(args);
    	Ice.InitializationData initData = getInitData(argsH);
        return main(appName, argsH.value, initData);
    }

    public final int
    main(String appName, String[] args, InitializationData initializationData)
    {
        java.io.PrintWriter writer = getWriter();
		if(_communicator != null)
        {
            writer.println(appName + ": only one instance of the Application class can be used");
            return 1;
        }

        _testName = appName;

        //
        // We parse the properties here to extract Ice.ProgramName.
        // 
        StringSeqHolder argHolder = new StringSeqHolder(args);
        if(initializationData == null)
        {
        	initializationData = getInitData(argHolder);
        }
        
        InitializationData initData;
        if(initializationData != null)
        {
            initData = (InitializationData)initializationData.clone();
        }
        else
        {
            initData = new InitializationData();
        }
        initData.properties = Util.createProperties(argHolder, initData.properties);

        //
        // If the process logger is the default logger, we replace it with a
        // a logger which is using the program name for the prefix.
        //
        if(Util.getProcessLogger() instanceof LoggerI)
        {
            Util.setProcessLogger(new LoggerI(initData.properties.getProperty("Ice.ProgramName")));
        }

        int status = 0;

        try
        {
            _communicator = Util.initialize(argHolder, initData);

            status = run(argHolder.value);
        }
        catch(LocalException ex)
        {
            writer.println(_testName + ": " + ex);
            ex.printStackTrace();
            status = 1;
        }
        catch(java.lang.Exception ex)
        {
            writer.println(_testName + ": unknown exception");
            ex.printStackTrace(writer);
            status = 1;
        }
        catch(java.lang.Error err)
        {
            //
            // We catch Error to avoid hangs in some non-fatal situations
            //
            writer.println(_testName + ": Java error");
            err.printStackTrace(writer);
            status = 1;
        }

        if(_communicator != null)
        {
            try
            {
                _communicator.destroy();
            }
            catch(LocalException ex)
            {
                writer.println(_testName + ": " + ex);
                ex.printStackTrace(writer);
                status = 1;
            }
            catch(java.lang.Exception ex)
            {
                writer.println(_testName + ": unknown exception");
                ex.printStackTrace(writer);
                status = 1;
            }
            _communicator = null;
        }
        writer.flush();

        return status;
    }

    public abstract int run(String[] args);

    // Hook to override the initialization data.
	protected Ice.InitializationData getInitData(Ice.StringSeqHolder argsH)
	{
		return null;
	}
	
	public java.io.PrintWriter getWriter()
	{
		return _printWriter;
	}
	
	public void setWriter(java.io.Writer writer)
	{
		_printWriter = new java.io.PrintWriter(writer);
	}

    //
    // Return the application name, i.e., argv[0].
    //
    public String
    appName()
    {
        return _testName;
    }

    public Communicator
    communicator()
    {
        return _communicator;
    }

    private String _testName;
    private Communicator _communicator;
	private java.io.PrintWriter _printWriter = new java.io.PrintWriter(new java.io.OutputStreamWriter(System.out));
}
