// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package com.zeroc.hello;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class HelloWorld extends Activity
{
    enum DeliveryMode
    {
        DeliveryModeTwoway,
        DeliveryModeOneway,
        DeliveryModeBatchOneway,
        DeliveryModeDatagram,
        DeliveryModeBatchDatagram;

        Ice.ObjectPrx apply(Ice.ObjectPrx prx, boolean secure)
        {
            switch (this)
            {
            case DeliveryModeTwoway:
                prx = prx.ice_twoway();
                break;
            case DeliveryModeOneway:
                prx = prx.ice_oneway();
                break;
            case DeliveryModeBatchOneway:
                prx = prx.ice_batchOneway();
                break;
            case DeliveryModeDatagram:
                prx = prx.ice_datagram();
                break;
            case DeliveryModeBatchDatagram:
                prx = prx.ice_batchDatagram();
                break;
            }
            if(this != DeliveryModeDatagram && this != DeliveryModeBatchDatagram)
            {
                prx = prx.ice_secure(secure);
            }
            return prx;
        }

        public DeliveryMode toggleBatch(boolean batch)
        {
            if(batch)
            {
                switch (this)
                {
                case DeliveryModeOneway:
                    return DeliveryModeBatchOneway;

                case DeliveryModeDatagram:
                    return DeliveryModeBatchDatagram;
                }
            }
            else
            {
                switch (this)
                {
                case DeliveryModeBatchOneway:
                    return DeliveryModeOneway;

                case DeliveryModeBatchDatagram:
                    return DeliveryModeDatagram;
                }
            }
            assert false;
            return this;
        }

        public boolean isBatch()
        {
            return this == DeliveryModeBatchOneway || this == DeliveryModeBatchDatagram;
        }
    }

    private Demo.HelloPrx createProxy()
    {
        String host = _hostname.getText().toString().trim();
        assert (host.length() > 0);
        // Change the preferences if necessary.
        if(!_prefs.getString(HOSTNAME_KEY, DEFAULT_HOST).equals(host))
        {
            SharedPreferences.Editor edit = _prefs.edit();
            edit.putString(HOSTNAME_KEY, host);
            edit.commit();
        }

        String s = "hello:tcp -h " + host + " -p 10000:ssl -h " + host + " -p 10001:udp -h " + host + " -p 10000";
        Ice.ObjectPrx prx = _communicator.stringToProxy(s);
        prx = _deliveryMode.apply(prx, _secure.isChecked());
        int timeout = _timeout.getProgress();
        if(timeout != 0)
        {
            prx = prx.ice_timeout(timeout);
        }
        return Demo.HelloPrxHelper.uncheckedCast(prx);
    }

    class SayHelloI extends Demo.AMI_Hello_sayHello implements Ice.AMISentCallback
    {
        private boolean _response = false;

        @Override
        synchronized public void ice_exception(final Ice.LocalException ex)
        {
            assert (!_response);
            _response = true;

            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    handleException(ex);
                }
            });
        }

        synchronized public void ice_sent()
        {
            if(_response)
            {
                return;
            }
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    if(_deliveryMode == DeliveryMode.DeliveryModeTwoway)
                    {
                        _status.setText("Waiting for response");
                    }
                    else
                    {
                        _status.setText("Ready");
                        _activity.setVisibility(View.INVISIBLE);
                    }
                }
            });
        }

        @Override
        synchronized public void ice_response()
        {
            assert (!_response);
            _response = true;
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    _activity.setVisibility(View.INVISIBLE);
                    _status.setText("Ready");
                }
            });
        }
    }

    private void sayHello()
    {
        Demo.HelloPrx hello = createProxy();
        try
        {
            if(!_deliveryMode.isBatch())
            {
                if(hello.sayHello_async(new SayHelloI(), _delay.getProgress()))
                {
                    if(_deliveryMode == DeliveryMode.DeliveryModeTwoway)
                    {
                        _activity.setVisibility(View.VISIBLE);
                        _status.setText("Waiting for response");
                    }
                }
                else
                {
                    _activity.setVisibility(View.VISIBLE);
                    _status.setText("Sending request");
                }
            }
            else
            {
                hello.sayHello(_delay.getProgress());
                _status.setText("Queued hello request");
            }
        }
        catch(Ice.LocalException ex)
        {
            handleException(ex);
        }
    }

    private void handleException(Ice.LocalException ex)
    {
        _status.setText("Ready");
        _activity.setVisibility(View.INVISIBLE);

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Error");
        builder.setMessage(ex.toString());
        builder.show();
    }

    private void shutdown()
    {
        Demo.HelloPrx hello = createProxy();
        try
        {
            if(!_deliveryMode.isBatch())
            {
                hello.shutdown_async(new Demo.AMI_Hello_shutdown() {

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
                    public void ice_response()
                    {
                        runOnUiThread(new Runnable()
                        {
                            public void run()
                            {
                                _activity.setVisibility(View.INVISIBLE);
                                _status.setText("Ready");
                            }
                        });
                    }
                });
                if(_deliveryMode == DeliveryMode.DeliveryModeTwoway)
                {
                    _activity.setVisibility(View.VISIBLE);
                    _status.setText("Waiting for response");
                }
            }
            else
            {
                hello.shutdown();
                _status.setText("Queued shutdown request");
            }
        }
        catch(Ice.LocalException ex)
        {
            handleException(ex);
        }
    }

    private void flush()
    {
        new Thread(new Runnable() {
            public void run()
            {
                try
                {
                    _communicator.flushBatchRequests();
                }
                catch(final Ice.LocalException ex)
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
        }).start();

        _status.setText("Flushed batch requests");
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        _sayHelloButton = (Button) findViewById(R.id.sayHello);
        _sayHelloButton.setOnClickListener(new android.view.View.OnClickListener()
        {
            public void onClick(android.view.View v)
            {
                sayHello();
            }
        });
        _shutdownButton = (Button) findViewById(R.id.shutdown);
        _shutdownButton.setOnClickListener(new android.view.View.OnClickListener()
        {
            public void onClick(android.view.View v)
            {
                shutdown();
            }
        });

        _hostname = (EditText) findViewById(R.id.hostname);
        _hostname.addTextChangedListener(new TextWatcher()
        {
            public void afterTextChanged(Editable s)
            {
                String host = _hostname.getText().toString().trim();
                if(host.length() == 0)
                {
                    _sayHelloButton.setEnabled(false);
                    _shutdownButton.setEnabled(false);
                }
                else
                {
                    _sayHelloButton.setEnabled(true);
                    _shutdownButton.setEnabled(true);
                }
            }

            public void beforeTextChanged(CharSequence s, int start, int count, int after)
            {
            }

            public void onTextChanged(CharSequence s, int start, int count, int after)
            {
            }
        });

        _secure = (android.widget.CheckBox) findViewById(R.id.secure);

        final Button flush = (Button) findViewById(R.id.flush);
        flush.setOnClickListener(new android.view.View.OnClickListener()
        {
            public void onClick(android.view.View v)
            {
                flush();
            }
        });

        final android.widget.CheckBox batch = (android.widget.CheckBox) findViewById(R.id.batch);
        batch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
            {
                _deliveryMode = _deliveryMode.toggleBatch(isChecked);
                flush.setEnabled(isChecked);
            }
        });

        android.widget.Spinner mode = (android.widget.Spinner) findViewById(R.id.mode);
        ArrayAdapter modeAdapter = new ArrayAdapter(this, android.R.layout.simple_spinner_item, new String[] {
                "Twoway", "Oneway", "Datagram" });
        mode.setAdapter(modeAdapter);
        mode.setSelection(0);
        mode.setOnItemSelectedListener(new android.widget.AdapterView.OnItemSelectedListener()
        {
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
            {
                if(id == 0) // Twoway
                {
                    _deliveryMode = DeliveryMode.DeliveryModeTwoway;
                    batch.setEnabled(false);
                    flush.setEnabled(false);
                    _secure.setEnabled(true);
                }
                else if(id == 1) // Oneway
                {
                    if(batch.isChecked())
                    {
                        _deliveryMode = DeliveryMode.DeliveryModeBatchOneway;
                    }
                    else
                    {
                        _deliveryMode = DeliveryMode.DeliveryModeOneway;
                    }
                    batch.setEnabled(true);
                    flush.setEnabled(batch.isChecked());
                    _secure.setEnabled(true);
                }
                else if(id == 2) // Datagram
                {
                    if(batch.isChecked())
                    {
                        _deliveryMode = DeliveryMode.DeliveryModeBatchDatagram;
                    }
                    else
                    {
                        _deliveryMode = DeliveryMode.DeliveryModeDatagram;
                    }
                    batch.setEnabled(true);
                    flush.setEnabled(batch.isChecked());
                    _secure.setEnabled(false);
                }
            }

            public void onNothingSelected(AdapterView<?> arg0)
            {
            }
        });

        final android.widget.TextView delayView = (android.widget.TextView) findViewById(R.id.delayView);
        _delay = (SeekBar) findViewById(R.id.delay);
        _delay.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
        {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromThumb)
            {
                delayView.setText(String.format("%.1f", progress / 1000.0));
            }

            public void onStartTrackingTouch(SeekBar seekBar)
            {
            }

            public void onStopTrackingTouch(SeekBar seekBar)
            {
            }
        });

        final android.widget.TextView timeoutView = (android.widget.TextView) findViewById(R.id.timeoutView);
        _timeout = (SeekBar) findViewById(R.id.timeout);
        _timeout.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
        {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromThumb)
            {
                timeoutView.setText(String.format("%.1f", progress / 1000.0));
            }

            public void onStartTrackingTouch(SeekBar seekBar)
            {
            }

            public void onStopTrackingTouch(SeekBar seekBar)
            {
            }
        });

        _activity = (ProgressBar) findViewById(R.id.activity);
        _activity.setVisibility(View.INVISIBLE);
        _status = (android.widget.TextView) findViewById(R.id.status);

        _prefs = getPreferences(MODE_PRIVATE);
        _hostname.setText(_prefs.getString(HOSTNAME_KEY, DEFAULT_HOST));

        // Setup the defaults.
        _deliveryMode = DeliveryMode.DeliveryModeTwoway;
        batch.setEnabled(false);
        flush.setEnabled(false);
        _secure.setEnabled(true);

        // Show the initializing dialog.
        showDialog(DIALOG_INITIALIZING);

        // SSL initialization can take some time. To avoid blocking the
        // calling thread, we perform the initialization in a separate thread.
        new Thread(new Runnable()
        {
            public void run()
            {
                initializeCommunicator();
            }
        }).start();
    }

    /** Called when the activity is destroyed. */
    public void onDestroy()
    {
        super.onDestroy();
        if(_communicator != null)
        {
            try
            {
                _communicator.destroy();
            }
            catch(Ice.LocalException ex)
            {
                ex.printStackTrace();
            }
        }
    }

    @Override
    protected Dialog onCreateDialog(int id)
    {
        switch (id)
        {
        case DIALOG_INITIALIZING:
            ProgressDialog dialog = new ProgressDialog(this);
            dialog.setTitle("Initializing");
            dialog.setMessage("Please wait while loading...");
            dialog.setIndeterminate(true);
            dialog.setCancelable(false);
            return dialog;
        }
        return null;
    }

    // Called in a separate thread.
    private void initializeCommunicator()
    {
        try
        {
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.setProperty("Ice.InitPlugins", "0");
            initData.properties.setProperty("Ice.Trace.Network", "3");
            initData.properties.setProperty("IceSSL.Trace.Security", "3");
            initData.properties.setProperty("IceSSL.KeystoreType", "BKS");
            initData.properties.setProperty("IceSSL.TruststoreType", "BKS");
            initData.properties.setProperty("IceSSL.Password", "password");
            initData.properties.setProperty("Ice.Plugin.IceSSL", "IceSSL.PluginFactory");
            _communicator = Ice.Util.initialize(initData);

            IceSSL.Plugin plugin = (IceSSL.Plugin)_communicator.getPluginManager().getPlugin("IceSSL");
            // Be sure to pass the same input stream to the SSL plug-in for
            // both the keystore and the truststore. This makes startup a
            // little faster since the plugin will not initialize
            // two keystores.
            java.io.InputStream certs = getResources().openRawResource(R.raw.certs);
            plugin.setKeystoreStream(certs);
            plugin.setTruststoreStream(certs);

            _communicator.getPluginManager().initializePlugins();

            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    dismissDialog(DIALOG_INITIALIZING);
                    _status.setText("Ready");
                }
            });
        }
        catch(final Exception ex)
        {
            runOnUiThread(new Runnable()
            {
                public void run()
                {
                    dismissDialog(DIALOG_INITIALIZING);
                    AlertDialog.Builder builder = new AlertDialog.Builder(HelloWorld.this);
                    builder.setTitle("Initialization Error");
                    builder.setMessage(ex.toString());
                    builder.setPositiveButton("Ok", new DialogInterface.OnClickListener()
                    {
                        public void onClick(DialogInterface dialog, int whichButton)
                        {
                            finish();
                        }
                    });
                    builder.show();
                }
            });
        }
    }

    private static final int DIALOG_INITIALIZING = 1;
    private static final String DEFAULT_HOST = "10.0.2.2";
    private static final String HOSTNAME_KEY = "host";

    private Ice.Communicator _communicator = null;
    private DeliveryMode _deliveryMode;

    private Button _sayHelloButton;
    private Button _shutdownButton;
    private EditText _hostname;
    private CheckBox _secure;
    private TextView _status;
    private SeekBar _delay;
    private SeekBar _timeout;
    private ProgressBar _activity;
    private SharedPreferences _prefs;
}
