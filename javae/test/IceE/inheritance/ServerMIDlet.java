// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

public class ServerMIDlet extends ServerBase
{
    public void
    updateProperties(Ice.Properties properties)
    {
        String s = "default -p " + _port.getString() + " -t " + _timeout.getString();
        properties.setProperty("TestAdapter.Endpoints", s);
    }
}

