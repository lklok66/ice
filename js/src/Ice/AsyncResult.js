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

        require("Ice/AsyncResultBase");
        require("Ice/Debug");
        require("Ice/Promise");
        require("Ice/Protocol");
        require("Ice/Exception");
        require("Ice/BasicStream");

        var Ice = global.Ice || {};

        var AsyncResultBase = Ice.AsyncResultBase;
        var Debug = Ice.Debug;
        var Promise = Ice.Promise;
        var Protocol = Ice.Protocol;
        var UserException = Ice.UserException;
        var BasicStream = Ice.BasicStream;

        var AsyncResult = function(communicator, op, connection, proxy, adapter, completedFn, sentFn)
        {
            //
            // AsyncResult can be constructed by a sub-type's prototype, in which case the
            // arguments are undefined.
            //
            AsyncResultBase.call(this, communicator, op, connection, proxy, adapter);
            if(communicator !== undefined)
            {
                this._completed = completedFn;
                this._sent = sentFn;
                this._is = null;
                this._os = communicator !== null ?
                    new BasicStream(this._instance, Protocol.currentProtocolEncoding, false) : null;
                this._state = 0;
                this._exception = null;
            }
        };

        AsyncResult.prototype = new AsyncResultBase();
        AsyncResult.prototype.constructor = AsyncResult;

        AsyncResult.OK = 0x1;
        AsyncResult.Done = 0x2;
        AsyncResult.Sent = 0x4;

        AsyncResult.prototype.__os = function()
        {
            return this._os;
        };

        AsyncResult.prototype.__is = function()
        {
            return this._is;
        };

        AsyncResult.prototype.__startReadParams = function()
        {
            this._is.startReadEncaps();
            return this._is;
        };

        AsyncResult.prototype.__endReadParams = function()
        {
            this._is.endReadEncaps();
        };

        AsyncResult.prototype.__readEmptyParams = function()
        {
            this._is.skipEmptyEncaps(null);
        };

        AsyncResult.prototype.__readParamEncaps = function()
        {
            return this._is.readEncaps(null);
        };

        AsyncResult.prototype.__throwUserException = function()
        {
            Debug.assert((this._state & AsyncResult.Done) !== 0);
            if((this._state & AsyncResult.OK) === 0)
            {
                try
                {
                    this._is.startReadEncaps();
                    this._is.throwException();
                }
                catch(ex)
                {
                    if(ex instanceof UserException)
                    {
                        this._is.endReadEncaps();
                    }
                    throw ex;
                }
            }
        };

        AsyncResult.prototype.__exception = function(ex)
        {
            this._state |= AsyncResult.Done;
            this.fail(ex);
        };

        AsyncResult.prototype.__response = function()
        {
            //
            // Note: no need to change the state here, specializations are responsible for
            // changing the state.
            //

            if(this.proxy !== null && this.proxy.ice_isTwoway())
            {
                Debug.assert(this._completed !== null);
                this._completed(this);
            }
        };

        Ice.AsyncResult = AsyncResult;

        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/AsyncResult"));
