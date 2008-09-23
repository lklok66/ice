// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_OBJC_ACCEPTOR_H
#define ICE_OBJC_ACCEPTOR_H

#include <Ice/TransceiverF.h>
#include <Ice/InstanceF.h>
#include <Ice/TraceLevelsF.h>
#include <Ice/LoggerF.h>
#include <Ice/Acceptor.h>

#ifndef _WIN32
#   include <sys/socket.h> // For struct sockaddr_storage
#endif

#include <CoreFoundation/CFDictionary.h>

namespace IceObjC
{

class EndpointI;

class Acceptor : public IceInternal::Acceptor
{
public:

    virtual SOCKET fd();
    virtual void close();
    virtual void listen();
    virtual IceInternal::TransceiverPtr accept();
    virtual std::string toString() const;

    int effectivePort() const;

private:

    Acceptor(const IceInternal::InstancePtr&, const std::string&, int, Ice::Short, CFDictionaryRef);
    virtual ~Acceptor();
    friend class EndpointI;

    IceInternal::InstancePtr _instance;
    IceInternal::TraceLevelsPtr _traceLevels;
    Ice::LoggerPtr _logger;
    SOCKET _fd;
    int _backlog;
    struct sockaddr_storage _addr;
    Ice::Short _type;
    CFDictionaryRef _settings;
};

}

#endif
