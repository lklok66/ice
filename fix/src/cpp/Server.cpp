// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <ExecutorI.h>
#include <BridgeImpl.h>
#include <dbname.h>

#include <quickfix/Application.h>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <list>

using namespace std;
using namespace IceFIX;

namespace IceFIX
{

const char* clordIdDBName = "clordidRR";
const char* seqnumDBName = "seqnumRR";
const char* clientDBName = "clients";
const char* messageDBName = "messages";

}

namespace 
{

class BridgeI : public Bridge
{
public:

    BridgeI(const BridgeImplPtr& bridge) :
        _bridge(bridge)
    {
    }

    virtual void
    connect(const string& id, const ReporterPrx& reporter, ExecutorPrx& executor, const Ice::Current& current)
    {
        _bridge->connect(id, reporter, executor, current);
    }

private:

    const BridgeImplPtr _bridge;
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

    virtual void
    clean(Ice::Long l, const Ice::Current& current)
    {
        _bridge->clean(l, current);
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

int
main(int argc, char* argv[])
{
    BridgeServer app;
    return app.main(argc, argv, "config.icefix");
}

BridgeServer::BridgeServer()
    // For debugging purposes only.
    // : Ice::Application(Ice::NoSignalHandling)
{
}

int
BridgeServer::run(int argc, char* argv[])
{
    if(argc > 1)
    {
        cerr << appName() << ": too many arguments" << endl;
        return EXIT_FAILURE;
    }

    const string dbenv = "db";
    BridgeImplPtr bridge = new BridgeImpl(communicator(), dbenv);

    const string fixConfig = communicator()->getProperties()->getProperty("IceFIX.FIXConfig");
    if(fixConfig.empty())
    {
        cerr << appName() << ": the property `IceFIX.FIXConfig' is not set" << endl;
        return EXIT_FAILURE;
    }

    FIX::SessionSettings settings(fixConfig);
    FIX::FileStoreFactory storeFactory(settings);
    FIX::ScreenLogFactory logFactory(true, true, true);
    FIX::SocketInitiator initiator(*bridge, storeFactory, settings, logFactory);
    bridge->setInitiator(&initiator);

    set < FIX::SessionID > sessions = settings.getSessions();
    
    if(sessions.size() != 1)
    {
        cerr << appName() << ": one FIX session must be defined" << endl;
        return EXIT_FAILURE;
    }
    FIX::SessionID sessionID = *sessions.begin();
    if(settings.get(sessionID).getString( "ConnectionType" ) != "initiator" )
    {
        cerr << appName() << ": the session must be an initiator" << endl;
        return EXIT_FAILURE;
    }

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("IceFIX.Bridge");
    adapter->addServantLocator(new ExecutorLocatorI(new ExecutorI(communicator(), dbenv, sessionID)),
                               "executor");
    adapter->add(new BridgeI(bridge), communicator()->stringToIdentity("bridge"));
    adapter->add(new BridgeAdminI(bridge), communicator()->stringToIdentity("admin"));
    adapter->activate();
    communicator()->waitForShutdown();

    bridge->stop();

    return EXIT_SUCCESS;
}
