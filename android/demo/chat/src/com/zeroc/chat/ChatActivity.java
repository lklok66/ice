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
import android.content.DialogInterface;
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
    private ChatRoomListener _listener = new ChatRoomListener()
    {
        public void init(final List<String> users)
        {
            _strings.clear();
            _adapter.notifyDataSetChanged();
        }

        public void join(final long timestamp, final String name)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - <system-message> - " + name + " joined.");
        }

        public void leave(final long timestamp, final String name)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - " + "<system-message> - " + name + " left.");
        }

        public void send(final long timestamp, final String name, final String message)
        {
            add(ChatUtils.formatTimestamp(timestamp) + " - <" + name + "> " + ChatUtils.unstripHtml(message));
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
        
        private void add(String msg)
        {
            _strings.add(msg);
            if(_strings.size() > AppSession.MAX_MESSAGES)
            {
                _strings.removeFirst();
            }
            _adapter.notifyDataSetChanged();
        }
    };

    public void onClick(View v)
    {
        sendText();
    }
    
    public boolean onKey(View v, int keyCode, KeyEvent event)
    {
        System.out.println("ChatActivity: onKey");
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

    @Override
    public void onResume()
    {
        System.out.println("ChatActivity: onResume");
        super.onResume();

        // If the add of the listener fails the session has been destroyed, and
        // we're done.
        if(!AppSessionManager.instance().addChatRoomListener(_listener, true))
        {
            finish();
        }
    }

    @Override
    public void onStop()
    {
        System.out.println("ChatActivity: onStop");
        super.onStop();
        
        AppSessionManager.instance().removeChatRoomListener(_listener);
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
        AppSessionManager.instance().destroySession();
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

        AppSessionManager.instance().send(t);
    }
}
