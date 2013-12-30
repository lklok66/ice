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
        
        var Ice = global.Ice || {};

        //
        // Ice.Exception
        //
        var Exception = function(cause)
        {
            this.ice_cause = cause;
        };
        
        Exception.captureStackTrace = function(object)
        {
            var stack = new Error().stack;
            
            var formattedStack;
            
            //
            // In IE 10 and greater the stack will be filled once the Error is throw
            // we don't need to do anything.
            //
            if(stack !== undefined)
            {
                Object.defineProperty(object, "stack", {
                    get: function(){
                        if(formattedStack === undefined)
                        {
                            //
                            // Format the stack
                            //
                            var lines = stack.split("\n");
                            for(var i = 0; i < lines.length; ++i)
                            {
                                if(lines[i].indexOf(object.__name) !== -1)
                                {
                                    if(i < lines.length)
                                    {
                                        lines = lines.slice(i + 1);
                                        break;
                                    }
                                }
                            }
                            formattedStack = object.__name + ": " + object.message + "\n"; 
                            formattedStack += lines.join("\n");
                        }
                        return formattedStack;
                    }
                });
            }
        };

        Exception.prototype = new Error();
        Exception.prototype.constructor = Exception;

        Exception.prototype.ice_name = function()
        {
            return "Ice::Exception";
        };

        Exception.prototype.toString = function()
        {
            return this.ice_name();
        };
        
        Object.defineProperty(Exception.prototype, "__name", {
            configurable:true,
            get:function(){ return "Ice.Exception"; }
        });

        Ice.Exception = Exception;

        //
        // Ice.LocalException
        //
        var LocalException = function(cause)
        {
            Exception.call(this, cause);
            Exception.captureStackTrace(this);
        };

        LocalException.prototype = new Exception();
        LocalException.prototype.constructor = LocalException;

        LocalException.prototype.ice_name = function()
        {
            return "Ice::LocalException";
        };
        
        Object.defineProperty(LocalException.prototype, "__name", {
            configurable:true,
            get:function(){ return "Ice.LocalException"; }
        });

        Ice.LocalException = LocalException;

        //
        // Ice.UserException
        //
        var UserException = function(cause)
        {
            Exception.call(this, cause);
            Exception.captureStackTrace(this);
        };

        UserException.prototype = new Exception();
        UserException.prototype.constructor = UserException;

        UserException.prototype.ice_name = function()
        {
            return "Ice::UserException";
        };

        UserException.prototype.__write = function(os)
        {
            os.startWriteException(null);
            this.__writeImpl(os);
            os.endWriteException();
        };

        UserException.prototype.__read = function(is)
        {
            is.startReadException();
            this.__readImpl(is);
            is.endReadException(false);
        };
        
        Object.defineProperty(UserException.prototype, "__name", {
            configurable:true,
            get:function(){ return "Ice.UserException"; }
        });
        
        Ice.UserException = UserException;

        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Exception"));
