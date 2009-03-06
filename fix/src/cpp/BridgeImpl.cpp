// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/IceUtil.h>
#include <MessageDB.h>
#include <MessageDBKey.h>
#include <BridgeImpl.h>
#include <dbname.h>

#include <list>

using namespace std;
using namespace IceFIX;
using namespace FIXBridge;

namespace FIX
{

USER_DEFINE_STRING(IceFIXClientId, 6464);

}


namespace IceFIX
{

// This ensures the databases are kept open, ensuring decent
// performance.
class DBCache : public IceUtil::Shared
{
public:

    DBCache(const Ice::CommunicatorPtr& communicator, const string& name) :
        _con(Freeze::createConnection(communicator, name)),
        _clientDB(_con, clientDBName),
        _clordIdDB(_con, clordIdDBName),
        _seqNumDB(_con, seqnumDBName),
        _messageDB(_con, messageDBName),
        _messageDBKey(_con, messageDBKeyName)
    {
    }

    ~DBCache()
    {
    }

private:

    const Freeze::ConnectionPtr _con;
    const FIXBridge::ClientDB _clientDB;
    const FIXBridge::RoutingRecordDB _clordIdDB;
    const FIXBridge::RoutingRecordDB _seqNumDB;
    const FIXBridge::MessageDB _messageDB;
    const FIXBridge::MessageDBKey _messageDBKey;
};

}


BridgeImpl::BridgeImpl(const string& name, const Ice::CommunicatorPtr& communicator, const string& instanceName,
                       const string& dbenv) :
    _name(name),
    _communicator(communicator),
    _instanceName(instanceName),
    _dbenv(dbenv),
    _dbCache(new DBCache(communicator, name)),
    _trace(communicator->getProperties()->getPropertyAsIntWithDefault(name + ".Trace.Bridge", 0)),
    _timer(new IceUtil::Timer()),
    _retryInterval(IceUtil::Time::seconds(communicator->getProperties()->getPropertyAsIntWithDefault(
                                              name + ".RetryInterval", 60))),
    _forwardTimeout(communicator->getProperties()->getPropertyAsIntWithDefault(
                        name + ".RetryInterval", 60)/2.0 * 1000),
    _initiator(0),
    _active(false),
    _session(0)
{
    // Initialize the database, if necessary.
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    MessageDBKey messageDBKey(connection, messageDBKeyName);
    for(;;)
    {
        try
        {
            MessageDBKey::iterator p = messageDBKey.find(0);
            if(p == messageDBKey.end())
            {
                messageDBKey.insert(make_pair(0, 0));
            }
            break;
        }
        catch(const Freeze::DeadlockException&)
        {
            continue;
        }
        catch(const Freeze::DatabaseException& ex)
        {
            halt(ex);
        }
    }
}

void
BridgeImpl::stop()
{
    bool stop = false;
    {
        IceUtil::Mutex::Lock sync(*this);
        if(_active)
        {
            if(_trace > 0)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "logout";
            }

            stop = true;
            _active = false;
        }
        _timer->destroy();

        for(map<string, ClientImplPtr>::const_iterator p = _clients.begin(); p != _clients.end(); ++p)
        {
            p->second->stop();
        }
    }

    if(stop)
    {
        _initiator->stop();
    }
}

void
BridgeImpl::setInitiator(FIX::Initiator* initiator)
{
    _initiator = initiator;
}

string
BridgeImpl::_cpp_register(const QoS& qos, const Ice::Current&)
{
    validateQoS(qos);

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clients(connection, clientDBName);
    string clientId = IceUtil::generateUUID();

    for(;;)
    {
        try
        {

            Client c;
            c.id = clientId;
            c.qos = qos;
            pair<ClientDB::iterator, bool> q = clients.insert(make_pair(clientId, c));
            assert(q.second);
            break;
        }
        catch(const Freeze::DeadlockException&)
        {
            continue;
        }
        catch(const Freeze::DatabaseException& ex)
        {
            halt(ex);
        }
    }

    IceUtil::Mutex::Lock sync(*this);
    ClientImplPtr client = new ClientImpl(_timer, _communicator, _dbenv, _retryInterval, _trace,
                                          clientId, qos, 0);
    _clients.insert(make_pair(clientId, client));
    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "register: " << clientId;
    }
    return clientId;
}

