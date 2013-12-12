// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Logger = function(prefix)
{
    if(prefix !== undefined && prefix.length > 0)
    {
        this._prefix = prefix + ": ";
    }
    else
    {
        this._prefix = "";
    }
};

Logger.prototype.print = function(message)
{
    this.write(message, false);
};

Logger.prototype.trace = function(category, message)
{
    var s = [];
    var d = new Date();
    s.push("-- ");
    s.push(this.timestamp());
    s.push(' ');
    s.push(this._prefix);
    s.push(category);
    s.push(": ");
    s.push(message);
    this.write(s.join(""), true);
};

Logger.prototype.warning = function(message)
{
    var s = [];
    var d = new Date();
    s.push("-! ");
    s.push(this.timestamp());
    s.push(' ');
    s.push(this._prefix);
    s.push("warning: ");
    s.push(message);
    this.write(s.join(""), true);
};

Logger.prototype.error = function(message)
{
    var s = [];
    var d = new Date();
    s.push("!! ");
    s.push(this.timestamp());
    s.push(' ');
    s.push(this._prefix);
    s.push("error: ");
    s.push(message);
    this.write(s.join(""), true);
};

Logger.prototype.cloneWithPrefix = function(prefix)
{
    return new Logger(prefix);
};

Logger.prototype.write = function(message, indent)
{
    if(indent)
    {
        message = message.replace(/\n/g, "\n   ");
    }

    /* TODO: Don't need to append a newline when using console.log
    message += "\n";
    */

    // TODO: Is console.log() always available?
    console.log(message);
};

Logger.prototype.timestamp = function()
{
    var d = new Date();
    var mon = d.getMonth() + 1;
    mon = mon < 10 ? "0" + mon : mon;
    var day = d.getDate();
    day = day < 10 ? "0" + day : day;
    // TODO: Include milliseconds?
    return mon + "-" + day + "-" + d.getFullYear() + " " + d.toLocaleTimeString();
};

module.exports = Logger;
