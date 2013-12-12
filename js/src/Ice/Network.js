// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Address = require("./Address");

module.exports.EnableIPv4 = 0;
module.exports.EnableIPv6 = 1;
module.exports.EnableBoth = 2;

module.exports.compareAddress = function(addr1, addr2)
{
    if(addr1.port < addr2.port)
    {
        return -1;
    }
    else if(addr2.port < addr1.port)
    {
        return 1;
    }

    if(addr1.host < addr2.host)
    {
        return -1;
    }
    else if(addr2.host < addr1.host)
    {
        return 1;
    }

    if(addr1.family < addr2.family)
    {
        return -1;
    }
    else if(addr2.family < addr1.family)
    {
        return 1;
    }

    return 0;
};

module.exports.getAddressForServer = function(host, port, protocol, preferIPv6)
{
    if(host === undefined || host === null || host.length === 0)
    {
        if(protocol !== module.exports.EnableIPv4)
        {
            return new Address("::0", (port === undefined || port === null) ? 0 : port);
        }
        else
        {
            return new Address("0.0.0.0", (port === undefined || port === null) ? 0 : port);
        }
    }
    return new Address(host, (port === undefined || port === null) ? 0 : port);
};

module.exports.addrToString = function(host, port)
{
    return host + ":" + port;
};

module.exports.addressesToString = function(localHost, localPort, remoteHost, remotePort, proxy, targetAddr)
{
    remoteHost = remoteHost === undefined ? null : remoteHost;
    proxy = proxy === undefined ? null : proxy;
    targetAddr = targetAddr === undefined ? null : targetAddr;

    var s = [];
    s.push("local address = ");
    s.push(module.exports.addrToString(localHost, localPort));

    if(proxy !== null)
    {
        if(remoteHost === null)
        {
            var addr = proxy.getAddress();
            remoteHost = addr.host;
            remotePort = addr.port;
        }
        s.push("\n");
        s.push(proxy.getName());
        s.push(" proxy address = ");
        s.push(module.exports.addrToString(remoteHost, remotePort));
        s.push("\nremote address = ");
        s.push(module.exports.addrToString(targetAddr.host, targetAddr.port));
    }
    else
    {
        if(remoteHost === null && targetAddr !== null)
        {
            remoteHost = targetAddr.host;
            remotePort = targetAddr.port;
        }

        if(remoteHost === null)
        {
            s.push("\nremote address = <not connected>");
        }
        else
        {
            s.push("\nremote address = ");
            s.push(module.exports.addrToString(remoteHost, remotePort));
        }
    }

    return s.join("");
};