void
BridgeImpl::registerWithId(const string& clientId, const QoS& qos, const Ice::Current&)
{
    validateQoS(qos);

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clients(connection, clientDBName);
    for(;;)
    {
        try
        {
            ClientDB::const_iterator p = clients.find(clientId);
            if(p != clients.end())
            {
                throw RegistrationException("id is already registered");
            }

            Client c;
            c.qos = qos;
            c.id = clientId;
            pair<ClientDB::iterator, bool> q = clients.insert(make_pair(clientId, c));
            assert(q.second);
            break;
        }
        catch(const Freeze::DeadlockException&)
        {
            continue;
        }
        catch(const Freeze::DatabaseException& ex)
        {
            halt(ex);
        }
    }

    IceUtil::Mutex::Lock sync(*this);
    ClientImplPtr client = new ClientImpl(_timer, _communicator, _dbenv, _retryInterval, _trace,
                                          clientId, qos, 0);
    _clients.insert(make_pair(clientId, client));
    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "register: " << clientId;
    }
}

void
BridgeImpl::unregister(const string& clientId, bool force, const Ice::Current&)
{
    IceUtil::Mutex::Lock sync(*this);
    map<string, ClientImplPtr>::iterator p = _clients.find(clientId);
    if(p == _clients.end())
    {
        throw RegistrationException("id not registered");
    }
    p->second->destroy(force);
    _clients.erase(p);
}

ExecutorPrx
BridgeImpl::connect(const string& clientId, const ReporterPrx& reporter, const Ice::Current& current)
{
    IceUtil::Mutex::Lock sync(*this);
    map<string, ClientImplPtr>::const_iterator p = _clients.find(clientId);
    if(p == _clients.end())
    {
        throw RegistrationException("id not registered");
    }
    p->second->connect(reporter->ice_timeout(_forwardTimeout));

    Ice::Identity oid;
    oid.category = _instanceName + "-Executor";
    oid.name = clientId;
    return ExecutorPrx::uncheckedCast(current.adapter->createProxy(oid));
}

void
BridgeImpl::activate(const Ice::Current&)
{
    IceUtil::Mutex::Lock sync(*this);
    if(!_active)
    {
        if(_trace > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), "Bridge");
            trace << "activate";
        }

        _active = true;
        _initiator->start();
    }
}

void
BridgeImpl::deactivate(const Ice::Current&)
{
    bool stop = false;
    {
        IceUtil::Mutex::Lock sync(*this);
        if(_active)
        {
            if(_trace > 0)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "deactivate";
            }

            _active = false;
            stop = true;
        }
    }

    // This must be called outside of the mutex since stop blocks
    // until the initiator is stopped, and in the process of stopping
    // the initiator thread calls onLogout (which locks the same
    // mutex).
    if(stop)
    {
        _initiator->stop();
    }
}

Ice::Long
BridgeImpl::clean(Ice::Long t, bool commit, const Ice::Current&)
{
    IceUtil::Time timeout = IceUtil::Time::seconds(t);

    long erased;

    IceUtil::Time start = IceUtil::Time::now();

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    RoutingRecordDB clordidDB(connection, clordIdDBName);
    RoutingRecordDB seqnumDB(connection, seqnumDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);

            erased = 0;
            RoutingRecordDB::iterator p = clordidDB.begin();
            while(p != clordidDB.end())
            {
                if(start - IceUtil::Time::seconds(p->second.timestamp) > timeout)
                {
                    RoutingRecordDB::iterator tmp = p;
                    ++p;
                    clordidDB.erase(tmp);
                    ++erased;
                }
                else
                {
                    ++p;
                }
            }
            p = seqnumDB.begin();
            while(p != seqnumDB.end())
            {
                if(start - IceUtil::Time::seconds(p->second.timestamp) > timeout)
                {
                    RoutingRecordDB::iterator tmp = p;
                    ++p;
                    ++erased;
                    seqnumDB.erase(tmp);
                }
                else
                {
                    ++p;
                }
            }
            if(commit)
            {
                txn.commit();
            }
            break;
        }
        catch(const Freeze::DeadlockException&)
        {
            continue;
        }
        catch(const Freeze::DatabaseException& ex)
        {
            halt(ex);
        }
    }

    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "clean: removed " << erased << " records";
    }
    return erased;
}

