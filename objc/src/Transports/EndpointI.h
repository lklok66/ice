// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_OBJC_ENDPOINT_I_H
#define ICE_OBJC_ENDPOINT_I_H

#include <Ice/EndpointI.h>
#include <Ice/EndpointFactory.h>

#include <CoreFoundation/CFDictionary.h>

namespace IceObjC
{

const Ice::Short TcpEndpointType = 1;
const Ice::Short SslEndpointType = 2;

class EndpointI : public IceInternal::EndpointI
{
public:

    EndpointI(const IceInternal::InstancePtr&, const std::string&, Ice::Int, Ice::Int, const std::string&, bool, 
              Ice::Short, CFDictionaryRef);
    EndpointI(const IceInternal::InstancePtr&, const std::string&, bool, Ice::Short, CFDictionaryRef);
    EndpointI(IceInternal::BasicStream*, Ice::Short, CFDictionaryRef);

    virtual void streamWrite(IceInternal::BasicStream*) const;
    virtual std::string toString() const;
    virtual Ice::Short type() const;
    virtual Ice::Int timeout() const;
    virtual IceInternal::EndpointIPtr timeout(Ice::Int) const;
    virtual IceInternal::EndpointIPtr connectionId(const std::string&) const;
    virtual bool compress() const;
    virtual IceInternal::EndpointIPtr compress(bool) const;
    virtual bool datagram() const;
    virtual bool secure() const;
    virtual bool unknown() const;
    virtual IceInternal::TransceiverPtr transceiver(IceInternal::EndpointIPtr&) const;
    virtual std::vector<IceInternal::ConnectorPtr> connectors() const;
    virtual void connectors_async(const IceInternal::EndpointI_connectorsPtr&) const;
    virtual IceInternal::AcceptorPtr acceptor(IceInternal::EndpointIPtr&, const std::string&) const;
    virtual std::vector<IceInternal::EndpointIPtr> expand() const;
    virtual bool equivalent(const IceInternal::EndpointIPtr&) const;

    virtual bool operator==(const IceInternal::EndpointI&) const;
    virtual bool operator!=(const IceInternal::EndpointI&) const;
    virtual bool operator<(const IceInternal::EndpointI&) const;

private:

    virtual std::vector<IceInternal::ConnectorPtr> connectors(const std::vector<struct sockaddr_storage>&) const;

#if defined(__SUNPRO_CC)
    //
    // COMPILERFIX: prevent the compiler from emitting a warning about
    // hidding these operators.
    //
    using LocalObject::operator==;
    using LocalObject::operator<;
#endif

    //
    // All members are const, because endpoints are immutable.
    //
    const IceInternal::InstancePtr _instance;
    const std::string _host;
    const Ice::Int _port;
    const Ice::Int _timeout;
    const std::string _connectionId;
    const bool _compress;
    const Ice::Short _type;
    const CFDictionaryRef _settings;
};

class EndpointFactory : public IceInternal::EndpointFactory
{
public:

    EndpointFactory(const IceInternal::InstancePtr&, bool);

    virtual ~EndpointFactory();

    virtual Ice::Short type() const;
    virtual std::string protocol() const;
    virtual IceInternal::EndpointIPtr create(const std::string&, bool) const;
    virtual IceInternal::EndpointIPtr read(IceInternal::BasicStream*) const;
    virtual void destroy();

private:

    IceInternal::InstancePtr _instance;
    const Ice::Short _type;
    const std::string _protocol;
    CFMutableDictionaryRef _settings;
};

}

#endif
