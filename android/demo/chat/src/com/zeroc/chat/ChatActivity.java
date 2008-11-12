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
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;

public class ChatActivity extends ListActivity implements OnClickListener, OnKeyListener
{
    class ChatListener implements ChatRoomListener
    {
        public void init(final List<String> users)
        {
        }

        public void join(final long timestamp, final String name)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _adapter.add(ChatUtils.formatTimestamp(timestamp) + " - <system-message> - " + name + " joined.");
                }
            });
        }

        public void leave(final long timestamp, final String name)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _adapter.add(ChatUtils.formatTimestamp(timestamp) + " - " + "<system-message> - " + name + " left.");
                }
            });
        }

        public void send(final long timestamp, final String name, final String message)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _adapter.add(ChatUtils.formatTimestamp(timestamp) + " - <" + name + "> " + ChatUtils.unstripHtml(message));
                }
            });
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
        finish();
    }
    
    private void handleException(Ice.UserException ex)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Error");
        builder.setMessage(ex.toString());
        builder.show();
        finish();
    }
    
    private void sendText()
    {
        String t = _text.getText().toString().trim();
        if(t.length() > MAX_MESSAGE_SIZE)
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Error");
            builder.setMessage("Message length exceeded, maximum length is " + MAX_MESSAGE_SIZE + " characters.");
            builder.show();
            return;
        }
        _text.setText("");
        
        _session.getSession().send_async(new Chat.AMI_ChatSession_send()
        {
            @Override
            public void ice_exception(final Ice.LocalException ex)
            {
                runOnUiThread(new Runnable()
                {
                    public void run()
                    {
                        handleException(ex);
                    }
                });
            }

            @Override
            public void ice_exception(final Ice.UserException ex)
            {
                runOnUiThread(new Runnable()
                {
                    public void run()
                    {
                        handleException(ex);
                    }
                });
            }

            @Override
            public void ice_response(long ret)
            {
            }
        }, t);
    }

    public void onClick(View v)
    {
        sendText();
    }

    public boolean onKey(View v, int keyCode, KeyEvent event)
    {
        if(event.getAction() == KeyEvent.ACTION_DOWN)
        {
            switch (keyCode)
            {
            case KeyEvent.KEYCODE_DPAD_CENTER:
            case KeyEvent.KEYCODE_ENTER:
                sendText();
                return true;
            }
        }
        return false;
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.chat);

        _text = (android.widget.EditText) findViewById(R.id.text);

        _adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, _strings);
        setListAdapter(_adapter);

        _text.setOnClickListener(this);
        _text.setOnKeyListener(this);
        
        _session = LoginActivity.getSession();
        _session.addChatRoomListener(_listener);
        _session.init();
    }
    
    @Override
    public void onStop()
    {
        super.onStop();
        if(isFinishing())
        {
            _session.removeChatRoomListener(_listener);
        }
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        boolean result = super.onCreateOptionsMenu(menu);
        menu.add(0, USERS_ID, 0, R.string.menu_users);
        menu.add(0, LOGOUT_ID, 0, R.string.menu_logout);
        return result;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {
        case USERS_ID:
            Intent result = new Intent();
            result.setClass(getApplicationContext(), UserViewActivity.class);
            startActivityForResult(result, 0);
            return true;
            
        case LOGOUT_ID:
            finish();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
    
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        // Called when the user view activity is complete. If RESULT_FIRST_USER
        // is returned, the session has been destroyed.
        if(resultCode == RESULT_FIRST_USER)
        {
            finish();
        }
    }

    private static final int USERS_ID = Menu.FIRST;
    private static final int LOGOUT_ID = Menu.FIRST +1;
    private static final int MAX_MESSAGE_SIZE = 1024;
    
    private EditText _text;
    private ArrayAdapter<String> _adapter;
    private ArrayList<String> _strings = new ArrayList<String>();
    private ChatListener _listener = new ChatListener();
    private AppSession _session;
}
