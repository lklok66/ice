// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package com.zeroc.chat;

import java.util.ArrayList;
import java.util.List;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ListActivity;
import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.widget.ArrayAdapter;

public class UserViewActivity extends ListActivity implements ServiceConnection
{
    private static final int DIALOG_FATAL = 1;
    private String _lastError = "";
    private static final String BUNDLE_KEY_LAST_ERROR = "zeroc:lastError";

    private List<String> _users = new ArrayList<String>();
    private ArrayAdapter<String> _adapter;
    private Chat.Android.ServicePrx _service;
    private Chat.Android.ChatRoomListenerPrx _listener;
    private Ice.Object _listenerImpl = new Chat.Android._ChatRoomListenerDisp()
    {
        public void init(final List<String> users, Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _users.clear();
                    _users.addAll(users);
                    _adapter.notifyDataSetChanged();
                }
            });
        }

        public void join(final long timestamp, final String name, Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _adapter.add(name);
                }
            });
        }

        public void leave(final long timestamp, final String name, Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _adapter.remove(name);
                }
            });
        }

        public void send(final long timestamp, final String name, final String message, Ice.Current current)
        {
        }

        public void error(final String ex, Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {

                    _lastError = ex;
                    showDialog(DIALOG_FATAL);
                }
            });
        }

        public void inactivity(Ice.Current current)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {

                    _lastError = "You were logged out due to inactivity.";
                    showDialog(DIALOG_FATAL);
                }
            });
        }
    };

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.user);

        _adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, _users);
        setListAdapter(_adapter);

        if(savedInstanceState != null)
        {
            _lastError = savedInstanceState.getString(BUNDLE_KEY_LAST_ERROR);
        }
    }

    public void onServiceConnected(ComponentName name, IBinder service)
    {
        System.out.println("UserViewActivity.onServiceConnected");

        // TODO: This is ugly. It would be better if there was a way to get the
        // service directly using the binder.
        ChatApp app = (ChatApp)getApplication();
        _service = Chat.Android.ServicePrxHelper.uncheckedCast(app.getCommunicator().stringToProxy(
                "ChatService:tcp -p 12222"));
        _listener = Chat.Android.ChatRoomListenerPrxHelper.uncheckedCast(app.getAdapter().addWithUUID(_listenerImpl));

        // If the add of the listener fails the session has been destroyed, and
        // we're done.
        if(!_service.addChatRoomListener(_listener, false))
        {
            finish();
        }
    }

    public void onServiceDisconnected(ComponentName name)
    {
        System.out.println("UserViewActivity.onServiceDisconnected");
    }

    @Override
    public void onResume()
    {
        System.out.println("ChatActivity: onResume");
        super.onResume();

        Intent result = new Intent();
        result.setComponent(new ComponentName("com.zeroc.chat", "com.zeroc.chat.ChatService"));
        bindService(result, this, BIND_AUTO_CREATE);
    }

    @Override
    public void onStop()
    {
        System.out.println("ChatActivity: onStop");
        super.onStop();
        unbindService(this);

        if(_listener != null)
        {
            _service.removeChatRoomListener(_listener);
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
        case DIALOG_FATAL:
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Error");
            builder.setMessage(_lastError);
            if(id == DIALOG_FATAL)
            {
                builder.setPositiveButton("Ok", new DialogInterface.OnClickListener()
                {
                    public void onClick(DialogInterface dialog, int whichButton)
                    {
                        finish();
                    }
                });
            }
            return builder.create();
        }

        }

        return null;
    }
}
