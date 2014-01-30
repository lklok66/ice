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
    
    require("Ice/AsyncResult");
    require("Ice/AsyncStatus");
    require("Ice/BatchOutgoingAsync");
    require("Ice/Protocol");

    var Ice = global.Ice || {};

    var AsyncResult = Ice.AsyncResult;
    var AsyncStatus = Ice.AsyncStatus;
    var BatchOutgoingAsync = Ice.BatchOutgoingAsync;
    var Protocol = Ice.Protocol;

    var ProxyBatchOutgoingAsync = function(prx, operation)
    {
        BatchOutgoingAsync.call(this, prx.ice_getCommunicator(), operation);
        this._proxy = prx;
    };

    ProxyBatchOutgoingAsync.prototype = new BatchOutgoingAsync();
    ProxyBatchOutgoingAsync.prototype.constructor = ProxyBatchOutgoingAsync;

    ProxyBatchOutgoingAsync.prototype.__send = function()
    {
        Protocol.checkSupportedProtocol(this._proxy.__reference().getProtocol());

        //
        // We don't automatically retry if ice_flushBatchRequests fails. Otherwise, if some batch
        // requests were queued with the connection, they would be lost without being noticed.
        //
        var handler = null;
        var cnt = -1; // Don't retry.
        try
        {
            handler = this._proxy.__getRequestHandler();
            handler.flushAsyncBatchRequests(this);
        }
        catch(__ex)
        {
            cnt = this._proxy.__handleException(handler, __ex, 0, cnt);
        }
    };

    Ice.ProxyBatchOutgoingAsync = ProxyBatchOutgoingAsync;
    global.Ice = Ice;
}());
