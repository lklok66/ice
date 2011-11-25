// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice FIX is licensed to you under the terms described in the
// ICE_FIX_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef BRIDGE_H
#define BRIDGE_H

#include <Ice/Ice.h>
#include <Freeze/Freeze.h>
#include <ClientDB.h>
#include <ClientImpl.h>
#include <RoutingRecordDB.h>
#include <IceFIX/IceFIX.h>
#include <quickfix/Session.h>
#include <quickfix/Application.h>
#include <quickfix/Initiator.h>

namespace IceFIX
{

class DBCache;
typedef IceUtil::Handle<DBCache> DBCachePtr;

class BridgeImpl : public FIX::Application, public IceUtil::Shared, public IceUtil::Mutex
{
public:

    BridgeImpl(const std::string&, const Ice::CommunicatorPtr& communicator, const std::string&,
               const std::string&);

    void stop();
    void setInitiator(FIX::Initiator*);
    std::string _cpp_register(const QoS&, const Ice::Current&);
    void registerWithId(const std::string&, const QoS&, const Ice::Current&);
    void unregister(const std::string&, bool, const Ice::Current&);
    ExecutorPrx connect(const std::string&, const ReporterPrx&, const Ice::Current&);
    void activate(const Ice::Current&);
    void deactivate(const Ice::Current&);
    Ice::Long clean(Ice::Long, bool, const Ice::Current&);
    std::string dbstat(const Ice::Current&);
    BridgeStatus getStatus(const Ice::Current&);
    ClientInfoSeq getClients(const Ice::Current&);

    void executorDestroy(const Ice::Current&);
    int execute(const std::string&, const Ice::Current&);

private:

    void validateQoS(const QoS&);
    void halt(const Freeze::DatabaseException&) const;
    virtual void onCreate(const FIX::SessionID&);
    virtual void onLogon(const FIX::SessionID&);
    virtual void onLogout(const FIX::SessionID&);
    virtual void toAdmin(FIX::Message&, const FIX::SessionID&);
    virtual void toApp(FIX::Message& message, const FIX::SessionID&)
        throw(FIX::DoNotSend);
    virtual void fromAdmin(const FIX::Message&, const FIX::SessionID&)
        throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
    virtual void fromApp(const FIX::Message&, const FIX::SessionID&)
        throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);
    void route(const FIX::Message&);

    const std::string _name;
    const Ice::CommunicatorPtr _communicator;
    const std::string _instanceName;
    const std::string _dbenv;
    const DBCachePtr _dbCache;
    const int _trace;
    const IceUtil::TimerPtr _timer;
    const IceUtil::Time _retryInterval;
    const int _forwardTimeout; // This must be < _retryInterval.
    // The initiator is conceptually const also, since it is set at
    // startup and never changed.
    /*const*/ FIX::Initiator* _initiator;

    bool _active;
    FIX::Session* _session;

    std::map<std::string, ClientImplPtr> _clients;
};
typedef IceUtil::Handle<BridgeImpl> BridgeImplPtr;

}

#endif
