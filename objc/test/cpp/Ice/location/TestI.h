// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_LOCATION_TEST_I_H
#define TEST_LOCATION_TEST_I_H

#include <LocationTest.h>
#include <vector>
#include <location/ServerLocator.h>

namespace Test
{
    
namespace Location
{

class ServerManagerI : public Test::Location::ServerManager
{
public:
    
    ServerManagerI(const ServerLocatorRegistryPtr&, const Ice::InitializationData&);
    
    virtual void startServer(const Ice::Current&);
    virtual void shutdown(const Ice::Current&);
    
private:
    
    std::vector<Ice::CommunicatorPtr> _communicators;
    ServerLocatorRegistryPtr _registry;
    Ice::InitializationData _initData;
    int _nextPort;
};

class HelloI : public Test::Location::Hello
{
public:
    
    virtual void sayHello(const Ice::Current&);
};

class TestI : public Test::Location::TestIntf
{
public:
    
    TestI(const Ice::ObjectAdapterPtr&, const Ice::ObjectAdapterPtr&, const ServerLocatorRegistryPtr&);
    
    virtual void shutdown(const Ice::Current&);
    virtual ::Test::Location::HelloPrx getHello(const Ice::Current&);
    virtual ::Test::Location::HelloPrx getReplicatedHello(const Ice::Current&);
    virtual void migrateHello(const Ice::Current&);
    
private:
    
    Ice::ObjectAdapterPtr _adapter1;
    Ice::ObjectAdapterPtr _adapter2;
    ServerLocatorRegistryPtr _registry;
    
};
    
}
    
}

#endif
