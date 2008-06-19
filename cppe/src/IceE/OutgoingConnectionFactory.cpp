// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceE/OutgoingConnectionFactory.h>
#include <IceE/Connection.h>
#include <IceE/Instance.h>
#include <IceE/LoggerUtil.h>
#include <IceE/TraceLevels.h>
#include <IceE/DefaultsAndOverrides.h>
#include <IceE/Connector.h>
#include <IceE/Endpoint.h>
#include <IceE/RouterInfo.h>
#include <IceE/LocalException.h>
#include <IceE/Functional.h>
#ifdef ICEE_HAS_ROUTER
#   include <IceE/Reference.h>
#endif
#include <list>

using namespace std;
using namespace Ice;
using namespace IceInternal;

IceUtil::Shared* IceInternal::upCast(OutgoingConnectionFactory* p) { return p; }

void
IceInternal::OutgoingConnectionFactory::destroy()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    if(_destroyed)
    {
        return;
    }

#ifdef _STLP_BEGIN_NAMESPACE
    // voidbind2nd is an STLport extension for broken compilers in IceE/Functional.h
    for_each(_connections.begin(), _connections.end(),
             voidbind2nd(Ice::secondVoidMemFun1<ConnectorPtr, Connection, Connection::DestructionReason>
                         (&Connection::destroy), Connection::CommunicatorDestroyed));
#else
    for_each(_connections.begin(), _connections.end(),
             bind2nd(Ice::secondVoidMemFun1<const ConnectorPtr, Connection, Connection::DestructionReason>
                     (&Connection::destroy), Connection::CommunicatorDestroyed));
#endif

    _destroyed = true;
    notifyAll();
}

void
IceInternal::OutgoingConnectionFactory::waitUntilFinished()
{
    multimap<ConnectorPtr, ConnectionPtr> connections;

    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        
        //
        // First we wait until the factory is destroyed. We also wait
        // until there are no pending connections anymore. Only then
        // we can be sure the _connections contains all connections.
        //
        while(!_destroyed || !_pending.empty())
        {
            wait();
        }
        
        //
        // We want to wait until all connections are finished outside the
        // thread synchronization.
        //
        connections.swap(_connections);
    }

    for_each(connections.begin(), connections.end(),
             Ice::secondVoidMemFun<const ConnectorPtr, Connection>(&Connection::waitUntilFinished));
}

