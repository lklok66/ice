// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice FIX is licensed to you under the terms described in the
// ICE_FIX_LICENSE file included in this distribution.
//
// **********************************************************************

#include <ClientImpl.h>
#include <ClientDB.h>
#include <MessageDB.h>
#include <dbname.h>

using namespace std;
using namespace IceFIX;
using namespace FIXBridge;

namespace 
{

class SendTimerTask : public IceUtil::TimerTask
{
public:

    SendTimerTask(const ClientImplPtr& client) :
        _client(client)
    {
    }

    virtual void runTimerTask()
    {
        _client->send();
    }

private:

    const ClientImplPtr _client;
};

class MessageAsyncI : public AMI_Reporter_message
{
public:

    MessageAsyncI(const ClientImplPtr& client, const ReporterPrx& reporter) :
        _client(client), _reporter(reporter)
    {
    }
    
    void
    ice_response() 
    {
        _client->response();
    }
    
    void
    ice_exception(const ::Ice::Exception& ex)
    {
        _client->exception(_reporter, ex);
    }

private:

    const ClientImplPtr _client;
    const ReporterPrx _reporter;
};

}

ClientImpl::ClientImpl(const IceUtil::TimerPtr& timer, const Ice::CommunicatorPtr& communicator, const string& dbenv,
                       const IceUtil::Time& retryInterval, int trace, const string& id, const IceFIX::QoS& qos,
                       const IceFIX::ReporterPrx& reporter) :
        _timer(timer), _communicator(communicator), _dbenv(dbenv), _trace(trace), _retryInterval(retryInterval),
        _id(id), _filtered(true), _reporter(reporter), _destroy(false)
{
    IceFIX::QoS::const_iterator p = qos.find("filtered");
    if(p != qos.end())
    {
        if(p->second == "false")
        {
            _filtered = false;
        }
    }
}

bool
ClientImpl::filtered() const
{
    return _filtered;
}

IceFIX::ReporterPrx
ClientImpl::reporter() const
{
    Lock sync(*this);
    return _reporter;
}

void
ClientImpl::enqueue(const Ice::Long& messageId, const string& data)
{
    Lock sync(*this);
    if(_destroy)
    {
        return;
    }
    _queue.push_back(messageId);
    if(_reporter && _queue.size() == 1)
    {
        sendImpl(messageId, data);
    }
}

void
ClientImpl::enqueue(const Ice::Long& messageId)
{
    Lock sync(*this);
    if(_destroy)
    {
        return;
    }
    _queue.push_back(messageId);
    if(_reporter && _queue.size() == 1)
    {
        sendImpl(messageId);
    }
}

void
ClientImpl::connect(const IceFIX::ReporterPrx& reporter)
{
    Lock sync(*this);
    if(_destroy)
    {
        return; // Ignore
    }
    
    // We need to check _reporter prior to setReporter to know if
    // we're currently sending.
    bool sending = _reporter && !_queue.empty();

    setReporter(reporter);

    // If we were not sending, and we can now send then start sending.
    if(_timerTask || (!sending && _reporter && !_queue.empty()))
    {
        if(!_timerTask || _timer->cancel(_timerTask))
        {
            _timerTask = 0;
            sendImpl(_queue.front());
        }
    }
}

void
ClientImpl::destroy(bool force)
{
    Lock sync(*this);
    if(!force)
    {
        if(_reporter)
        {
            throw RegistrationException("client is connected");
        }
        
        if(!_queue.empty())
        {
            ostringstream os;
            os << "client has " << _queue.size() << " queued messages";
            throw RegistrationException(os.str());
        }
    }

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clients(connection, clientDBName);
    MessageDB messageDB(connection, messageDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);

            ClientDB::iterator p = clients.find(_id);
            assert(p != clients.end());

            // Dequeue all queued messages. We operate on a copy in
            // case the transaction is restarted.
            list<Ice::Long> queueCopy = _queue;
            while(!queueCopy.empty())
            {
                Ice::Long messageId = queueCopy.front();
                queueCopy.pop_front();
                forwarded(messageDB, messageId);
            }

            if(_trace > 0)
            {
                Ice::Trace trace(_communicator->getLogger(), "Bridge");
                trace << "unregister: " << _id;
            }

            clients.erase(p);
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

    _queue.clear();
    _destroy = true;
}

void
ClientImpl::disconnect()
{
    Lock sync(*this);

    // If we're not connected, we're done.
    if(!_reporter) 
    {
        return;
    }
    setReporter(0);
}

