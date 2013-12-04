// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

function Promise()
{
    this._state = Promise.StatePending;
    this._listeners = [];
}

Promise.StatePending = 0;
Promise.StateSuccess = 1;
Promise.StateFailed = 2;

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
            self._listeners.push({promise:promise, onResponse:onResponse, onException:onException, onProgress:onProgress});
            self.resolve();
        }, 0);
    return promise;
}

Promise.prototype.exception = function(onException)
{
    return this.then(null, onException);
}

Promise.prototype.resolve = function()
{   
    if(this._state === Promise.StatePending)
    {
        return;
    }

    var obj;
    while((obj = this._listeners.pop()))
    {
        //
        // We use an anonymous function here to caputre the listeners
        // in the loop.
        //
        (function(self, listener)
        {
            var callback = self._state === Promise.StateSuccess ? listener.onResponse : listener.onException;
            try
            {
                if(typeof callback !== 'function')
                {
                    listener.promise.setState(self._state, self._args);
                }
                else
                {                
                    var result = callback.apply(null, self._args);
                    //
                    // Callback returns a new promise
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
                listener.promise.fail(e);
            }
        })(this, obj);
    }
}

Promise.prototype.progress = function()
{   
    //
    // Don't report progress events after the promise is fulfilled.
    //
    if(this._state !== Promise.StatePending)
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
            for(i = 0; i < self._listeners.length; ++i)
            {
                listener = self._listeners[i];

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
}

Promise.prototype.setState = function(state, args)
{
    if(this._state === Promise.StatePending && state !== Promise.StatePending)
    {
        this._state = state;
        this._args = args;
        //
        // Use setTimeout so the listener are not resolved until the call stack is empty.
        //
        var self = this;
        setTimeout(function(){ self.resolve(); }, 0);
    }
}

Promise.prototype.succeed = function()
{
    if(this._state === Promise.StatePending)
    {
        var args = arguments;
        this.setState(Promise.StateSuccess, args);
    }
}

Promise.prototype.fail = function()
{
    if(this._state === Promise.StatePending)
    {
        var args = arguments;
        this.setState(Promise.StateFailed, args);
    }
}

/**
 * 
 * Create a new promise object that is fulfilled when all the promise arguments
 * are fulfilled or is rejected when one of the promises is rejected.
 * 
 **/
Promise.all = function()
{
    if(arguments.length === 1)
    {
        return arguments[0];
    }
    
    var promise = new Promise();
    var promises = Array.prototype.slice.call(arguments);
    var results = new Array(arguments.length);
    
    var pending = promises.length;
    for(var i = 0; i < promises.length; ++i)
    {
        (function(j)
        {
            promises[j].then(
                function()
                {
                    var args = arguments;
                    results[j] = arguments;
                    pending--;
                    if(pending === 0)
                    {
                        promise.succeed.apply(promise, results);
                    }
                },
                function()
                {
                    var args = arguments;
                    promise.fail.apply(promise, args)
                });
        })(i);
    }
    return promise;
}

/**
 * 
 * Create a new promise object and call function fn with
 * the promise as its first argument, then return the created
 * promise.
 * 
 **/
Promise.deferred = function(fn)
{
    var promise = new Promise();
    fn.apply(null, [promise]);
    return promise;
}

module.exports = Promise;
