// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    
    require("Ice/Class");
    
    var global = this;
    var Ice = global.Ice || {};
    
    var ConnectionReaper = Ice.__defineClass({
        __init__: function()
        {
            this._connections = [];
        },
        add: function(connection)
        {
            this._connections.push(connection);
        },
        swapConnections: function()
        {
            if(this._connections.length === 0)
            {
                return null;
            }
            var connections = this._connections;
            this._connections = [];
            return connections;
        }
    });
    
    Ice.ConnectionReaper = ConnectionReaper;
    global.Ice = Ice;
}());
