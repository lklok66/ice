// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_OBJC_TRANSCEIVER_H
#define ICE_OBJC_TRANSCEIVER_H

#include <Ice/InstanceF.h>
#include <Ice/TraceLevelsF.h>
#include <Ice/LoggerF.h>
#include <Ice/StatsF.h>
#include <Ice/Transceiver.h>

#include <CoreFoundation/CFStream.h>

namespace IceObjC
{

class Instance;
typedef IceUtil::Handle<Instance> InstancePtr;

class Transceiver : public IceInternal::Transceiver
{
    enum State
    {
        StateNeedConnect,
        StateConnectPending,
        StateConnected
    };

public:

    Transceiver(const InstancePtr&, SOCKET, CFReadStreamRef, CFWriteStreamRef, bool, const std::string&);
    virtual ~Transceiver();

    virtual SOCKET fd();
    virtual void close();
    virtual bool write(IceInternal::Buffer&);
    virtual bool read(IceInternal::Buffer&);
    virtual bool hasMoreData();
    virtual std::string type() const;
    virtual std::string toString() const;
    virtual IceInternal::SocketStatus initialize();
    virtual void checkSendSize(const IceInternal::Buffer&, size_t);

private:
    
    void checkCertificates();

    const InstancePtr _instance;
    const IceInternal::TraceLevelsPtr _traceLevels;
    const Ice::LoggerPtr _logger;
    const Ice::StatsPtr _stats;
    const std::string _host;
    SOCKET _fd;
    CFReadStreamRef _readStream;
    CFWriteStreamRef _writeStream;

    State _state;
    std::string _desc;
    bool _checkCertificates;
};

}

#endif
