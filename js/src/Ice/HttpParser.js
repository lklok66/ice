// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Debug = require("./Debug");
var HashMap = require("./HashMap");

//
// Parser Type.
//
var Type = {};
Type.Unknown = 0;
Type.Request = 1;
Type.Response = 2;

//
// Parser State.
//
var State = {};
State.Init = 0;
State.Type = 1;
State.TypeCheck = 2;
State.Request = 3;
State.RequestMethod = 4;
State.RequestMethodSP = 5;
State.RequestURI = 6;
State.RequestURISP = 7;
State.RequestLF = 8;
State.HeaderFieldStart = 9;
State.HeaderFieldContStart = 10;
State.HeaderFieldCont = 11;
State.HeaderFieldNameStart = 12;
State.HeaderFieldName = 13;
State.HeaderFieldNameEnd = 14;
State.HeaderFieldValueStart = 15;
State.HeaderFieldValue = 16;
State.HeaderFieldValueEnd = 17;
State.HeaderFieldLF = 18;
State.HeaderFieldEndLF = 19;
State.Version = 20;
State.VersionH = 21;
State.VersionHT = 22;
State.VersionHTT = 23;
State.VersionHTTP = 24;
State.VersionMajor = 25;
State.VersionMinor = 26;
State.Response = 27;
State.ResponseVersionSP = 28;
State.ResponseStatus = 29;
State.ResponseReasonStart = 30;
State.ResponseReason = 31;
State.ResponseLF = 32;
State.Complete = 33;

var CR = '\r';
var LF = '\n';

//
// Helper function to read an ASCII string from the buffer.
//
function readString(buffer, offset, length)
{
    var s = "";
    for(var i = 0; i < length; ++i)
    {
        s += String.fromCharCode(buffer.getAt(offset + i));
    }
    return s;
}

var HttpParser = function()
{
    this._type = Type.Unknown;
    this._method = null;
    this._uri = null;
    this._headers = new HashMap(); // map<String, String>;
    this._versionMajor = 0;
    this._versionMinor = 0;
    this._status = 0;
    this._reason = null;
    this._state = State.Init;
};

Object.defineProperty(HttpParser.prototype, "type", {
    get: function() { return this._type; }
});
    
Object.defineProperty(HttpParser.prototype, "method", {
    get: function() { 
        Debug.assert(this._type === Type.Request);
        return this._method; 
    }
});

Object.defineProperty(HttpParser.prototype, "uri", {
    get: function() { 
        Debug.assert(this._type === Type.Request);
        return this._uri;
    }
});

Object.defineProperty(HttpParser.prototype, "headers", {
    get: function() { return this._headers; }
});

Object.defineProperty(HttpParser.prototype, "versionMajor", {
    get: function() { return this._versionMajor; }
});

Object.defineProperty(HttpParser.prototype, "versionMinor", {
    get: function() { return this._versionMinor; }
});

Object.defineProperty(HttpParser.prototype, "status", {
    get: function() { return this._status; }
});

Object.defineProperty(HttpParser.prototype, "reason", {
    get: function() { return this._reason; }
});

HttpParser.prototype.getHeader = function(name, toLower)
{
    var value = this._headers.find(name.toLowerCase());
    if(value === undefined || value === null)
    {
        return undefined;
    }
    else
    {
        return toLower ? value.trim().toLowerCase() : value.trim();
    }
};

HttpParser.prototype.isCompleteMessage = function(buffer, begin, end)
{
    var p = begin;

    //
    // Skip any leading CR-LF characters.
    //
    var c;
    
    while(p < end)
    {
        c = String.fromCharCode(buffer.getAt(p));
        if(c !== CR && c !== LF)
        {
            break;
        }
        ++p;
    }
    
    //
    // Look for adjacent CR-LF/CR-LF or LF/LF.
    //
    var seenFirst = false;
    while(p < end)
    {
        c = String.fromCharCode(buffer.getAt(p++));
        process.stdout.write(c);
        if(c === LF)
        {
            if(seenFirst)
            {
                return p;
            }
            else
            {
                seenFirst = true;
            }
        }
        else if(c !== CR)
        {
            seenFirst = false;
        }
    }

    return 0;
};

