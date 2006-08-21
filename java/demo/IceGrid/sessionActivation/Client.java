// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

import Demo.*;

public class Client extends Ice.Application
{
    static private class SessionKeepAliveThread extends Thread
    {
        SessionKeepAliveThread(IceGrid.SessionPrx session, long timeout)
	{
	    _session = session;
	    _timeout = timeout;
	    _terminated = false;
	}

	synchronized public void
	run()
	{
            while(!_terminated)
            {
                try
                {
                    wait(_timeout);
                }
                catch(InterruptedException e)
                {
                }
                if(_terminated)
                {
		    break;
		}
                try
                {
                    _session.keepAlive();
                }
                catch(Ice.LocalException ex)
                {
		    break;
                }
            }
	}

	synchronized private void
	terminate()
	{
	    _terminated = true;
	    notify();
	}

	final private IceGrid.SessionPrx _session;
	final private long _timeout;
	private boolean _terminated;
    }

    private void
    menu()
    {
        System.out.println(
            "usage:\n" +
            "t: send greeting\n" +
            "x: exit\n" +
            "?: help\n");
    }

    public int
    run(String[] args)
    {
	int status = 0;
        IceGrid.RegistryPrx registry = 
	    IceGrid.RegistryPrxHelper.checkedCast(communicator().stringToProxy("DemoIceGrid/Registry"));
	if(registry == null)
	{
            System.err.println("could not contact registry");
	    return 1;
	}

        java.io.BufferedReader in = new java.io.BufferedReader(new java.io.InputStreamReader(System.in));
	IceGrid.SessionPrx session;
	while(true)
	{
	    System.out.println("This demo accepts any user-id / password combination.");

	    try
	    {
		String id;
		System.out.print("user id: ");
		System.out.flush();
		id = in.readLine();
		
		String pw;
		System.out.print("password: ");
		System.out.flush();
		pw = in.readLine();
		
		try
		{
		    session = registry.createSession(id, pw);
		    break;
		}
		catch(IceGrid.PermissionDeniedException ex)
		{
		    System.out.println("permission denied:\n" + ex.reason);
		}
	    }
            catch(java.io.IOException ex)
            {
                ex.printStackTrace();
            }
	}

	SessionKeepAliveThread keepAlive = new SessionKeepAliveThread(session, registry.getSessionTimeout() / 2);
	keepAlive.start();

	try
	{
	    HelloPrx hello = HelloPrxHelper.checkedCast(
		session.allocateObjectById(communicator().stringToIdentity("hello")));

	    menu();
	    
	    String line = null;
	    do
	    {
		try
		{
		    System.out.print("==> ");
		    System.out.flush();
		    line = in.readLine();
		    if(line == null)
		    {
			break;
		    }
		    if(line.equals("t"))
		    {
			hello.sayHello();
		    }
		    else if(line.equals("x"))
		    {
			// Nothing to do
		    }
		    else if(line.equals("?"))
		    {
			menu();
		    }
		    else
		    {
			System.out.println("unknown command `" + line + "'");
			menu();
		    }
		}
		catch(java.io.IOException ex)
		{
		    ex.printStackTrace();
		}
		catch(Ice.LocalException ex)
		{
		    ex.printStackTrace();
		}
	    }
	    while(!line.equals("x"));
	}
	catch(IceGrid.AllocationException ex)
	{
	    System.err.println("could not allocate object: " + ex.reason);
	    return 1;
	}
	catch(IceGrid.ObjectNotRegisteredException ex)
	{
	    System.err.println("object not registered with registry");
	    return 1;
	}
	catch(Exception ex)
	{
	    System.err.println("expected exception: " + ex);
	    status = 1;
	}

	keepAlive.terminate();
	try
	{
	    keepAlive.join();
	}
	catch(InterruptedException e)
	{
	}
	session.destroy();

        return status;
    }

    public static void
    main(String[] args)
    {
	Client app = new Client();
	int status = app.main("Client", args, "config.client");
	System.exit(status);
    }
}
