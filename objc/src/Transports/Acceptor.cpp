// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Transceiver.h>
#include <EndpointI.h>
#include <Acceptor.h>

#include <Ice/Instance.h>
#include <Ice/TraceLevels.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Network.h>
#include <Ice/Exception.h>
#include <Ice/Properties.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CFNetwork/CFNetwork.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

SOCKET
IceObjC::Acceptor::fd()
{
    return _fd;
}

void
IceObjC::Acceptor::close()
{
    if(_traceLevels->network >= 1)
    {
        Trace out(_logger, _traceLevels->networkCat);
        out << "stopping to accept " << (_type == TcpEndpointType ? "tcp" : "ssl") << " connections at " << toString();
    }

    SOCKET fd = _fd;
    _fd = INVALID_SOCKET;
    closeSocket(fd);
}

void
IceObjC::Acceptor::listen()
{
    try
    {
        doListen(_fd, _backlog);
    }
    catch(...)
    {
        _fd = INVALID_SOCKET;
        throw;
    }

    if(_traceLevels->network >= 1)
    {
        Trace out(_logger, _traceLevels->networkCat);
        out << "accepting " << (_type == TcpEndpointType ? "tcp" : "ssl") << " connections at " << toString();
    }
}

TransceiverPtr
IceObjC::Acceptor::accept()
{
    SOCKET fd = doAccept(_fd);
    setBlock(fd, false);
    setTcpBufSize(fd, _instance->initializationData().properties, _logger);

    if(_traceLevels->network >= 1)
    {
        Trace out(_logger, _traceLevels->networkCat);
        out << "accepted " << (_type == TcpEndpointType ? "tcp" : "ssl") << " connection\n" << fdToString(fd);
    }

    //
    // Create the read/write streams
    //
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    CFStreamCreatePairWithSocket(NULL, fd, &readStream, &writeStream);

    if(_type == SslEndpointType)
    {
        if(!CFReadStreamSetProperty(readStream, kCFStreamPropertySocketSecurityLevel, 
                                    kCFStreamSocketSecurityLevelNegotiatedSSL) ||
           !CFWriteStreamSetProperty(writeStream, kCFStreamPropertySocketSecurityLevel, 
                                     kCFStreamSocketSecurityLevelNegotiatedSSL))
        {
            throw Ice::SocketException(__FILE__, __LINE__, 0);
        }
            
        if(!CFReadStreamSetProperty(readStream, kCFStreamPropertySSLSettings, _settings) ||
           !CFWriteStreamSetProperty(writeStream, kCFStreamPropertySSLSettings, _settings))
        {
            throw Ice::SocketException(__FILE__, __LINE__, 0);
        }
    }

    if(!CFReadStreamOpen(readStream) || !CFWriteStreamOpen(writeStream))
    {
        int error = 0;
        if(CFReadStreamGetStatus(readStream) == kCFStreamStatusError)
        {
            CFErrorRef err = CFReadStreamCopyError(readStream);
            error = CFErrorGetCode(err);
            CFRelease(err);
        }
        else if(CFWriteStreamGetStatus(writeStream) == kCFStreamStatusError)
        {
            CFErrorRef err = CFWriteStreamCopyError(writeStream);
            error = CFErrorGetCode(err);
            CFRelease(err);
        }
        throw Ice::ConnectFailedException(__FILE__, __LINE__, error);
    }

    if(_type == TcpEndpointType)
    {
        return new Transceiver(_instance, fd, "tcp", readStream, writeStream, true);
    }
    else
    {
        return new Transceiver(_instance, fd, "ssl", readStream, writeStream, true);
    }
}

string
IceObjC::Acceptor::toString() const
{
    return addrToString(_addr);
}

int
IceObjC::Acceptor::effectivePort() const
{
    return getPort(_addr);
}

IceObjC::Acceptor::Acceptor(const InstancePtr& instance, const string& host, int port, Ice::Short type, 
                            CFDictionaryRef settings) :
    _instance(instance),
    _traceLevels(instance->traceLevels()),
    _logger(instance->initializationData().logger),
    _type(type)
{
#ifdef SOMAXCONN
    _backlog = instance->initializationData().properties->getPropertyAsIntWithDefault("Ice.TCP.Backlog", SOMAXCONN);
#else
    _backlog = instance->initializationData().properties->getPropertyAsIntWithDefault("Ice.TCP.Backlog", 511);
#endif

    try
    {
        getAddressForServer(host, port, _addr, _instance->protocolSupport());
        _fd = createSocket(false, _addr.ss_family);
        setBlock(_fd, false);
        setTcpBufSize(_fd, _instance->initializationData().properties, _logger);
#ifndef _WIN32
        //
        // Enable SO_REUSEADDR on Unix platforms to allow re-using the
        // socket even if it's in the TIME_WAIT state. On Windows,
        // this doesn't appear to be necessary and enabling
        // SO_REUSEADDR would actually not be a good thing since it
        // allows a second process to bind to an address even it's
        // already bound by another process.
        //
        // TODO: using SO_EXCLUSIVEADDRUSE on Windows would probably
        // be better but it's only supported by recent Windows
        // versions (XP SP2, Windows Server 2003).
        //
        setReuseAddress(_fd, true);
#endif
        if(_traceLevels->network >= 2)
        {
            Trace out(_logger, _traceLevels->networkCat);
            out << "attempting to bind to " << (_type == TcpEndpointType ? "tcp" : "ssl") << " socket " << toString();
        }
        doBind(_fd, _addr);

        if(_type == SslEndpointType)
        {
            CFMutableDictionaryRef s = CFDictionaryCreateMutableCopy(0, 0, settings);
            CFDictionarySetValue(s, kCFStreamSSLIsServer, kCFBooleanTrue);
            _settings = s;
        }
        else
        {
            _settings = settings;
        }
    }
    catch(...)
    {
        _fd = INVALID_SOCKET;
        throw;
    }
}

IceObjC::Acceptor::~Acceptor()
{
    assert(_fd == INVALID_SOCKET);
}
