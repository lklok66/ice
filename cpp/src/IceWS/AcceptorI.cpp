// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceWS/AcceptorI.h>
#include <IceWS/Instance.h>
#include <IceWS/TransceiverI.h>
#include <IceWS/Util.h>

#include <Ice/Communicator.h>
#include <Ice/Exception.h>
#include <Ice/LocalException.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Properties.h>
#include <IceUtil/StringUtil.h>

#ifdef ICE_USE_IOCP
#  include <Mswsock.h>
#endif

using namespace std;
using namespace Ice;
using namespace IceWS;

IceInternal::NativeInfoPtr
IceWS::AcceptorI::getNativeInfo()
{
    return this;
}

#ifdef ICE_USE_IOCP
IceInternal::AsyncInfo*
#  ifndef NDEBUG
IceWS::AcceptorI::getAsyncInfo(IceInternal::SocketOperation status)
#  else
IceWS::AcceptorI::getAsyncInfo(IceInternal::SocketOperation)
#  endif
{
    assert(status == IceInternal::SocketOperationRead);
    return &_info;
}
#endif

void
IceWS::AcceptorI::close()
{
    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "stopping to accept ws connections at " << toString();
    }

    SOCKET fd = _fd;
    _fd = INVALID_SOCKET;
    IceInternal::closeSocket(fd);
}

void
IceWS::AcceptorI::listen()
{
    try
    {
        IceInternal::doListen(_fd, _backlog);
    }
    catch(...)
    {
        _fd = INVALID_SOCKET;
        throw;
    }

    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "listening for ws connections at " << toString();

        vector<string> interfaces =
            IceInternal::getHostsForEndpointExpand(IceInternal::inetAddrToString(_addr), _instance->protocolSupport(),
                                                   true);
        if(!interfaces.empty())
        {
            out << "\nlocal interfaces: ";
            out << IceUtilInternal::joinString(interfaces, ", ");
        }
    }
}

#ifdef ICE_USE_IOCP
void
IceWS::AcceptorI::startAccept()
{
    LPFN_ACCEPTEX AcceptEx = NULL; // a pointer to the 'AcceptEx()' function
    GUID GuidAcceptEx = WSAID_ACCEPTEX; // The Guid
    DWORD dwBytes;
    if(WSAIoctl(_fd,
                SIO_GET_EXTENSION_FUNCTION_POINTER,
                &GuidAcceptEx,
                sizeof(GuidAcceptEx),
                &AcceptEx,
                sizeof(AcceptEx),
                &dwBytes,
                NULL,
                NULL) == SOCKET_ERROR)
    {
        SocketException ex(__FILE__, __LINE__);
        ex.error = IceInternal::getSocketErrno();
        throw ex;
    }

    assert(_acceptFd == INVALID_SOCKET);
    _acceptFd = IceInternal::createSocket(false, _addr);
    const int sz = static_cast<int>(_acceptBuf.size() / 2);
    if(!AcceptEx(_fd, _acceptFd, &_acceptBuf[0], 0, sz, sz, &_info.count, &_info))
    {
        if(!IceInternal::wouldBlock())
        {
            SocketException ex(__FILE__, __LINE__);
            ex.error = IceInternal::getSocketErrno();
            throw ex;
        }
    }
}

void
IceWS::AcceptorI::finishAccept()
{
    if(static_cast<int>(_info.count) == SOCKET_ERROR || _fd == INVALID_SOCKET)
    {
        IceInternal::closeSocketNoThrow(_acceptFd);
        _acceptFd = INVALID_SOCKET;
        _acceptError = _info.error;
    }
}
#endif

IceInternal::TransceiverPtr
IceWS::AcceptorI::accept()
{
#ifndef ICE_USE_IOCP
    SOCKET fd = IceInternal::doAccept(_fd);
#else
    if(_acceptFd == INVALID_SOCKET)
    {
        SocketException ex(__FILE__, __LINE__);
        ex.error = _acceptError;
        throw ex;
    }

    if(setsockopt(_acceptFd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&_acceptFd, sizeof(_acceptFd)) ==
       SOCKET_ERROR)
    {
        IceInternal::closeSocketNoThrow(_acceptFd);
        _acceptFd = INVALID_SOCKET;
        SocketException ex(__FILE__, __LINE__);
        ex.error = IceInternal::getSocketErrno();
        throw ex;
    }

    SOCKET fd = _acceptFd;
    _acceptFd = INVALID_SOCKET;
#endif

    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "attempting to accept ws connection\n" << IceInternal::fdToString(fd);
    }

    //
    // WebSocket handshaking is performed in TransceiverI::initialize, since
    // accept must not block.
    //
    return new TransceiverI(_instance, fd, _adapterName);
}

string
IceWS::AcceptorI::toString() const
{
    return IceInternal::addrToString(_addr);
}

int
IceWS::AcceptorI::effectivePort() const
{
    if(_addr.saStorage.ss_family == AF_INET)
    {
        return ntohs(_addr.saIn.sin_port);
    }
    else
    {
        return ntohs(_addr.saIn6.sin6_port);
    }
}

IceWS::AcceptorI::AcceptorI(const InstancePtr& instance, const string& adapterName, const string& host, int port) :
    _instance(instance),
    _adapterName(adapterName),
    _logger(instance->communicator()->getLogger()),
    _addr(IceInternal::getAddressForServer(host, port, instance->protocolSupport(), instance->preferIPv6()))
#ifdef ICE_USE_IOCP
    , _acceptFd(INVALID_SOCKET),
    _info(IceInternal::SocketOperationRead)
#endif
{
#ifdef SOMAXCONN
    _backlog = instance->communicator()->getProperties()->getPropertyAsIntWithDefault("Ice.TCP.Backlog", SOMAXCONN);
#else
    _backlog = instance->communicator()->getProperties()->getPropertyAsIntWithDefault("Ice.TCP.Backlog", 511);
#endif

    IceInternal::ProtocolSupport protocol = instance->protocolSupport();
    _fd = IceInternal::createServerSocket(false, _addr, protocol);
#ifdef ICE_USE_IOCP
    _acceptBuf.resize((sizeof(sockaddr_storage) + 16) * 2);
#endif
    IceInternal::setBlock(_fd, false);
    IceInternal::setTcpBufSize(_fd, _instance->communicator()->getProperties(), _logger);
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
    IceInternal::setReuseAddress(_fd, true);
#endif
    if(_instance->networkTraceLevel() >= 2)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "attempting to bind to ws socket " << toString();
    }
    const_cast<IceInternal::Address&>(_addr) = IceInternal::doBind(_fd, _addr);
}

IceWS::AcceptorI::~AcceptorI()
{
    assert(_fd == INVALID_SOCKET);
#ifdef ICE_USE_IOCP
    assert(_acceptFd == INVALID_SOCKET);
#endif
}
