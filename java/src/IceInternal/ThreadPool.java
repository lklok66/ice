// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public final class ThreadPool
{
    private final static boolean TRACE_REGISTRATION = false;
    private final static boolean TRACE_SHUTDOWN = false;
    private final static boolean TRACE_THREAD = false;

    public
    ThreadPool(Instance instance, String prefix, int timeout)
    {
        _instance = instance;
        _destroyed = false;
        _prefix = prefix;
        _timeout = timeout;
        _threadIndex = 0;
        _running = 0;
        _inUse = 0;
        _load = 1.0;
        _serialize = _instance.initializationData().properties.getPropertyAsInt(_prefix + ".Serialize") > 0;

        String programName = _instance.initializationData().properties.getProperty("Ice.ProgramName");
        if(programName.length() > 0)
        {
            _programNamePrefix = programName + "-";
        }
        else
        {
            _programNamePrefix = "";
        }

        //
        // We use just one thread as the default. This is the fastest
        // possible setting, still allows one level of nesting, and
        // doesn't require to make the servants thread safe.
        //
        int size = _instance.initializationData().properties.getPropertyAsIntWithDefault(_prefix + ".Size", 1);
        if(size < 1)
        {
            String s = _prefix + ".Size < 1; Size adjusted to 1";
            _instance.initializationData().logger.warning(s);
            size = 1;
        }               

        int sizeMax = 
            _instance.initializationData().properties.getPropertyAsIntWithDefault(_prefix + ".SizeMax", size);
        if(sizeMax < size)
        {
            String s = _prefix + ".SizeMax < " + _prefix + ".Size; SizeMax adjusted to Size (" + size + ")";
            _instance.initializationData().logger.warning(s);
            sizeMax = size;
        }
                
        int sizeWarn = _instance.initializationData().properties.getPropertyAsIntWithDefault(
                                                                _prefix + ".SizeWarn", sizeMax * 80 / 100);
        if(sizeWarn > sizeMax)
        {
            String s = _prefix + ".SizeWarn > " + _prefix + ".SizeMax; adjusted SizeWarn to SizeMax (" + sizeMax + ")";
            _instance.initializationData().logger.warning(s);
            sizeWarn = sizeMax;
        }

        _size = size;
        _sizeMax = sizeMax;
        _sizeWarn = sizeWarn;
        
        try
        {
            _threads = new java.util.ArrayList<WorkerThread>();
            for(int i = 0; i < _size; i++)
            {
                WorkerThread thread = new WorkerThread(_programNamePrefix + _prefix + "-" +
                                                                   _threadIndex++);
                _threads.add(thread);
                thread.start();
                ++_running;
            }
        }
        catch(RuntimeException ex)
        {
            java.io.StringWriter sw = new java.io.StringWriter();
            java.io.PrintWriter pw = new java.io.PrintWriter(sw);
            ex.printStackTrace(pw);
            pw.flush();
            String s = "cannot create thread for `" + _prefix + "':\n" + sw.toString();
            _instance.initializationData().logger.error(s);

            destroy();
            joinWithAllThreads();
            throw ex;
        }
    }

    public synchronized void
    destroy()
    {
        if(TRACE_SHUTDOWN)
        {
            trace("destroy");
        }

        assert(!_destroyed);
        _destroyed = true;
        notifyAll();
    }
    
    public synchronized void
    execute(ThreadPoolWorkItem workItem)
    {
        if(TRACE_REGISTRATION)
        {
            trace("adding work item");
        }

        if(_destroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        _workItems.add(workItem);
        notify();
    }
    
    public synchronized void
    finish(Ice.ConnectionI connection)
    {
        if(_destroyed)
        {
            throw new Ice.CommunicatorDestroyedException();
        }
        _finished.add(connection);
        notify();
    }
    
    public void
    joinWithAllThreads()
    {
        //
        // _threads is immutable after destroy() has been called,
        // therefore no synchronization is needed. (Synchronization
        // wouldn't be possible here anyway, because otherwise the
        // other threads would never terminate.)
        //
        java.util.Iterator<WorkerThread> i = _threads.iterator();
        while(i.hasNext())
        {
            WorkerThread thread = i.next();
            
            while(true)
            {
                try
                {
                    thread.join();
                    break;
                }
                catch(InterruptedException ex)
                {
                }
            }
        }
    }

    public String
    prefix()
    {
        return _prefix;
    }
    
    public boolean
    serialize()
    {
        return _serialize;
    }

    //
    // Each thread supplies a BasicStream, to avoid creating excessive
    // garbage (Java only).
    //
    private void
    run()
    {
        while(true)
        {
            ThreadPoolWorkItem workItem = null;
            Ice.ConnectionI handler = null;

            synchronized(this)
            {
                while(_finished.size() == 0 && _workItems.size() == 0 && !_destroyed)
                {
                    try
                    {
                        if(_timeout > 0)
                        {
                            wait(_timeout * 1000);
                            // TODO: If timeed out
                            boolean timedOut = false;
                            if(timedOut)
                            {
                                //
                                // Initiate server shutdown.
                                //
                                try
                                {
                                    _instance.objectAdapterFactory().shutdown();
                                }
                                catch(Ice.CommunicatorDestroyedException e)
                                {
                                }
                                continue;
                            }
                        }
                        else
                        {
                            wait();
                        }
                    }
                    catch(InterruptedException e)
                    {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }

                }
                assert _finished.size() > 0 || _workItems.size() > 0 || _destroyed;
                    
                //
                // There are two possibilities for an interrupt:
                //
                // 1. The thread pool has been destroyed.
                //
                // 2. A work item has been scheduled.
                //

                if(!_finished.isEmpty())
                {
                    handler = _finished.removeFirst();
                }
                else if(!_workItems.isEmpty())
                {
                    //
                    // Work items must be executed first even if the thread pool is destroyed.
                    //
                    workItem = _workItems.removeFirst();
                }
                else
                {
                    assert _destroyed;
                    return;
                }
                
                if(_sizeMax > 1)
                {
                    assert(_inUse >= 0);
                    ++_inUse;

                    if(_inUse == _sizeWarn)
                    {
                        String s = "thread pool `" + _prefix + "' is running low on threads\n"
                            + "Size=" + _size + ", " + "SizeMax=" + _sizeMax + ", " + "SizeWarn=" + _sizeWarn;
                        _instance.initializationData().logger.warning(s);
                    }
                    
                    assert _inUse <= _running;
                    if(_inUse < _sizeMax && _inUse == _running)
                    {
                        try
                        {
                            WorkerThread thread = new WorkerThread(_programNamePrefix + _prefix + "-" + _threadIndex++);
                            thread.start();
                            _threads.add(thread);
                            ++_running;
                        }
                        catch(RuntimeException ex)
                        {
                            String s = "cannot create thread for `" + _prefix + "':\n" + ex;
                            _instance.initializationData().logger.error(s);
                        }
                    }
                }
            }

            //
            // Now we are outside the thread synchronization.
            //
            if(workItem != null)
            {
                try
                {
                    workItem.execute();
                }
                catch(Ice.LocalException ex)
                {
                    java.io.StringWriter sw = new java.io.StringWriter();
                    java.io.PrintWriter pw = new java.io.PrintWriter(sw);
                    ex.printStackTrace(pw);
                    pw.flush();
                    String s = "exception in `" + _prefix + "' while calling execute():\n" + sw.toString();
                    _instance.initializationData().logger.error(s);
                }
            }
            else
            {
                assert handler != null;

                //
                // Notify a handler about its removal from the thread pool.
                //
                try
                {
                    handler.finished(this);
                }
                catch(Ice.LocalException ex)
                {
                    java.io.StringWriter sw = new java.io.StringWriter();
                    java.io.PrintWriter pw = new java.io.PrintWriter(sw);
                    ex.printStackTrace(pw);
                    pw.flush();
                    String s = "exception in `" + _prefix + "' while calling finished():\n" +
                    sw.toString() + "\n" + handler.toString();
                    _instance.initializationData().logger.error(s);
                }
            }

            if(_sizeMax > 1)
            {
                synchronized(this)
                {
                    if(!_destroyed)
                    {
                        //
                        // First we reap threads that have been
                        // destroyed before.
                        //
                        int sz = _threads.size();
                        assert(_running <= sz);
                        if(_running < sz)
                        {
                            java.util.Iterator<WorkerThread> i = _threads.iterator();
                            while(i.hasNext())
                            {
                                WorkerThread thread = i.next();

                                if(!thread.isAlive())
                                {
                                    try
                                    {
                                        thread.join();
                                        i.remove();
                                    }
                                    catch(InterruptedException ex)
                                    {
                                    }
                                }
                            }
                        }
                        
                        //
                        // Now we check if this thread can be destroyed, based
                        // on a load factor.
                        //

                        //
                        // The load factor jumps immediately to the number of
                        // threads that are currently in use, but decays
                        // exponentially if the number of threads in use is
                        // smaller than the load factor. This reflects that we
                        // create threads immediately when they are needed,
                        // but want the number of threads to slowly decline to
                        // the configured minimum.
                        //
                        double inUse = (double)_inUse;
                        if(_load < inUse)
                        {
                            _load = inUse;
                        }
                        else
                        {
                            final double loadFactor = 0.05; // TODO: Configurable?
                            final double oneMinusLoadFactor = 1 - loadFactor;
                            _load = _load * oneMinusLoadFactor + _inUse * loadFactor;
                        }

                        if(_running > _size)
                        {
                            int load = (int)(_load + 0.5);

                            //
                            // We add one to the load factor because one
                            // additional thread is needed for select().
                            //
                            if(load + 1 < _running)
                            {
                                assert(_inUse > 0);
                                --_inUse;
                                
                                assert(_running > 0);
                                --_running;
                                
                                return;
                            }
                        }
                        
                        assert(_inUse > 0);
                        --_inUse;
                    }
                }
                
                if(TRACE_THREAD)
                {
                    trace("thread " + Thread.currentThread() + " is active");
                }
            }
        }
    }

    

    private void
    trace(String msg)
    {
        System.err.println(_prefix + ": " + msg);
    }

    private Instance _instance;
    private boolean _destroyed;
    private final String _prefix;
    private final String _programNamePrefix;

    private java.util.LinkedList<ThreadPoolWorkItem> _workItems = new java.util.LinkedList<ThreadPoolWorkItem>();
    private java.util.LinkedList<Ice.ConnectionI> _finished = new java.util.LinkedList<Ice.ConnectionI>();
    
    private int _timeout;

    private final class WorkerThread extends Thread
    {
        WorkerThread(String name)
        {
            super(name);
        }

        public void
        run()
        {
            if(_instance.initializationData().threadHook != null)
            {
                _instance.initializationData().threadHook.start();
            }

            try
            {
                ThreadPool.this.run();
            }
            catch(Ice.LocalException ex)
            {
                java.io.StringWriter sw = new java.io.StringWriter();
                java.io.PrintWriter pw = new java.io.PrintWriter(sw);
                ex.printStackTrace(pw);
                pw.flush();
                String s = "exception in `" + _prefix + "' thread " + getName() + ":\n" + sw.toString();
                _instance.initializationData().logger.error(s);
            }
            catch(java.lang.Exception ex)
            {
                java.io.StringWriter sw = new java.io.StringWriter();
                java.io.PrintWriter pw = new java.io.PrintWriter(sw);
                ex.printStackTrace(pw);
                pw.flush();
                String s = "unknown exception in `" + _prefix + "' thread " + getName() + ":\n" + sw.toString();
                _instance.initializationData().logger.error(s);
            }
            if(_instance.initializationData().threadHook != null)
            {
                _instance.initializationData().threadHook.stop();
            }

            if(TRACE_THREAD)
            {
                trace("run() terminated");
            }
        }
    }

    private final int _size; // Number of threads that are pre-created.
    private final int _sizeMax; // Maximum number of threads.
    private final int _sizeWarn; // If _inUse reaches _sizeWarn, a "low on threads" warning will be printed.
    private final boolean _serialize; // True if requests need to be serialized over the connection.

    private java.util.List<WorkerThread> _threads; // All threads, running or not.
    private int _threadIndex; // For assigning thread names.
    private int _running; // Number of running threads.
    private int _inUse; // Number of threads that are currently in use.
    private double _load; // Current load in number of threads.
}
