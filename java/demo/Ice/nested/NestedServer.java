// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

import Demo.*;

class NestedServer extends Ice.Application
{
    public int
    run(String[] args)
    {
        Ice.ObjectAdapter adapter = communicator().createObjectAdapter("Nested.Server");
        NestedPrx self = NestedPrxHelper.uncheckedCast(adapter.createProxy(communicator().stringToIdentity("nestedServer")));
        adapter.add(new NestedI(self), communicator().stringToIdentity("nestedServer"));
        adapter.activate();
        communicator().waitForShutdown();
        return 0;
    }
}
