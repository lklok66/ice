// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Config.h>
#ifdef _WIN32
#   include <winsock2.h>
#endif

#include <IceWS/Instance.h>
#include <IceWS/EndpointI.h>
#include <IceWS/Util.h>

#include <Ice/Communicator.h>
#include <Ice/LocalException.h>
#include <Ice/Logger.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Properties.h>
#include <Ice/ProtocolPluginFacade.h>

#include <IceUtil/DisableWarnings.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;
using namespace IceWS;

IceUtil::Shared* IceWS::upCast(IceWS::Instance* p) { return p; }

IceWS::Instance::Instance(const CommunicatorPtr& communicator) :
    _logger(communicator->getLogger()),
    _initialized(false)
{
    _facade = getProtocolPluginFacade(communicator);

    //
    // Register the endpoint factory. We have to do this now, rather than
    // in initialize, because the communicator may need to interpret
    // proxies before the plug-in is fully initialized.
    //
    EndpointFactoryPtr tcpFactory = _facade->getEndpointFactory(TCPEndpointType);
    assert(tcpFactory);
    _facade->addEndpointFactory(new EndpointFactoryI(this, tcpFactory, WSEndpointType, "ws"));

    EndpointFactoryPtr sslFactory;
    if(communicator->getPluginManager()->getPlugin("IceSSL"))
    {
        sslFactory = _facade->getEndpointFactory(2); // IceSSL::EndpointType = 2
        assert(sslFactory);
        _facade->addEndpointFactory(new EndpointFactoryI(this, sslFactory, WSSEndpointType, "wss"));
    }
}

IceWS::Instance::~Instance()
{
}

void
IceWS::Instance::initialize()
{
    if(_initialized)
    {
        return;
    }

#if 0
    try
    {
        const string propPrefix = "IceWS.";
        PropertiesPtr properties = communicator()->getProperties();
    }
#endif

    _initialized = true;
}

CommunicatorPtr
IceWS::Instance::communicator() const
{
    return _facade->getCommunicator();
}

EndpointHostResolverPtr
IceWS::Instance::endpointHostResolver() const
{
    return _facade->getEndpointHostResolver();
}

ProtocolSupport
IceWS::Instance::protocolSupport() const
{
    return _facade->getProtocolSupport();
}

bool
IceWS::Instance::preferIPv6() const
{
    return _facade->preferIPv6();
}

NetworkProxyPtr
IceWS::Instance::networkProxy() const
{
    return _facade->getNetworkProxy();
}

string
IceWS::Instance::defaultHost() const
{
    return _facade->getDefaultHost();
}

Ice::EncodingVersion
IceWS::Instance::defaultEncoding() const
{
    return _facade->getDefaultEncoding();
}

int
IceWS::Instance::networkTraceLevel() const
{
    return _facade->getNetworkTraceLevel();
}

string
IceWS::Instance::networkTraceCategory() const
{
    return _facade->getNetworkTraceCategory();
}

size_t
IceWS::Instance::messageSizeMax() const
{
    return _facade->getMessageSizeMax();
}

void
IceWS::Instance::destroy()
{
    _facade = 0;
}