string
BridgeImpl::dbstat(const Ice::Current&)
{
    ostringstream os;

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    RoutingRecordDB clordidDB(connection, clordIdDBName);
    RoutingRecordDB seqnumDB(connection, seqnumDBName);
    ClientDB clientDB(connection, clientDBName);
    MessageDB messageDB(connection, messageDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);

            os.clear();
            os << "client database has " << clientDB.size() << " records" << endl;
            os << "routing databases have " << clordidDB.size() + seqnumDB.size() << " records" << endl;
            os << "message database has " << messageDB.size() << " records" << endl;
            break;
        }
        catch(const Freeze::DeadlockException&)
        {
            continue;
        }
        catch(const Freeze::DatabaseException& ex)
        {
            halt(ex);
        }
    }
    return os.str();
}

BridgeStatus
BridgeImpl::getStatus(const Ice::Current&)
{
    IceUtil::Mutex::Lock sync(*this);
    BridgeStatus status;
    if(!_active)
    {
        status = BridgeStatusNotActive;
    }
    else
    {
        if(_session->isLoggedOn())
        {
            status = BridgeStatusLoggedOn;
        }
        else
        {
            status = BridgeStatusActive;
        }
    }
    return status;
}

ClientInfoSeq
BridgeImpl::getClients(const Ice::Current&)
{
    IceUtil::Mutex::Lock sync(*this);
    ClientInfoSeq s;
    for(map<string, ClientImplPtr>::const_iterator p = _clients.begin(); p != _clients.end(); ++p)
    {
        ClientInfo info;
        info.id = p->first;
        info.isConnected = p->second->reporter();
        s.push_back(info);
    }
    return s;
}

void
BridgeImpl::executorDestroy(const Ice::Current& current)
{
    IceUtil::Mutex::Lock sync(*this);
    map<string, ClientImplPtr>::iterator p = _clients.find(current.id.name);
    if(p != _clients.end())
    {
        p->second->disconnect();
    }
}

int
BridgeImpl::execute(const string& data, const Ice::Current& current)
{
    FIX::Message message(data);

    // If there is a ClOrdID then ensure it is not a duplicate.
    if(message.isSetField(FIX::FIELD::ClOrdID))
    {
        FIX::ClOrdID id;
        message.getField(id);

        const std::string& msgTypeValue = message.getHeader().getField(FIX::FIELD::MsgType);

        Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _name);
        RoutingRecordDB clordidDB(connection, clordIdDBName);
        for(;;)
        {
            try
            {
                RoutingRecordDB::const_iterator p = clordidDB.find(id);
                // OrderStatusRequest messages are restricted to come
                // from the client that originated the order.
                if(msgTypeValue == "H")
                {
                    if(p == clordidDB.end())
                    {
                        Ice::Warning warning(_communicator->getLogger());
                        warning << "OrderStatusRequest: No matching ClOrdID: `" << id << "'";
                        throw ExecuteException("No matching ClOrdID for OrderStatusRequest");
                    }
                    if(p->second.id != current.id.name)
                    {
                        Ice::Warning warning(_communicator->getLogger());
                        warning << "OrderStatusRequest: OrderStatusRequest is limited to originating client";
                        throw ExecuteException("client mismatch in ClOrdID for OrderStatusRequest");
                    }
                }
                else
                {
                    if(p != clordidDB.end())
                    {
						ostringstream os;
						os << "duplicate ClOrdID: `" << id << "'";
                        Ice::Warning warning(_communicator->getLogger());
                        warning << os.str();
                        throw ExecuteException(os.str());
                    }
                }
                break;
            }
            catch(const Freeze::DeadlockException&)
            {
                continue;
            }
            catch(const Freeze::DatabaseException& ex)
            {
                halt(ex);
            }
        }
    }

    FIX::IceFIXClientId clientId(current.id.name);
    message.getHeader().setField(clientId);

    // No need to fill the header, since we're sending
    // specifically to the session object.
    _session->send(message);

    FIX::MsgSeqNum seqNum;
    message.getHeader().getField(seqNum);

    return seqNum;
}

