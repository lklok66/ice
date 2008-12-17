// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public abstract class TransceiverReadThread extends Thread
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

    synchronized public void read(Buffer buf, AsyncCallback callback)
    {
        _buf = buf;
        _callback = callback;
        notify();
    }

    public void run()
    {
        while(true)
        {
            AsyncCallback cb;
            Buffer buf;
            
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
                        _callback.complete(null); // TODO: What exception?
                    }
                    return;
                }
                buf = _buf;
                cb = _callback;
                
                // Clear the buffer and callback. Another read, however,
                // should not be requested until the complete callback is
                // called.
                _buf = null;
                _callback = null;
            }
            RuntimeException ex = null;

            try
            {
                read(buf);
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

    protected abstract void read(Buffer buf);
}