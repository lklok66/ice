// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_FOR_ACTIONSCRIPT_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var global = this;
    require("Ice/BatchOutgoingAsync");

    var Ice = global.Ice || {};

    var BatchOutgoingAsync = Ice.BatchOutgoingAsync;

    var ConnectionBatchOutgoingAsync = function(con, communicator, operation)
    {
        BatchOutgoingAsync.call(this, communicator, operation);
        this._connection = con;
    };

    ConnectionBatchOutgoingAsync.prototype = new BatchOutgoingAsync();
    ConnectionBatchOutgoingAsync.prototype.constructor = ConnectionBatchOutgoingAsync;

    ConnectionBatchOutgoingAsync.prototype.__send = function()
    {
        this._connection.flushAsyncBatchRequests(this);
    };

    Ice.ConnectionBatchOutgoingAsync = ConnectionBatchOutgoingAsync;
    global.Ice = Ice;
}());
