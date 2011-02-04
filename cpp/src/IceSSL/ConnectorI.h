// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_SSL_CONNECTOR_I_H
#define ICE_SSL_CONNECTOR_I_H

#include <Ice/LoggerF.h>
#include <Ice/TransceiverF.h>
#include <Ice/Connector.h>
#include <IceSSL/InstanceF.h>

#ifdef _WIN32
#   include <winsock2.h>
#else
#   include <sys/socket.h> // For struct sockaddr_storage
#endif

namespace IceSSL
{

class EndpointI;

class ConnectorI : public IceInternal::Connector
{
public:
    
    virtual IceInternal::TransceiverPtr connect();

    virtual Ice::Short type() const;
    virtual std::string toString() const;

    virtual bool operator==(const IceInternal::Connector&) const;
    virtual bool operator!=(const IceInternal::Connector&) const;
    virtual bool operator<(const IceInternal::Connector&) const;

private:
    
    ConnectorI(const InstancePtr&, const std::string&, const struct sockaddr_storage&, Ice::Int, const std::string&);
    virtual ~ConnectorI();
    friend class EndpointI;

    const InstancePtr _instance;
    const Ice::LoggerPtr _logger;
    const std::string _host;
    struct sockaddr_storage _addr;
    const Ice::Int _timeout;
    const std::string _connectionId;
};

}

#endif
