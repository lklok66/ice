// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

require("Ice/Ice");

var Ice = global.Ice || {};

var id = new Ice.InitializationData();
id.properties = Ice.createProperties();
id.properties.setProperty("Hello.Proxy", "hello:tcp -h 127.0.0.1 -p 10000");

var Client = require("./Client").Demo.Client;

var c = new Client();

c.run(id).then(
    function(){ c.destroy().then(function() { console.log("destroy called"); }).exception(function(ex) { console.log(ex); } ); },
    function(ex){
        console.log(ex.stack);
        c.destroy().exception(function(ex) { console.log(ex); } );
    });
