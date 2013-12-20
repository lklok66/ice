// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Address = require("./Address");
var HashUtil = require("./HashUtil");
var StringUtil = require("./StringUtil");
var TcpTransceiver = require("./TcpTransceiver");

var Endp = require("./Endpoint").Ice;
var LocalEx = require("./LocalException").Ice;

var TcpEndpointI = function(instance, ho, po, ti, conId, co)
{
    this._instance = instance;
    this._host = ho;
    this._port = po;
    this._timeout = ti;
    this._connectionId = conId;
    this._compress = co;
    this.calcHashValue();
}

TcpEndpointI.prototype = new Endp.Endpoint();
TcpEndpointI.prototype.constructor = TcpEndpointI;

TcpEndpointI.fromString = function(instance, str, oaEndpoint)
{
    var host = null;
    var port = 0;
    var timeout = -1;
    var compress = false;

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
            throw new LocalEx.EndpointParseException("expected an endpoint option but found `" + option +
                                                     "' in endpoint `tcp " + str + "'");
        }

        var argument = null;
        if(i < arr.length && arr[i].charAt(0) != '-')
        {
            argument = arr[i++];
            if(argument.charAt(0) == '\"' && argument.charAt(argument.length - 1) == '\"')
            {
                argument = argument.substring(1, argument.length - 1);
            }
        }

        switch(option.charAt(1))
        {
            case 'h':
            {
                if(argument == null)
                {
                    throw new LocalEx.EndpointParseException("no argument provided for -h option in endpoint `tcp "
                                                             + str + "'");
                }

                host = argument;
                break;
            }

            case 'p':
            {
                if(argument == null)
                {
                    throw new LocalEx.EndpointParseException("no argument provided for -p option in endpoint `tcp "
                                                             + str + "'");
                }

                try
                {
                    port = StringUtil.toInt(argument);
                }
                catch(ex)
                {
                    throw new LocalEx.EndpointParseException("invalid port value `" + argument +
                                                             "' in endpoint `tcp " + str + "'");
                }

                if(port < 0 || port > 65535)
                {
                    throw new LocalEx.EndpointParseException("port value `" + argument +
                                                             "' out of range in endpoint `tcp " + str + "'");
                }

                break;
            }

            case 't':
            {
                if(argument == null)
                {
                    throw new LocalEx.EndpointParseException("no argument provided for -t option in endpoint `tcp "
                                                             + str + "'");
                }

                try
                {
                    timeout = StringUtil.toInt(argument);
                }
                catch(ex)
                {
                    throw new LocalEx.EndpointParseException("invalid timeout value `" + argument +
                                                             "' in endpoint `tcp " + str + "'");
                }

                break;
            }

            case 'z':
            {
                if(argument != null)
                {
                    throw new LocalEx.EndpointParseException("unexpected argument `" + argument +
                                                             "' provided for -z option in `tcp " + str + "'");
                }

                compress = true;
                break;
            }

            default:
            {
                throw new LocalEx.EndpointParseException("unknown option `" + option + "' in `tcp " + str + "'");
            }
        }
    }

    if(host == null)
    {
        host = instance.defaultsAndOverrides().defaultHost;
    }
    else if(host == "*")
    {
        if(oaEndpoint)
        {
            host = null;
        }
        else
        {
            throw new LocalEx.EndpointParseException("`-h *' not valid for proxy endpoint `tcp " + str + "'");
        }
    }

    if(host == null)
    {
        host = "";
    }

    return new TcpEndpointI(instance, host, port, timeout, "", compress)
}

TcpEndpointI.fromStream = function(s)
{
    s.startReadEncaps();
    var host = s.readString();
    var port = s.readInt();
    var timeout = s.readInt();
    var compress = s.readBool();
    s.endReadEncaps();
    return new TcpEndpointI(s.instance(), host, port, timeout, "", compress)
}

//
// Convert the endpoint to its string form
//
TcpEndpointI.prototype.toString = function()
{
    //
    // WARNING: Certain features, such as proxy validation in Glacier2,
    // depend on the format of proxy strings. Changes to toString() and
    // methods called to generate parts of the reference string could break
    // these features. Please review for all features that depend on the
    // format of proxyToString() before changing this and related code.
    //
    var s = "tcp";

    if(this._host != null && this._host.length > 0)
    {
        s += " -h ";
        var addQuote = this._host.indexOf(':') != -1;
        if(addQuote)
        {
            s += "\"";
        }
        s += this._host;
        if(addQuote)
        {
            s += "\"";
        }
    }

    s += " -p " + this._port;

    if(this._timeout != -1)
    {
        s += " -t " + this._timeout;
    }
    if(this._compress)
    {
        s += " -z";
    }
    return s;
}

//
// Return the endpoint information.
//
TcpEndpointI.prototype.getInfo = function()
{
    return new TCPEndpointInfoI(this._timeout, this._compress, this._host, this._port);
}

//
// Marshal the endpoint
//
TcpEndpointI.prototype.streamWrite = function(s)
{
    s.writeShort(Endp.TCPEndpointType);
    s.startWriteEncaps();
    s.writeString(this._host);
    s.writeInt(this._port);
    s.writeInt(this._timeout);
    s.writeBool(this._compress);
    s.endWriteEncaps();
}

//
// Return the endpoint type
//
TcpEndpointI.prototype.type = function()
{
    return Endp.TCPEndpointType;
}

//
// Return the timeout for the endpoint in milliseconds. 0 means
// non-blocking, -1 means no timeout.
//
TcpEndpointI.prototype.timeout = function()
{
    return this._timeout;
}

