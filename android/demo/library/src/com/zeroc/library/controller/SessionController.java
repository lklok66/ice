package com.zeroc.library.controller;

import android.os.Handler;

public class SessionController
{
    public interface SessionListener
    {
        void onDestroy();
    }

    private Handler _handler;
    private Ice.Communicator _communicator;
    private QueryController _query;
    private SessionListener _sessionListener;
    private boolean _fatal = false;
    private boolean _destroyed = false;

    private SessionAdapter _session;
    
    class SessionRefreshThread extends Thread
    {
        SessionRefreshThread(long timeout)
        {
            _timeout = timeout; // seconds.
        }

        synchronized public void
        run()
        {
            while(!_terminated)
            {
                // check idle.
                try
                {
                    wait(_timeout);
                }
                catch(InterruptedException e)
                {
                }
                if(!_terminated)
                {
                    try
                    {
                        _session.refresh();
                    }
                    catch(Ice.LocalException ex)
                    {
                        postSessionDestroyed();
                        _terminated = true;
                    }
                }
            }
        }

        synchronized private void
        terminate()
        {
            _terminated = true;
            notify();
        }

        final private long _timeout;
        private boolean _terminated = false;
    }
    private SessionRefreshThread _refresh;
    
    synchronized private void postSessionDestroyed()
    {
        _fatal = true;
        if(_sessionListener != null)
        {
            final SessionListener listener = _sessionListener;
            _handler.post(new Runnable()
            {
                public void run()
                {
                    listener.onDestroy();
                }
            });
        }
    }

    SessionController(Handler handler, Ice.Communicator communicator, SessionAdapter session, long refreshTimeout)
    {
        _communicator = communicator;
        _session = session;
        _handler = handler;

        _refresh = new SessionRefreshThread(refreshTimeout);
        _refresh.start();

        _query = new QueryController(_handler, _session.getLibrary());
    }

    synchronized public void destroy()
    {
        if(_destroyed)
        {
            return;
        }
        _destroyed = true;

        final SessionRefreshThread refresh = _refresh;
        final Ice.Communicator communicator = _communicator;
        final SessionAdapter session = _session;
        _refresh = null;
        _session = null;
        _communicator = null;

        new Thread(new Runnable()
        {
            public void run()
            {
                refresh.terminate();
                while(refresh.isAlive())
                {
                    try
                    {
                        refresh.join();
                    }
                    catch(InterruptedException e)
                    {
                    }
                }

                try
                {
                    session.destroy();
                }
                catch(Exception e)
                {
                }
            
                try
                {
                    communicator.destroy();
                }
                catch(Exception e)
                {
                }
            }
        }).start();
    }
    
    synchronized public void setSessionListener(SessionListener listener)
    {
        _sessionListener = listener;
        if(_fatal)
        {
            listener.onDestroy();
        }
    }
    
    synchronized public QueryController createQuery(QueryController.QueryListener listener, QueryController.QueryType t, String q)
    {
        _query.destroy();
        _query = new QueryController(_handler, _session.getLibrary(), listener, t, q);
        return _query;
    }

    synchronized public QueryController getCurrentQuery()
    {
        return _query;
    }
}