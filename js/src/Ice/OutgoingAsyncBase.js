// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var BasicStream = require("./BasicStream");
var Debug = require("./Debug");
var Ex = require("./Exception");
var Promise = require("./Promise");

var OK = 0x1;
var Done = 0x2;
var Sent = 0x4;

var OutgoingAsyncBase = function(communicator, op, completed)
{
    Promise.call(this);
    Debug.assert(completed !== null && completed !== undefined);
    this._communicator = communicator;
    this._instance = communicator.instance;
    this._operation = op;
    this._completed = completed;
    this._connection = null; // TODO: Necessary?
    this._proxy = null; // TODO: Necessary?
    this._adapter = null; // TODO: Necessary?
    this._is = null;
    this._os = new BasicStream(this._instance, Protocol.currentProtocolEncoding, false);
    this._state = 0;
    this._sentSynchronously = false;
};

OutgoingAsyncBase.prototype = new Promise();
OutgoingAsyncBase.prototype.constructor = OutgoingAsyncBase;

// TODO: Are these properties necessary?
Object.defineProperty(OutgoingAsyncBase.prototype, "communicator", {
    get: function() { return this._communicator; }
});

Object.defineProperty(OutgoingAsyncBase.prototype, "connection", {
    get: function() { return this._connection; }
});

Object.defineProperty(OutgoingAsyncBase.prototype, "proxy", {
    get: function() { return this._proxy; }
});

Object.defineProperty(OutgoingAsyncBase.prototype, "adapter", {
    get: function() { return this._adapter; }
});

Object.defineProperty(OutgoingAsyncBase.prototype, "operation", {
    get: function() { return this._operation; }
});

OutgoingAsyncBase.prototype.__os = function()
{
    return this._os;
};

OutgoingAsyncBase.prototype.__is = function()
{
    return this._is;
};

OutgoingAsyncBase.prototype.__throwUserException = function()
{
    Debug.assert((this._state & Done) !== 0);
    if((this._state & OK) === 0)
    {
        try
        {
            this._is.startReadEncaps();
            this._is.throwException();
        }
        catch(ex)
        {
            if(ex instanceof Ex.UserException)
            {
                this._is.endReadEncaps();
            }
            throw ex;
        }
    }
}

OutgoingAsyncBase.prototype.__exception = function(ex)
{
    this._state |= Done;
    this.fail(ex);
}

OutgoingAsyncBase.prototype.__response = function()
{
    //
    // Note: no need to change the state here, specializations are responsible for
    // changing the state.
    //

    if(this.proxy !== null && this.proxy.ice_isTwoway())
    {
        this._completed(this);
    }
}

module.exports = OutgoingAsyncBase;
