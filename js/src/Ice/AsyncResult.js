// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Ice.AsyncResult
//
var AsyncResult = function(communicator, connection, proxy, adapter, op)
{
    this.communicator = communicator;
    this.connection = connection;
    this.proxy = proxy;
    this.adapter = adapter;
    this.operation = op;
    this.completed = false;
    this.exception = null;
    this.responseCallback = null;
    this.exceptionCallback = null;
    this.cbContext = null;
    this.cookie = null;
};

AsyncResult.prototype.getCommunicator = function()
{
    return this.communicator;
};

AsyncResult.prototype.getConnection = function()
{
    return this.connection;
};

AsyncResult.prototype.getProxy = function()
{
    return this.proxy;
};

AsyncResult.prototype.getAdapter = function()
{
    return this.adapter;
};

AsyncResult.prototype.getOperation = function()
{
    return this.operation;
};

AsyncResult.prototype.getCookie = function()
{
    return this.cookie;
};

AsyncResult.prototype.isCompleted = function()
{
    return this.completed;
};

AsyncResult.prototype.throwLocalException = function()
{
    if(this.exception !== null)
    {
        throw this.exception;
    }
};

AsyncResult.prototype.whenCompleted = function(responseCallback, exceptionCallback, cbContext, cookie)
{
    if(this.responseCallback !== null || this.exceptionCallback !== null || this.cbContext !== null ||
       this.cookie !== null)
    {
        throw new Error("whenCompleted already invoked");
    }

    this.responseCallback = responseCallback === undefined ? null : responseCallback;
    this.exceptionCallback = exceptionCallback === undefined ? null : exceptionCallback;
    this.cbContext = cbContext === undefined ? null : cbContext;
    this.cookie = cookie === undefined ? null : cookie;

    //
    // The request may have already completed before the application calls whenCompleted.
    //
    if(this.completed)
    {
        this.__finished();
    }
};

AsyncResult.prototype.__exception = function(ex)
{
    this.completed = true;
    this.exception = ex;
    this.__finished();
};

AsyncResult.prototype.__finished = function()
{
    if(this.exception !== null)
    {
        if(this.exceptionCallback !== null)
        {
            try
            {
                this.exceptionCallback.call(this.cbContext === null ? this.exceptionCallback : this.cbContext,
                                            this.exception);
            }
            catch(ex)
            {
                this.__warning(ex);
            }
        }
    }
    else if(this.responseCallback !== null)
    {
        try
        {
            this.__response();
        }
        catch(ex)
        {
            this.__warning(ex);
        }
    }
};

AsyncResult.prototype.__warning = function(ex)
{
    console.log(ex);
    /* TODO
    const msg:String = "exception raised by AMI callback:\n" + Ex.toString(ex);
    if(_communicator !== null &&
       _communicator.getProperties().getPropertyAsIntWithDefault("Ice.Warn.AMICallback", 1) > 0)
    {
        _communicator.getLogger().warning(msg);
    }
    else if(_communicator === null)
    {
        Ice.Util.getProcessLogger().warning(msg);
    }
    */
};

module.exports = AsyncResult;
