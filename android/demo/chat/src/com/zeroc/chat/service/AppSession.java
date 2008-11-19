// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package com.zeroc.chat.service;

import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.UUID;

public class AppSession
{
    private Ice.Communicator _communicator;
    private Chat.ChatSessionPrx _session;
    private List<Chat.Android.ChatRoomListenerPrx> _listeners = new LinkedList<Chat.Android.ChatRoomListenerPrx>();
    private LinkedList<ChatEventReplay> _replay = new LinkedList<ChatEventReplay>();
    private List<String> _users = new LinkedList<String>();

    static final int MAX_MESSAGES = 200;
    private static final int INACTIVITY_TIMEOUT = 5 * 60 * 1000; // 5 Minutes

    private boolean _destroyed = false;
    private long _lastSend;
    private long _refreshTimeout;

    public AppSession(IceSSL.CertificateVerifier verifier, String hostname, String username, String password,
            boolean secure) throws Glacier2.CannotCreateSessionException, Glacier2.PermissionDeniedException
    {
        Ice.InitializationData initData = new Ice.InitializationData();

        initData.properties = Ice.Util.createProperties();
        initData.properties.setProperty("Ice.ACM.Client", "0");
        initData.properties.setProperty("Ice.RetryIntervals", "-1");
        initData.properties.setProperty("Ice.Trace.Network", "0");
        initData.properties.setProperty("Ice.Plugin.IceSSL", "IceSSL.PluginFactory");
        initData.properties.setProperty("IceSSL.TrustOnly.Client", "CN=Glacier2");

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

        Ice.ObjectAdapter adapter = _communicator.createObjectAdapterWithRouter("ChatDemo.Client", router);

        Ice.Identity callbackId = new Ice.Identity();
        callbackId.name = UUID.randomUUID().toString();
        callbackId.category = router.getCategoryForClient();

        Ice.ObjectPrx cb = adapter.add(new ChatCallbackI(), callbackId);
        _session.setCallback(Chat.ChatRoomCallbackPrxHelper.uncheckedCast(cb));

        adapter.activate();
        _lastSend = System.currentTimeMillis();

        _refreshTimeout = (router.getSessionTimeout() * 1000) / 2;
    }
    
    synchronized public long getRefreshTimeout()
    {
        return _refreshTimeout;
    }

    synchronized public void destroy()
    {
        if(_destroyed)
        {
            return;
        }

        _destroyed = true;

        System.out.println("AppSession: destroy");

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

    // This method is only called by the UI thread.
    synchronized public void send(String t)
    {
        if(_session == null)
        {
            return;
        }

        _lastSend = System.currentTimeMillis();
        Chat.AMI_ChatSession_send cb = new Chat.AMI_ChatSession_send()
        {
            @Override
            public void ice_exception(Ice.LocalException ex)
            {
                localException(ex.toString());
            }

            @Override
            public void ice_exception(Ice.UserException ex)
            {
                localException(ex.toString());
            }

            @Override
            public void ice_response(long ret)
            {
            }
        };
        _session.send_async(cb, t);
    }

    // This method is only called by the UI thread.
    public synchronized boolean addChatRoomListener(Chat.Android.ChatRoomListenerPrx cb, boolean replay)
    {
        if(_destroyed)
        {
            return false;
        }

        _listeners.add(cb);
        cb.init(_users);
        if(replay)
        {
            // Replay the entire state.
            for(Iterator<ChatEventReplay> p = _replay.iterator(); p.hasNext();)
            {
                p.next().replay(cb);
            }
        }
        return true;
    }

    // This method is only called by the UI thread.
    synchronized public void removeChatRoomListener(Chat.Android.ChatRoomListenerPrx cb)
    {
        _listeners.remove(cb);
    }
    
    // Returns false if the session has been destroyed, true otherwise.
    synchronized public boolean refresh()
    {
        if(_destroyed)
        {
            return false;
        }

        // If the user has not sent a message in the INACTIVITY_TIMEOUT interval
        // then drop the session.
        if(System.currentTimeMillis() - _lastSend > INACTIVITY_TIMEOUT)
        {
            destroy();
            for(Iterator<Chat.Android.ChatRoomListenerPrx> p = _listeners.iterator(); p.hasNext();)
            {
                Chat.Android.ChatRoomListenerPrx l = p.next();
                l.inactivity();
            }
            return false;
        }

        try
        {
            // TODO: This should be async.
            _session.ice_ping();
        }
        catch(Ice.LocalException e)
        {
            localException(e.toString());
            return false;
        }

        return true;
    }

    private interface ChatEventReplay
    {
        public void replay(Chat.Android.ChatRoomListenerPrx cb);
    }

    private class ChatCallbackI extends Chat._ChatRoomCallbackDisp
    {
        synchronized public void init(String[] users, Ice.Current current)
        {
            final List<String> u = Arrays.asList(users);
            _users.clear();
            _users.addAll(u);

            // No replay event for init.
            for(Iterator<Chat.Android.ChatRoomListenerPrx> p = _listeners.iterator(); p.hasNext();)
            {
                Chat.Android.ChatRoomListenerPrx l = p.next();
                l.init(u);
            }
        }

        synchronized public void join(final long timestamp, final String name, Ice.Current current)
        {
            _replay.add(new ChatEventReplay()
            {
                public void replay(Chat.Android.ChatRoomListenerPrx cb)
                {
                    cb.join(timestamp, name);
                }
            });
            if(_replay.size() > MAX_MESSAGES)
            {
                _replay.removeFirst();
            }

            _users.add(name);

            for(Iterator<Chat.Android.ChatRoomListenerPrx> p = _listeners.iterator(); p.hasNext();)
            {
                Chat.Android.ChatRoomListenerPrx l = p.next();
                l.join(timestamp, name);
            }
        }

        synchronized public void leave(final long timestamp, final String name, Ice.Current current)
        {
            _replay.add(new ChatEventReplay()
            {
                public void replay(Chat.Android.ChatRoomListenerPrx cb)
                {
                    cb.leave(timestamp, name);
                }
            });
            if(_replay.size() > MAX_MESSAGES)
            {
                _replay.removeFirst();
            }

            _users.remove(name);

            for(Iterator<Chat.Android.ChatRoomListenerPrx> p = _listeners.iterator(); p.hasNext();)
            {
                Chat.Android.ChatRoomListenerPrx l = p.next();
                l.leave(timestamp, name);
            }
        }

        synchronized public void send(final long timestamp, final String name, final String message, Ice.Current current)
        {
            _replay.add(new ChatEventReplay()
            {
                public void replay(Chat.Android.ChatRoomListenerPrx cb)
                {
                    cb.send(timestamp, name, message);
                }
            });
            if(_replay.size() > MAX_MESSAGES)
            {
                _replay.removeFirst();
            }

            for(Iterator<Chat.Android.ChatRoomListenerPrx> p = _listeners.iterator(); p.hasNext();)
            {
                Chat.Android.ChatRoomListenerPrx l = p.next();
                l.send(timestamp, name, message);
            }
        }
    }

    // Any exception destroys the session.
    synchronized private void localException(String msg)
    {
        destroy();

        for(Iterator<Chat.Android.ChatRoomListenerPrx> p = _listeners.iterator(); p.hasNext();)
        {
            Chat.Android.ChatRoomListenerPrx l = p.next();
            l.error(msg);
        }
    }
}
