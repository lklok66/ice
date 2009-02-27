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
#include <ExecutorI.h>
#include <BridgeImpl.h>
#include <dbname.h>

#include <list>

using namespace std;
using namespace IceFIX;
using namespace FIXBridge;

namespace 
{

class SendTimerTask : public IceUtil::TimerTask
{
public:

    SendTimerTask(const BridgeImplPtr& bridge, Ice::Long messageId) :
        _bridge(bridge), _messageId(messageId)
    {
    }

    virtual void
    runTimerTask()
    {
        _bridge->send(_messageId);
    }

private:

    const BridgeImplPtr _bridge;
    const Ice::Long _messageId;
};

class RoutingHandler : public IceUtil::Shared, public IceUtil::Mutex
{
public:

    RoutingHandler(const BridgeImplPtr& bridge, int trace, int messageId, const Ice::CommunicatorPtr& communicator) :
        _bridge(bridge), _trace(trace), _messageId(messageId), _communicator(communicator)
    {
    }

    void add(const string& clientId)
    {
        IceUtil::Mutex::Lock sync(*this);
        _replies.insert(clientId);
    }

    void exception(const string& clientId, const ReporterPrx& reporter, const Ice::Exception& ex)
    {
        bool done = false;
        {
            IceUtil::Mutex::Lock sync(*this);
            int count = _replies.erase(clientId);
            assert(count == 1);
            done = _replies.empty();
        }

        if(_trace > 1)
        {
            Ice::Trace trace(_communicator->getLogger(), "Bridge");
            trace << "routing: " << _messageId << ": client: " << clientId << ": failed: " << ex;
        }

        if(dynamic_cast<const Ice::ObjectNotExistException*>(&ex))
        {
            _bridge->clientError(clientId, reporter);
        }

        if(done)
        {
            _bridge->sendComplete(_messageId, _routed);
        }
    }

    void response(const string& clientId)
    {
        bool done = false;
        {
            IceUtil::Mutex::Lock sync(*this);
            int count = _replies.erase(clientId);
            assert(count == 1);
            done = _replies.empty();
            _routed.insert(clientId);
        }
        if(done)
        {
            _bridge->sendComplete(_messageId,_routed);
        }
    }

private:

    const BridgeImplPtr _bridge;
    const int _trace;
    const int _messageId;
    const Ice::CommunicatorPtr _communicator;
    set<string> _replies;
    set<string> _routed;
};
typedef IceUtil::Handle<RoutingHandler> RoutingHandlerPtr;

class MessageAsyncI : public AMI_Reporter_message
{
public:

    MessageAsyncI(const string& clientId, const ReporterPrx& reporter, const RoutingHandlerPtr& handler) :
        _clientId(clientId), _reporter(reporter), _handler(handler)
    {
    }

    virtual void ice_response() 
    {
        _handler->response(_clientId);
    }

    virtual void ice_exception(const ::Ice::Exception& ex)
    {
        _handler->exception(_clientId, _reporter, ex);
    }

private:

    const string _clientId;
    const ReporterPrx _reporter;
    const RoutingHandlerPtr _handler;
};

}

namespace IceFIX
{

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
    _trace(communicator->getProperties()->getPropertyAsIntWithDefault(name + ".Trace", 0)),
    _timer(new IceUtil::Timer()),
    _retryInterval(IceUtil::Time::seconds(communicator->getProperties()->getPropertyAsIntWithDefault(
                                              name + ".RetryInterval", 60))),
    _forwardTimeout(communicator->getProperties()->getPropertyAsIntWithDefault(
                        name + ".RetryInterval", 60)/2 * 1000),
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
    IceUtil::Mutex::Lock sync(*this);
    if(_active)
    {
        if(_trace > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), "Bridge");
            trace << "logout";
        }

        _initiator->stop();
        _active = false;
    }
    _timer->destroy();
}

void
BridgeImpl::setInitiator(FIX::Initiator* initiator)
{
    _initiator = initiator;
}

string
BridgeImpl::_cpp_register(const QoS& qos, const Ice::Current& current)
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

    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "register: " << clientId;
    }
    return clientId;
}

void
BridgeImpl::registerWithId(const string& clientId, const QoS& qos, const Ice::Current& current)
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
            p = q.first;
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
        trace << "register: " << clientId;
    }
}