/* jshint -W086 */
HttpParser.prototype.parse = function(buffer, begin)
{
    if(this._state == State.Complete)
    {
        this._state = State.Init;
    }
    
    var p = begin,
        start,
        value, newValue;
    while(p < buffer.limit && this._state != State.Complete)
    {
        var c = String.fromCharCode(buffer.getAt(p));
        switch(this._state)
        {
            case State.Init:
            {
                this._method = "";
                this._uri = "";
                this._versionMajor = -1;
                this._versionMinor = -1;
                this._status = -1;
                this._reason = "";
                this._headers.clear();
                this._state = State.Type;
                continue;
            }
            case State.Type:
            {
                if(c === CR || c === LF)
                {
                    break;
                }
                else if(c === 'H')
                {
                    //
                    // Could be the start of "HTTP/1.1" or "HEAD".
                    //
                    this._state = State.TypeCheck;
                    break;
                }
                else
                {
                    this._state = State.Request;
                    continue;
                }
            }
            case State.TypeCheck:
            {
                if(c === 'T') // Continuing "H_T_TP/1.1"
                {
                    this._state = State.Response;
                }
                else if(c === 'E') // Expecting "HEAD"
                {
                    this._state = State.Request;
                    this._method += 'H';
                    this._method += 'E';
                }
                else
                {
                    throw new Error("malformed request or response");
                }
                break;
            }
            case State.Request:
            {
                this._type = Type.Request;
                this._state = State.RequestMethod;
                continue;
            }
            case State.RequestMethod:
            {
                if(c === ' ' || c === CR || c === LF)
                {
                    this._state = State.RequestMethodSP;
                    continue;
                }
                this._method += c;
                break;
            }
            case State.RequestMethodSP:
            {
                if(c === ' ')
                {
                    break;
                }
                else if(c === CR || c === LF)
                {
                    throw new Error("malformed request");
                }
                this._state = State.RequestURI;
                continue;
            }
            case State.RequestURI:
            {
                if(c === ' ' || c === CR || c === LF)
                {
                    this._state = State.RequestURISP;
                    continue;
                }
                this._uri += c;
                break;
            }
            case State.RequestURISP:
            {
                if(c === ' ')
                {
                    break;
                }
                else if(c === CR || c === LF)
                {
                    throw new Error("malformed request");
                }
                this._state = State.Version;
                continue;
            }
            case State.RequestLF:
            {
                if(c !== LF)
                {
                    throw new Error("malformed request");
                }
                this._state = State.HeaderFieldStart;
                break;
            }
            case State.HeaderFieldStart:
            {
                //
                // We've already seen a LF to reach this state.
                //
                // Another CR or LF indicates the end of the header fields.
                //
                if(c === CR)
                {
                    this._state = State.HeaderFieldEndLF;
                    break;
                }
                else if(c === LF)
                {
                    this._state = State.Complete;
                    break;
                }
                else if(c === ' ')
                {
                    //
                    // Could be a continuation line.
                    //
                    this._state = State.HeaderFieldContStart;
                    break;
                }

                this._state = State.HeaderFieldNameStart;
                continue;
            }
            case State.HeaderFieldContStart:
            {
                if(c === ' ')
                {
                    break;
                }

                this._state = State.HeaderFieldCont;
                start = p;
                continue;
            }
            case State.HeaderFieldCont:
            {
                if(c === CR || c === LF)
                {
                    if(p > start)
                    {
                        if(this._headerName === '')
                        {
                            throw new Error("malformed header");
                        }
                        value = this._headers.get(this._headerName);
                        Debug.assert(value !== undefined);
                        newValue = value + " " + readString(buffer, start, p - start);
                        
                        this._headers.set(this._headerName, newValue);
                        this._state = c === CR ? State.HeaderFieldLF : State.HeaderFieldStart;
                    }
                    else
                    {
                        //
                        // Could mark the end of the header fields.
                        //
                        this._state = c === CR ? State.HeaderFieldEndLF : State.Complete;
                    }
                }

                break;
            }
            case State.HeaderFieldNameStart:
            {
                Debug.assert(c !== ' ');
                start = p;
                this._headerName = "";
                this._state = State.HeaderFieldName;
                continue;
            }
            case State.HeaderFieldName:
            {
                if(c === ' ' || c === ':')
                {
                    this._state = State.HeaderFieldNameEnd;
                    continue;
                }
                else if(c === CR || c === LF)
                {
                    throw new Error("malformed header");
                }
                break;
            }
            case State.HeaderFieldNameEnd:
            {
                if(this._headerName.length === 0)
                {
                    this._headerName =  readString(buffer, start, p - start).toLowerCase();
                    value = this._headers.get(this._headerName);
                    //
                    // Add a placeholder entry if necessary.
                    //
                    if(value === undefined)
                    {
                        this._headers.set(this._headerName, "");
                    }
                }

                if(c === ' ')
                {
                    break;
                }
                else if(c !== ':' || p === start)
                {
                    throw new Error("malformed header");
                }

                this._state = State.HeaderFieldValueStart;
                break;
            }
            case State.HeaderFieldValueStart:
            {
                if(c === ' ')
                {
                    break;
                }

                //
                // Check for "Name:\r\n"
                //
                if(c === CR)
                {
                    this._state = State.HeaderFieldLF;
                    break;
                }
                else if(c === LF)
                {
                    this._state = State.HeaderFieldStart;
                    break;
                }

                start = p;
                this._state = State.HeaderFieldValue;
                continue;
            }
            case State.HeaderFieldValue:
            {
                if(c === CR || c === LF)
                {
                    this._state = State.HeaderFieldValueEnd;
                    continue;
                }
                break;
            }
            case State.HeaderFieldValueEnd:
            {
                Debug.assert(c === CR || c === LF);
                if(p > start)
                {
                    value = this._headers.get(this._headerName);
                    if(value === undefined || value === "")
                    {
                        this._headers.set(this._headerName, readString(buffer, start, p - start));
                    }
                    else
                    {
                        this._headers.set(this._headerName, value + ", " + readString(buffer, start, p - start));
                    }
                }

                if(c === CR)
                {
                    this._state = State.HeaderFieldLF;
                }
                else
                {
                    this._state = State.HeaderFieldStart;
                }
                break;
            }
            case State.HeaderFieldLF:
            {
                if(c !== LF)
                {
                    throw new Error("malformed header");
                }
                this._state = State.HeaderFieldStart;
                break;
            }
            case State.HeaderFieldEndLF:
            {
                if(c !== LF)
                {
                    throw new Error("malformed header");
                }
                this._state = State.Complete;
                break;
            }
            case State.Version:
            {
                if(c !== 'H')
                {
                    throw new Error("malformed version");
                }
                this._state = State.VersionH;
                break;
            }
            case State.VersionH:
            {
                if(c !== 'T')
                {
                    throw new Error("malformed version");
                }
                this._state = State.VersionHT;
                break;
            }
            case State.VersionHT:
            {
                if(c !== 'T')
                {
                    throw new Error("malformed version");
                }
                this._state = State.VersionHTT;
                break;
            }
            case State.VersionHTT:
            {
                if(c !== 'P')
                {
                    throw new Error("malformed version");
                }
                this._state = State.VersionHTTP;
                break;
            }
            case State.VersionHTTP:
            {
                if(c !== '/')
                {
                    throw new Error("malformed version");
                }
                this._state = State.VersionMajor;
                break;
            }
            case State.VersionMajor:
            {
                if(c === '.')
                {
                    if(this._versionMajor == -1)
                    {
                        throw new Error("malformed version");
                    }
                    this._state = State.VersionMinor;
                    break;
                }
                else if(c < '0' || c > '9')
                {
                    throw new Error("malformed version");
                }
                if(this._versionMajor === -1)
                {
                    this._versionMajor = 0;
                }
                this._versionMajor *= 10;
                this._versionMajor += (c - '0');
                break;
            }
            case State.VersionMinor:
            {
                if(c === CR)
                {
                    if(this._versionMinor === -1 || this._type !== Type.Request)
                    {
                        throw new Error("malformed version");
                    }
                    this._state = State.RequestLF;
                    break;
                }
                else if(c === LF)
                {
                    if(this._versionMinor === -1 || this._type !== Type.Request)
                    {
                        throw new Error("malformed version");
                    }
                    this._state = State.HeaderFieldStart;
                    break;
                }
                else if(c === ' ')
                {
                    if(this._versionMinor === -1 || this._type !== Type.Response)
                    {
                        throw new Error("malformed version");
                    }
                    this._state = State.ResponseVersionSP;
                    break;
                }
                else if(c < '0' || c > '9')
                {
                    throw new Error("malformed version");
                }
                if(this._versionMinor === -1)
                {
                    this._versionMinor = 0;
                }
                this._versionMinor *= 10;
                this._versionMinor += (c - '0');
                break;
            }
            case State.Response:
            {
                this._type = Type.Response;
                this._state = State.VersionHT;
                continue;
            }
            case State.ResponseVersionSP:
            {
                if(c === ' ')
                {
                    break;
                }

                this._state = State.ResponseStatus;
                continue;
            }
            case State.ResponseStatus:
            {
                // TODO: Is reason string optional?
                if(c === CR)
                {
                    if(this._status === -1)
                    {
                        throw new Error("malformed response status");
                    }
                    this._state = State.ResponseLF;
                    break;
                }
                else if(c === LF)
                {
                    if(this._status === -1)
                    {
                        throw new Error("malformed response status");
                    }
                    this._state = State.HeaderFieldStart;
                    break;
                }
                else if(c === ' ')
                {
                    if(this._status == -1)
                    {
                        throw Error("malformed response status");
                    }
                    this._state = State.ResponseReasonStart;
                    break;
                }
                else if(c < '0' || c > '9')
                {
                    throw new Error("malformed response status");
                }
                if(this._status === -1)
                {
                    this._status = 0;
                }
                this._status *= 10;
                this._status += (c - '0');
                break;
            }
            case State.ResponseReasonStart:
            {
                //
                // Skip leading spaces.
                //
                if(c === ' ')
                {
                    break;
                }

                this._state = State.ResponseReason;
                start = p;
                continue;
            }
            case State.ResponseReason:
            {
                if(c === CR || c === LF)
                {
                    if(p > start)
                    {
                        this._reason = readString(buffer, start, p - start);
                    }
                    this._state = c == CR ? State.ResponseLF : State.HeaderFieldStart;
                }

                break;
            }
            case State.ResponseLF:
            {
                if(c !== LF)
                {
                    throw new Error("malformed status line");
                }
                this._state = State.HeaderFieldStart;
                break;
            }
            case State.Complete:
            {
                Debug.assert(false); // Shouldn't reach
            }
        }
        ++p;
    }
    return this._state === State.Complete;
};
/* jshint +W086 */

module.exports = HttpParser;
