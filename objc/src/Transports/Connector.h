// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_OBJC_CONNECTOR_H
#define ICE_OBJC_CONNECTOR_H

#include <Ice/TransceiverF.h>
#include <Ice/InstanceF.h>
#include <Ice/TraceLevelsF.h>
#include <Ice/LoggerF.h>
#include <Ice/Connector.h>

#ifdef _WIN32
#   include <winsock2.h>
#else
#   include <sys/socket.h>
#endif

#include <CoreFoundation/CFDictionary.h>

namespace IceObjC
{

class EndpointI;

class Connector : public IceInternal::Connector
{
public:
    
    virtual IceInternal::TransceiverPtr connect();

    virtual Ice::Short type() const;
    virtual std::string toString() const;

    virtual bool operator==(const IceInternal::Connector&) const;
    virtual bool operator!=(const IceInternal::Connector&) const;
    virtual bool operator<(const IceInternal::Connector&) const;

private:
    
    Connector(const IceInternal::InstancePtr&, const struct sockaddr_storage&, Ice::Int, const std::string&,
              Ice::Short, CFDictionaryRef);
    virtual ~Connector();
    friend class EndpointI;

    const IceInternal::InstancePtr _instance;
    const IceInternal::TraceLevelsPtr _traceLevels;
    const Ice::LoggerPtr _logger;
    struct sockaddr_storage _addr;
    const Ice::Int _timeout;
    const std::string _connectionId;
    const Ice::Short _type;
    CFDictionaryRef _settings;
};

}

#endif
