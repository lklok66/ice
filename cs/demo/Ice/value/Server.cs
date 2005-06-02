// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

public class Server : Ice.Application
{
    public override int run(string[] args)
    {
        Ice.ObjectAdapter adapter = communicator().createObjectAdapter("Value");
        Ice.Object @object = new InitialI(adapter);
        adapter.add(@object, Ice.Util.stringToIdentity("initial"));
        adapter.activate();
        communicator().waitForShutdown();
        return 0;
    }
    
    public static void Main(string[] args)
    {
        Server app = new Server();
        int status = app.main(args, "config");
        System.Environment.Exit(status);
    }
}
