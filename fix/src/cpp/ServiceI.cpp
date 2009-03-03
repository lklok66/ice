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
#include <IceBox/IceBox.h>

#include <ExecutorI.h>
#include <BridgeImpl.h>
#include <dbname.h>

#include <quickfix/Application.h>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>

using namespace std;
using namespace IceFIX;

namespace IceFIX
{

class ServiceImpl;

class ServiceI : public ::IceBox::Service
{
public:

    ServiceI();
    virtual ~ServiceI();

    virtual void start(const std::string&, const Ice::CommunicatorPtr&, const Ice::StringSeq&);
    virtual void stop();

private:

    void validateProperties(const std::string&, const Ice::PropertiesPtr&, const Ice::LoggerPtr&);

    ServiceImpl* _impl;
};

};

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
const char* messageDBKeyName = "messageKey";

class Log : public FIX::Log
{
public:

    Log(const string& name, const Ice::CommunicatorPtr& communicator) :
        _communicator(communicator),
        _prefix("GLOBAL")
    {
        init(name);
    }

    Log(const string& name, const Ice::CommunicatorPtr& communicator, const FIX::SessionID& id) :
        _communicator(communicator),
        _prefix(id.toString())
    {
        init(name);
    }
    
    virtual void clear() 
    {
    }

    virtual void onIncoming(const string& msg)
    {
        if(_traceIncoming > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), _prefix);
            trace << msg;
        }
    }

    virtual void onOutgoing(const string& msg)
    {
        if(_traceOutgoing > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), _prefix);
            trace << msg;
        }
    }

    virtual void onEvent(const string& msg)
    {
        if(_traceEvent)
        {
            Ice::Trace trace(_communicator->getLogger(), _prefix);
            trace << msg;
        }
    }

private:

    void init(const string& name)
    {
        Ice::PropertiesPtr properties = _communicator->getProperties();
        _traceIncoming = properties->getPropertyAsIntWithDefault(name + ".Trace.Incoming", 0);
        _traceOutgoing = properties->getPropertyAsIntWithDefault(name + ".Trace.Outgoing", 0);
        _traceEvent = properties->getPropertyAsIntWithDefault(name + ".Trace.Event", 0);
    }

    const Ice::CommunicatorPtr _communicator;
    const string _prefix;
    /*const*/ int _traceIncoming;
    /*const*/ int _traceOutgoing;
    /*const*/ int _traceEvent;
};

class LogFactory : public FIX::LogFactory
{
public:

    LogFactory(const string& name, const Ice::CommunicatorPtr& communicator) :
        _name(name),
        _communicator(communicator)
    {
    }

    virtual FIX::Log* create()
    {
        return new Log(_name, _communicator);
    }

    virtual FIX::Log* create(const FIX::SessionID& id)
    {
        return new Log(_name, _communicator, id);
    }

    virtual void destroy(FIX::Log* l)
    {
        delete l;
    }

private:

    const string _name;
    const Ice::CommunicatorPtr _communicator;
};

class ServiceImpl
{
public:

    ServiceImpl(const string& name, const Ice::CommunicatorPtr& communicator, const string& fixConfig,
                const BridgeImplPtr& b) :
        bridge(b),
        settings(fixConfig),
        storeFactory(settings),
        logFactory(name, communicator),
        initiator(*b, storeFactory, settings, logFactory)
    {
    }

    BridgeImplPtr bridge;

    FIX::SessionSettings settings;
    FIX::FileStoreFactory storeFactory;
    LogFactory logFactory;
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

