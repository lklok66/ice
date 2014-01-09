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
        
        require("Ice/Promise");
        require("Ice/ReferenceMode");
        
        var Ice = global.Ice || {};
        
        var Promise = Ice.Promise;
        var ReferenceMode = Ice.ReferenceMode;

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
            return this._connection.sendAsyncRequest(out, this._compress, this._response);
        };

        ConnectionRequestHandler.prototype.flushAsyncBatchRequests = function(out)
        {
            return this._connection.flushAsyncBatchRequests(out);
        };

        ConnectionRequestHandler.prototype.getReference = function()
        {
            return this._reference;
        };

        ConnectionRequestHandler.prototype.getConnection = function()
        {
            return this._connection;
        };

        ConnectionRequestHandler.prototype.onConnection = function(r)
        {
            r.succeed(r, this._connection);
        };

        Ice.ConnectionRequestHandler = ConnectionRequestHandler;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/ConnectionRequestHandler"));
