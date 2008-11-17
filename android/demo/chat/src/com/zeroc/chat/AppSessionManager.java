package com.zeroc.chat;

import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;

import android.os.Handler;

public class AppSessionManager
{
    static private AppSessionManager _instance;
    private SessionListener _cb;
    private SessionListenerEvent _pendingEvent;
    private IceSSL.CertificateVerifier _verifier;
    private Handler _handler;
    private AppSession _session;
    private boolean _confirmConnectionResult = false;
    private boolean _confirmConnection = false;
    private boolean _loginInProgress = false;
    
    interface SessionListener
    {
        public void onLogin();

        public void onConnectConfirm();

        public void onException(Ice.UserException ex);

        public void onException(Ice.LocalException ex);
    }

    static public void init(java.io.InputStream cert)
        throws CertificateException
    {
        if(_instance == null)
        {
            _instance = new AppSessionManager(cert);
        }
    }

    static public AppSessionManager instance()
    {
        return _instance;
    }

    synchronized public void destroySession()
    {
        if(_session != null)
        {
            _session.destroy();
            _session = null;
        }
    }
    
    synchronized public boolean getLoginInProgress()
    {
        return _loginInProgress;
    }

    public boolean addChatRoomListener(ChatRoomListener listener, boolean replay)
    {
        if(_session == null)
        {
            return false;
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
    
    synchronized public void send(String t)
    {
        if(_session != null)
        {
            _session.send(t);
        }
    }

    synchronized public void setSessionListener(SessionListener cb)
    {
        _cb = cb;
        if(_pendingEvent != null)
        {
            _pendingEvent.replay(_cb);
            _pendingEvent = null;
        }
    }
    
    synchronized public void setConfirmConnection(boolean rc)
    {
        _confirmConnectionResult = true;
        _confirmConnection = rc;
        notify();
    }
    
    synchronized public void login(final String hostname, final String username, final String password, final boolean secure)
    {
        assert _session == null;
        assert !_loginInProgress;

        setLoginInProgress(true);
        _confirmConnectionResult = false;

        new Thread(new Runnable()
        {
            public void run()
            {
                try
                {
                    loginComplete(new AppSession(_handler, _verifier, hostname, username, password, secure));
                }
                catch(final Ice.UserException ex)
                {
                    setPendingEvent(new SessionListenerEvent()
                    {
                        public void replay(SessionListener l)
                        {
                            l.onException(ex);
                        }
                    });
                }
                catch(final Ice.LocalException ex)
                {
                    setPendingEvent(new SessionListenerEvent()
                    {
                        public void replay(SessionListener l)
                        {
                            l.onException(ex);
                        }
                    });
                }
                finally
                {
                    setLoginInProgress(false);
                }
            }
        }).start();
    }

    synchronized private void loginComplete(AppSession session)
    {
        _session = session;
        setPendingEvent(new SessionListenerEvent()
        {
            public void replay(SessionListener l)
            {
                l.onLogin();
            }
        });
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
                wait();
            }
            catch(InterruptedException e)
            {
            }
        }
        return _confirmConnection;
    }

    private AppSessionManager(java.io.InputStream cert)
        throws CertificateException
    {
        //
        // Read the CA certificate embedded in the Jar file.
        //
        CertificateFactory cf = CertificateFactory.getInstance("X.509");
        Certificate caCert = cf.generateCertificate(cert);
        _verifier = new CertificateVerifier(caCert);
        
        // This constructor must always be called in the ui thread.
        _handler = new Handler();
    }

    synchronized private void setPendingEvent(final SessionListenerEvent pending)
    {
        if(_cb != null)
        {
            _handler.post(new Runnable()
            {
                public void run()
                {
                    pending.replay(_cb);
                }
            });
        }
        else
        {
            _pendingEvent = pending;
        }
    }

    synchronized private void setLoginInProgress(boolean b)
    {
        _loginInProgress = b;
    }
}