// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){

        require("Ice/Debug");
        require("Ice/Promise");
        require("Ice/Exception");

        var Ice = global.Ice || {};

        var Debug = Ice.Debug;
        var Promise = Ice.Promise;

        var AsyncResultBase = function(communicator, op, connection, proxy, adapter)
        {
            //
            // AsyncResultBase can be constructed by a sub-type's prototype, in which case the
            // arguments are undefined.
            //
            Promise.call(this);
            if(communicator !== undefined)
            {
                this._communicator = communicator;
                this._instance = communicator !== null ? communicator.instance : null;
                this._operation = op;
                this._connection = connection;
                this._proxy = proxy;
                this._adapter = adapter;
            }
        };

        AsyncResultBase.prototype = new Promise();
        AsyncResultBase.prototype.constructor = AsyncResultBase;

        //
        // Intercept the call to fail() so that we can attach a reference to "this"
        // to the exception.
        //
        AsyncResultBase.prototype.fail = function()
        {
            var args = arguments;
            if(args.length > 0 && args[0] instanceof Error)
            {
                args[0]._asyncResult = this;
            }
            Promise.prototype.fail.apply(this, args);
        };

        Object.defineProperty(AsyncResultBase.prototype, "communicator", {
            get: function() { return this._communicator; }
        });

        Object.defineProperty(AsyncResultBase.prototype, "connection", {
            get: function() { return this._connection; }
        });

        Object.defineProperty(AsyncResultBase.prototype, "proxy", {
            get: function() { return this._proxy; }
        });

        Object.defineProperty(AsyncResultBase.prototype, "adapter", {
            get: function() { return this._adapter; }
        });

        Object.defineProperty(AsyncResultBase.prototype, "operation", {
            get: function() { return this._operation; }
        });

        AsyncResultBase.prototype.__exception = function(ex)
        {
            this.fail(ex);
        };

        Ice.AsyncResultBase = AsyncResultBase;

        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/AsyncResultBase"));
