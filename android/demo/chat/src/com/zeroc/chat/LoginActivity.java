// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package com.zeroc.chat;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;

import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.Certificate;

public class LoginActivity extends Activity
{
    class CertificateVerifier implements IceSSL.CertificateVerifier
    {
        class AcceptInvalidCertDialog
        {
            private boolean _haveResult;
            private boolean _result;

            public boolean show()
            {
                LoginActivity.this.runOnUiThread(new Runnable() {
                    public void run()
                {
                    final String msg = "The server certificate does not match the official ZeroC chat server " +
                    "certificate, do you want to continue and connect to this chat server?";
                
                    AlertDialog.Builder builder = new AlertDialog.Builder(LoginActivity.this);
                    builder.setTitle("Warning");
                    builder.setMessage(msg);
                    builder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            setResult(true);
                        }
                    });
                    builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            setResult(false);
                        }
                    });
                    builder.show();
                }
                });
                return waitResult();
            }
                    
            synchronized void setResult(boolean rc)
            {
                _haveResult = true;
                _result = rc;
                notify();
            }

            synchronized boolean waitResult()
            {
                while(!_haveResult)
                {
                    try
                    {
                        wait();
                    }
                    catch(InterruptedException e)
                    {
                    }
                
                }
                return _result;
            }
        };

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
                return new AcceptInvalidCertDialog().show();
            }
            return false;
        }

        final private Certificate _caCert;
        boolean accept = false;
    };


    private void setLoginState()
    {
        String host = _hostname.getText().toString().trim();
        String username = _username.getText().toString().trim();
        _login.setEnabled(host.length() > 0 && username.length() > 0);
    }

    private void handleException(String s)
    {
        if(isFinishing())
        {
            return;
        }
        
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Error");
        builder.setMessage(s);
        builder.show();
        setLoginState();
    }

    private void loginComplete(AppSession session)
    {
        // If the user pressed the back key during login, then we're done.
        if(isFinishing())
        {
            session.destroy();
            return;
        }

        _session = session;
        Intent result = new Intent();
        result.setClass(getApplicationContext(), ChatActivity.class);
        startActivity(result);
    }
    
    private void login()
    {
        final String hostname = _hostname.getText().toString().trim();
        final String username = _username.getText().toString().trim();
        final String password = _password.getText().toString().trim();
        final boolean secure = _secure.isChecked();

        // We don't want to save obviously bogus hostnames in the application
        // preferences. These two regexp validates that the hostname is well
        // formed.
        // Note that this regexp doesn't handle IPv6 addresses.
        final String hostre = "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\\-]*[A-Za-z0-9])$";
        final String ipre = "^([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$";
        if(!hostname.matches(hostre) && !hostname.matches(ipre))
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Error");
            builder.setMessage("The hostname is invalid");
            builder.show();
            return;
        }
        
        // Update preferences.
        SharedPreferences.Editor edit = _prefs.edit();
        if(!_prefs.getString(HOSTNAME_KEY, DEFAULT_HOST).equals(hostname))
        {
            edit.putString(HOSTNAME_KEY, hostname);
        }
        if(!_prefs.getString(USERNAME_KEY, "").equals(username))
        {
            edit.putString(USERNAME_KEY, username);
        }
        if(!_prefs.getString(PASSWORD_KEY, "").equals(password))
        {
            edit.putString(PASSWORD_KEY, password);
        }
        if(_prefs.getBoolean(SECURE_KEY, DEFAULT_SECURE)!= secure)
        {
            edit.putBoolean(SECURE_KEY, secure);
        }
        edit.commit();
        
        _login.setEnabled(false);

        new Thread(new Runnable()
        {
            public void run()
            {
                try
                {
                    final AppSession session = new AppSession(_initData, _verifier, hostname, username, password, secure);
                    runOnUiThread(new Runnable()
                    {
                        public void run()
                        {
                            loginComplete(session);
                        }
                    });
                }
                catch(final Glacier2.CannotCreateSessionException e)
                {
                    runOnUiThread(new Runnable()
                    {
                        public void run()
                        {
                            handleException(String.format("Session creation failed: %s", e.reason));
                        }
                    });
                }
                catch(final Glacier2.PermissionDeniedException e)
                {
                    runOnUiThread(new Runnable()
                    {
                        public void run()
                        {
                            handleException(String.format("Login failed: %s", e.reason));
                        }
                    });
                }
                catch(final Ice.LocalException ex)
                {
                    runOnUiThread(new Runnable()
                    {
                        public void run()
                        {
                            handleException(ex.toString());
                        }
                    });
                }
            }
        }).start();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        if(_session != null)
        {
            _session.destroy();
            _session = null;
        }
        
        setLoginState();
    }

    @Override
    public void onPause()
    {
        if(isFinishing())
        {
            if(_session != null)
            {
                _session.destroy();
                _session = null;
            }
        }
        super.onStop();
    }
    
    @Override
    public void onDestroy()
    {
        super.onDestroy();
    
        if(_session != null)
        {
            _session.destroy();
            _session = null;
        }
    }
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login);

        _login = (Button) findViewById(R.id.login);
        _login.setOnClickListener(new android.view.View.OnClickListener()
        {
            public void onClick(android.view.View v)
            {
                login();
            }
        });

        _hostname = (EditText) findViewById(R.id.hostname);
        _hostname.addTextChangedListener(new TextWatcher()
        {
            public void afterTextChanged(Editable s)
            {
                setLoginState();
            }

            public void beforeTextChanged(CharSequence s, int start, int count, int after)
            {
            }

            public void onTextChanged(CharSequence s, int start, int count, int after)
            {
            }
        });

        _username = (EditText) findViewById(R.id.username);
        _username.addTextChangedListener(new TextWatcher()
        {
            public void afterTextChanged(Editable s)
            {
                setLoginState();
            }

            public void beforeTextChanged(CharSequence s, int start, int count, int after)
            {
            }

            public void onTextChanged(CharSequence s, int start, int count, int after)
            {
            }
        });
        _password = (EditText) findViewById(R.id.password);
        _secure = (android.widget.CheckBox) findViewById(R.id.secure);
        
        _prefs = getPreferences(MODE_PRIVATE);

        _hostname.setText(_prefs.getString(HOSTNAME_KEY, DEFAULT_HOST));
        _username.setText(_prefs.getString(USERNAME_KEY, ""));
        _password.setText(_prefs.getString(PASSWORD_KEY, ""));
        _secure.setChecked(_prefs.getBoolean(SECURE_KEY, DEFAULT_SECURE));
        
        _initData = new Ice.InitializationData();

        _initData.properties = Ice.Util.createProperties();
        _initData.properties.setProperty("Ice.ACM.Client", "0");
        _initData.properties.setProperty("Ice.RetryIntervals", "-1");
        _initData.properties.setProperty("Ice.Trace.Network", "0");
        _initData.properties.setProperty("Ice.Plugin.IceSSL", "IceSSL.PluginFactory");
        _initData.properties.setProperty("Ice.TrustOnly.Client", "CN=Glacier2");
        
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
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Error");
            builder.setMessage("Error importing certificate: " + e.toString());
            builder.show();
        }
    }

    // Used by the chat & user activities to get the current session object.
    static public AppSession getSession()
    {
        return _session;
    }

    private static final String DEFAULT_HOST = "demo.zeroc.com";
    private static final boolean DEFAULT_SECURE = false;
    private static final String HOSTNAME_KEY = "host";
    private static final String USERNAME_KEY = "username";
    private static final String PASSWORD_KEY = "password";
    private static final String SECURE_KEY = "secure";

    private Button _login;
    private EditText _hostname;
    private EditText _username;
    private EditText _password;
    private CheckBox _secure;
    private SharedPreferences _prefs;
    private Ice.InitializationData _initData;
    
    static private AppSession _session;
    private IceSSL.CertificateVerifier _verifier;
}