    virtual ExecutorPrx
    connect(const string& id, const ReporterPrx& reporter, const Ice::Current& current)
    {
        return _bridge->connect(id, reporter, current);
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
    unregister(const string& id, bool force, const Ice::Current& current)
    {
        _bridge->unregister(id, force, current);
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
    validateProperties(name, properties, communicator->getLogger());

    const string dbenv = name;
    string instanceName = properties->getPropertyWithDefault(name + ".InstanceName", "IceFIX");
    BridgeImplPtr bridge = new BridgeImpl(name, communicator, instanceName, dbenv);

    const string fixConfig = properties->getProperty(name + ".FIXConfig");
    if(fixConfig.empty())
    {
        Ice::Warning warning(communicator->getLogger());
        warning << "the property `" << name << ".FIXConfig' is not set";
        throw IceBox::FailureException(__FILE__, __LINE__, "IceFIX service configuration is invalid");
    }
    _impl = new ServiceImpl(name, communicator, fixConfig, bridge);

    bridge->setInitiator(&_impl->initiator);

    set < FIX::SessionID > sessions = _impl->settings.getSessions();
    
    if(sessions.size() != 1)
    {
        Ice::Warning warning(communicator->getLogger());
        warning << "one FIX session must be defined";
        throw IceBox::FailureException(__FILE__, __LINE__, "IceFIX service configuration is invalid");
    }

    FIX::SessionID sessionID = *sessions.begin();
    if(_impl->settings.get(sessionID).getString( "ConnectionType" ) != "initiator" )
    {
        Ice::Warning warning(communicator->getLogger());
        warning << "the session must be an initiator";
        throw IceBox::FailureException(__FILE__, __LINE__, "IceFIX service configuration is invalid");
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

//
// Match `s' against the pattern `pat'. A * in the pattern acts
// as a wildcard: it matches any non-empty sequence of characters.
// We match by hand here because it's portable across platforms 
// (whereas regex() isn't). Only one * per pattern is supported.
//
// Taken from IceUtilInternal.
//
static bool
match(const string& s, const string& pat, bool emptyMatch = false)
{
    assert(!s.empty());
    assert(!pat.empty());

    //
    // If pattern does not contain a wildcard just compare strings.
    //
    string::size_type beginIndex = pat.find('*');
    if(beginIndex == string::npos)
    {
        return s == pat;
    }

    //
    // Make sure start of the strings match
    //
    if(beginIndex > s.length() || s.substr(0, beginIndex) != pat.substr(0, beginIndex))
    {
        return false;
    }

    //
    // Make sure there is something present in the middle to match the
    // wildcard. If emptyMatch is true, allow a match of "".
    //
    string::size_type endLength = pat.length() - beginIndex - 1;
    if(endLength > s.length())
    {
        return false;
    }
    string::size_type endIndex = s.length() - endLength;
    if(endIndex < beginIndex || (!emptyMatch && endIndex == beginIndex))
    {
        return false;
    }

    //
    // Make sure end of the strings match
    //
    if(s.substr(endIndex, s.length()) != pat.substr(beginIndex + 1, pat.length()))
    {
        return false;
    }

    return true;
}

void
ServiceI::validateProperties(const string& name, const Ice::PropertiesPtr& properties, const Ice::LoggerPtr& logger)
{
    static const string suffixes[] =
    {
        "Bridge.AdapterId",
        "Bridge.Endpoints",
        "Bridge.Locator",
        "Bridge.PublishedEndpoints",
        "Bridge.RegisterProcess",
        "Bridge.ReplicaGroupId",
        "Bridge.Router",
        "Bridge.ThreadPool.Size",
        "Bridge.ThreadPool.SizeMax",
        "Bridge.ThreadPool.SizeWarn",
        "Bridge.ThreadPool.StackSize",
        "Trace.Incoming",
        "Trace.Outgoing",
        "Trace.Event",
        "Trace.Bridge",
        "InstanceName",
        "RetryInterval",
        "FIXConfig",
    };

    vector<string> unknownProps;
    string prefix = name + ".";
    Ice::PropertyDict props = properties->getPropertiesForPrefix(prefix);
    for(Ice::PropertyDict::const_iterator p = props.begin(); p != props.end(); ++p)
    {
        bool valid = false;
        for(unsigned int i = 0; i < sizeof(suffixes)/sizeof(*suffixes); ++i)
        {
            string prop = prefix + suffixes[i];
            if(match(p->first, prop))
            {
                valid = true;
                break;
            }
        }
        if(!valid)
        {
            unknownProps.push_back(p->first);
        }
    }

    if(!unknownProps.empty())
    {
        Ice::Warning out(logger);
        out << "found unknown properties for IceFIX service `" << name << "':";
        for(vector<string>::const_iterator p = unknownProps.begin(); p != unknownProps.end(); ++p)
        {
            out << "\n    " << *p;
        }
    }
}
