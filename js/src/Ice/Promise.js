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
    var Ice = global.Ice || {};
    
    //
    // Promise State
    //
    var State = {};
    State.Pending = 0;
    State.Success = 1;
    State.Failed = 2;

    var Promise = function()
    {
        this.__state = State.Pending;
        this.__listeners = [];
    };
    
    Promise.State = State;

    Promise.prototype.then = function(onResponse, onException, onProgress)
    {
        var promise = new Promise();
        var self = this;
        //
        // Use setTimeout so the listeners are not resolved until the call stack is empty.
        //
        setTimeout(
            function()
            {
                self.__listeners.push(
                    {
                        promise:promise,
                        onResponse:onResponse,
                        onException:onException,
                        onProgress:onProgress
                    });
                self.resolve();
            }, 0);
        return promise;
    };

    Promise.prototype.exception = function(onException)
    {
        return this.then(null, onException);
    };

    var resolveImp = function(self, listener)
    {
        var callback = self.__state === State.Success ? listener.onResponse : listener.onException;
        try
        {
            if(typeof callback !== 'function')
            {
                listener.promise.setState(self.__state, self._args);
            }
            else
            {
                var result = callback.apply(null, self._args);

                //
                // Callback can return a new promise.
                //
                if(result && typeof result.then === 'function')
                {
                    result.then(
                        function()
                        {
                            var args = arguments;
                            listener.promise.succeed.apply(listener.promise, args);
                        },
                        function()
                        {
                            var args = arguments;
                            listener.promise.fail.apply(listener.promise, args);
                        });
                }
                else
                {
                    listener.promise.succeed(result);
                }
            }
        }
        catch(e)
        {
            listener.promise.fail.call(listener.promise, e);
        }
    };

    Promise.prototype.resolve = function()
    {
        if(this.__state === State.Pending)
        {
            return;
        }

        var obj;
        while((obj = this.__listeners.pop()))
        {
            //
            // We use a separate function here to capture the listeners
            // in the loop.
            //
            resolveImp(this, obj);
        }
    };

    Promise.prototype.progress = function()
    {
        //
        // Don't report progress events after the promise is fulfilled.
        //
        if(this.__state !== State.Pending)
        {
            return;
        }

        var args = arguments;
        var self = this;

        //
        // Use setTimeout so the listener are not notified until the call stack is empty.
        //
        setTimeout(
            function()
            {
                var i, listener;
                for(i = 0; i < self.__listeners.length; ++i)
                {
                    listener = self.__listeners[i];

                    try
                    {
                        if(listener && typeof listener.onProgress === 'function')
                        {
                            listener.onProgress.apply(null, args);
                        }
                    }
                    catch(e)
                    {
                        //TODO ignore or trace a warning when the progress callback throws.
                        console.log(e);
                    }
                }
            }, 0);
    };

    Promise.prototype.setState = function(state, args)
    {
        if(this.__state === State.Pending && state !== State.Pending)
        {
            this.__state = state;
            this._args = args;
            //
            // Use setTimeout so the listeners are not resolved until the call stack is empty.
            //
            var self = this;
            setTimeout(function(){ self.resolve(); }, 0);
        }
    };

    Promise.prototype.succeed = function()
    {
        var args = arguments;
        this.setState(State.Success, args);
    };

    Promise.prototype.fail = function()
    {
        var args = arguments;
        this.setState(State.Failed, args);
    };

    Promise.prototype.succeeded = function()
    {
        return this.__state === State.Success;
    };

    Promise.prototype.failed = function()
    {
        return this.__state === State.Failed;
    };

    Promise.prototype.completed = function()
    {
        return this.__state !== State.Pending;
    };

    //
    // Create a new promise object that is fulfilled when all the promise arguments
    // are fulfilled or is rejected when one of the promises is rejected.
    //
    Promise.all = function()
    {
        var promise = new Promise();
        var promises = Array.prototype.slice.call(arguments);
        var results = new Array(arguments.length);

        var pending = promises.length;
        for(var i = 0; i < promises.length; ++i)
        {
            //
            // Create an anonymous function to capture the loop index
            //
            
            /*jshint -W083 */
            (function(j)
            {
                promises[j].then(
                    function()
                    {
                        results[j] = arguments;
                        pending--;
                        if(pending === 0)
                        {
                            promise.succeed.apply(promise, results);
                        }
                    },
                    function()
                    {
                        promise.fail.apply(promise, arguments);
                    });
            }(i));
            /*jshint +W083 */
        }
        return promise;
    };

    //
    // Create a new promise object and call function fn with
    // the promise as its first argument, then return the new
    // promise.
    //
    Promise.deferred = function(fn)
    {
        var promise = new Promise();
        fn.apply(null, [promise]);
        return promise;
    };

    //
    // Create a new promise, call succeed with the received arguments,
    // then return the new promise.
    //
    Promise.succeed = function()
    {
        var args = arguments;
        var p = new Promise();
        p.succeed.apply(p, args);
        return p;
    };

    //
    // Create a new promise, call fail with the received arguments,
    // then return the new promise.
    //
    Promise.fail = function()
    {
        var args = arguments;
        var p = new Promise();
        p.fail.apply(p, args);
        return p;
    };

    Ice.Promise = Promise;
    global.Ice = Ice;
}());
