// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEGRID_SESSIONI_H
#define ICEGRID_SESSIONI_H

#include <IceUtil/Mutex.h>
#include <IceGrid/ReapThread.h>
#include <IceGrid/Session.h>
#include <IceGrid/SessionServantLocatorI.h>
#include <set>

namespace IceGrid
{

class Database;
typedef IceUtil::Handle<Database> DatabasePtr;

class TraceLevels;
typedef IceUtil::Handle<TraceLevels> TraceLevelsPtr;

class AllocationRequest;
typedef IceUtil::Handle<AllocationRequest> AllocationRequestPtr;

class Allocatable;
typedef IceUtil::Handle<Allocatable> AllocatablePtr;

class WaitQueue;
typedef IceUtil::Handle<WaitQueue> WaitQueuePtr;

class SessionI;
typedef IceUtil::Handle<SessionI> SessionIPtr;

class BaseSessionI : virtual Ice::Object, public IceUtil::Mutex
{
public:

    virtual ~BaseSessionI();

    virtual void keepAlive(const Ice::Current&);

    IceUtil::Time timestamp() const;
    void shutdown();

    Ice::ObjectPrx registerWithServantLocator(const SessionServantLocatorIPtr&, const Ice::ConnectionPtr&);
    Ice::ObjectPrx registerWithObjectAdapter(const Ice::ObjectAdapterPtr&);

    const std::string& getId() const { return _id; }
    bool useFilters() const { return _filters; }
    
protected:

    virtual void destroyImpl(bool);

    BaseSessionI(const std::string&, const std::string&, const DatabasePtr&, bool);

    const std::string _id;
    const std::string _prefix;
    const TraceLevelsPtr _traceLevels;
    const DatabasePtr _database;
    const bool _filters;
    SessionServantLocatorIPtr _servantLocator;
    Ice::ObjectAdapterPtr _adapter;
    Ice::Identity _identity;
    bool _destroyed;
    IceUtil::Time _timestamp;
};
typedef IceUtil::Handle<BaseSessionI> BaseSessionIPtr;

class SessionDestroyedException
{
};

class SessionI : public BaseSessionI, public Session
{
public:

    SessionI(const std::string&, const DatabasePtr&, bool, const WaitQueuePtr&, const Glacier2::SessionControlPrx&);
    SessionI(const std::string&, const DatabasePtr&, bool, const WaitQueuePtr&, const Ice::ConnectionPtr&);
    virtual ~SessionI();

    virtual void keepAlive(const Ice::Current& current) { BaseSessionI::keepAlive(current); }

    virtual void allocateObjectById_async(const AMD_Session_allocateObjectByIdPtr&, const Ice::Identity&,
                                          const Ice::Current&);
    virtual void allocateObjectByType_async(const AMD_Session_allocateObjectByTypePtr&, const std::string&,
                                            const Ice::Current&);
    virtual void releaseObject(const Ice::Identity&, const Ice::Current&);
    virtual void setAllocationTimeout(int, const Ice::Current&);
    virtual void destroy(const Ice::Current&);

    int getAllocationTimeout() const;
    const WaitQueuePtr& getWaitQueue() const { return _waitQueue; }
    Glacier2::SessionControlPrx getSessionControl() const { return _sessionControl; }

    bool addAllocationRequest(const AllocationRequestPtr&);
    void removeAllocationRequest(const AllocationRequestPtr&);
    void addAllocation(const AllocatablePtr&);
    void removeAllocation(const AllocatablePtr&);

protected:

    virtual void destroyImpl(bool);

    const WaitQueuePtr _waitQueue;
    const Glacier2::SessionControlPrx _sessionControl;
    const Ice::ConnectionPtr _connection;
    int _allocationTimeout;
    std::set<AllocationRequestPtr> _requests;
    std::set<AllocatablePtr> _allocations;
};

class ClientSessionFactory : virtual public IceUtil::Shared
{
public:

    ClientSessionFactory(const Ice::ObjectAdapterPtr&, const DatabasePtr&, const WaitQueuePtr&, const ReapThreadPtr&);

    Glacier2::SessionPrx createGlacier2Session(const std::string&, const Glacier2::SessionControlPrx&);
    SessionIPtr createSessionServant(const std::string&, const Glacier2::SessionControlPrx&);

    const TraceLevelsPtr& getTraceLevels() const;

private:

    const Ice::ObjectAdapterPtr _adapter;
    const DatabasePtr _database;
    const WaitQueuePtr _waitQueue;
    const ReapThreadPtr _reaper;
    const bool _filters;
};
typedef IceUtil::Handle<ClientSessionFactory> ClientSessionFactoryPtr;

class ClientSessionManagerI : virtual public Glacier2::SessionManager
{
public:

    ClientSessionManagerI(const ClientSessionFactoryPtr&);
    
    virtual Glacier2::SessionPrx create(const std::string&, const Glacier2::SessionControlPrx&, const Ice::Current&);

private:

    const ClientSessionFactoryPtr _factory;
};

class ClientSSLSessionManagerI : virtual public Glacier2::SSLSessionManager
{
public:

    ClientSSLSessionManagerI(const  ClientSessionFactoryPtr&);
    
    virtual Glacier2::SessionPrx create(const Glacier2::SSLInfo&, const Glacier2::SessionControlPrx&, 
                                        const Ice::Current&);

private:

    const ClientSessionFactoryPtr _factory;
};

};

#endif
