// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package com.zeroc.chat;

import java.util.LinkedList;
import java.util.List;

import com.zeroc.chat.service.ChatRoomListener;
import com.zeroc.chat.service.ChatService;
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
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;

public class ChatActivity extends ListActivity
{
    private static final int DIALOG_ERROR = 1;
    private static final int DIALOG_FATAL = 2;
    private String _lastError = "";
    private static final String BUNDLE_KEY_LAST_ERROR = "zeroc:lastError";

    private static final int USERS_ID = Menu.FIRST;
    private static final int LOGOUT_ID = Menu.FIRST + 1;
    private static final int MAX_MESSAGE_SIZE = 1024;

    private EditText _text;
    private ArrayAdapter<String> _adapter;
    private LinkedList<String> _strings = new LinkedList<String>();
    private Service _service;
    final private ChatRoomListener _listener = new ChatRoomListener()
    {
        public void init(final List<String> users)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _strings.clear();
                    _adapter.notifyDataSetChanged();
                }
            });
        }

        public void join(long timestamp, String name)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - <system-message> - " + name + " joined.");
        }

        public void leave(long timestamp, String name)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - " + "<system-message> - " + name + " left.");
        }

        public void send(long timestamp, String name, final String message)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - <" + name + "> " + ChatUtils.unstripHtml(message));
        }

        public void error(final String error)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _lastError = error;
                    showDialog(DIALOG_FATAL);
                }
            });
        }

        public void inactivity()
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

        private void add(final String msg)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _strings.add(msg);
                    if(_strings.size() > 200) // AppSession.MAX_MESSAGES)
                    {
                        _strings.removeFirst();
                    }
                    _adapter.notifyDataSetChanged();
                }
            });
        }
    };
    final private ServiceConnection _connection = new ServiceConnection()
    {
        public void onServiceConnected(ComponentName name, IBinder service)
        { // This is called when the connection with the service has been
            // established, giving us the service object we can use to
            // interact with the service. Because we have bound to a explicit
            // service that we know is running in our own process, we can
            // cast its IBinder to a concrete class and directly access it.
            _service = ((com.zeroc.chat.service.ChatService.LocalBinder)service).getService();
            if(!_service.addChatRoomListener(_listener, true))
            {
                finish();
            }
        }

        public void onServiceDisconnected(ComponentName name)
        {
        }
    };

    // Hook the back key to logout the session.
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if(keyCode == KeyEvent.KEYCODE_BACK)
        {
            logout();
            return true;
        }
        return false;
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.chat);

        _text = (EditText)findViewById(R.id.text);

        _adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, _strings);
        setListAdapter(_adapter);

        _text.setOnClickListener(new OnClickListener()
        {
            public void onClick(View v)
            {
                sendText();
            }
        });
        _text.setOnKeyListener(new OnKeyListener()
        {
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
        });

        if(savedInstanceState != null)
        {
            _lastError = savedInstanceState.getString(BUNDLE_KEY_LAST_ERROR);
        }
    }

    @Override
    public void onResume()
    {
        super.onResume();

        bindService(new Intent(ChatActivity.this, ChatService.class), _connection, BIND_AUTO_CREATE);
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
            startActivity(new Intent(ChatActivity.this, UserViewActivity.class));
            return true;

        case LOGOUT_ID:
            finish();
            return true;
        }

        return super.onOptionsItemSelected(item);
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

    private void logout()
    {
        _service.logout();
        finish();
    }

    private void sendText()
    {
        String t = _text.getText().toString().trim();
        if(t.length() == 0)
        {
            return;
        }
        if(t.length() > MAX_MESSAGE_SIZE)
        {
            _lastError = "Message length exceeded, maximum length is " + MAX_MESSAGE_SIZE + " characters.";
            showDialog(DIALOG_ERROR);
            return;
        }
        _text.setText("");

        _service.send(t);
    }
}
