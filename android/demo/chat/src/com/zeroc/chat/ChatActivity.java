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

public class ChatActivity extends ListActivity implements OnClickListener, OnKeyListener, ServiceConnection
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
                    _strings.clear();
                    _adapter.notifyDataSetChanged();
                }
            });
        }

        public void join(final long timestamp, final String name, Ice.Current current)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - <system-message> - " + name + " joined.");
        }

        public void leave(final long timestamp, final String name, Ice.Current current)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - " + "<system-message> - " + name + " left.");
        }

        public void send(final long timestamp, final String name, final String message, Ice.Current current)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - <" + name + "> " + ChatUtils.unstripHtml(message));
        }

        public void error(final String error, Ice.Current current)
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
        System.out.println("ChatActivity: onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.chat);

        _text = (EditText)findViewById(R.id.text);

        _adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, _strings);
        setListAdapter(_adapter);

        _text.setOnClickListener(this);
        _text.setOnKeyListener(this);
        
        if(savedInstanceState != null)
        {
            _lastError = savedInstanceState.getString(BUNDLE_KEY_LAST_ERROR);
        }
    }
    
    public void onServiceConnected(ComponentName name, IBinder service)
    {
        System.out.println("ChatActivity.onServiceConnected");
        // TODO: This is ugly. It would be better if there was a way to get the service directly using
        // the binder.
        ChatApp app = (ChatApp)getApplication();
        _service = Chat.Android.ServicePrxHelper.uncheckedCast(app.getCommunicator().stringToProxy(
                "ChatService:tcp -p 12222"));
        _listener = Chat.Android.ChatRoomListenerPrxHelper.uncheckedCast(app.getAdapter().addWithUUID(_listenerImpl));
        // If the add of the listener fails the session has been destroyed, and
        // we're done.
        if(!_service.addChatRoomListener(_listener, true))
        {
            finish();
        }
    }

    public void onServiceDisconnected(ComponentName name)
    {
        System.out.println("ChatActivity.onServiceDisconnected");
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
            startActivity(result);
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
