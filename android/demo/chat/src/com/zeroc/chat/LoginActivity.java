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
import android.app.Dialog;
import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.IBinder;
import android.text.Editable;
import android.text.TextWatcher;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;

public class LoginActivity extends Activity implements ServiceConnection
{
    private static final int DIALOG_ERROR = 1;
    private static final int DIALOG_CONFIRM = 2;
    private String _lastError = "";

    private static final String DEFAULT_HOST = "demo.zeroc.com";
    private static final boolean DEFAULT_SECURE = false;
    private static final String HOSTNAME_KEY = "host";
    private static final String USERNAME_KEY = "username";
    private static final String PASSWORD_KEY = "password";
    private static final String SECURE_KEY = "secure";

    private static final String BUNDLE_KEY_LAST_ERROR = "zeroc:lastError";

    private Button _login;
    private EditText _hostname;
    private EditText _username;
    private EditText _password;
    private CheckBox _secure;
    private SharedPreferences _prefs;

    private boolean _loginInProgress = false;
    private Chat.Android.ServicePrx _service;
    private Chat.Android.SessionListenerPrx _listener;

    private Ice.Object _listenerImpl = new Chat.Android._SessionListenerDisp()
    {
        public void onConnectConfirm(Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    showDialog(DIALOG_CONFIRM);
                }
            });
        }

        public void onLogin(Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    Intent result = new Intent();
                    result.setClass(getApplicationContext(), ChatActivity.class);
                    startActivity(result);
                }
            });
        }

        public void onException(final String ex, Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    handleException(ex);
                }
            });
        }
    };

    private void setLoginState()
    {
        if(_loginInProgress)
        {
            _login.setEnabled(false);
        }
        else
        {
            String host = _hostname.getText().toString().trim();
            String username = _username.getText().toString().trim();
            _login.setEnabled(host.length() > 0 && username.length() > 0);
        }
    }

    private void handleException(String s)
    {
        setLoginState();

        _lastError = s;
        showDialog(DIALOG_ERROR);
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
            _lastError = "The hostname is invalid";
            showDialog(DIALOG_ERROR);
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
        if(_prefs.getBoolean(SECURE_KEY, DEFAULT_SECURE) != secure)
        {
            edit.putBoolean(SECURE_KEY, secure);
        }
        edit.commit();

        _login.setEnabled(false);

        // Kick off the login process. The activity is notified of changes
        // in the login process through calls to the SessionListener.
        _service.login(hostname, username, password, secure);
    }

    public void onServiceConnected(ComponentName name, IBinder service)
    {
        System.out.println("LoginActivity.onServiceConnected");
        // TODO: This is ugly. It would be better if there was a way to get the service directly using
        // the binder.
        ChatApp app = (ChatApp)getApplication();
        _service = Chat.Android.ServicePrxHelper.uncheckedCast(app.getCommunicator().stringToProxy(
                "ChatService:tcp -p 12222"));
        _listener = Chat.Android.SessionListenerPrxHelper.uncheckedCast(app.getAdapter().addWithUUID(_listenerImpl));
        _loginInProgress = _service.setSessionListener(_listener);
        setLoginState();
    }

    public void onServiceDisconnected(ComponentName name)
    {
        System.out.println("LoginActivity.onServiceDisconnected");
    }
    
    @Override
    protected void onResume()
    {
        System.out.println("LoginActivity: onResume");

        super.onResume();
        
        Intent result = new Intent();
        result.setComponent(new ComponentName("com.zeroc.chat", "com.zeroc.chat.ChatService"));
        bindService(result, this, BIND_AUTO_CREATE);
    }

    @Override
    public void onStop()
    {
        System.out.println("LoginActivity: onStop");
        
        super.onStop();
        unbindService(this);
        
        if(_listener != null)
        {
            _service.setSessionListener(null);
            _service = null;

            try
            {
                ChatApp app = (ChatApp)getApplication();
                app.getAdapter().remove(_listener.ice_getIdentity());
            }
            catch(Ice.NotRegisteredException ex)
            {
            }

            _listener = null;
        }       
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        System.out.println("LoginActivity: onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login);

        _login = (Button)findViewById(R.id.login);
        _login.setOnClickListener(new android.view.View.OnClickListener()
        {
            public void onClick(android.view.View v)
            {
                login();
            }
        });
        _login.setEnabled(false);

        _hostname = (EditText)findViewById(R.id.hostname);
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

        _username = (EditText)findViewById(R.id.username);
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
        _password = (EditText)findViewById(R.id.password);
        _secure = (CheckBox)findViewById(R.id.secure);

        _prefs = getPreferences(MODE_PRIVATE);

        if(savedInstanceState == null)
        {
            _hostname.setText(_prefs.getString(HOSTNAME_KEY, DEFAULT_HOST));
            _username.setText(_prefs.getString(USERNAME_KEY, ""));
            _password.setText(_prefs.getString(PASSWORD_KEY, ""));
            _secure.setChecked(_prefs.getBoolean(SECURE_KEY, DEFAULT_SECURE));
        }
        else
        {
            _lastError = savedInstanceState.getString(BUNDLE_KEY_LAST_ERROR);
        }

        // Start the ChatService.
        Intent result = new Intent();
        result.setComponent(new ComponentName("com.zeroc.chat", "com.zeroc.chat.ChatService"));
        startService(result);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        outState.putString(BUNDLE_KEY_LAST_ERROR, _lastError);
    }

    @Override
    protected Dialog onCreateDialog(int id)
    {
        switch (id)
        {
        case DIALOG_ERROR:
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Error");
            builder.setMessage(_lastError);
            return builder.create();
        }

        case DIALOG_CONFIRM:
        {
            final String msg = "The server certificate does not match the official ZeroC chat server "
                    + "certificate, do you want to continue and connect to this chat server?";

            AlertDialog.Builder builder = new AlertDialog.Builder(LoginActivity.this);
            builder.setTitle("Warning");
            builder.setMessage(msg);
            builder.setPositiveButton("Ok", new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    _service.confirmConnection(true);
                }
            });
            builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    _service.confirmConnection(false);
                }
            });
            return builder.create();
        }

        }

        return null;
    }
}
