// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package com.zeroc.chat;

import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.UUID;

class AppSession
{
    AppSession(Ice.InitializationData initData, IceSSL.CertificateVerifier verifier, String hostname, String username, String password, boolean secure)
            throws Glacier2.CannotCreateSessionException, Glacier2.PermissionDeniedException
    {
        _communicator = Ice.Util.initialize(initData);
        IceSSL.Plugin plugin = (IceSSL.Plugin)_communicator.getPluginManager().getPlugin("IceSSL");
        plugin.setCertificateVerifier(verifier);

        String s;
        if(secure)
        {
            s = "Glacier2/router:ssl -p 4064 -h " + hostname + " -t 10000";
        }
        else
        {
            s = "Glacier2/router:tcp -p 4502 -h " + hostname + " -t 10000";
        }

        Ice.ObjectPrx proxy = _communicator.stringToProxy(s);
        Ice.RouterPrx r = Ice.RouterPrxHelper.uncheckedCast(proxy);

        _communicator.setDefaultRouter(r);

        Glacier2.RouterPrx router = Glacier2.RouterPrxHelper.checkedCast(r);
        Glacier2.SessionPrx glacier2session = router.createSession(username, password);
        _session = Chat.ChatSessionPrxHelper.uncheckedCast(glacier2session);

        _adapter = _communicator.createObjectAdapterWithRouter("ChatDemo.Client", router);

        _callbackId = new Ice.Identity();
        _callbackId.name = UUID.randomUUID().toString();
        _callbackId.category = router.getCategoryForClient();

        _adapter.activate();

        _refresh = new SessionRefresh((router.getSessionTimeout() * 1000) / 2);
        _refresh.start();
    }
    
    public void init()
    {
        Ice.ObjectPrx cb = _adapter.add(new ChatCallbackI(), _callbackId);
        _session.setCallback(Chat.ChatRoomCallbackPrxHelper.uncheckedCast(cb));
    }

    public void destroy()
    {
        _refresh.destroy();
        while(_refresh.isAlive())
        {
            try
            {
                _refresh.join();
            }
            catch(InterruptedException e1)
            {
            }
        }
        _refresh = null;

        _session.destroy_async(new Glacier2.AMI_Session_destroy()
        {
            @Override
            public void ice_exception(Ice.LocalException ex)
            {
            }

            @Override
            public void ice_response()
            {
            }
        });
        _session = null;
        
        try
        {
            _communicator.destroy();
        }
        catch(Ice.LocalException e)
        {
        }
        _communicator = null;
    }

    public Chat.ChatSessionPrx getSession()
    {
        return _session;
    }

    synchronized void addChatRoomListener(ChatRoomListener cb)
    {
        _listeners.add(cb);
        cb.init(_users);
    }

    synchronized public void removeChatRoomListener(ChatRoomListener cb)
    {
        _listeners.remove(cb);
    }

    synchronized private void localException(Ice.LocalException ex)
    {
        for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
        {
            p.next().exception(ex);
        }
    }

    class ChatCallbackI extends Chat._ChatRoomCallbackDisp
    {
        synchronized public void init(String[] users, Ice.Current current)
        {
            _users.clear();
            _users.addAll(Arrays.asList(users));
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                p.next().init(_users);
            }
        }

        synchronized public void join(long timestamp, String name, Ice.Current current)
        {
            _users.add(name);
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                p.next().join(timestamp, name);
            }
        }

        synchronized public void leave(long timestamp, String name, Ice.Current current)
        {
            _users.remove(name);
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                p.next().leave(timestamp, name);
            }
        }

        synchronized public void send(long timestamp, String name, String message, Ice.Current current)
        {
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                p.next().send(timestamp, name, message);
            }
        }
    }

    class SessionRefresh extends Thread
    {
        SessionRefresh(long l)
        {
            _timeout = l;
        }

        synchronized public void destroy()
        {
            _destroy = true;
            notify();
        }

        synchronized public void run()
        {
            while(true)
            {
                try
                {
                    wait(_timeout);
                }
                catch(InterruptedException e1)
                {
                }
                
                if(_destroy)
                {
                    return;
                }
                
                try
                {
                    _session.ice_ping();
                }
                catch(Ice.LocalException e)
                {
                    localException(e);
                    return;
                }
            }
        }
        
        private long _timeout;
        private boolean _destroy = false;
    }

    private Ice.Communicator _communicator;
    private Ice.ObjectAdapter _adapter;
    private Chat.ChatSessionPrx _session;
    private SessionRefresh _refresh;
    private List<ChatRoomListener> _listeners = new LinkedList<ChatRoomListener>();
    private List<String> _users = new java.util.ArrayList<String>();
    private Ice.Identity _callbackId;
}
