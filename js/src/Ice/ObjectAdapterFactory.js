// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    require("Ice/Class");
    require("Ice/AsyncResultBase");
    require("Ice/LocalException");
    require("Ice/ObjectAdapterI");
    require("Ice/Promise");
    require("Ice/UUID");

    var Ice = global.Ice || {};

    var AsyncResultBase = Ice.AsyncResultBase;
    var ObjectAdapterI = Ice.ObjectAdapterI;
    var Promise = Ice.Promise;
    var UUID = Ice.UUID;

    //
    // Only for use by Instance.
    //
    var ObjectAdapterFactory = Ice.Class({
        __init__: function(instance, communicator)
        {
            this._instance = instance;
            this._communicator = communicator;
            this._adapters = [];
            this._adapterNamesInUse = [];
            this._shutdownPromise = new Promise();
        },
        shutdown: function()
        {
            //
            // Ignore shutdown requests if the object adapter factory has
            // already been shut down.
            //
            if(this._instance === null)
            {
                return this._shutdownPromise;
            }

            this._instance = null;
            this._communicator = null;

            var count = this._adapters.length;
            if(count > 0)
            {
                var self = this;

                var successCB = function(r)
                {
                    if(--count === 0)
                    {
                        self._shutdownPromise.succeed();
                    }
                };
               
                var exceptionCB = function(ex)
                {
                    self._shutdownPromise.fail(ex);
                };
                
                for(var i = 0; i < this._adapters.length; ++i)
                {
                    this._adapters[i].deactivate().then(successCB).exception(exceptionCB);
                }
            }
            else
            {
                this._shutdownPromise.succeed();
            }

            return this._shutdownPromise;
        },
        waitForShutdown: function()
        {
            //
            // Communicator.waitForShutdown returns this promise, so we use an AsyncResultBase.
            //
            var promise = new AsyncResultBase(this._communicator, "waitForShutdown", null, null, null);

            var self = this;

            //
            // First we wait for the shutdown of the factory itself.
            //
            this._shutdownPromise.then(
                function()
                {
                    //
                    // Now we wait for deactivation of each object adapter.
                    //
                    var count = self._adapters.length;
                    if(count > 0)
                    {
                        var successCB = function(r)
                        {
                            if(--count === 0)
                            {
                                promise.succeed(promise);
                            }
                        };
                        
                        var exceptionCB = function(ex)
                        {
                            promise.fail(ex);
                        };
                        
                        for(var i = 0; i < self._adapters.length; ++i)
                        {
                            self._adapters[i].waitForDeactivate().then(successCB).exception(exceptionCB);
                        }
                    }
                    else
                    {
                        promise.succeed(promise);
                    }
                }).exception(
                    function(ex)
                    {
                        promise.fail(ex);
                    });

            return promise;
        },
        isShutdown: function()
        {
            return this._instance === null;
        },
        destroy: function()
        {
            var promise = new Promise();

            var self = this;

            //
            // First wait for shutdown to finish.
            //
            this.waitForShutdown().then(
                function(r)
                {
                    var count = self._adapters.length;
                    if(count > 0)
                    {
                        var successCB = function(r)
                        {
                            if(--count === 0)
                            {
                                promise.succeed();
                                self._adapters = [];
                            }
                        };
                        
                        var exceptionCB = function(ex)
                        {
                            promise.fail(ex);
                        };
                        
                        for(var i = 0; i < self._adapters.length; ++i)
                        {
                            self._adapters[i].destroy().then(successCB).exception(exceptionCB);
                        }
                    }
                    else
                    {
                        promise.succeed();
                    }
                }).exception(
                    function(ex)
                    {
                        promise.fail(ex);
                    });

            return promise;
        },
        createObjectAdapter: function(name, router, promise)
        {
            if(this._instance === null)
            {
                throw new Ice.ObjectAdapterDeactivatedException();
            }

            var adapter = null;
            try
            {
                if(name.length === 0)
                {
                    var uuid = UUID.generateUUID();
                    adapter = new ObjectAdapterI(this._instance, this._communicator, this, uuid, null, true, promise);
                }
                else
                {
                    if(this._adapterNamesInUse.indexOf(name) !== -1)
                    {
                        throw new Ice.AlreadyRegisteredException("object adapter", name);
                    }
                    adapter = new ObjectAdapterI(this._instance, this._communicator, this, name, router, false, promise);
                    this._adapterNamesInUse.push(name);
                }
                this._adapters.push(adapter);
            }
            catch(ex)
            {
                promise.fail(ex);
            }
        },
        findObjectAdapter: function(proxy)
        {
            if(this._instance === null)
            {
                return null;
            }

            for(var i = 0; i < this._adapters.length; ++i)
            {
                try
                {
                    if(this._adapters[i].isLocal(proxy))
                    {
                        return this._adapters[i];
                    }
                }
                catch(ex)
                {
                    if(ex instanceof Ice.ObjectAdapterDeactivatedException)
                    {
                        // Ignore.
                    }
                    else
                    {
                        throw ex;
                    }
                }
            }

            return null;
        },
        removeObjectAdapter: function(adapter)
        {
            if(this._instance === null)
            {
                return;
            }

            var n = this._adapters.indexOf(adapter);
            if(n !== -1)
            {
                this._adapters.splice(n, 1);
            }

            n = this._adapterNamesInuse.indexOf(adapter.getName());
            if(n !== -1)
            {
                this._adapterNamesInUse.splice(n, 1);
            }
        }
    });
    
    Ice.ObjectAdapterFactory = ObjectAdapterFactory;
    global.Ice = Ice;
}(typeof (global) === "undefined" ? window : global));