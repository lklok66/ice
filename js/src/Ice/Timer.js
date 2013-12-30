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
        
        require("Ice/HashMap");
        require("Ice/TimeUtil");
        require("Ice/LocalException");
        
        var Ice = global.Ice || {};
        
        var HashMap = Ice.HashMap;
        var TimeUtil = Ice.TimeUtil;
        var CommunicatorDestroyedException = Ice.CommunicatorDestroyedException;

        var Timer = function(instance)
        {
            this._instance = instance;
            this._destroyed = false;
            this._tokenId = 0;
            this._tokens = new HashMap();
        }

        Timer.prototype.destroy = function()
        {
            this._destroyed = true;
            this._tokens.clear();
        }

        Timer.prototype.schedule = function(callback, delay)
        {
            if(this._destroyed)
            {
                throw new CommunicatorDestroyedException();
            }

            var token = this._tokenId++;
            var self = this;

            // TODO: Need portability fixes for browsers?
            var id = setTimeout(function() { self.handleTimeout(token); }, delay);
            this._tokens.set(token, { callback: callback, id: id, isInterval: false });

            return token;
        }

        Timer.prototype.scheduleRepeated = function(callback, period)
        {
            if(this._destroyed)
            {
                throw new CommunicatorDestroyedException();
            }

            var token = this._tokenId++;
            var self = this;

            // TODO: Need portability fixes for browsers?
            var id = setInterval(function() { self.handleInterval(token); }, period);
            this._tokens.set(token, { callback: callback, id: id, isInterval: true });

            return token;
        }

        Timer.prototype.cancel = function(id)
        {
            if(this._destroyed)
            {
                return false;
            }

            var token = this._tokens.get(id);
            if(token === undefined)
            {
                return false;
            }

            this._tokens.delete(id);
            if(token.isInterval)
            {
                clearInterval(token.id);
            }
            else
            {
                clearTimeout(token.id);
            }

            return true;
        }

        Timer.prototype.handleTimeout = function(id)
        {
            if(this._destroyed)
            {
                return;
            }

            var token = this._tokens.get(id);
            if(token !== undefined)
            {
                this._tokens.delete(id);
                token.callback();
            }
        }

        Timer.prototype.handleInterval = function(id)
        {
            if(this._destroyed)
            {
                return;
            }

            var token = this._tokens.get(id);
            if(token !== undefined)
            {
                token.callback();
            }
        }

        Ice.Timer = Timer;
        global.Ice = Ice;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Ice/Timer"));
