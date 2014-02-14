// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(global){
    var Ice = global.Ice;
    var Test = global.Test;

    var test = function(b)
    {
        if(!b)
        {
            throw new Error("test failed");
        }
    };

    var run = function(communicator, p)
    {
        Ice.Promise.try(
            function()
            {
                p = p.ice_oneway();
                p.ice_ping();
            }
        ).then(
            function()
            {
                try
                {
                    p.ice_isA(Test.MyClass.ice_staticId());
                    test(false);
                }
                catch(ex)
                {
                    // Expected: twoway proxy required
                }
                try
                {
                    p.ice_id();
                    test(false);
                }
                catch(ex)
                {
                    // Expected: twoway proxy required
                }
                try
                {
                    p.ice_ids();
                    test(false);
                }
                catch(ex)
                {
                    // Expected: twoway proxy required
                }
                
                return p.opVoid();
            }
        ).then(
            function()
            {
                return p.opIdempotent();
            }
        ).then(
            function()
            {
                return p.opNonmutating();
            }
        ).then(
            function()
            {
                try
                {
                    p.opByte(0xff, 0x0f);
                    test(false);
                }
                catch(ex)
                {
                    // Expected: twoway proxy required
                }
            }
        );
    };

    global.Oneways = { run: run };
}(typeof (global) === "undefined" ? window : global));
