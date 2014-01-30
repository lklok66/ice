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
    
    var ConnectionReaper = function()
    {
        this._connections = [];
    };

    ConnectionReaper.prototype.add = function(connection)
    {
        this._connections.push(connection);
    };

    ConnectionReaper.prototype.swapConnections = function()
    {
        if(this._connections.length === 0)
        {
            return null;
        }
        var connections = this._connections;
        this._connections = [];
        return connections;
    };

    Ice.ConnectionReaper = ConnectionReaper;
    global.Ice = Ice;
}());
