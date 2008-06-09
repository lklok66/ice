// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

import Test.*;

public class TestI extends _TestIntfDisp
{
    TestI(Ice.ObjectAdapter adapter1, Ice.ObjectAdapter adapter2, ServerLocatorRegistry registry)
    {
        _adapter1 = adapter1;
        _adapter2 = adapter2;
        _registry = registry;

        _registry.addObject(_adapter1.add(new HelloI(), _adapter1.getCommunicator().stringToIdentity("hello")));
    }

    public void
    shutdown(Ice.Current current)
    {
        _adapter1.getCommunicator().shutdown();
    }

    public HelloPrx
    getHello(Ice.Current current)
    {
        return HelloPrxHelper.uncheckedCast(_adapter1.createIndirectProxy(_adapter1.getCommunicator().stringToIdentity("hello")));
    }

    public HelloPrx
    getReplicatedHello(Ice.Current current)
    {
        return HelloPrxHelper.uncheckedCast(_adapter1.createProxy(_adapter1.getCommunicator().stringToIdentity("hello")));
    }

    public void
    migrateHello(Ice.Current current)
    {
        final Ice.Identity id = _adapter1.getCommunicator().stringToIdentity("hello");
        try
        {
            _registry.addObject(_adapter2.add(_adapter1.remove(id), id));
        }
        catch(Ice.NotRegisteredException ex)
        {
            _registry.addObject(_adapter1.add(_adapter2.remove(id), id));
        }
    }

    private ServerLocatorRegistry _registry;
    private Ice.ObjectAdapter _adapter1;
    private Ice.ObjectAdapter _adapter2;
}
