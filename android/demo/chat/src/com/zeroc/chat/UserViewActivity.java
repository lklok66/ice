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
import android.content.DialogInterface;
import android.os.Bundle;
import android.widget.ArrayAdapter;

public class UserViewActivity extends ListActivity
{
    private static final int DIALOG_FATAL = 1;
    private String _lastError = "";
    private static final String BUNDLE_KEY_LAST_ERROR = "zeroc:lastError";

    private List<String> _users = new ArrayList<String>();
    private ArrayAdapter<String> _adapter;
    private ChatRoomListener _listener = new ChatRoomListener()
    {
        public void init(final List<String> users)
        {
            _users.clear();
            _users.addAll(users);
            _adapter.notifyDataSetChanged();
        }

        public void join(final long timestamp, final String name)
        {
            _adapter.add(name);
        }

        public void leave(final long timestamp, final String name)
        {
            _adapter.remove(name);
        }

        public void send(final long timestamp, final String name, final String message)
        {
        }

        public void exception(final Ice.LocalException ex)
        {
            _lastError = ex.toString();
            showDialog(DIALOG_FATAL);
        }

        public void exception(final Ice.UserException ex)
        {
            _lastError = ex.toString();
            showDialog(DIALOG_FATAL);
        }
        
        public void inactivity()
        {
            _lastError = "You were logged out due to inactivity.";
            showDialog(DIALOG_FATAL);
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
        
        // If the add of the listener fails the session has been destroyed, and
        // we're done.
        if(!AppSessionManager.instance().addChatRoomListener(_listener, false))
        {
            finish();
        }
    }

    @Override
    public void onStop()
    {
        super.onStop();
        AppSessionManager.instance().removeChatRoomListener(_listener);
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
