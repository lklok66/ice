// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var net = require("net");

var Debug = require("./Debug");
var Ex = require("./Exception");
var ExUtil = require("./ExUtil");
var Network = require("./Network");
var SocketOperation = require("./SocketOperation");

var Conn = require("./Connection").Ice;
var LocalEx = require("./LocalException").Ice;

var StateNeedConnect = 0;
var StateConnectPending = 1;
var StateProxyConnectRequest = 2;
var StateProxyConnectRequestPending = 3;
var StateConnected = 4;
var StateClosed = 5;
var StateError = 6;

var TcpTransceiver = function(instance)
{
    this._traceLevels = instance.traceLevels();
    this._logger = instance.initializationData().logger;
    this._readBuffers = [];
    this._readPosition = 0;
}

TcpTransceiver.createOutgoing = function(instance, addr)
{
    var transceiver = new TcpTransceiver(instance);

    transceiver._fd = new net.Socket();
    transceiver._proxy = instance.networkProxy();
    transceiver._addr = addr;
    transceiver._connected = false;
    transceiver._desc = "remote address: " + Network.addrToString(addr.host, addr.port) + " <not connected>";
    transceiver._state = StateNeedConnect;

    return transceiver;
}

TcpTransceiver.createIncoming = function(instance, fd)
{
    var transceiver = new TcpTransceiver(instance);

    transceiver._fd = fd;
    transceiver._proxy = null;
    transceiver._addr = null;
    transceiver._connected = true;
    transceiver._desc = fdToString(fd);
    transceiver._state = StateConnected;

    return transceiver;
}

TcpTransceiver.prototype.setCallbacks = function(
    connectedCallback,      // function()
    bytesAvailableCallback, // function()
    bytesWrittenCallback,   // function()
    closedCallback,         // function()
    errorCallback           // function(ex)
)
{
    this._connectedCallback = connectedCallback;
    this._bytesAvailableCallback = bytesAvailableCallback;
    this._bytesWrittenCallback = bytesWrittenCallback;
    this._closedCallback = closedCallback;
    this._errorCallback = errorCallback;

    var self = this;
    this._fd.on("connect", function() { self.socketConnected(); });
    this._fd.on("close", function(err) { self.socketClosed(err); });
    this._fd.on("error", function(err) { self.socketError(err); });

    //
    // Don't register for incoming data right away.
    //
    //this._fd.on("data", function(buf) { self.socketBytesAvailable(buf); });
}

//
// Returns SocketOperation.None when initialization is complete.
//
TcpTransceiver.prototype.initialize = function(readBuffer, writeBuffer)
{
    try
    {
        if(this._state === StateNeedConnect)
        {
            this._state = StateConnectPending;
            this._fd.connect(this._addr.port, this._addr.host);
            return SocketOperation.Connect; // Waiting for connect to complete.
        }
        else if(this._state === StateConnectPending)
        {
            //
            // Socket is connected.
            //

            this._desc = fdToString(this._fd, this._proxy, this._addr);

            if(this._proxy !== null)
            {
                //
                // Prepare the read & write buffers in advance.
                //
                this._proxy.beginWriteConnectRequest(this._addr.host, this._addr.port, writeBuffer);
                this._proxy.beginReadConnectRequestResponse(readBuffer);

                //
                // Write the proxy connection message.
                //
                if(this.write(writeBuffer))
                {
                    //
                    // Write completed immediately.
                    //
                    this._proxy.endWriteConnectRequest(writeBuffer);

                    //
                    // Try to read the response.
                    //
                    var dummy = { value: false };
                    if(this.read(readBuffer, dummy))
                    {
                        //
                        // Read completed immediately - fall through.
                        //
                        this._proxy.endReadConnectRequestResponse(readBuffer);
                    }
                    else
                    {
                        //
                        // Return SocketOperation.Read to indicate we need to complete the read.
                        //
                        this._state = StateProxyConnectRequestPending; // Wait for proxy response
                        return SocketOperation.Read;
                    }
                }
                else
                {
                    //
                    // Return SocketOperationWrite to indicate we need to complete the write.
                    //
                    this._state = StateProxyConnectRequest; // Send proxy connect request
                    return SocketOperation.Write;
                }
            }

            this._state = StateConnected;
        }
        else if(this._state === StateProxyConnectRequest)
        {
            //
            // Write completed.
            //
            this._proxy.endWriteConnectRequest(writeBuffer);
            this._state = StateProxyConnectRequestPending; // Wait for proxy response
            return SocketOperation.Read;
        }
        else if(this._state === StateProxyConnectRequestPending)
        {
            //
            // Read completed.
            //
            this._proxy.endReadConnectRequestResponse(readBuffer);
            this._state = StateConnected;
        }
    }
    catch(ex)
    {
        if(ex instanceof Ex.LocalException)
        {
            if(this._traceLevels.network >= 2)
            {
                var s = [];
                s.push("failed to establish tcp connection\n");
                s.push(fdToString(this._fd, this._proxy, this._addr.host, this._addr.port));
                this._logger.trace(this._traceLevels.networkCat, s.join(""));
            }
        }
        throw ex;
    }

    Debug.assert(this._state === StateConnected);
    if(this._traceLevels.network >= 1)
    {
        var s = "tcp connection established\n" + this._desc;
        this._logger.trace(this._traceLevels.networkCat, s);
    }

    return SocketOperation.None;
}

TcpTransceiver.prototype.register = function()
{
    //
    // Register the socket data listener.
    //
    var self = this;
    this._fd.on("data", function(buf) { self.socketBytesAvailable(buf); });
}

TcpTransceiver.prototype.unregister = function()
{
    //
    // Unregister the socket data listener.
    //
    this._fd.removeAllListeners("data");
}