//
// Return a new endpoint with a different timeout value, provided
// that timeouts are supported by the endpoint. Otherwise the same
// endpoint is returned.
//
TcpEndpointI.prototype.changeTimeout = function(timeout)
{
    if(timeout === this._timeout)
    {
        return this;
    }
    else
    {
        return new TcpEndpointI(this._instance, this._host, this._port, timeout, this._connectionId, this._compress);
    }
}

//
// Return a new endpoint with a different connection id.
//
TcpEndpointI.prototype.changeConnectionId = function(connectionId)
{
    if(connectionId === this._connectionId)
    {
        return this;
    }
    else
    {
        return new TcpEndpointI(this._instance, this._host, this._port, this._timeout, connectionId, this._compress);
    }
}

//
// Return true if the endpoints support bzip2 compress, or false
// otherwise.
//
TcpEndpointI.prototype.compress = function()
{
    return this._compress;
}

//
// Return a new endpoint with a different compression value,
// provided that compression is supported by the
// endpoint. Otherwise the same endpoint is returned.
//
TcpEndpointI.prototype.changeCompress = function(compress)
{
    if(compress === this._compress)
    {
        return this;
    }
    else
    {
        return new TcpEndpointI(this._instance, this._host, this._port, this._timeout, this._connectionId, compress);
    }
}

//
// Return true if the endpoint is datagram-based.
//
TcpEndpointI.prototype.datagram = function()
{
    return false;
}

//
// Return true if the endpoint is secure.
//
TcpEndpointI.prototype.secure = function()
{
    return false;
}

//
// Return a server side transceiver for this endpoint, or null if a
// transceiver can only be created by an acceptor. In case a
// transceiver is created, this operation also returns a new
// "effective" endpoint, which might differ from this endpoint,
// for example, if a dynamic port number is assigned.
//
TcpEndpointI.prototype.transceiver = function(endpoint)
{
    endpoint.value = this;
    return null;
}

//
// Return an acceptor for this endpoint, or null if no acceptors
// is available. In case an acceptor is created, this operation
// also returns a new "effective" endpoint, which might differ
// from this endpoint, for example, if a dynamic port number is
// assigned.
//
TcpEndpointI.prototype.acceptor = function(endpoint, adapterName)
{
    var p = new TcpAcceptor(this._instance, this._host, this._port);
    endpoint.value = new TcpEndpointI(this._instance, this._host, p.effectivePort(), this._timeout, this._connectionId,
                                      this._compress);
    return p;
}

TcpEndpointI.prototype.connect = function()
{
    if(this._instance.traceLevels().network >= 2)
    {
        var msg = "trying to establish tcp connection to " + this._host + ":" + this._port;
        this._instance.initializationData().logger.trace(this._instance.traceLevels().networkCat, msg);
    }

    return TcpTransceiver.createOutgoing(this._instance, new Address(this._host, this._port));
}

//
// Check whether the endpoint is equivalent to another one.
//
TcpEndpointI.prototype.equivalent = function(endpoint)
{
    if(!(endpoint instanceof TcpEndpointI))
    {
        return false;
    }
    return endpoint._host === this._host && endpoint._port === this_port;
}

TcpEndpointI.prototype.hashCode = function()
{
    return this._hashCode;
}

//
// Compare endpoints for sorting purposes
//
TcpEndpointI.prototype.equals = function(p)
{
    if(!(p instanceof TcpEndpointI))
    {
        return false;
    }

    if(this === p)
    {
        return true;
    }

    if(this._host !== p._host)
    {
        return false;
    }

    if(this._port !== p._port)
    {
        return false;
    }

    if(this._timeout !== p._timeout)
    {
        return false;
    }

    if(this._connectionId !== p._connectionId)
    {
        return false;
    }

    if(this._compress !== p._compress)
    {
        return false;
    }

    return true;
}

TcpEndpointI.prototype.compareTo = function(p)
{
    if(this === p)
    {
        return 0;
    }

    if(p === null)
    {
        return 1;
    }

    if(!(p instanceof TcpEndpointI))
    {
        return this.type() < p.type() ? -1 : 1;
    }

    if(this._port < p._port)
    {
        return -1;
    }
    else if(p._port < this._port)
    {
        return 1;
    }

    if(this._timeout < p._timeout)
    {
        return -1;
    }
    else if(p._timeout < this._timeout)
    {
        return 1;
    }

    if(this._connectionId != p._connectionId)
    {
        return this._connectionId < p._connectionId ? -1 : 1;
    }

    if(!this._compress && p._compress)
    {
        return -1;
    }
    else if(!p._compress && this._compress)
    {
        return 1;
    }

    if(this._host == p._host)
    {
        return 0;
    }
    else
    {
        return this._host < p._host ? -1 : 1;
    }
}

TcpEndpointI.prototype.calcHashValue = function()
{
    var h = 5381;
    h = HashUtil.addNumber(h, Endp.TCPEndpointType);
    h = HashUtil.addString(h, this._host);
    h = HashUtil.addNumber(h, this._port);
    h = HashUtil.addNumber(h, this._timeout);
    h = HashUtil.addString(h, this._connectionId);
    h = HashUtil.addBoolean(h, this._compress);
    this._hashCode = h;
}

module.exports = TcpEndpointI;

var TCPEndpointInfoI = new function(timeout, compress, host, port)
{
    Endp.TCPEndpointInfo.call(this, timeout, compress, host, port);
}

TCPEndpointInfoI.prototype = new Endp.TCPEndpointInfo();
TCPEndpointInfoI.prototype.constructor = TCPEndpointInfoI;

TCPEndpointInfoI.prototype.type = function()
{
    return Endp.TCPEndpointType;
}

TCPEndpointInfoI.prototype.datagram = function()
{
    return false;
}

TCPEndpointInfoI.prototype.secure = function()
{
    return false;
}
