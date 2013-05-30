// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_WS_INSTANCE_H
#define ICE_WS_INSTANCE_H

#include <IceWS/InstanceF.h>
#include <Ice/CommunicatorF.h>
#include <Ice/LoggerF.h>
#include <Ice/Network.h>
#include <Ice/ProtocolPluginFacadeF.h>
#include <Ice/EndpointIF.h>
#include <Ice/EndpointFactoryF.h>
#include <IceWS/Plugin.h>

namespace IceWS
{

class Instance : public IceUtil::Shared
{
public:

    Instance(const Ice::CommunicatorPtr&);
    ~Instance();

    void initialize();

    Ice::CommunicatorPtr communicator() const;
    IceInternal::EndpointHostResolverPtr endpointHostResolver() const;
    IceInternal::ProtocolSupport protocolSupport() const;
    bool preferIPv6() const;
    IceInternal::NetworkProxyPtr networkProxy() const;
    std::string defaultHost() const;
    Ice::EncodingVersion defaultEncoding() const;
    int networkTraceLevel() const;
    std::string networkTraceCategory() const;
    size_t messageSizeMax() const;

    void destroy();

private:

    Ice::LoggerPtr _logger;
    IceInternal::ProtocolPluginFacadePtr _facade;
    bool _initialized;
};

}

#endif
