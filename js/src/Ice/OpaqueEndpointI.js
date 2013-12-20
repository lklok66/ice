// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Base64 = require("./Base64").Ice.Base64;
var Debug = require("./Debug").Ice.Debug;
var HashUtil = require("./HashUtil").Ice.HashUtil;
var Protocol = require("./Protocol").Ice.Protocol;
var StringUtil = require("./StringUtil").Ice.StringUtil;

var _merge = require("Ice/Util").merge;

var Ice = {};
_merge(Ice, require("./Endpoint").Ice);
_merge(Ice, require("./LocalException").Ice);

var OpaqueEndpointI = function()
{
}

OpaqueEndpointI.prototype = new Ice.Endpoint();
OpaqueEndpointI.prototype.constructor = OpaqueEndpointI;

OpaqueEndpointI.fromString = function(str)
{
    var result = new OpaqueEndpointI();

    result._rawEncoding = Protocol.Encoding_1_0;

    var topt = 0;
    var vopt = 0;

    var arr = str.split(/[ \t\n\r]+/);
    var i = 0;
    while(i < arr.length)
    {
        if(arr[i].length == 0)
        {
            i++;
            continue;
        }

        var option = arr[i++];
        if(option.length != 2 && option.charAt(0) != '-')
        {
            throw new Ice.EndpointParseException("expected an endpoint option but found `" + option +
                                                     "' in endpoint `opaque " + str + "'");
        }

        var argument = null;
        if(i < arr.length && arr[i].charAt(0) != '-')
        {
            argument = arr[i++];
        }

        switch(option.charAt(1))
        {
            case 't':
            {
                if(argument === null)
                {
                    throw new Ice.EndpointParseException("no argument provided for -t option in endpoint `opaque "
                                                             + str + "'");
                }

                var type;

                try
                {
                    type = StringUtil.toInt(argument);
                }
                catch(ex)
                {
                    throw new Ice.EndpointParseException("invalid type value `" + argument +
                                                             "' in endpoint `opaque " + str + "'");
                }

                if(type < 0 || type > 65535)
                {
                    throw new Ice.EndpointParseException("type value `" + argument +
                                                             "' out of range in endpoint `opaque " + str + "'");
                }

                result._type = type;
                ++topt;
                if(topt > 1)
                {
                    throw new Ice.EndpointParseException("multiple -t options in endpoint `opaque " + str + "'");
                }
                break;
            }

            case 'v':
            {
                if(argument === null || argument.length === 0)
                {
                    throw new Ice.EndpointParseException("no argument provided for -v option in endpoint `opaque "
                                                             + str + "'");
                }
                for(var j = 0; j < argument.length; ++j)
                {
                    if(!Base64.isBase64(argument.charAt(j)))
                    {
                        throw new Ice.EndpointParseException("invalid base64 character `" + argument.charAt(j) +
                                                                 "' (ordinal " + argument.charCodeAt(j) +
                                                                 ") in endpoint `opaque " + str + "'");
                    }
                }
                result._rawBytes = Base64.decode(argument);
                ++vopt;
                if(vopt > 1)
                {
                    throw new Ice.EndpointParseException("multiple -v options in endpoint `opaque " + str + "'");
                }
                break;
            }

            case 'e':
            {
                if(argument === null)
                {
                    throw new Ice.EndpointParseException("no argument provided for -e option in endpoint `opaque "
                                                             + str + "'");
                }
                try
                {
                    result._rawEncoding = Protocol.stringToEncodingVersion(argument);
                }
                catch(e)
                {
                    throw new Ice.EndpointParseException("invalid encoding version `" + argument +
                                                             "' in endpoint `opaque " + str + "':\n" + e.str);
                }
                break;
            }

            default:
            {
                throw new Ice.EndpointParseException("invalid option `" + option + "' in endpoint `opaque " +
                                                         str + "'");
            }
        }
    }

    if(topt != 1)
    {
        throw new Ice.EndpointParseException("no -t option in endpoint `opaque " + str + "'");
    }
    if(vopt != 1)
    {
        throw new Ice.EndpointParseException("no -v option in endpoint `opaque " + str + "'");
    }
    result.calcHashValue();
    return result;
}

OpaqueEndpointI.fromStream = function(type, s)
{
    var result = new OpaqueEndpointI();
    result._type = type;
    result._rawEncoding = s.startReadEncaps();
    var sz = s.getReadEncapsSize();
    result._rawBytes = s.readBlob(sz);
    s.endReadEncaps();
    result.calcHashValue();
    return result;
}

//
// Marshal the endpoint
//
OpaqueEndpointI.prototype.streamWrite = function(s)
{
    s.writeShort(this._type);
    s.startWriteEncaps(this._rawEncoding, DefaultFormat); // TODO
    s.writeBlob(this._rawBytes);
    s.endWriteEncaps();
}

//
// Convert the endpoint to its string form
//
OpaqueEndpointI.prototype.toString = function()
{
    var val = Base64.encode(this._rawBytes);
    return "opaque -t " + this._type + " - e " + Protocol.encodingVersionToString(this._rawEncoding) + " -v " + val;
}

//
// Return the endpoint information.
//
OpaqueEndpointI.prototype.getInfo = function()
{
    return new OpaqueEndpointInfoI(-1, false, this._rawEncoding, this._rawBytes, this._type);
}

//
// Return the endpoint type
//
OpaqueEndpointI.prototype.type = function()
{
    return this._type;
}

OpaqueEndpointI.prototype.protocol = function()
{
    return "opaque";
}

