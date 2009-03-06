// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef CLIENT_IMPL_H
#define CLIENT_IMPL_H

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <Freeze/Freeze.h>
#include <IceFIX/IceFIX.h>
#include <BridgeTypes.h>
#include <MessageDB.h>

#include <list>

namespace IceFIX
{

class ClientImpl :  public IceUtil::Shared, public IceUtil::Mutex
{
public:

    ClientImpl(const IceUtil::TimerPtr&, const Ice::CommunicatorPtr&, const std::string&,
               const IceUtil::Time&, int, const std::string&, const IceFIX::QoS&,
               const IceFIX::ReporterPrx&);

    bool filtered() const;
    IceFIX::ReporterPrx reporter() const;

    void enqueue(const Ice::Long&, const FIXBridge::Message&);
    void enqueue(const Ice::Long&);
    void connect(const IceFIX::ReporterPrx&);
    void destroy(bool);
    void disconnect();
    void send();
    void stop();

    void response();
    void exception(const IceFIX::ReporterPrx&, const Ice::Exception&);

private:

    void setReporter(const ReporterPrx&);
    void forwarded(FIXBridge::MessageDB&, Ice::Long);
    void sendImpl(const Ice::Long&);
    void sendImpl(const Ice::Long&, const FIXBridge::Message&);
    void halt(const Freeze::DatabaseException&) const;

    const IceUtil::TimerPtr _timer;
    const Ice::CommunicatorPtr _communicator;
    const std::string _dbenv;
    const int _trace;
    const IceUtil::Time _retryInterval;
    const std::string _id;
    /*const*/ bool _filtered;
    IceFIX::ReporterPrx _reporter;
    bool _destroy;
    bool _sending;
    std::list<Ice::Long> _queue;
    IceUtil::TimerTaskPtr _timerTask;
};
typedef IceUtil::Handle<ClientImpl> ClientImplPtr;

}

#endif