void
BridgeImpl::validateQoS(const IceFIX::QoS& qos)
{
    for(IceFIX::QoS::const_iterator p = qos.begin(); p != qos.end(); ++p)
    {
        if(p->first == "filtered")
        {
        }
        else
        {
            throw RegistrationException("invalid qos `" + p->first + "'");
        }
    }
}

void
BridgeImpl::halt(const Freeze::DatabaseException& ex) const
{
    Ice::Error error(_communicator->getLogger());
    error << "fatal exception: " << ex << "\n*** Aborting application ***";
    abort();
}

void
BridgeImpl::onCreate(const FIX::SessionID& session)
{
    // This can only be called on construction, so no need to sync,
    // and deadlock exceptions cannot occur.
    //
    // IceUtil::Mutex::Lock sync(*this);

    // Once onCreate has been called messages can be sent to the
    // trading partner.
    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "onCreate: " << session;
    }

    _session = FIX::Session::lookupSession(session);

    // Build an initial set of clients and queue the set of messages
    // per client. I don't want to start enqueuing from within the
    // database access since that will start concurrent database and
    // allow the possibilities of a deadlock exception.
    list<pair<Ice::Long, list<ClientImplPtr> > > m;

    // Recreate each client.
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clientsDB(connection, clientDBName);
    MessageDB messageDB(connection, messageDBName);
    try
    {
        Freeze::TransactionHolder txn(connection);
        {
            for(ClientDB::const_iterator p = clientsDB.begin(); p != clientsDB.end(); ++p)
            {
                IceFIX::ReporterPrx reporter;
                if(p->second.reporter)
                {
                    reporter = p->second.reporter->ice_timeout(_forwardTimeout);
                }
                ClientImplPtr client = new ClientImpl(_timer, _communicator, _dbenv, _retryInterval, _trace,
                                                      p->second.id, p->second.qos, reporter);
                _clients.insert(make_pair(p->second.id, client));
            }
        }

        MessageDB::iterator p = messageDB.begin();
        while(p != messageDB.end())
        {
            // The set of clients which have already received the
            // message.
            set<string> forwarded;
            // The set of clients which should receive the message.
            set<string> clients;

            Message msg = p->second;

            copy(msg.forwarded.begin(), msg.forwarded.end(), inserter(forwarded, forwarded.begin()));
            copy(msg.clients.begin(), msg.clients.end(), inserter(clients, clients.begin()));

            // Compute the set difference. The result is the set of
            // clients which have not received the message.
            set<string> routing;
            set_difference(clients.begin(), clients.end(), forwarded.begin(), forwarded.end(),
                           inserter(routing, routing.begin()));

            list<ClientImplPtr> c;

            for(set<string>::const_iterator q = routing.begin(); q != routing.end(); ++q)
            {
                map<string, ClientImplPtr>::const_iterator r = _clients.find(*q);
                if(r == _clients.end())
                {
                    msg.forwarded.push_back(*q);
                }
                else
                {
                    c.push_back(r->second);
                }
            }

            if(msg.forwarded.size() == msg.clients.size())
            {
                assert(c.empty());
                if(_trace > 1)
                {
                    Ice::Trace trace(_communicator->getLogger(), "Bridge");
                    trace << "send messageId: " << p->first
                          << ": message fully routed (unregistered client), erasing";
                }
                MessageDB::iterator tmp = p;
                ++p;
                messageDB.erase(tmp);
            }
            else
            {
                // Save the message, the forwarded list changed.
                if(c.size() != routing.size())
                {
                    p.set(msg);
                }

                m.push_back(make_pair(p->first, c));
                ++p;
            }
        }
        txn.commit();
    }
    catch(const Freeze::DeadlockException&)
    {
        Ice::Error error(_communicator->getLogger());
        error << "fatal exception: unexpected DeadlockException\n*** Aborting application ***";
        abort();
    }
    catch(const Freeze::DatabaseException& ex)
    {
        halt(ex);
    }

    // Now enqueue all the messages, at this point the database becomes active.
    for(list<pair<Ice::Long, list<ClientImplPtr> > >::const_iterator p = m.begin(); p != m.end(); ++p)
    {
        for(list<ClientImplPtr>::const_iterator q = p->second.begin(); q != p->second.end(); ++q)
        {
            (*q)->enqueue(p->first);
        }
    }
}

