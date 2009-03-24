// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{

    using System.Collections;
    using System.Diagnostics;
    using IceUtilInternal;

    public sealed class ObjectAdapterFactory
    {
        public void shutdown()
        {
            Hashtable adapters;
            lock(this)
            {
                //
                // Ignore shutdown requests if the object adapter factory has
                // already been shut down.
                //
                if(instance_ == null)
                {
                    return;
                }

                adapters = _adapters;
                
                instance_ = null;
                _communicator = null;
                
                System.Threading.Monitor.PulseAll(this);
            }

            //
            // Deactivate outside the thread synchronization, to avoid
            // deadlocks.
            //
            foreach(Ice.ObjectAdapter adapter in adapters.Values)
            {
                adapter.deactivate();
            }
        }
        
        public void waitForShutdown()
        {
            Hashtable adapters;
            lock(this)
            {
                //
                // First we wait for the shutdown of the factory itself.
                //
                while(instance_ != null)
                {
                    System.Threading.Monitor.Wait(this);
                }
                
                //
                // If some other thread is currently shutting down, we wait
                // until this thread is finished.
                //
                while(_waitForShutdown)
                {
                    System.Threading.Monitor.Wait(this);
                }
                _waitForShutdown = true;
                adapters = _adapters;
            }

            //
            // Now we wait for deactivation of each object adapter.
            //
            if(adapters != null)
            {
                foreach(Ice.ObjectAdapter adapter in adapters.Values)
                {
                    adapter.waitForDeactivate();
                }
            }
            
            lock(this)
            {
                //
                // Signal that waiting is complete.
                //
                _waitForShutdown = false;
                System.Threading.Monitor.PulseAll(this);
            }
        }

        public bool isShutdown()
        {
            lock(this)
            {
                return instance_ == null;
            }
        }

        public void destroy()
        {
            //
            // First wait for shutdown to finish.
            //
            waitForShutdown();

            Hashtable adapters;
            lock(this)
            {
                adapters = _adapters;

                //
                // We set _adapters to null because our destructor must not
                // invoke methods on member objects.
                //
                _adapters = null;
            }

            if(adapters != null)
            {
                foreach(Ice.ObjectAdapter adapter in adapters.Values)
                {
                    adapter.destroy();
                }
            }
        }
        
        public Ice.ObjectAdapter createObjectAdapter(string name, string endpoints, Ice.RouterPrx router)
        {
            lock(this)
            {
                if(instance_ == null)
                {
                    throw new Ice.ObjectAdapterDeactivatedException();
                }
                
                Ice.ObjectAdapter adapter = (Ice.ObjectAdapter)_adapters[name];
                if(adapter != null)
                {
                    Ice.AlreadyRegisteredException ex = new Ice.AlreadyRegisteredException();
                    ex.kindOfObject = "object adapter";
                    ex.id = name;
                    throw ex;
                }

                if(name.Length == 0 && (endpoints.Length != 0 || router != null))
                {
                    Ice.InitializationException ex = new Ice.InitializationException();
                    ex.reason = "Cannot configure endpoints or router with nameless object adapter";
                    throw ex;
                }
                
                if(name.Length == 0)
                {
                    string uuid = Ice.Util.generateUUID();
                    adapter = new Ice.ObjectAdapterI(instance_, _communicator, this, uuid, "", null, true);
                    _adapters[uuid] = adapter;
                }
                else
                {
                    adapter = new Ice.ObjectAdapterI(instance_, _communicator, this, name, endpoints, router, false);
                    _adapters[name] = adapter;
                }
                return adapter;
            }
        }
        
        public Ice.ObjectAdapter findObjectAdapter(Ice.ObjectPrx proxy)
        {
            ArrayList adapters;
            lock(this)
            {
                if(instance_ == null)
                {
                    return null;
                }
                
                adapters = new ArrayList(_adapters.Values);
            }
            
            foreach(Ice.ObjectAdapterI adapter in adapters)
            {
                try
                {
                    if(adapter.isLocal(proxy))
                    {
                        return adapter;
                    }
                }
                catch(Ice.ObjectAdapterDeactivatedException)
                {
                    // Ignore.
                }
            }

            return null;
        }

        public void removeObjectAdapter(string name)
        {
            lock(this)
            {
                if(instance_ == null)
                {
                    return;
                }

                _adapters.Remove(name);
            }
        }
        
        public void flushBatchRequests()
        {
            ArrayList adapters;
            lock(this)
            {
                if(_adapters == null)
                {
                    return;
                }
                
                adapters = new ArrayList(_adapters.Values);
            }

            foreach(Ice.ObjectAdapterI adapter in adapters)
            {
                adapter.flushBatchRequests();
            }
        }
        
        //
        // Only for use by Instance.
        //
        internal ObjectAdapterFactory(Instance instance, Ice.Communicator communicator)
        {
            instance_ = instance;
            _communicator = communicator;
            _adapters = new Hashtable();
            _waitForShutdown = false;
        }
        
        private Instance instance_;
        private Ice.Communicator _communicator;
        private Hashtable _adapters;
        private bool _waitForShutdown;
    }

}
