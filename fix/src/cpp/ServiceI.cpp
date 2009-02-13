// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <ServiceI.h>

#include <ExecutorI.h>
#include <BridgeImpl.h>
#include <dbname.h>

#include <quickfix/Application.h>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>

using namespace std;
using namespace IceFIX;

extern "C"
{

//
// Factory function
//
ICE_DECLSPEC_EXPORT IceBox::Service*
create(Ice::CommunicatorPtr communicator)
{
    return new ServiceI;
}

}

namespace IceFIX
{

const char* clordIdDBName = "clordidRR";
const char* seqnumDBName = "seqnumRR";
const char* clientDBName = "clients";
const char* messageDBName = "messages";

class ServiceImpl
{
public:

    ServiceImpl(const string& fixConfig, const BridgeImplPtr& b) :
        bridge(b),
        settings(fixConfig),
        storeFactory(settings),
        logFactory(true, true, true),
        initiator(*b, storeFactory, settings, logFactory)
    {
    }

    BridgeImplPtr bridge;

    FIX::SessionSettings settings;
    FIX::FileStoreFactory storeFactory;
    FIX::ScreenLogFactory logFactory;
    FIX::SocketInitiator initiator;

    Ice::ObjectAdapterPtr adapter;
};

}

namespace 
{

class BridgeI : public Bridge
{
public:

    BridgeI(const BridgeImplPtr& bridge, const IceFIX::BridgeAdminPrx& admin) :
        _bridge(bridge), _admin(admin)
    {
    }

    virtual IceFIX::BridgeAdminPrx
    getAdmin(const Ice::Current&)
    {
        return _admin;
    }

    virtual void
    connect(const string& id, const ReporterPrx& reporter, ExecutorPrx& executor, const Ice::Current& current)
    {
        _bridge->connect(id, reporter, executor, current);
    }

private:

    const BridgeImplPtr _bridge;
    const IceFIX::BridgeAdminPrx _admin;
};

class BridgeAdminI : public BridgeAdmin
{
public:

    BridgeAdminI(const BridgeImplPtr& bridge) :
        _bridge(bridge)
    {
    }

    virtual void
    activate(const Ice::Current& current)
    {
        _bridge->activate(current);
    }

    virtual void
    deactivate(const Ice::Current& current)
    {
        _bridge->deactivate(current);
    }

    virtual Ice::Long
    clean(Ice::Long l, bool commit, const Ice::Current& current)
    {
        return _bridge->clean(l, commit, current);
    }

    virtual string
    dbstat(const Ice::Current& current)
    {
        return _bridge->dbstat(current);
    }

    virtual BridgeStatus
    getStatus(const Ice::Current& current)
    {
        return _bridge->getStatus(current);
    }

    virtual string
    _cpp_register(const QoS & qos, const Ice::Current& current)
    {
        return _bridge->_cpp_register(qos, current);
    }

    virtual void
    registerWithId(const string& id, const QoS & qos, const Ice::Current& current)
    {
        _bridge->registerWithId(id, qos, current);
    }

    virtual void
    unregister(const string& id, const Ice::Current& current)
    {
        _bridge->unregister(id, current);
    }

    virtual ClientInfoSeq
    getClients(const Ice::Current& current)
    {
        return _bridge->getClients(current);
    }

private:

    const BridgeImplPtr _bridge;
};

class BridgeServer : public Ice::Application
{
public:

    BridgeServer();

    virtual int
    run(int, char*[]);
};

}

ServiceI::ServiceI() :
    _impl(0)
{
}

ServiceI::~ServiceI()
{
}

void
ServiceI::start(const string& name, const Ice::CommunicatorPtr& communicator, const Ice::StringSeq& args)
{
    Ice::PropertiesPtr properties = communicator->getProperties();
    //const string dbenv = properties->getPropertyWithDefault(name + ".Data", "db");
    const string dbenv = name;
    string instanceName = properties->getPropertyWithDefault(name + ".InstanceName", "IceFIX");
    BridgeImplPtr bridge = new BridgeImpl(communicator, instanceName, dbenv);

    const string fixConfig = properties->getProperty(name + ".FIXConfig");
    if(fixConfig.empty())
    {
        Ice::Warning warning(communicator->getLogger());
        warning << "the property `" << name << ".FIXConfig' is not set";
        throw IceBox::FailureException(__FILE__, __LINE__, "IceFIX service configuration is uncorrect");
    }
    _impl = new ServiceImpl(fixConfig, bridge);

    bridge->setInitiator(&_impl->initiator);

    set < FIX::SessionID > sessions = _impl->settings.getSessions();
    
    if(sessions.size() != 1)
    {
        Ice::Warning warning(communicator->getLogger());
        warning << "one FIX session must be defined";
        throw IceBox::FailureException(__FILE__, __LINE__, "IceFIX service configuration is uncorrect");
    }

    FIX::SessionID sessionID = *sessions.begin();
    if(_impl->settings.get(sessionID).getString( "ConnectionType" ) != "initiator" )
    {
        Ice::Warning warning(communicator->getLogger());
        warning << "the session must be an initiator";
        throw IceBox::FailureException(__FILE__, __LINE__, "IceFIX service configuration is uncorrect");
    }

    _impl->adapter = communicator->createObjectAdapter(name + ".Bridge");
    _impl->adapter->addServantLocator(new ExecutorLocatorI(new ExecutorI(communicator, dbenv, sessionID)),
                                      instanceName + "-Executor");
    IceFIX::BridgeAdminPrx adminPrx = IceFIX::BridgeAdminPrx::uncheckedCast(
        _impl->adapter->createProxy(communicator->stringToIdentity(instanceName + "/Admin")));
    _impl->adapter->add(new BridgeI(bridge, adminPrx), communicator->stringToIdentity(instanceName + "/Bridge"));
    _impl->adapter->add(new BridgeAdminI(bridge), adminPrx->ice_getIdentity());
    _impl->adapter->activate();
}

void
ServiceI::stop()
{
    assert(_impl != 0);
    _impl->bridge->stop();
    _impl->adapter->destroy();
    delete _impl;
}