void
BridgeImpl::onLogon(const FIX::SessionID& session)
{
    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "onLogon: " << session;
    }
}

void
BridgeImpl::onLogout(const FIX::SessionID& session)
{
    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "onLogout: " << session;
    }
}

void
BridgeImpl::toAdmin(FIX::Message&, const FIX::SessionID&)
{
}

void
BridgeImpl::toApp(FIX::Message& message, const FIX::SessionID&)
    throw (FIX::DoNotSend)
{
    // Ignore duplicated messages.
    if(message.getHeader().isSetField(FIX::FIELD::PossDupFlag))
    {
        return;
    }

    // Create the reverse routing record for the incoming FIX message.
    if(!message.isSetField(FIX::FIELD::ClOrdID))
    {
        Ice::Warning warning(_communicator->getLogger());
        warning << "message is forwarded with no ClOrdID";
        return;
    }

    FIX::MsgSeqNum seqNum;
    message.getHeader().getField(seqNum);

    FIX::ClOrdID clOrdID;
    message.getField(clOrdID);

    FIX::IceFIXClientId clientId;
    try
    {
        message.getHeader().getField(clientId);
        message.getHeader().removeField(clientId.getField());
    }
    catch(const FIX::FieldNotFound&)
    {
        Ice::Warning warning(_communicator->getLogger());
        warning << "message is missing IceFIXClientId";
        return;
    }

    RoutingRecord o;
    o.id = clientId;
    o.timestamp = IceUtil::Time::now().toSeconds();

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    RoutingRecordDB clordidDB(connection, clordIdDBName);
    RoutingRecordDB seqnumDB(connection, seqnumDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);

            pair<RoutingRecordDB::iterator, bool> p;
            // We don't insert routing records for OrderStatusRequest
            // messages.
            const string& msgTypeValue = message.getHeader().getField(FIX::FIELD::MsgType);
            if(msgTypeValue != "H")
            {
                p = clordidDB.insert(make_pair(clOrdID, o));
                // Duplicate ClOrdID is a client error.
                if(!p.second)
                {
                    if(p.first->second.id != clientId)
                    {
                        Ice::Warning warning(_communicator->getLogger());
                        warning << "duplicate ClOrdID detected";
                    }
                    return;
                }
            }

            // seqnum routing messages are inserted regardless of
            // message type, for reverse routing of BusinessReject
            // messages.
            ostringstream os;
            os << seqNum;
            p = seqnumDB.insert(make_pair(os.str(), o));

            // If a record already exists for this sequence number,
            // overwrite it.
            if(!p.second)
            {
                p.first.set(o);
            }

            txn.commit();
            break;
        }
        catch(const Freeze::DeadlockException&)
        {
            continue;
        }
        catch(const Freeze::DatabaseException& ex)
        {
            halt(ex);
        }
    }
}

void
BridgeImpl::fromAdmin(const FIX::Message& message, const FIX::SessionID& session)
    throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
    const std::string& msgTypeValue = message.getHeader().getField(FIX::FIELD::MsgType);
    if(msgTypeValue != "3") // Reject
    {
        return;
    }

    if(_trace > 1)
    {
        FIX::MsgSeqNum seqNum;
        message.getHeader().getField(seqNum);

        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "fromAdmin: seqNum: " << seqNum << " session: `" << session << "'";
        trace << " XML\n" << message.toXML();
    }
    route(message);
}

void
BridgeImpl::fromApp(const FIX::Message& message, const FIX::SessionID& session)
    throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    if(_trace > 1)
    {
        FIX::MsgSeqNum seqNum;
        message.getHeader().getField(seqNum);

        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "fromApp: seqNum: " << seqNum << " session: `" << session << "'";
        trace << " XML\n" << message.toXML();
    }
    route(message);
}

