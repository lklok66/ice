// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Transceiver.h>
#include <EndpointI.h>
#include <Connector.h>

#include <Ice/Instance.h>
#include <Ice/TraceLevels.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Network.h>
#include <Ice/Exception.h>

#include <CoreFoundation/CoreFoundation.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

TransceiverPtr
IceObjC::Connector::connect()
{
    if(_traceLevels->network >= 2)
    {
        Trace out(_logger, _traceLevels->networkCat);
        out << "trying to establish " << _instance->protocol() << " connection to " << toString();
    }

    try
    {
        //
        // Create the read/write streams
        //
        CFReadStreamRef readStream;
        CFWriteStreamRef writeStream;
        CFDataRef addr = nil;
        if(_addr.ss_family == AF_INET)
        {
            addr = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(&_addr), sizeof(sockaddr_in));
        }
        else if(_addr.ss_family == AF_INET6)
        {
            addr = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(&_addr), sizeof(sockaddr_in6));
        }
        CFHostRef host = CFHostCreateWithAddress(NULL, addr);
        CFRelease(addr);
        CFStreamCreatePairWithSocketToCFHost(NULL, host, getPort(_addr), &readStream, &writeStream);
        CFRelease(host);

        _instance->setupStreams(readStream, writeStream, false, _host);

        SOCKET fd;
        CFDataRef d = (CFDataRef)CFReadStreamCopyProperty(readStream, kCFStreamPropertySocketNativeHandle);
        CFDataGetBytes(d, CFRangeMake(0, sizeof(SOCKET)), reinterpret_cast<UInt8*>(&fd));
        CFRelease(d);

        if(!CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanFalse) || 
           !CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanFalse))
        {
            throw Ice::ConnectFailedException(__FILE__, __LINE__, 0);
        }

        setBlock(fd, false);
        setTcpBufSize(fd, _instance->initializationData().properties, _logger);

        return new Transceiver(_instance, fd, readStream, writeStream, false, _host);
    }
    catch(const Ice::LocalException& ex)
    {
        if(_traceLevels->network >= 2)
        {
            Trace out(_logger, _traceLevels->networkCat);
            out << "failed to establish " << _instance->protocol() << " connection to ";
            out << toString() << "\n" << ex;
        }
        throw;
    }
}

Short
IceObjC::Connector::type() const
{
    return _instance->type();
}

string
IceObjC::Connector::toString() const
{
    return addrToString(_addr);
}

bool
IceObjC::Connector::operator==(const IceInternal::Connector& r) const
{
    const Connector* p = dynamic_cast<const Connector*>(&r);
    if(!p)
    {
        return false;
    }

    if(compareAddress(_addr, p->_addr) != 0)
    {
        return false;
    }

    if(_timeout != p->_timeout)
    {
        return false;
    }

    if(_connectionId != p->_connectionId)
    {
        return false;
    }

    return true;
}

bool
IceObjC::Connector::operator!=(const IceInternal::Connector& r) const
{
    return !operator==(r);
}

bool
IceObjC::Connector::operator<(const IceInternal::Connector& r) const
{
    const Connector* p = dynamic_cast<const Connector*>(&r);
    if(!p)
    {
        return type() < r.type();
    }

    if(_timeout < p->_timeout)
    {
        return true;
    }
    else if(p->_timeout < _timeout)
    {
        return false;
    }

    if(_connectionId < p->_connectionId)
    {
        return true;
    }
    else if(p->_connectionId < _connectionId)
    {
        return false;
    }
    return compareAddress(_addr, p->_addr) == -1;
}

IceObjC::Connector::Connector(const InstancePtr& instance, 
                              const struct sockaddr_storage& addr, 
                              Ice::Int timeout, 
                              const string& connectionId,
                              const string& host) :
    _instance(instance),
    _traceLevels(instance->traceLevels()),
    _logger(instance->initializationData().logger),
    _addr(addr),
    _timeout(timeout),
    _connectionId(connectionId),
    _host(host)
{
}

IceObjC::Connector::~Connector()
{
}