void
ClientImpl::send()
{
    Lock sync(*this);
    _timerTask = 0;

    // If we're destroyed, there are no messages to send, or the
    // client is not connected we don't send the next message.
    if(_destroy || !_reporter)
    {
        return;
    }
    assert(!_queue.empty());
    sendImpl(_queue.front());
}

void
ClientImpl::stop()
{
    Lock sync(*this);
    _destroy = true;
}

void
ClientImpl::response()
{
    Lock sync(*this);
    
    // If we were unregistered in the meantime, we're done.
    if(_destroy && _queue.empty())
    {
        return;
    }

    // Pop the head of the message queue, and add ourself to the
    // forwarded list.
    Ice::Long messageId = _queue.front();
    _queue.pop_front();

    // The next message to send.
    Message msg;

    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    MessageDB messageDB(connection, messageDBName);
    for(;;)
    {
        try
        {
            Freeze::TransactionHolder txn(connection);
            forwarded(messageDB, messageId);

            // Gather the next message to send, if the queue isn't
            // empty and we're ready to send another message.
            if(!_destroy && !_queue.empty() && _reporter)
            {
                MessageDB::const_iterator q = messageDB.find(_queue.front());
                assert(q != messageDB.end());
                msg = q->second;
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

    // If we're already sending, or we've been destroyed, or there are
    // no messages to send, we're outta here.
    if(!_destroy && !_queue.empty() && _reporter)
    {
        sendImpl(_queue.front(), msg.data);
    }
}

void
ClientImpl::exception(const ReporterPrx& reporter, const Ice::Exception& ex)
{
    Lock sync(*this);
    if(_destroy)
    {
        return;
    }

    if(_trace > 1)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "client: `" << _id << "': exception: " << ex;
    }

    // Schedule a retry, if the exception isn't ONE and we're still
    // connected.
    if(!dynamic_cast<const Ice::ObjectNotExistException*>(&ex) && _reporter)
    {
        if(_trace > 1)
        {
            Ice::Trace trace(_communicator->getLogger(), "Bridge");
            trace << "client: `" << _id << "': retry in " << _retryInterval.toDuration();
        }
        _timerTask = new SendTimerTask(this);
        _timer->schedule(_timerTask, _retryInterval);
        return;
    }

    // Otherwise clear the reporter, as long as the reporter proxy has
    // not changed.
    if(reporter == _reporter)
    {
        setReporter(0);
    }
}

void
ClientImpl::forwarded(MessageDB& messageDB, Ice::Long id)
{
    MessageDB::iterator q = messageDB.find(id);
    assert(q != messageDB.end());

    Message msg = q->second;

    msg.forwarded.push_back(_id);
    if(msg.forwarded.size() == msg.clients.size())
    {
        if(_trace > 1)
        {
            Ice::Trace trace(_communicator->getLogger(), "Bridge");
            trace << "response messageId: " << id << ": message fully routed, erasing";
        }
        messageDB.erase(q);
    }
    else
    {
        q.set(msg);
    }
}

void 
ClientImpl::sendImpl(const Ice::Long& messageId)
{
    Message msg;
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    MessageDB messageDB(connection, messageDBName);
    for(;;)
    {
        try
        {
            MessageDB::const_iterator q = messageDB.find(messageId);
            assert(q != messageDB.end());
            msg = q->second;
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

    sendImpl(messageId, msg.data);
}

void
ClientImpl::sendImpl(const Ice::Long& messageId, const string& data)
{
    assert(!_destroy && !_queue.empty() && _reporter);
    if(_trace > 1)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << "sending messageId: " << messageId << " to client: `" << _id << "': " << _reporter;
    }
    _reporter->message_async(new MessageAsyncI(this, _reporter), data);
}

void
ClientImpl::setReporter(const ReporterPrx& reporter)
{
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _dbenv);
    ClientDB clients(connection, clientDBName);
    for(;;)
    {
        try
        {
            Client c;

            ClientDB::iterator p = clients.find(_id);
            if(p != clients.end())
            {
                c = p->second;
                c.reporter = reporter;
                p.set(c);
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

    _reporter = reporter;

    if(_trace > 0)
    {
        Ice::Trace trace(_communicator->getLogger(), "Bridge");
        trace << (_reporter ? string("connect: ") : string("disconnect: ")) << _id;
    }
}

void
ClientImpl::halt(const Freeze::DatabaseException& ex) const
{
    Ice::Error error(_communicator->getLogger());
    error << "fatal exception: " << ex << "\n*** Aborting application ***";
    abort();
}

