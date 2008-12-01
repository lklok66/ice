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

import com.zeroc.chat.service.ChatRoomListener;
import com.zeroc.chat.service.ChatService;
import com.zeroc.chat.service.NoSessionException;
import com.zeroc.chat.service.Service;

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

public class UserViewActivity extends ListActivity
{
    private static final int DIALOG_FATAL = 1;
    private String _lastError = "";
    private static final String BUNDLE_KEY_LAST_ERROR = "zeroc:lastError";

    private List<String> _users = new ArrayList<String>();
    private ArrayAdapter<String> _adapter;
    private Service _service;
    final private ChatRoomListener _listener = new ChatRoomListener()
    {
        public void init(final List<String> users)
        {
            _users.clear();
            _users.addAll(users);
            _adapter.notifyDataSetChanged();
        }

        public void join(long timestamp, final String name)
        {
            _adapter.add(name);
        }

        public void leave(long timestamp, final String name)
        {
            _adapter.remove(name);
        }

        public void send(long timestamp, String name, String message)
        {
        }

        public void error(final String ex)
        {
            _lastError = ex;
            showDialog(DIALOG_FATAL);
        }

        public void inactivity()
        {
            _lastError = "You were logged out due to inactivity.";
            showDialog(DIALOG_FATAL);
        }
    };

    final private ServiceConnection _connection = new ServiceConnection()
    {
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            // This is called when the connection with the service has been
            // established, giving us the service object we can use to
            // interact with the service. Because we have bound to a explicit
            // service that we know is running in our own process, we can
            // cast its IBinder to a concrete class and directly access it.
            _service = ((com.zeroc.chat.service.ChatService.LocalBinder)service).getService();
            try
            {
                _service.addChatRoomListener(_listener, false);
            }
            catch(NoSessionException e)
            {
                finish();
            }
        }

        public void onServiceDisconnected(ComponentName name)
        {
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

    @Override
    public void onResume()
    {
        super.onResume();

        bindService(new Intent(UserViewActivity.this, ChatService.class), _connection, BIND_AUTO_CREATE);
    }

    @Override
    public void onStop()
    {
        super.onStop();
        unbindService(_connection);

        if(_service != null)
        {
            _service.removeChatRoomListener(_listener);
            _service = null;
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        outState.putString(BUNDLE_KEY_LAST_ERROR, _lastError);
    }

    @Override
    protected Dialog onCreateDialog(final int id)
    {
        switch(id)
        {
        case DIALOG_FATAL:
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Error");
            builder.setMessage(""); // Details provided in onPrepareDialog
            builder.setPositiveButton("Ok", new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    _lastError = "";
                    if(id == DIALOG_FATAL)
                    {
                        finish();
                    }
                }
            });
            return builder.create();
        }

        }

        return null;
    }
    
    @Override
    protected void onPrepareDialog(int id, Dialog dialog)
    {
        super.onPrepareDialog(id, dialog);
        AlertDialog alert = (AlertDialog)dialog;
        alert.setMessage(_lastError);
    }
}