TcpTransceiver.prototype.close = function()
{
    if(this._connected && this._traceLevels.network >= 1)
    {
        this._logger.trace(this._traceLevels.networkCat, "closing " + this.type() + " connection\n" + this._desc);
    }

    Debug.assert(this._fd !== null);
    try
    {
        this._fd.destroy();
    }
    catch(ex)
    {
        throw translateError(ex);
    }
    finally
    {
        this._fd = null;
    }
}

//
// Returns true if all of the data was flushed to the kernel buffer.
//
TcpTransceiver.prototype.write = function(byteBuffer)
{
    var remaining = byteBuffer.remaining;
    Debug.assert(remaining > 0);

    //
    // Create a slice of the source buffer representing the remaining data to be written.
    //
    var slice = byteBuffer.b.slice(byteBuffer.position, byteBuffer.position + remaining);

    //
    // The socket will accept all of the data.
    //
    byteBuffer.position = byteBuffer.position + remaining;

    var self = this;

    var sync = true;
    sync = this._fd.write(slice, null,
            //
            // Callback function invoked when data is eventually written.
            //
            function()
            {
                self.socketBytesWritten(remaining, sync);
            });

    return sync;
}

TcpTransceiver.prototype.read = function(byteBuffer, moreData)
{
    moreData.value = false;

    if(this._readBuffers.length === 0)
    {
        return false; // No data available.
    }

    var avail = this._readBuffers[0].length - this._readPosition;
    Debug.assert(avail > 0);
    var remaining = byteBuffer.remaining;

    while(byteBuffer.remaining > 0)
    {
        if(avail > byteBuffer.remaining)
        {
            avail = byteBuffer.remaining;
        }

        this._readBuffers[0].copy(byteBuffer.b, byteBuffer.position, this._readPosition, this._readPosition + avail);

        byteBuffer.position += avail;
        this._readPosition += avail;
        if(this._readPosition === this._readBuffers[0].length)
        {
            //
            // We've exhausted the current read buffer.
            //
            this._readPosition = 0;
            this._readBuffers.shift();
            if(this._readBuffers.length === 0)
            {
                break; // No more data - we're done.
            }
            else
            {
                avail = this._readBuffers[0].length;
            }
        }
    }

    var n = remaining - byteBuffer.remaining;
    if(n > 0 && this._traceLevels.network >= 3)
    {
        var msg = "received " + n + " of " + remaining + " bytes via " + this.type() + "\n" + this._desc;
        this._logger.trace(this._traceLevels.networkCat, msg);
    }

    moreData.value = this._readBuffers.length > 0;

    return byteBuffer.remaining === 0;
}

TcpTransceiver.prototype.type = function()
{
    return "tcp";
}

TcpTransceiver.prototype.getInfo = function()
{
    Debug.assert(this._fd !== null);
    var info = this.createInfo();
    info.localAddress = this._fd.localAddress;
    info.localPort = this._fd.localPort;
    info.remoteAddress = this._fd.remoteAddress;
    info.remotePort = this._fd.remotePort;
    return info;
}

TcpTransceiver.prototype.createInfo = function()
{
    return new Conn.TCPConnectionInfo();
}

TcpTransceiver.prototype.checkSendSize = function(stream, messageSizeMax)
{
    if(stream.size > messageSizeMax)
    {
        ExUtil.throwMemoryLimitException(stream.size, messageSizeMax);
    }
}

TcpTransceiver.prototype.toString = function()
{
    return this._desc;
}

TcpTransceiver.prototype.socketConnected = function()
{
    this._connected = true;
    this._desc = fdToString(this._fd, this._proxy, this._addr);

    if(this._traceLevels.network >= 1)
    {
        this._logger.trace(this._traceLevels.networkCat, this.type() + " connection established\n" + this._desc);
    }

    Debug.assert(this._connectedCallback !== null);
    this._connectedCallback();
}

TcpTransceiver.prototype.socketBytesAvailable = function(buf)
{
    Debug.assert(this._bytesAvailableCallback !== null);
    //
    // TODO: Should we set a limit on how much data we can read?
    // We can call _fd.pause() to temporarily stop reading.
    //
    if(buf.length > 0)
    {
        this._readBuffers.push(buf);
        this._bytesAvailableCallback();
    }
}

TcpTransceiver.prototype.socketBytesWritten = function(n, sync)
{
    if(this._traceLevels.network >= 3)
    {
        var msg = "sent " + n + " bytes via " + this.type() + "\n" + this._desc;
        this._logger.trace(this._traceLevels.networkCat, msg);
    }

    //
    // Don't invoke the callback if the data was written synchronously.
    //
    if(!sync)
    {
        Debug.assert(this._bytesWrittenCallback !== null);
        this._bytesWrittenCallback();
    }
}

TcpTransceiver.prototype.socketClosed = function(err)
{
    //
    // Don't call the closed callback if an error occurred; the error callback
    // will be called.
    //
    if(!err)
    {
        Debug.assert(this._closedCallback !== null);
        this._closedCallback();
    }
}

TcpTransceiver.prototype.socketError = function(err)
{
    Debug.assert(this._errorCallback !== null);
    this._errorCallback(translateError(err));
}

function fdToString(fd, proxy, targetAddr)
{
    if(fd === null)
    {
        return "<closed>";
    }

    return Network.addressesToString(fd.localAddress, fd.localPort, fd.remoteAddress, fd.remotePort, proxy,
                                     targetAddr);
}

function translateError(ex)
{
    // TODO: Search the exception's error message for symbols like ECONNREFUSED ?
    return new LocalEx.SocketException(0, ex);
}

module.exports = TcpTransceiver;
