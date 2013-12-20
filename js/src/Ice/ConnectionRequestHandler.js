// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Promise = require("./Promise");
var ReferenceMode = require("./ReferenceMode");

var ConnectionRequestHandler = function(ref, connection, compress)
{
    this._reference = ref;
    this._response = ref.getMode() == ReferenceMode.ModeTwoway;
    this._connection = connection;
    this._compress = compress;
};

ConnectionRequestHandler.prototype.prepareBatchRequest = function(out)
{
    this._connection.prepareBatchRequest(out);
};

ConnectionRequestHandler.prototype.finishBatchRequest = function(out)
{
    this._connection.finishBatchRequest(out, this._compress);
};

ConnectionRequestHandler.prototype.abortBatchRequest = function()
{
    this._connection.abortBatchRequest();
};

ConnectionRequestHandler.prototype.sendAsyncRequest = function(out)
{
    this._connection.sendAsyncRequest(out, this._compress, this._response);
};

ConnectionRequestHandler.prototype.flushAsyncBatchRequests = function(out)
{
    this._connection.flushAsyncBatchRequests(out);
};

ConnectionRequestHandler.prototype.getReference = function()
{
    return this._reference;
};

ConnectionRequestHandler.prototype.getConnection = function()
{
    return this._connection;
};

ConnectionRequestHandler.prototype.onConnection = function()
{
    return Promise.succeed(this._connection);
};

module.exports = ConnectionRequestHandler;