void
BridgeImpl::route(const FIX::Message& message)
{
    const std::string& msgTypeValue = message.getHeader().getField(FIX::FIELD::MsgType);

    if(_trace > 1)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "msgTypeValue: `" << msgTypeValue << "'";
    }

    Ice::Long key = -1;

    FIX::MsgSeqNum seqNum;
    message.getHeader().getField(seqNum);
    string data = message.toString();

    list<ClientImplPtr> clients;

    // Need to lock the mutex due to accessing the _clients map.
    IceUtil::Mutex::Lock sync(*this);

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    MessageDB messageDB(connection, messageDBName);
    MessageDBKey messageDBKey(connection, messageDBKeyName);
    RoutingRecordDB clordidDB(connection, clordIdDBName);
    RoutingRecordDB seqnumDB(connection, seqnumDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);

            MessageDBKey::iterator p = messageDBKey.find(0);
            assert(p != messageDBKey.end());
            key = p->second;
            p.set(key+1);

            clients.clear();

            Message m;
            m.data = data;

            // Find all non-filtered clients, and add to the clients
            // list.
            for(map<string, ClientImplPtr>::const_iterator r = _clients.begin(); r != _clients.end(); ++r)
            {
                if(!r->second->filtered())
                {
                    clients.push_back(r->second);
                    m.clients.push_back(r->first);
                }
            }

            if(msgTypeValue == "j" || msgTypeValue == "3")
            {
                try
                {
                    FIX::RefSeqNum refSeqNum;
                    message.getField(refSeqNum);

                    ostringstream os;
                    os << refSeqNum;
                    RoutingRecordDB::const_iterator q = seqnumDB.find(os.str());
                    if(q == seqnumDB.end())
                    {
                        Ice::Warning warning(_communicator->getLogger());
                        warning << "Bridge: fromApp: can't find routing record for RefSeqNumber: " << refSeqNum;
                    }
                    else
                    {
                        map<string, ClientImplPtr>::const_iterator r = _clients.find(q->second.id);
                        if(r != _clients.end())
                        {
                            clients.push_back(r->second);
                            m.clients.push_back(r->first);
                        }
                        else
                        {
                            Ice::Warning warning(_communicator->getLogger());
                            warning << "Bridge: fromApp: can't find client with id: `" << q->second.id << "'";
                        }
                    }
                }
                catch(const FIX::FieldNotFound&)
                {
                    Ice::Warning warning(_communicator->getLogger());
                    warning << "Bridge: fromApp: unable to route business reject message because of missing RefSeqNum";
                }
            }
            else if(message.isSetField(FIX::FIELD::ClOrdID))
            {
                FIX::ClOrdID clOrdID;
                message.getField(clOrdID);

                RoutingRecordDB::const_iterator q = clordidDB.find(clOrdID);
                if(q == clordidDB.end())
                {
                    Ice::Warning warning(_communicator->getLogger());
                    warning << "Bridge: fromApp: can't find routing record for ClOrdID: `" << clOrdID << "'";
                }
                else
                {
                    map<string, ClientImplPtr>::const_iterator r = _clients.find(q->second.id);
                    if(r != _clients.end())
                    {
                        m.clients.push_back(r->first);
                        clients.push_back(r->second);
                    }
                    else
                    {
                        Ice::Warning warning(_communicator->getLogger());
                        warning << "Bridge: fromApp: can't find client with id: `" << q->second.id << "'";
                    }
                }
            }

            if(clients.empty())
            {
                Ice::Warning warning(_communicator->getLogger());
                warning << "Bridge: fromApp: " << seqNum << ": no route for message\n";
                warning << message.toXML();
                return;
            }

            if(_trace > 1)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "fromApp: messageId: " << key << " seqNum: " << seqNum << ": route message to clients: ";
                for(vector<string>::const_iterator p = m.clients.begin(); p != m.clients.end(); ++p)
                {
                    if(p != m.clients.begin())
                    {
                        trace << ",";
                    }
                    trace << "`" << *p << "'";
                }
            }

            messageDB.insert(make_pair(key, m));

            txn.commit();
            break;
        }
        catch(const Freeze::DeadlockException&)
        {
            continue;
        }
        catch(const Freeze::DatabaseException& ex)
        {
            halt(ex);
        }
    }

    for(list<ClientImplPtr>::const_iterator p = clients.begin(); p != clients.end(); ++p)
    {
        (*p)->enqueue(key, data);
    }
}
