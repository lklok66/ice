// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

public class Server
{
    public static void
    main(String[] args)
    {
        BackendServer app = new BackendServer();
        int status = app.main("Server", args);
	System.gc();
        System.exit(status);
    }
}