ConnectionPtr
IceInternal::OutgoingConnectionFactory::create(const vector<EndpointPtr>& endpts)
{
    assert(!endpts.empty());
    vector<pair<ConnectorPtr, EndpointPtr> > connectors;

    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

        if(_destroyed)
        {
            throw CommunicatorDestroyedException(__FILE__, __LINE__);
        }

        //
        // Reap connections for which destruction has completed.
        //
        std::multimap<ConnectorPtr, ConnectionPtr>::iterator p = _connections.begin();
        while(p != _connections.end())
        {
            if(p->second->isFinished())
            {
                _connections.erase(p++);
            }
            else
            {
                ++p;
            }
        }

        vector<EndpointPtr> endpoints = endpts;
        vector<EndpointPtr>::iterator q;
        for(q = endpoints.begin(); q != endpoints.end(); ++q)
        {
            //
            // Modify endpoints with overrides.
            //
            if(_instance->defaultsAndOverrides()->overrideTimeout)
            {
                *q = (*q)->timeout(_instance->defaultsAndOverrides()->overrideTimeoutValue);
            }

            //
            // Create connectors for the endpoints.
            //
            vector<ConnectorPtr> cons = (*q)->connectors();
            assert(cons.size() > 0);

            vector<ConnectorPtr>::const_iterator r;
            for(r = cons.begin(); r != cons.end(); ++r)
            {
                connectors.push_back(make_pair(*r, *q));
            }
        }

        //
        // Search for existing connections.
        //
        vector<pair<ConnectorPtr, EndpointPtr> >::const_iterator r;
        for(r = connectors.begin(); r != connectors.end(); ++r)
        {
            pair<multimap<ConnectorPtr, ConnectionPtr>::iterator,
                 multimap<ConnectorPtr, ConnectionPtr>::iterator> pr = _connections.equal_range((*r).first);

            while(pr.first != pr.second)
            {
                //
                // Don't return connections for which destruction has
                // been initiated.
                //
                if(!pr.first->second->isDestroyed())
                {
                    return pr.first->second;
                }

                ++pr.first;
            }
        }

        //
        // If some other thread is currently trying to establish a
        // connection to any of our endpoints, we wait until this
        // thread is finished.
        //
        bool searchAgain = false;
        while(!_destroyed)
        {
            for(r = connectors.begin(); r != connectors.end(); ++r)
            {
                if(_pending.find((*r).first) != _pending.end())
                {
                    break;
                }
            }

            if(r == connectors.end())
            {
                break;
            }

            searchAgain = true;

            wait();
        }

        if(_destroyed)
        {
            throw CommunicatorDestroyedException(__FILE__, __LINE__);
        }

        //
        // Search for existing connections again if we waited above,
        // as new connections might have been added in the meantime.
        //
        if(searchAgain)
        {        
            for(r = connectors.begin(); r != connectors.end(); ++r)
            {
                pair<multimap<ConnectorPtr, ConnectionPtr>::iterator,
                     multimap<ConnectorPtr, ConnectionPtr>::iterator> pr = _connections.equal_range((*r).first);

                while(pr.first != pr.second)
                {
                    //
                    // Don't return connections for which destruction has
                    // been initiated.
                    //
                    if(!pr.first->second->isDestroyed())
                    {
                        return pr.first->second;
                    }

                    ++pr.first;
                }
            }
        }

        //
        // No connection to any of our endpoints exists yet, so we
        // will try to create one. To avoid that other threads try to
        // create connections to the same endpoints, we add our
        // endpoints to _pending.
        //
        for(r = connectors.begin(); r != connectors.end(); ++r)
        {
            _pending.insert((*r).first);
        }
    }

    ConnectorPtr connector;
    ConnectionPtr connection;
    auto_ptr<LocalException> exception;
    
    vector<pair<ConnectorPtr, EndpointPtr> >::const_iterator q;
    for(q = connectors.begin(); q != connectors.end(); ++q)
    {
        connector = (*q).first;
        EndpointPtr endpoint = (*q).second;
        
        try
        {
            Int timeout;
            if(_instance->defaultsAndOverrides()->overrideConnectTimeout)
            {
                timeout = _instance->defaultsAndOverrides()->overrideConnectTimeoutValue;
            }
            // It is not necessary to check for overrideTimeout,
            // the endpoint has already been modified with this
            // override, if set.
            else
            {
                timeout = endpoint->timeout();
            }
            
            TransceiverPtr transceiver = connector->connect(timeout);
            assert(transceiver);

#ifdef ICEE_PURE_CLIENT
            connection = new Connection(_instance, transceiver, endpoint);
#else
            connection = new Connection(_instance, transceiver, endpoint, 0);
#endif
            //
            // Wait for the connection to be validated by the
            // connection thread.  Once the connection has been
            // validated it will be activated also.
            //
            connection->waitForValidation();
            break;
        }
        catch(const LocalException& ex)
        {
            exception.reset(dynamic_cast<LocalException*>(ex.ice_clone()));

            //
            // If a connection object was constructed, then validate()
            // must have raised the exception.
            //
            if(connection)
            {
                connection->waitUntilFinished(); // We must call waitUntilFinished() for cleanup.
                connection = 0;
            }
        }
        
        TraceLevelsPtr traceLevels = _instance->traceLevels();
        if(traceLevels->retry >= 2)
        {
            Trace out(_instance->initializationData().logger, traceLevels->retryCat);

            out << "connection to endpoint failed";
            if(q + 1 != connectors.end())
            {
                out << ", trying next endpoint\n";
            }
            else
            {
                out << " and no more endpoints to try\n";
            }
            out << (*exception.get()).toString();
        }
    }
    
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        
        //
        // Signal other threads that we are done with trying to
        // establish connections to our endpoints.
        //
        for(q = connectors.begin(); q != connectors.end(); ++q)
        {
            _pending.erase((*q).first);
        }
        notifyAll();

        if(!connection)
        {
            assert(exception.get());
            exception->ice_throw();
        }
        else
        {
            _connections.insert(_connections.end(), pair<const ConnectorPtr, ConnectionPtr>(connector, connection));

            if(_destroyed)
            {
                connection->destroy(Connection::CommunicatorDestroyed);
                throw CommunicatorDestroyedException(__FILE__, __LINE__);
            }
        }
    }

    assert(connection);
    return connection;
}

