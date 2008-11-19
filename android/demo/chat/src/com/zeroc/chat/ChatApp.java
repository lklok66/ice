package com.zeroc.chat;

import android.app.Application;

public class ChatApp extends Application
{
    private Ice.Communicator _communicator;
    private Ice.ObjectAdapter _adapter;

    /** Called when the application is starting, before any other application objects have been created. */
    @Override
    public void onCreate()
    {
        super.onCreate();
        Ice.InitializationData initData = new Ice.InitializationData();

        initData.properties = Ice.Util.createProperties();
        initData.properties.setProperty("Ice.Trace.Network", "0");

        _communicator = Ice.Util.initialize(initData);
        _adapter = _communicator.createObjectAdapterWithEndpoints("Session", "tcp -p 12223");
        _adapter.activate();
    }
    
    /** Called when the application is stopping. */
    @Override
    public void onTerminate()
    {
        super.onTerminate();
        if(_communicator != null)
        {
            try
            {
                _communicator.destroy();
            }
            catch(Ice.LocalException ex)
            {
            }
        }
    }
    
    public Ice.Communicator getCommunicator()
    {
        return _communicator;
    }
    
    protected Ice.ObjectAdapter getAdapter()
    {
        return _adapter;
    }
}
