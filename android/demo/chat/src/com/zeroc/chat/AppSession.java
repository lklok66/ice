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

import android.os.Handler;

class AppSession
{
    private Ice.Communicator _communicator;
    private Chat.ChatSessionPrx _session;
    private SessionRefresh _refresh;
    private List<ChatRoomListener> _listeners = new LinkedList<ChatRoomListener>();
    private LinkedList<ChatEventReplay> _replay = new LinkedList<ChatEventReplay>();
    private List<String> _users = new LinkedList<String>();

    static final int MAX_MESSAGES = 200;
    private static final int INACTIVITY_TIMEOUT = 5 * 60 * 1000; // 5 Minutes

    private Handler _handler;
    private boolean _destroyed = false;
    private long _lastSend;

    AppSession(Handler handler, IceSSL.CertificateVerifier verifier, String hostname, String username, String password, boolean secure)
        throws Glacier2.CannotCreateSessionException, Glacier2.PermissionDeniedException
    {
        _handler = handler;
        
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

        _refresh = new SessionRefresh((router.getSessionTimeout() * 1000) / 2);
        _refresh.start();
    }
    
    synchronized public void destroy()
    {
        if(_destroyed)
        {
            return;
        }

        _destroyed = true;
        
        System.out.println("AppSession: destroy");
        
        _refresh.destroy();
        while(Thread.currentThread() != _refresh && _refresh.isAlive())
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
            public void ice_exception(final Ice.LocalException ex)
            {
                localException(ex);
            }

            @Override
            public void ice_exception(final Ice.UserException ex)
            {
                localException(ex);
            }

            @Override
            public void ice_response(long ret)
            {
            }
        };
        _session.send_async(cb, t);
    }

    // This method is only called by the UI thread.
    synchronized boolean addChatRoomListener(ChatRoomListener cb, boolean replay)
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
    synchronized public void removeChatRoomListener(ChatRoomListener cb)
    {
        _listeners.remove(cb);
    }

    private interface ChatEventReplay
    {
        public void replay(ChatRoomListener cb);
    }

    private class ChatCallbackI extends Chat._ChatRoomCallbackDisp
    {
        synchronized public void init(String[] users, Ice.Current current)
        {
            final List<String> u = Arrays.asList(users);
            _users.clear();
            _users.addAll(u);
     
            // No replay event for init.
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                final ChatRoomListener l = p.next();
                _handler.post(new Runnable()
                {
                    public void run()
                    {
                        l.init(u);
                    }
                });
            }
        }

        synchronized public void join(final long timestamp, final String name, Ice.Current current)
        {
            _replay.add(new ChatEventReplay()
            {
                public void replay(ChatRoomListener cb)
                {
                    cb.join(timestamp, name);
                }
            });
            if(_replay.size() > MAX_MESSAGES)
            {
                _replay.removeFirst();
            }
            
            _users.add(name);
            
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                final ChatRoomListener l = p.next();
                _handler.post(new Runnable()
                {
                    public void run()
                    {
                        l.join(timestamp, name);
                    }
                });
            }
        }

        synchronized public void leave(final long timestamp, final String name, Ice.Current current)
        {
            _replay.add(new ChatEventReplay()
            {
                public void replay(ChatRoomListener cb)
                {
                    cb.leave(timestamp, name);
                }
            });
            if(_replay.size() > MAX_MESSAGES)
            {
                _replay.removeFirst();
            }
            
            _users.remove(name);
            
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                final ChatRoomListener l = p.next();
                _handler.post(new Runnable()
                {
                    public void run()
                    {
                        l.leave(timestamp, name);
                    }
                });
            }
        }

        synchronized public void send(final long timestamp, final String name, final String message, Ice.Current current)
        {
            _replay.add(new ChatEventReplay()
            {
                public void replay(ChatRoomListener cb)
                {
                    cb.send(timestamp, name, message);
                }
            });
            if(_replay.size() > MAX_MESSAGES)
            {
                _replay.removeFirst();
            }
            
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                final ChatRoomListener l = p.next();
                _handler.post(new Runnable()
                {
                    public void run()
                    {
                        l.send(timestamp, name, message);
                    }
                });
            }
        }
    }

    private class SessionRefresh extends Thread
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

        public void run()
        {
            while(true)
            {
                synchronized(this)
                {
                    if(_destroy)
                    {
                        return;
                    }
                    try
                    {
                        // Note that this is, strictly speaking, incorrect. If the
                        // CPU goes to sleep the wait will potentially be called
                        // much later. The correct way to manage this is with the 
                        // AlarmManager.
                        wait(_timeout);
                    }
                    catch(InterruptedException e1)
                    {
                    }
                    if(_destroy)
                    {
                        return;
                    }
                }
                refresh();
            }
        }
        
        private long _timeout;
        private boolean _destroy = false;
    }
    
    synchronized private void refresh()
    {
        if(_destroyed)
        {
            return;
        }
        
        // If the user has not sent a message in the INACTIVITY_TIMEOUT interval
        // then drop the session.
        if(System.currentTimeMillis() - _lastSend > INACTIVITY_TIMEOUT)
        {
            destroy();
            for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
            {
                final ChatRoomListener l = p.next();
                _handler.post(new Runnable()
                {
                    public void run()
                    {
                        l.inactivity();
                    }
                });
            }
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
    
    // Any exception destroys the session.
    synchronized private void localException(final Ice.LocalException ex)
    {
        destroy();
        
        for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
        {
            final ChatRoomListener l = p.next();
            _handler.post(new Runnable()
            {
                public void run()
                {
                    l.exception(ex);
                }
            });
        }
    }
    
    synchronized private void localException(final Ice.UserException ex)
    {
        destroy();
        
        for(Iterator<ChatRoomListener> p = _listeners.iterator(); p.hasNext();)
        {
            final ChatRoomListener l = p.next();
            _handler.post(new Runnable()
            {
                public void run()
                {
                    l.exception(ex);
                }
            });
        }
    }
}