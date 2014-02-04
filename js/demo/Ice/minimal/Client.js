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
    
    Demo.HelloPrx.checkedCast(communicator.stringToProxy("hello:tcp -h localhost -p 10000")).then(
        function(asyncResult, hello)
        {
            return hello.sayHello();
        }
    ).then(
        function(asyncResult)
        {
            var c = communicator;
            communicator =  null;
            return c.destroy();
        }
    ).exception(
        function(ex)
        {
            if(communicator !== null)
            {
                communicator.destroy();
            }
            console.log(ex.toString());
        }
    );
}());