#ifdef ICEE_HAS_ROUTER

void
IceInternal::OutgoingConnectionFactory::setRouterInfo(const RouterInfoPtr& routerInfo)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

    if(_destroyed)
    {
        throw CommunicatorDestroyedException(__FILE__, __LINE__);
    }

    assert(routerInfo);
    //
    // Search for connections to the router's client proxy
    // endpoints, and update the object adapter for such
    // connections, so that callbacks from the router can be
    // received over such connections.
    //
#ifndef ICEE_PURE_CLIENT
    ObjectAdapterPtr adapter = routerInfo->getAdapter();
#endif
    vector<EndpointPtr> endpoints = routerInfo->getClientEndpoints();
    vector<EndpointPtr>::const_iterator p;
    for(p = endpoints.begin(); p != endpoints.end(); ++p)
    {
        EndpointPtr endpoint = *p;

        //
        // Modify endpoints with overrides.
        //
        if(_instance->defaultsAndOverrides()->overrideTimeout)
        {
            endpoint = endpoint->timeout(_instance->defaultsAndOverrides()->overrideTimeoutValue);
        }

#ifndef ICEE_PURE_CLIENT
        multimap<ConnectorPtr, ConnectionPtr>::const_iterator q;
        for(q = _connections.begin(); q != _connections.end(); ++q)
        {
            if((*q).second->endpoint() == endpoint)
            {
                try
                {
                    (*q).second->setAdapter(adapter);
                }
                catch(const Ice::LocalException&)
                {
                    //
                    // Ignore, the connection is being closed or closed.
                    //
                }
            }
        }
#endif
    }
}

#endif

#ifdef ICEE_HAS_BATCH
void
IceInternal::OutgoingConnectionFactory::flushBatchRequests()
{
    list<ConnectionPtr> c;

    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);

        for(std::multimap<ConnectorPtr, ConnectionPtr>::const_iterator p = _connections.begin();
            p != _connections.end();
            ++p)
        {
            c.push_back(p->second);
        }
    }

    for(list<ConnectionPtr>::const_iterator p = c.begin(); p != c.end(); ++p)
    {
        try
        {
            (*p)->flushBatchRequests();
        }
        catch(const LocalException&)
        {
            // Ignore.
        }
    }
}
#endif

IceInternal::OutgoingConnectionFactory::OutgoingConnectionFactory(const InstancePtr& instance) :
    _instance(instance),
    _destroyed(false)
{
}

IceInternal::OutgoingConnectionFactory::~OutgoingConnectionFactory()
{
    assert(_destroyed);
    assert(_connections.empty());
}

#ifndef ICEE_PURE_CLIENT

void
IceInternal::OutgoingConnectionFactory::removeAdapter(const ObjectAdapterPtr& adapter)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
    
    if(_destroyed)
    {
        return;
    }
    
    for(multimap<ConnectorPtr, ConnectionPtr>::const_iterator p = _connections.begin(); p != _connections.end(); ++p)
    {
        if(p->second->getAdapter() == adapter)
        {
            try
            {
                p->second->setAdapter(0);
            }
            catch(const Ice::LocalException&)
            {
                //
                // Ignore, the connection is being closed or closed.
                //
            }
        }
    }
}

#endif
