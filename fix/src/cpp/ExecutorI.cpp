// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <ExecutorI.h>
#include <RoutingRecordDB.h>
#include <ClientDB.h>
#include <dbname.h>

using namespace std;
using namespace IceFIX;
using namespace FIXBridge;

ExecutorI::ExecutorI(const Ice::CommunicatorPtr& communicator, const string& name, const FIX::SessionID& id) :
        _communicator(communicator), _name(name), _id(id)
{
}

int
ExecutorI::execute(const string& data, const Ice::Current& current)
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
    FIX::Session* session = FIX::Session::lookupSession(_id);
    if(session == 0)
    {
        throw ExecuteException("no session");
    }
    session->send(message);

    FIX::MsgSeqNum seqNum;
    message.getHeader().getField(seqNum);

    return seqNum;
}

void
ExecutorI::destroy(const Ice::Current& current)
{
    Freeze::ConnectionPtr connection = Freeze::createConnection(_communicator, _name);
    ClientDB clients(connection, clientDBName);

    for(;;)
    {
        try
        {
            Client c;

            ClientDB::iterator p = clients.find(current.id.name);
            if(p != clients.end())
            {
                c = p->second;
                c.reporter = 0;
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
}

void
ExecutorI::halt(const Freeze::DatabaseException& ex) const
{
    Ice::Error error(_communicator->getLogger());
    error << "fatal exception: " << ex << "\n*** Aborting application ***";

    abort();
}

ExecutorLocatorI::ExecutorLocatorI(const Ice::ObjectPtr& executor) :
    _executor(executor)
{
}

Ice::ObjectPtr
ExecutorLocatorI::locate(const Ice::Current&, Ice::LocalObjectPtr&)
{
    return _executor;
}

void
ExecutorLocatorI::finished(const Ice::Current&, const Ice::ObjectPtr&, const Ice::LocalObjectPtr&)
{
}

void
ExecutorLocatorI::deactivate(const string&)
{
}
