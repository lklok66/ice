package com.zeroc.chat.service;

import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.SystemClock;

import com.zeroc.chat.ChatActivity;
import com.zeroc.chat.R;

public class ChatService extends Service implements com.zeroc.chat.service.Service
{
    private static final int CHATACTIVE_NOTIFICATION = 0;
    private static final String REFRESH_EXTRA = "refresh";
    private AppSession _session = null;
    private SessionListenerEvent _pendingEvent;
    private CertificateVerifier _verifier;
    private boolean _confirmConnectionResult = false;
    private boolean _confirmConnection = false;
    private SessionListener _listener;
    private boolean _loginInProgress;
    private Handler _handler;

    public class LocalBinder extends Binder
    {
        public com.zeroc.chat.service.Service getService()
        {
            return ChatService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return new LocalBinder();
    }

    @Override
    public void onCreate()
    {
        super.onCreate();
        _handler = new Handler();

        //
        // Read the CA certificate embedded in the Jar file.
        //
        try
        {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            Certificate caCert = cf.generateCertificate(getResources().openRawResource(R.raw.zeroc_ca_cert));
            _verifier = new CertificateVerifier(caCert);
        }
        catch(CertificateException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        logout();
    }

    @Override
    synchronized public void onStart(Intent intent, int startId)
    {
        // Find out if we were started by the alarm manager
        // to refresh the current session.
        if(intent.hasExtra(REFRESH_EXTRA))
        {
            // If there is no associated session, or the refresh failed then
            // mark the session as destroyed.
            if(_session == null || !_session.refresh())
            {
                sessionDestroyed();
                _session = null;
            }
        }
    }

    // Called only from the UI thread.
    synchronized public boolean setSessionListener(SessionListener cb)
    {
        _listener = cb;
        if(_pendingEvent != null)
        {
            _pendingEvent.replay(_listener);
            _pendingEvent = null;
        }
        return _loginInProgress;
    }

    synchronized public void confirmConnection(boolean confirm)
    {
        _confirmConnectionResult = true;
        _confirmConnection = confirm;
        notify();
    }

    synchronized public void logout()
    {
        if(_session != null)
        {
            _session.destroy();
            _session = null;

            sessionDestroyed();
        }
    }

    synchronized public void login(final String hostname, final String username, final String password,
                                   final boolean secure)
    {
        assert _session == null;
        assert !_loginInProgress;

        _loginInProgress = true;
        _confirmConnectionResult = false;

        new Thread(new Runnable()
        {
            public void run()
            {
                try
                {
                    loginComplete(new AppSession(_handler, _verifier, hostname, username, password, secure), hostname);
                }
                catch(final Glacier2.CannotCreateSessionException ex)
                {
                    setPendingEvent(new SessionListenerEvent()
                    {
                        public void replay(SessionListener l)
                        {
                            l.onException(String.format("Session creation failed: %s", ex.reason));
                        }
                    });
                }
                catch(final Glacier2.PermissionDeniedException ex)
                {
                    setPendingEvent(new SessionListenerEvent()
                    {
                        public void replay(SessionListener l)
                        {
                            l.onException(String.format("Login failed: %s", ex.reason));
                        }
                    });
                }
                catch(final Ice.LocalException ex)
                {
                    setPendingEvent(new SessionListenerEvent()
                    {
                        public void replay(SessionListener l)
                        {
                            l.onException(String.format("Login failed: %s", ex.toString()));
                        }
                    });
                }
                finally
                {
                    synchronized(ChatService.this)
                    {
                        _loginInProgress = false;
                    }
                }
            }
        }).start();
    }

    synchronized public String addChatRoomListener(ChatRoomListener listener, boolean replay) throws NoSessionException
    {
        if(_session == null)
        {
            throw new NoSessionException();
        }
        return _session.addChatRoomListener(listener, replay);
    }

    synchronized public void removeChatRoomListener(ChatRoomListener listener)
    {
        if(_session != null)
        {
            _session.removeChatRoomListener(listener);
        }
    }

    synchronized public void send(String message)
    {
        if(_session != null)
        {
            _session.send(message);
        }
    }

    private interface SessionListenerEvent
    {
        public void replay(SessionListener l);
    }

    private class CertificateVerifier implements IceSSL.CertificateVerifier
    {
        public CertificateVerifier(Certificate caCert)
        {
            _caCert = caCert;
        }

        public boolean verify(IceSSL.ConnectionInfo info)
        {
            try
            {
                if(info.certs != null && info.certs.length > 0)
                {
                    info.certs[0].verify(_caCert.getPublicKey());
                    return true;
                }
            }
            catch(Exception ex)
            {
                setPendingEvent(new SessionListenerEvent()
                {
                    public void replay(SessionListener l)
                    {
                        l.onConnectConfirm();
                    }
                });
                return waitConfirmConnection();
            }
            return false;
        }

        final private Certificate _caCert;
    };

    synchronized private boolean waitConfirmConnection()
    {
        while(!_confirmConnectionResult)
        {
            try
            {
                // We don't want to block the connection process forever.
                wait(10 * 1000);
            }
            catch(InterruptedException e)
            {
            }
        }
        return _confirmConnection;
    }

    synchronized private void setPendingEvent(final SessionListenerEvent pending)
    {
        if(_listener != null)
        {
            final SessionListener listener = _listener;
            _handler.post(new Runnable()
            {
                public void run()
                {
                    pending.replay(listener);    
                }
            });
        }
        else
        {
            _pendingEvent = pending;
        }
    }

    synchronized private void loginComplete(AppSession session, String hostname)
    {
        _session = session;

        // Set up an alarm to refresh the session.
        Intent intent = new Intent(ChatService.this, ChatService.class);
        intent.putExtra(REFRESH_EXTRA, true);
        PendingIntent sender = PendingIntent.getService(ChatService.this, 0, intent, 0);

        long refreshTimeout = _session.getRefreshTimeout();

        long firstTime = SystemClock.elapsedRealtime() + refreshTimeout;

        AlarmManager am = (AlarmManager)getSystemService(ALARM_SERVICE);
        am.setRepeating(AlarmManager.ELAPSED_REALTIME_WAKEUP, firstTime, refreshTimeout, sender);

        // Display a notification that the user is logged in.
        Notification notification = new Notification(R.drawable.stat_sample, "Logged In", System.currentTimeMillis());
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0, new Intent(this, ChatActivity.class), 0);
        notification.setLatestEventInfo(this, "Chat Demo", "You are logged into " + hostname, contentIntent);
        NotificationManager n = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
        n.notify(CHATACTIVE_NOTIFICATION, notification);

        setPendingEvent(new SessionListenerEvent()
        {
            public void replay(SessionListener l)
            {
                l.onLogin();
            }
        });
    }

    private void cancelRefreshTimer()
    {
        Intent intent = new Intent(ChatService.this, ChatService.class);
        intent.putExtra(REFRESH_EXTRA, true);
        PendingIntent sender = PendingIntent.getService(ChatService.this, 0, intent, 0);

        // And cancel the alarm.
        AlarmManager am = (AlarmManager)getSystemService(ALARM_SERVICE);
        am.cancel(sender);
    }

    private void sessionDestroyed()
    {
        cancelRefreshTimer();
        NotificationManager n = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
        // Cancel the notification -- we use the same ID that we had used to
        // start it
        n.cancel(CHATACTIVE_NOTIFICATION);
    }
}
