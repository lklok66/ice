// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceWS/AcceptorI.h>
#include <IceWS/EndpointInfo.h>
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
    return _delegate->getNativeInfo();
}

#ifdef ICE_USE_IOCP
IceInternal::AsyncInfo*
IceWS::AcceptorI::getAsyncInfo(IceInternal::SocketOperation status)
{
    return _delegate->getAsyncInfo(status);
}
#endif

void
IceWS::AcceptorI::close()
{
    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "stopping to accept " << type() << " connections at " << toString();
    }

    _delegate->close();
}

void
IceWS::AcceptorI::listen()
{
    _delegate->listen();

    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "listening for " << type() << " connections at " << toString();

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
    _delegate->startAccept();
}

void
IceWS::AcceptorI::finishAccept()
{
    _delegate->finishAccept();
}
#endif

IceInternal::TransceiverPtr
IceWS::AcceptorI::accept()
{
    IceInternal::TransceiverPtr del = _delegate->accept();

    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "attempting to accept " << type() << " connection\n"
            << IceInternal::fdToString(del->getNativeInfo()->fd());
    }

    //
    // WebSocket handshaking is performed in TransceiverI::initialize, since
    // accept must not block.
    //
    return new TransceiverI(_instance, _type, del);
}

string
IceWS::AcceptorI::type() const
{
    return _type == WSEndpointType ? "ws" : "wss"; // TODO
}

string
IceWS::AcceptorI::toString() const
{
    return _delegate->toString();
}

IceWS::AcceptorI::AcceptorI(const InstancePtr& instance, Short ty, const IceInternal::AcceptorPtr& del,
                            const string& adapterName, const string& host, int port) :
    _instance(instance),
    _type(ty),
    _delegate(del),
    _adapterName(adapterName), // TODO: Necessary?
    _logger(instance->communicator()->getLogger()),
    _addr(getAddressForServer(host, port, instance->protocolSupport(), instance->preferIPv6()))
{
    // TODO: The delegate has already performed the bind at this point
    if(_instance->networkTraceLevel() >= 2)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "attempting to bind to " << type() << " socket " << toString();
    }
}

IceWS::AcceptorI::~AcceptorI()
{
}