//
// Return the timeout for the endpoint in milliseconds. 0 means
// non-blocking, -1 means no timeout.
//
OpaqueEndpointI.prototype.timeout = function()
{
    return -1;
}

//
// Return a new endpoint with a different timeout value, provided
// that timeouts are supported by the endpoint. Otherwise the same
// endpoint is returned.
//
OpaqueEndpointI.prototype.changeTimeout = function(t)
{
    return this;
}

//
// Return a new endpoint with a different connection id.
//
OpaqueEndpointI.prototype.changeConnectionId = function(connectionId)
{
    return this;
}

//
// Return true if the endpoints support bzip2 compress, or false
// otherwise.
//
OpaqueEndpointI.prototype.compress = function()
{
    return false;
}

//
// Return a new endpoint with a different compression value,
// provided that compression is supported by the
// endpoint. Otherwise the same endpoint is returned.
//
OpaqueEndpointI.prototype.changeCompress = function(compress)
{
    return this;
}

//
// Return true if the endpoint is datagram-based.
//
OpaqueEndpointI.prototype.datagram = function()
{
    return false;
}

//
// Return true if the endpoint is secure.
//
OpaqueEndpointI.prototype.secure = function()
{
    return false;
}

//
// Get the encoded endpoint.
//
OpaqueEndpointI.prototype.rawBytes = function()
{
    return this._rawBytes; // Returns a Uint8Array
}

//
// Return a server side transceiver for this endpoint, or null if a
// transceiver can only be created by an acceptor. In case a
// transceiver is created, this operation also returns a new
// "effective" endpoint, which might differ from this endpoint,
// for example, if a dynamic port number is assigned.
//
OpaqueEndpointI.prototype.transceiver = function(endpoint)
{
    endpoint.value = null;
    return null;
}

//
// Return an acceptor for this endpoint, or null if no acceptors
// is available. In case an acceptor is created, this operation
// also returns a new "effective" endpoint, which might differ
// from this endpoint, for example, if a dynamic port number is
// assigned.
//
OpaqueEndpointI.prototype.acceptor = function(endpoint, adapterName)
{
    endpoint.value = this;
    return null;
}

OpaqueEndpointI.prototype.connect = function()
{
    return null;
}

//
// Check whether the endpoint is equivalent to another one.
//
OpaqueEndpointI.prototype.equivalent = function(endpoint)
{
    return false;
}

OpaqueEndpointI.prototype.hashCode = function()
{
    return this._hashCode;
}

//
// Compare endpoints for sorting purposes
//
OpaqueEndpointI.prototype.equals = function(p)
{
    if(!(p instanceof OpaqueEndpointI))
    {
        return false;
    }

    if(this === p)
    {
        return true;
    }

    if(this._type !== p._type)
    {
        return false;
    }

    if(!this._rawEncoding.equals(p._rawEncoding))
    {
        return false;
    }

    if(this._rawBytes.length !== p._rawBytes.length)
    {
        return false;
    }
    for(var i = 0; i < this._rawBytes.length; i++)
    {
        if(this._rawBytes[i] !== p._rawBytes[i])
        {
            return false;
        }
    }

    return true;
}

OpaqueEndpointI.prototype.compareTo = function(p)
{
    if(this === p)
    {
        return 0;
    }

    if(p === null)
    {
        return 1;
    }

    if(!(p instanceof OpaqueEndpointI))
    {
        return this.type() < p.type() ? -1 : 1;
    }

    if(this._type < p._type)
    {
        return -1;
    }
    else if(p._type < this._type)
    {
        return 1;
    }

    if(this._rawEncoding.major < p._rawEncoding.major)
    {
        return -1;
    }
    else if(p._rawEncoding.major < this._rawEncoding.major)
    {
        return 1;
    }

    if(this._rawEncoding.minor < p._rawEncoding.minor)
    {
        return -1;
    }
    else if(p._rawEncoding.minor < this._rawEncoding.minor)
    {
        return 1;
    }

    if(this._rawBytes.length < p._rawBytes.length)
    {
        return -1;
    }
    else if(p._rawBytes.length < this._rawBytes.length)
    {
        return 1;
    }
    for(var i = 0; i < this._rawBytes.length; i++)
    {
        if(this._rawBytes[i] < p._rawBytes[i])
        {
            return -1;
        }
        else if(p._rawBytes[i] < this._rawBytes[i])
        {
            return 1;
        }
    }

    return 0;
}

OpaqueEndpointI.prototype.calcHashValue = function()
{
    var h = 5381;
    h = HashUtil.addNumber(h, this._type);
    h = HashUtil.addHashable(h, this._rawEncoding);
    h = HashUtil.addNumberArray(h, this._rawBytes);
    this._hashCode = h;
}

module.exports.Ice = {};
module.exports.Ice.OpaqueEndpointI = OpaqueEndpointI;

var OpaqueEndpointInfoI = function(timeout, compress, rawEncoding, rawBytes, type)
{
    Ice.OpaqueEndpointInfo.call(this, -1, false, rawEncoding, rawBytes);
    this._type = type;
}

OpaqueEndpointInfoI.prototype = new Ice.OpaqueEndpointInfo();
OpaqueEndpointInfoI.prototype.constructor = OpaqueEndpointInfoI();

OpaqueEndpointInfoI.prototype.type = function()
{
    return this._type;
}

OpaqueEndpointInfoI.prototype.datagram = function()
{
    return false;
}

OpaqueEndpointInfoI.prototype.secure = function()
{
    return false;
}
