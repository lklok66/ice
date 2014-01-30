// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var global = this;
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
    var ObjectAdapterFactory = function(instance, communicator)
    {
        this._instance = instance;
        this._communicator = communicator;
        this._adapters = [];
        this._adapterNamesInUse = [];
        this._shutdownPromise = new Promise();
    };

    ObjectAdapterFactory.prototype.shutdown = function()
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

            for(var i = 0; i < this._adapters.length; ++i)
            {
                this._adapters[i].deactivate().then(
                    function(r)
                    {
                        if(--count === 0)
                        {
                            self._shutdownPromise.succeed();
                        }
                    }).exception(
                        function(ex)
                        {
                            self._shutdownPromise.fail(ex);
                        });
            }
        }
        else
        {
            this._shutdownPromise.succeed();
        }

        return this._shutdownPromise;
    };

    ObjectAdapterFactory.prototype.waitForShutdown = function()
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
                    for(var i = 0; i < self._adapters.length; ++i)
                    {
                        self._adapters[i].waitForDeactivate().then(
                            function(r)
                            {
                                if(--count === 0)
                                {
                                    promise.succeed(promise);
                                }
                            }).exception(
                                function(ex)
                                {
                                    promise.fail(ex);
                                });
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
    };

    ObjectAdapterFactory.prototype.isShutdown = function()
    {
        return this._instance === null;
    };

    ObjectAdapterFactory.prototype.destroy = function()
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
                    for(var i = 0; i < self._adapters.length; ++i)
                    {
                        self._adapters[i].destroy().then(
                            function(r)
                            {
                                if(--count === 0)
                                {
                                    promise.succeed();
                                    self._adapters = [];
                                }
                            }).exception(
                                function(ex)
                                {
                                    promise.fail(ex);
                                });
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
    };

    ObjectAdapterFactory.prototype.createObjectAdapter = function(name, router)
    {
        if(this._instance === null)
        {
            throw new Ice.ObjectAdapterDeactivatedException();
        }

        var adapter = null;
        if(name.length === 0)
        {
            var uuid = UUID.generateUUID();
            adapter = new ObjectAdapterI(this._instance, this._communicator, this, uuid, null, true);
        }
        else
        {
            if(this._adapterNamesInUse.indexOf(name) !== -1)
            {
                throw new Ice.AlreadyRegisteredException("object adapter", name);
            }
            adapter = new ObjectAdapterI(this._instance, this._communicator, this, name, router, false);
            this._adapterNamesInUse.push(name);
        }
        this._adapters.push(adapter);
        return adapter;
    };

    ObjectAdapterFactory.prototype.findObjectAdapter = function(proxy)
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
    };

    ObjectAdapterFactory.prototype.removeObjectAdapter = function(adapter)
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
    };

    Ice.ObjectAdapterFactory = ObjectAdapterFactory;
    global.Ice = Ice;
}());
