// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public abstract class TransceiverWriteThread extends Thread
{
    private Buffer _buf = null;
    private AsyncCallback _callback = null;
    private boolean _destroyed = false;

    synchronized public void destroyThread()
    {
        _destroyed = true;
        notify();
    }
    
    protected synchronized boolean destroyed()
    {
        return _destroyed;
    }

    synchronized public void write(Buffer buf, AsyncCallback callback)
    {
        _buf = buf;
        _callback = callback;
        notify();
    }

    public void run()
    {
        while(true)
        {
            Buffer buf;
            AsyncCallback cb;
            
            synchronized(this)
            {
                while(_buf == null && !_destroyed)
                {
                    try
                    {
                        wait();
                    }
                    catch(InterruptedException e)
                    {
                    }
                }
                if(_destroyed)
                {
                    if(_callback != null)
                    {
                        _callback.complete(null);
                    }
                    return;
                }
                buf = _buf;
                cb = _callback;
                
                // Clear the buffer and callback. Another write, however,
                // should not be requested until the complete callback is
                // called.
                _buf = null;
                _callback = null;
            }
            RuntimeException ex = null;
            try
            {
                write(buf);    
            }
            catch(RuntimeException e)
            {
                ex = e;
            }

            try
            {
                cb.complete(ex);
            }
            catch(RuntimeException e)
            {
            }
        }
    }
    
    protected abstract void write(Buffer buf);
}