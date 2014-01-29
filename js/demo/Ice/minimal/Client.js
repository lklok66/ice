// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    
    require("Ice/Ice");
    require("Hello");
    
    var communicator = Ice.initialize();
    
    var cleanup = function(ex)
    {
        if(ex instanceof Error)
        {
            console.log(ex.stack);
        }
        if(communicator !== null)
        {
            var c = communicator;
            communicator = null;
            return c.destroy();
        }
    };
    
    var exceptionCB = function(ex) { throw ex; };
    
    Demo.HelloPrx.checkedCast(communicator.stringToProxy("hello:tcp -h localhost -p 10000")).then(
        function(asyncResult, hello)
        {
            return hello.sayHello();
        },
        exceptionCB
    ).then(cleanup, exceptionCB).exception(cleanup);
}());
