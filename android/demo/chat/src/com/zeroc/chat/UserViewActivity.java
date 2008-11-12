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
import android.app.ListActivity;
import android.os.Bundle;
import android.widget.ArrayAdapter;

public class UserViewActivity extends ListActivity
{
    class ChatListener implements ChatRoomListener
    {
        public void init(final List<String> users)
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

        public void join(final long timestamp, final String name)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _adapter.add(name);
                }
            });
        }

        public void leave(final long timestamp, final String name)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _adapter.remove(name);
                }
            });
        }

        public void send(final long timestamp, final String name, final String message)
        {
        }
        
        public void exception(final Ice.LocalException ex)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    handleException(ex);
                }
            });
        }
    }
    
    private void handleException(Ice.LocalException ex)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Error");
        builder.setMessage(ex.toString());
        builder.show();

        setResult(RESULT_FIRST_USER);
        
        finish();
    }
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.user);
        _adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, _users);
        setListAdapter(_adapter);
        
        _session = LoginActivity.getSession();
    }
    
    @Override
    public void onResume()
    {
        super.onResume();
        _session.addChatRoomListener(_listener);
    }
    
    @Override
    public void onStop()
    {
        super.onStop();
        _session.removeChatRoomListener(_listener);
    }
    
    private ChatListener _listener = new ChatListener();
    private List<String> _users = new ArrayList<String>();
    private ArrayAdapter<String> _adapter;
    private AppSession _session;
}
