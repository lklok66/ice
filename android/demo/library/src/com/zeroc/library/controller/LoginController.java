package com.zeroc.library.controller;

import android.os.Handler;

public class LoginController
{
    public interface LoginListener
    {
        void loginInProgress();
        void onLogin();
        void onLoginError();
    };
    
    private boolean _destroyed = false;
    private Handler _handler;
    private Ice.Communicator _communicator;
    private String _loginError;
    private LoginListener _loginListener;
    private SessionController _sessionController;
    
    synchronized private void postLoginFailure(final String loginError)
    {
        _loginError = loginError;
        if(_loginListener != null)
        {
            final LoginListener listener = _loginListener;
            _handler.post(new Runnable()
            {
                public void run()
                {
                    listener.onLoginError();
                }
            });
        }
    }

    public LoginController(final String hostname, final boolean secure, final boolean glacier2, LoginListener listener)
    {
        _handler = new Handler();
        _loginListener = listener;
        _loginListener.loginInProgress();

        new Thread(new Runnable()
        {
            public void run()
            {
                try
                {
                    Ice.InitializationData initData = new Ice.InitializationData();

                    initData.properties = Ice.Util.createProperties();
                    initData.properties.setProperty("Ice.ACM.Client", "0");
                    initData.properties.setProperty("Ice.RetryIntervals", "-1");
                    initData.properties.setProperty("Ice.Trace.Network", "0");
                    initData.properties.setProperty("Ice.Plugin.IceSSL", "IceSSL.PluginFactory");
                    initData.properties.setProperty("IceSSL.TrustOnly.Client", "CN=Glacier2");

                    _communicator = Ice.Util.initialize(initData);
                    long refreshTimeout;
                    SessionAdapter session = null;
                    
                    if(glacier2)
                    {
                        String s;

                        if(secure)
                        {
                            s = "DemoGlacier2/router:ssl -p 4064 -h " + hostname + " -t 10000";
                        }
                        else
                        {
                            s = "DemoGlacier2/router:tcp -p 4502 -h " + hostname + " -t 10000";
                        }

                        Ice.ObjectPrx proxy = _communicator.stringToProxy(s);
                        Ice.RouterPrx r = Ice.RouterPrxHelper.uncheckedCast(proxy);

                        _communicator.setDefaultRouter(r);

                        final Glacier2.RouterPrx router = Glacier2.RouterPrxHelper.checkedCast(r);
                        if(router == null)
                        {
                            postLoginFailure("Glacier2 proxy is invalid.");
                            return;
                        }
                        Glacier2.SessionPrx glacier2session = router.createSession("dummy", "none");
                        
                        final Demo.Glacier2SessionPrx sess = Demo.Glacier2SessionPrxHelper.uncheckedCast(glacier2session);
                        final Demo.LibraryPrx library = sess.getLibrary();
                        refreshTimeout = (router.getSessionTimeout() * 1000) / 2;
                        session = new SessionAdapter()
                        {
                            public void destroy()
                            {
                                try
                                {
                                    router.destroySession();
                                }
                                catch(Glacier2.SessionNotExistException e)
                                {
                                }
                            }

                            public void refresh()
                            {
                                sess.refresh();
                            }

                            public Demo.LibraryPrx getLibrary()
                            {
                                return library;
                            }
                        };
                    }
                    else
                    {
                        String s;
                        if(secure)
                        {
                            s = "SessionFactory:ssl -h " + hostname + " -p 10001 -t 10000";

                        }
                        else
                        {
                            s = "SessionFactory:tcp -h " + hostname + " -p 10000 -t 10000";
                        }
                        Ice.ObjectPrx proxy = _communicator.stringToProxy(s);
                        Demo.SessionFactoryPrx factory = Demo.SessionFactoryPrxHelper.checkedCast(proxy);
                        if(factory == null)
                        {
                            postLoginFailure("SessionFactory proxy is invalid.");
                            return;
                        }

                        final Demo.SessionPrx sess = factory.create();
                        final Demo.LibraryPrx library = sess.getLibrary();
                        refreshTimeout = (factory.getSessionTimeout() * 1000) / 2;
                        session = new SessionAdapter()
                        {
                            public void destroy()
                            {
                                sess.destroy();
                            }

                            public void refresh()
                            {
                                sess.refresh();
                            }
                            
                            public Demo.LibraryPrx getLibrary()
                            {
                                return library;
                            }
                        };
                    }

                    synchronized(LoginController.this)
                    {
                        if(_destroyed)
                        {
                            return;
                        }
                        _sessionController = new SessionController(_handler, _communicator, session, refreshTimeout);
                        if(_loginListener != null)
                        {
                            final LoginListener listener = _loginListener;
                            _handler.post(new Runnable()
                            {
                                public void run()
                                {
                                    listener.onLogin();
                                }
                            });
                        }
                    }
                }
                catch(final Glacier2.CannotCreateSessionException ex)
                {
                    postLoginFailure(String.format("Session creation failed: %s", ex.reason));
                }
                catch(Glacier2.PermissionDeniedException ex)
                {
                    postLoginFailure(String.format("Login failed: %s", ex.reason));
                }
                catch(Ice.LocalException ex)
                {
                    postLoginFailure(String.format("Login failed: %s", ex.toString()));
                }
            }
        }).start();
    }

    synchronized public void destroy()
    {
        if(_destroyed)
        {
            return;
        }

        _destroyed = true;
        if(_sessionController != null)
        {
            _sessionController.destroy();
            _sessionController = null;
        }
        else
        {
            try
            {
                _communicator.destroy();
            }
            catch(Exception e)
            {
            }
        }
    }
    
    synchronized public String getLoginError()
    {
        return _loginError;
    }
    
    synchronized public void setLoginListener(LoginListener listener)
    {
        _loginListener = listener;
        if(listener != null)
        {
            if(_loginError != null)
            {
                listener.onLoginError();
            }
            else if(_sessionController == null)
            {
                listener.loginInProgress();
            }
            else
            {
                listener.onLogin();
            }
        }
    }

    synchronized public SessionController getSessionController()
    {
        return _sessionController;
    }
 }