// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_WS_CONNECTOR_I_H
#define ICE_WS_CONNECTOR_I_H

#include <Ice/LoggerF.h>
#include <Ice/TransceiverF.h>
#include <Ice/Connector.h>
//#include <Ice/Protocol.h>
#include <Ice/Network.h>
#include <IceWS/InstanceF.h>

namespace IceWS
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

    ConnectorI(const InstancePtr&, Ice::Short, const IceInternal::ConnectorPtr&, const std::string&, int,
               const std::string&);
    virtual ~ConnectorI();
    friend class EndpointI;

    const InstancePtr _instance;
    const Ice::Short _type;
    const IceInternal::ConnectorPtr _delegate;
    const std::string _host;
    const int _port;
    const std::string _resource;
    const std::string _protocol;
    const Ice::LoggerPtr _logger;
};

}

#endif