void
BridgeImpl::unregister(const string& clientId, const Ice::Current& current)
{
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clients(connection, clientDBName);
    for(;;)
    {
        try
        {
            ClientDB::iterator p = clients.find(clientId);
            if(p == clients.end())
            {
                throw RegistrationException("client is not registered");
            }
            if(p->second.reporter)
            {
                throw RegistrationException("client is active");
            }

            if(_trace > 0)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "unregister: " << clientId;
            }

            clients.erase(p);
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

ExecutorPrx
BridgeImpl::connect(const string& clientId, const ReporterPrx& reporter, const Ice::Current& current)
{
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clients(connection, clientDBName);
    for(;;)
    {
        try
        {
            Client c;
            ClientDB::iterator p = clients.find(clientId);
            if(p == clients.end())
            {
                throw RegistrationException("id not registered");
            }

            c = p->second;
            c.reporter = reporter;
            p.set(c);

            if(_trace > 0)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "connect: " << clientId;
            }

            Ice::Identity oid;
            oid.category = _instanceName + "-Executor";
            oid.name = clientId;
            return ExecutorPrx::uncheckedCast(current.adapter->createProxy(oid));
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

    long before;
    long after;

    IceUtil::Time start = IceUtil::Time::now();

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    RoutingRecordDB clordidDB(connection, clordIdDBName);
    RoutingRecordDB seqnumDB(connection, seqnumDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);

            before = clordidDB.size() + seqnumDB.size();
            RoutingRecordDB::iterator p = clordidDB.begin();
            while(p != clordidDB.end())
            {
                if(start - IceUtil::Time::seconds(p->second.timestamp) > timeout)
                {
                    RoutingRecordDB::iterator tmp = p;
                    ++p;
                    clordidDB.erase(tmp);
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
                    seqnumDB.erase(tmp);
                }
                else
                {
                    ++p;
                }
            }
            after = clordidDB.size() + seqnumDB.size();
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
        trace << "clean: removed " << (before-after) << " records out of " << before;
    }
    return before - after;
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
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clients(connection, clientDBName);
    for(;;)
    {
        try
        {
            ClientInfoSeq s;
            for(ClientDB::const_iterator p = clients.begin(); p != clients.end(); ++p)
            {
                ClientInfo info;
                info.id = p->first;
                info.isConnected = p->second.reporter;
                s.push_back(info);
            }
            return s;
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

// For use by the SendTimerTask.
void
BridgeImpl::send(Ice::Long messageId)
{
    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "send: `" << messageId << "'";
    }

    // A list of client id, reporter proxy.
    list<pair<string, ReporterPrx> > c;
    // The message itself.
    Message msg;

    // Gather the set of clients to which to forward the message.
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    MessageDB messageDB(connection, messageDBName);
    ClientDB clientsDB(connection, clientDBName);
    for(;;)
    {
        try
        {
            MessageDB::const_iterator q = messageDB.find(messageId);
            assert(q != messageDB.end());
            // Ensure that the same message is not routed twice in the
            // event of a deadlock exception in accessing the
            // database.
            msg = q->second;

            // The set of clients which have already received the
            // message.
            set<string> forwarded;
            // The set of clients which should receive the message.
            set<string> clients;

            copy(msg.forwarded.begin(), msg.forwarded.end(), inserter(forwarded, forwarded.begin()));
            copy(msg.clients.begin(), msg.clients.end(), inserter(clients, clients.begin()));

            // Compute the set difference. The result is the set of
            // clients which have not received the message.
            set<string> routing;
            set_difference(clients.begin(), clients.end(), forwarded.begin(), forwarded.end(), inserter(routing,
                            routing.begin()));

            // Forward the message to the clients in the routing list.
            c.clear();
            for(set<string>::const_iterator p = routing.begin(); p != routing.end(); ++p)
            {
                ClientDB::iterator q = clientsDB.find(*p);
                if(q != clientsDB.end())
                {
                    c.push_back(make_pair(*p, q->second.reporter));
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

    // The routing handler keeps track of the number of pending
    // replies for the given message.
    RoutingHandlerPtr handler = new RoutingHandler(this, _trace, messageId, _communicator);
    int queued = 0;
    for(list<pair<string, ReporterPrx> >::const_iterator p = c.begin(); p != c.end(); ++p)
    {
        if(p->second)
        {
            ++queued;
            handler->add(p->first);

            ReporterPrx reporter = p->second->ice_timeout(_forwardTimeout);
            reporter->message_async(new MessageAsyncI(p->first, reporter, handler), msg.data);
        }
        else
        {
            if(_trace > 0)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "routing: `" << messageId << "': client: `" << p->first << "': unavailable";
            }
        }
    }

    // If no messages were sent, then immediately schedule a retry.
    if(queued == 0)
    {
        if(_trace > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), "Bridge");
            trace << "send: `" << messageId << "': retry sending message";
        }
        _timer->schedule(new SendTimerTask(this, messageId), _retryInterval);
    }
}

void
BridgeImpl::sendComplete(int messageId, const set<string>& routed)
{
    bool retry = true;

    // Update the forwarded list, if necessary.
    if(routed.size() > 0)
    {
        Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
        MessageDB messageDB(connection, messageDBName);

        for(;;)
        {
            try
            {
                MessageDB::iterator q = messageDB.find(messageId);
                assert(q != messageDB.end());
                // Ensure that the same message is not routed twice in the event of a deadlock exception
                // in accessing the database.
                Message msg = q->second;

                copy(routed.begin(), routed.end(), back_inserter(msg.forwarded));
                if(msg.forwarded.size() == msg.clients.size())
                {
                    if(_trace > 0)
                    {
                        Ice::Trace trace(_communicator->getLogger(), "Bridge");
                        trace << "send: " << messageId << ": message fully routed, erasing";
                    }

                    messageDB.erase(q);
                    retry = false;
                }
                else
                {
                    q.set(msg);
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

    if(retry)
    {
        if(_trace > 0)
        {
            Ice::Trace trace(_communicator->getLogger(), "Bridge");
            trace << "send: " << messageId << ": retry sending message";
        }
        _timer->schedule(new SendTimerTask(this, messageId), _retryInterval);
    }
}

void
BridgeImpl::clientError(const string& clientId, const ReporterPrx& reporter)
{

    // The reporter proxy is cleared under two circumstances.
    // Firstly, when if the executor is destroyed, and
    // secondly if the bridge receives an ONE.

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clientsDB(connection, clientDBName);

    for(;;)
    {
        try
        {
            ClientDB::iterator p = clientsDB.find(clientId);
            if(p != clientsDB.end())
            {
                Client c = p->second;
                // Only clear the reporter if the reporter proxy
                // hasn't changed, otherwise there could be a race
                // condition between a new client connecting, and an
                // exiting message failing.
                if(c.reporter == reporter)
                {
                    c.reporter = 0;
                    p.set(c);
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
    // No need for the mutex. This method is called on startup only..

    //IceUtil::Mutex::Lock sync(*this);

    // Once onCreate has been called messages can be sent to the
    // trading partner.
    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "onCreate: " << session;
    }

    _session = FIX::Session::lookupSession(session);

    // Build a queue of pending messages, and schedule each for retry.
    list<int> queue;
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    MessageDB messageDB(connection, messageDBName);
    for(;;)
    {
        try
        {
            queue.clear();
            for(MessageDB::const_iterator p = messageDB.begin(); p != messageDB.end(); ++p)
            {
                queue.push_back(p->first);
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

    // The messages should be sent in order. If I use
    // Time::seconds(0), then timers with the stamp timestamp do not
    // respect order, therefore ensure the timestamp is different for
    // each.
    Ice::Long usec = 0;
    for(list<int>::const_iterator p = queue.begin(); p != queue.end(); ++p)
    {
        _timer->schedule(new SendTimerTask(this, *p), IceUtil::Time::microSeconds(usec++));
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

    if(_trace > 0)
    {
        FIX::MsgSeqNum seqNum;
        message.getHeader().getField(seqNum);

        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "fromAdmin: " << seqNum << " session: " << session;
        trace << " XML\n" << message.toXML();
    }
    route(message);
}

void
BridgeImpl::fromApp(const FIX::Message& message, const FIX::SessionID& session)
    throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    if(_trace > 0)
    {
        FIX::MsgSeqNum seqNum;
        message.getHeader().getField(seqNum);

        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "fromApp: " << seqNum << " session: " << session;
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
        trace << "msgTypeValue: " << msgTypeValue;
    }

    Ice::Long key = -1;

    FIX::MsgSeqNum seqNum;
    message.getHeader().getField(seqNum);

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    MessageDB messageDB(connection, messageDBName);
    MessageDBKey messageDBKey(connection, messageDBKeyName);
    RoutingRecordDB clordidDB(connection, clordIdDBName);
    RoutingRecordDB seqnumDB(connection, seqnumDBName);
    ClientDB clientsDB(connection, clientDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);

            MessageDBKey::iterator p = messageDBKey.find(0);
            assert(p != messageDBKey.end());
            key = p->second;
            p.set(key+1);

            Message m;

            m.data = message.toString();

            set<string> clients;

            // Find all non-filtered clients, and add to the clients
            // list.
            for(ClientDB::const_iterator r = clientsDB.begin(); r != clientsDB.end(); ++r)
            {
                IceFIX::QoS::const_iterator qos = r->second.qos.find("filtered");
                if(qos != r->second.qos.end())
                {
                    if(qos->second == "false")
                    {
                        clients.insert(r->second.id);
                    }
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
                        warning << "Bridge: fromApp: can't find routing record for RefSeqNumber: `" << refSeqNum << "'";
                    }
                    else
                    {
                        ClientDB::const_iterator r = clientsDB.find(q->second.id);
                        if(r != clientsDB.end())
                        {
                            clients.insert(q->second.id);
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
                    ClientDB::const_iterator r = clientsDB.find(q->second.id);
                    if(r != clientsDB.end())
                    {
                        clients.insert(q->second.id);
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
                warning << "Bridge: fromApp: `" << seqNum << "': no route for message\n";
                warning << message.toXML();
                return;
            }

            if(_trace > 1)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "fromApp: `" << seqNum << "': route message to clients: ";
                for(set<string>::const_iterator p = clients.begin(); p != clients.end(); ++p)
                {
                    if(p != clients.begin())
                    {
                        trace << ",";
                    }
                    trace << "`" << *p << "'";
                }
            }

            copy(clients.begin(), clients.end(), back_inserter(m.clients));

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

    // TODO: Stop scheduling messages if all clients are offline.

    // Schedule a timer to send this message immediately.
    _timer->schedule(new SendTimerTask(this, key), IceUtil::Time::seconds(0));
}
