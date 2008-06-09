// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

import Test.*;

public class ServerManagerI extends _ServerManagerDisp
{
    ServerManagerI(Ice.ObjectAdapter adapter, ServerLocatorRegistry registry,
        Ice.InitializationData initData)
    {
        _adapter = adapter;
        _registry = registry;
        _communicators = new java.util.Vector();
        _initData = initData;
        
        _initData.properties.setProperty("TestAdapter2.Endpoints", "default");
        _initData.properties.setProperty("TestAdapter2.AdapterId", "TestAdapter2");
        _initData.properties.setProperty("TestAdapter.Endpoints", "default");
        _initData.properties.setProperty("TestAdapter.AdapterId", "TestAdapter");
        _initData.properties.setProperty("TestAdapter.ReplicaGroupId", "ReplicatedAdapter");
    }

    public void
    startServer(Ice.Current current)
    {
        java.util.Enumeration e = _communicators.elements();
        while(e.hasMoreElements())
        {
            Ice.Communicator c = (Ice.Communicator)e.nextElement();
            c.waitForShutdown();
            c.destroy();
        }
        _communicators.removeAllElements();

        //
        // Simulate a server: create a new communicator and object
        // adapter. The object adapter is started on a system allocated
        // port. The configuration used here contains the Ice.Locator
        // configuration variable. The new object adapter will register
        // its endpoints with the locator and create references
        // containing the adapter id instead of the endpoints.
        //
        Ice.Communicator serverCommunicator = Ice.Util.initialize(_initData);
        _communicators.addElement(serverCommunicator);

        Ice.ObjectAdapter adapter = serverCommunicator.createObjectAdapter("TestAdapter");
        Ice.ObjectAdapter adapter2 = serverCommunicator.createObjectAdapter("TestAdapter2");

        Ice.ObjectPrx locator = serverCommunicator.stringToProxy("locator:default -p 12010 -t 30000");
        adapter.setLocator(Ice.LocatorPrxHelper.uncheckedCast(locator));
        adapter2.setLocator(Ice.LocatorPrxHelper.uncheckedCast(locator));

        Ice.Object object = new TestI(adapter, adapter2, _registry);
        _registry.addObject(adapter.add(object, serverCommunicator.stringToIdentity("test")));
        _registry.addObject(adapter.add(object, serverCommunicator.stringToIdentity("test2")));

        adapter.activate();
        adapter2.activate();
    }

    public void
    shutdown(Ice.Current current)
    {
        java.util.Enumeration e = _communicators.elements();
        while(e.hasMoreElements())
        {
            ((Ice.Communicator)e.nextElement()).destroy();
        }
        _adapter.getCommunicator().shutdown();
    }

    private Ice.ObjectAdapter _adapter;
    private ServerLocatorRegistry _registry;
    private java.util.Vector _communicators;
    private Ice.InitializationData _initData;
}
