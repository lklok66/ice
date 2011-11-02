// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <Ice/Router.h>
#include <IceUtil/IceUtil.h>
#include <TestCommon.h>
#include <LocationTest.h>
#include <list>

using namespace std;
using namespace Test::Location;

namespace
{

class HelloI : virtual public Hello
{
public:
    
    virtual void
    sayHello(const Ice::Current& foo)
    {
        // Do nothing, this is just a dummy servant.
    }
};

class AMICallback : public IceUtil::Shared
{
public:
    AMICallback()
    {
    }
    
    void
    exception1(const Ice::Exception&)
    {
        test(false);
    }
    void
    exception2(const Ice::Exception& ex)
    {
        test(dynamic_cast<const Ice::NotRegisteredException*>(&ex));
    }
    
    void
    response1()
    {
    }
    
    void
    response2()
    {
        test(false);
    }
};
typedef IceUtil::Handle<AMICallback> AMICallbackPtr;

}

void
locationAllTests(const Ice::CommunicatorPtr& communicator, const string& ref)
{
    ServerManagerPrx manager = ServerManagerPrx::checkedCast(communicator->stringToProxy(ref));
    TestLocatorPrx locator = TestLocatorPrx::uncheckedCast(communicator->getDefaultLocator());
    test(manager);
    
    TestLocatorRegistryPrx registry = TestLocatorRegistryPrx::checkedCast(locator->getRegistry());
    test(registry);
    
    tprintf("testing stringToProxy... ");
    Ice::ObjectPrx base = communicator->stringToProxy("test @ TestAdapter");
    Ice::ObjectPrx base2 = communicator->stringToProxy("test @ TestAdapter");
    Ice::ObjectPrx base3 = communicator->stringToProxy("test");
    Ice::ObjectPrx base4 = communicator->stringToProxy("ServerManager"); 
    Ice::ObjectPrx base5 = communicator->stringToProxy("test2");
    Ice::ObjectPrx base6 = communicator->stringToProxy("test @ ReplicatedAdapter");
    tprintf("ok\n");
    
    tprintf("testing ice_locator and ice_getLocator... ");
    test(Ice::proxyIdentityEqual(base->ice_getLocator(), communicator->getDefaultLocator()));
    Ice::LocatorPrx anotherLocator = Ice::LocatorPrx::uncheckedCast(communicator->stringToProxy("anotherLocator"));
    base = base->ice_locator(anotherLocator);
    test(Ice::proxyIdentityEqual(base->ice_getLocator(), anotherLocator));
    communicator->setDefaultLocator(0);
    base = communicator->stringToProxy("test @ TestAdapter");
    test(!base->ice_getLocator());
    base = base->ice_locator(anotherLocator);
    test(Ice::proxyIdentityEqual(base->ice_getLocator(), anotherLocator));
    communicator->setDefaultLocator(locator);
    base = communicator->stringToProxy("test @ TestAdapter");
    test(Ice::proxyIdentityEqual(base->ice_getLocator(), communicator->getDefaultLocator())); 
    
    //
    // We also test ice_router/ice_getRouter (perhaps we should add a
    // test/Ice/router test?)
    //
    test(!base->ice_getRouter());
    Ice::RouterPrx anotherRouter = Ice::RouterPrx::uncheckedCast(communicator->stringToProxy("anotherRouter"));
    base = base->ice_router(anotherRouter);
    test(Ice::proxyIdentityEqual(base->ice_getRouter(), anotherRouter));
    Ice::RouterPrx router = Ice::RouterPrx::uncheckedCast(communicator->stringToProxy("dummyrouter"));
    communicator->setDefaultRouter(router);
    base = communicator->stringToProxy("test @ TestAdapter");
    test(Ice::proxyIdentityEqual(base->ice_getRouter(), communicator->getDefaultRouter()));
    communicator->setDefaultRouter(0);
    base = communicator->stringToProxy("test @ TestAdapter");
    test(!base->ice_getRouter());
    tprintf("ok\n");
    
    tprintf("starting server... ");
    manager->startServer();
    tprintf("ok\n");
    
    tprintf("testing checked cast... ");
    TestIntfPrx obj = TestIntfPrx::checkedCast(base);
    test(obj);
    TestIntfPrx obj2 = TestIntfPrx::checkedCast(base2);
    test(obj2);
    TestIntfPrx obj3 = TestIntfPrx::checkedCast(base3);
    test(obj3);
    ServerManagerPrx obj4 = ServerManagerPrx::checkedCast(base4);
    test(obj4);
    TestIntfPrx obj5 = TestIntfPrx::checkedCast(base5);
    test(obj5);
    TestIntfPrx obj6 = TestIntfPrx::checkedCast(base6);
    test(obj6);
    tprintf("ok\n");
    
    tprintf("testing id@AdapterId indirect proxy... ");
    obj->shutdown();
    manager->startServer();
    try
    {
        obj2->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    tprintf("ok\n");    
    
    tprintf("testing id@ReplicaGroupId indirect proxy... ");
    obj->shutdown();
    manager->startServer();
    try
    {
        obj6->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    tprintf("ok\n");    
    
    tprintf("testing identity indirect proxy... ");
    obj->shutdown();
    manager->startServer();
    try
    {
        obj3->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    try
    {
        obj2->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    obj->shutdown();
    manager->startServer();
    try
    {
        obj2->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    try
    {
        obj3->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    obj->shutdown();
    manager->startServer();
    
    try
    {
        obj2->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    obj->shutdown();
    manager->startServer();
    try
    {
        obj3->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    obj->shutdown();
    manager->startServer();
    try
    {
        obj2->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    obj->shutdown();
    manager->startServer();
    
    try
    {
        obj5->ice_ping();
    }
    catch(const Ice::LocalException& ex)
    {
        cerr << ex << endl;
        test(false);
    }
    tprintf("ok\n");
    
    tprintf("testing proxy with unknown identity... ");
    try
    {
        base = communicator->stringToProxy("unknown/unknown");
        base->ice_ping();
        test(false);
    }
    catch (const Ice::NotRegisteredException& ex)
    {
        test(ex.kindOfObject == "object");
        test(ex.id == "unknown/unknown");
    }
    tprintf("ok\n");
    
    tprintf("testing proxy with unknown adapter... ");
    try
    {
        base = communicator->stringToProxy("test @ TestAdapterUnknown");
        base->ice_ping();
        test(false);
    }
    catch (const Ice::NotRegisteredException& ex)
    {
        test(ex.kindOfObject == "object adapter");
        test(ex.id == "TestAdapterUnknown");
    }
    tprintf("ok\n");
    
    tprintf("testing locator cache timeout... ");
    
    int count = locator->getRequestCount();
    communicator->stringToProxy("test@TestAdapter")->ice_locatorCacheTimeout(0)->ice_ping(); // No locator cache.
    test(++count == locator->getRequestCount());
    communicator->stringToProxy("test@TestAdapter")->ice_locatorCacheTimeout(0)->ice_ping(); // No locator cache.
    test(++count == locator->getRequestCount());
    communicator->stringToProxy("test@TestAdapter")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout.
    test(count == locator->getRequestCount());
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(1300));
    communicator->stringToProxy("test@TestAdapter")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout.
    test(++count == locator->getRequestCount());
    
    communicator->stringToProxy("test")->ice_locatorCacheTimeout(0)->ice_ping(); // No locator cache.
    count += 2;
    test(count == locator->getRequestCount());
    communicator->stringToProxy("test")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout
    test(count == locator->getRequestCount());
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(1300));
    communicator->stringToProxy("test")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout
    count += 2;
    test(count == locator->getRequestCount());
    
    communicator->stringToProxy("test@TestAdapter")->ice_locatorCacheTimeout(-1)->ice_ping(); 
    test(count == locator->getRequestCount());
    communicator->stringToProxy("test")->ice_locatorCacheTimeout(-1)->ice_ping();
    test(count == locator->getRequestCount());
    communicator->stringToProxy("test@TestAdapter")->ice_ping(); 
    test(count == locator->getRequestCount());
    communicator->stringToProxy("test")->ice_ping();
    test(count == locator->getRequestCount());
    
    test(communicator->stringToProxy("test")->ice_locatorCacheTimeout(99)->ice_getLocatorCacheTimeout() == 99);
    
    tprintf("ok\n");
    
    tprintf("testing proxy from server... ");
    obj = TestIntfPrx::checkedCast(communicator->stringToProxy("test@TestAdapter"));
    HelloPrx hello = obj->getHello();
    test(hello->ice_getAdapterId() == "TestAdapter");
    hello->sayHello();
    hello = obj->getReplicatedHello();
    test(hello->ice_getAdapterId() == "ReplicatedAdapter");
    hello->sayHello();
    tprintf("ok\n");
    
    tprintf("testing locator request queuing... ");
    hello = obj->getReplicatedHello()->ice_locatorCacheTimeout(0)->ice_connectionCached(false);
    count = locator->getRequestCount();
    hello->ice_ping();
    test(++count == locator->getRequestCount());
    int i;
    
    list<Ice::AsyncResultPtr>  results;
    AMICallbackPtr cb = new AMICallback;
    for(i = 0; i < 1000; i++)
    {
        Ice::AsyncResultPtr result = hello->begin_sayHello(
                                                           newCallback_Hello_sayHello(cb, &AMICallback::response1, &AMICallback::exception1));
        results.push_back(result);
    }
    while(!results.empty())
    {
        Ice::AsyncResultPtr result = results.front();
        results.pop_front();
        result->waitForCompleted();
    }
    test(locator->getRequestCount() > count && locator->getRequestCount() < count + 999);
    if(locator->getRequestCount() > count + 800)
    {
        ostringstream os;
        os << "queuing = " + locator->getRequestCount() - count;
        tprintf(os.str().c_str());
    }
    count = locator->getRequestCount();
    hello = hello->ice_adapterId("unknown");
    for(i = 0; i < 1000; i++)
    {
        Ice::AsyncResultPtr result = hello->begin_sayHello(
                                                           newCallback_Hello_sayHello(cb, &AMICallback::response2, &AMICallback::exception2));
        results.push_back(result);
    }
    while(!results.empty())
    {
        Ice::AsyncResultPtr result = results.front();
        results.pop_front();
        result->waitForCompleted();
    }
    // Take into account the retries.
    test(locator->getRequestCount() > count && locator->getRequestCount() < count + 1999);
    if(locator->getRequestCount() > count + 800)
    {
        ostringstream os;
        os << "queuing = " << locator->getRequestCount() - count;
        tprintf(os.str().c_str());
    }
    tprintf("ok\n");
    
    tprintf("testing adapter locator cache... ");
    try
    {
        communicator->stringToProxy("test@TestAdapter3")->ice_ping();
        test(false);
    }
    catch(const Ice::NotRegisteredException& ex)
    {
        test(ex.kindOfObject == "object adapter");
        test(ex.id == "TestAdapter3");
    }
    registry->setAdapterDirectProxy("TestAdapter3", locator->findAdapterById("TestAdapter"));
    try
    {
        communicator->stringToProxy("test@TestAdapter3")->ice_ping();
        registry->setAdapterDirectProxy("TestAdapter3", communicator->stringToProxy("dummy:tcp"));
        communicator->stringToProxy("test@TestAdapter3")->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
        test(false);
    }
    
    try
    {
        communicator->stringToProxy("test@TestAdapter3")->ice_locatorCacheTimeout(0)->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {
    }
    try
    {
        communicator->stringToProxy("test@TestAdapter3")->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {   
    }
    registry->setAdapterDirectProxy("TestAdapter3", locator->findAdapterById("TestAdapter"));
    try
    {
        communicator->stringToProxy("test@TestAdapter3")->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
        test(false);
    }
    tprintf("ok\n");
    
    tprintf("testing well-known object locator cache... ");
    
    registry->addObject(communicator->stringToProxy("test3@TestUnknown"));
    try
    {
        communicator->stringToProxy("test3")->ice_ping();
        test(false);
    }
    catch(const Ice::NotRegisteredException& ex)
    {
        test(ex.kindOfObject == "object adapter");
        test(ex.id == "TestUnknown");
    }
    registry->addObject(communicator->stringToProxy("test3@TestAdapter4")); // Update
    registry->setAdapterDirectProxy("TestAdapter4", communicator->stringToProxy("dummy:tcp"));
    try
    {
        communicator->stringToProxy("test3")->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {
    }
    registry->setAdapterDirectProxy("TestAdapter4", locator->findAdapterById("TestAdapter"));
    try
    {
        communicator->stringToProxy("test3")->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
        test(false);
    }
    
    registry->setAdapterDirectProxy("TestAdapter4", communicator->stringToProxy("dummy:tcp"));
    try
    {
        communicator->stringToProxy("test3")->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
        test(false);
    }
    
    try
    {
        communicator->stringToProxy("test@TestAdapter4")->ice_locatorCacheTimeout(0)->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {
    }
    try
    {
        communicator->stringToProxy("test@TestAdapter4")->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {   
    }
    try
    {
        communicator->stringToProxy("test3")->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {
    }
    registry->addObject(communicator->stringToProxy("test3@TestAdapter"));
    try
    {
        communicator->stringToProxy("test3")->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
        test(false);
    }
    
    registry->addObject(communicator->stringToProxy("test4"));
    try
    {
        communicator->stringToProxy("test4")->ice_ping();
        test(false);
    }
    catch(const Ice::NoEndpointException&)
    {
    }
    tprintf("ok\n");
    
    tprintf("testing locator cache background updates... ");
    {
        Ice::InitializationData initData;
        initData.properties = communicator->getProperties()->clone();
        initData.properties->setProperty("Ice.BackgroundLocatorCacheUpdates", "1");
        Ice::CommunicatorPtr ic = Ice::initialize(initData);
        
        registry->setAdapterDirectProxy("TestAdapter5", locator->findAdapterById("TestAdapter"));
        registry->addObject(communicator->stringToProxy("test3@TestAdapter"));
        
        int count = locator->getRequestCount();
        ic->stringToProxy("test@TestAdapter5")->ice_locatorCacheTimeout(0)->ice_ping(); // No locator cache.
        ic->stringToProxy("test3")->ice_locatorCacheTimeout(0)->ice_ping(); // No locator cache.
        count += 3;
        test(count == locator->getRequestCount());
        registry->setAdapterDirectProxy("TestAdapter5", 0);
        registry->addObject(communicator->stringToProxy("test3:tcp"));
        ic->stringToProxy("test@TestAdapter5")->ice_locatorCacheTimeout(10)->ice_ping(); // 10s timeout.
        ic->stringToProxy("test3")->ice_locatorCacheTimeout(10)->ice_ping(); // 10s timeout.
        test(count == locator->getRequestCount());
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(1200));
        
        // The following request should trigger the background updates but still use the cached endpoints
        // and therefore succeed.
        ic->stringToProxy("test@TestAdapter5")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout.
        ic->stringToProxy("test3")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout.
        
        try
        {
            while(true)
            {
                ic->stringToProxy("test@TestAdapter5")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout.
                IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(10));
            }
        }
        catch(const Ice::LocalException&)
        {
            // Expected to fail once they endpoints have been updated in the background.
        }
        try
        {
            while(true)
            {
                ic->stringToProxy("test3")->ice_locatorCacheTimeout(1)->ice_ping(); // 1s timeout.
                IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(10));
            }
        }
        catch(const Ice::LocalException&)
        {
            // Expected to fail once they endpoints have been updated in the background.
        }
        ic->destroy();
    }
    tprintf("ok\n");
    
    tprintf("testing proxy from server after shutdown... ");
    hello = obj->getReplicatedHello();
    obj->shutdown();
    manager->startServer();
    hello->sayHello();
    tprintf("ok\n");
    
    tprintf("testing object migration... ");
    hello = HelloPrx::checkedCast(communicator->stringToProxy("hello"));
    obj->migrateHello();
    hello->sayHello();
    obj->migrateHello();
    hello->sayHello();
    obj->migrateHello();
    hello->sayHello();
    tprintf("ok\n");
    
    tprintf("shutdown server... ");
    obj->shutdown();
    tprintf("ok\n");
    
    tprintf("testing whether server is gone... ");
    try
    {
        obj2->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {
    }
    try
    {
        obj3->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {
    }
    try
    {
        obj5->ice_ping();
        test(false);
    }
    catch(const Ice::LocalException&)
    {
    }
    tprintf("ok\n");
    
    tprintf("testing indirect proxies to collocated objects... ");
    //
    // Set up test for calling a collocated object through an indirect, adapterless reference.
    //
    Ice::PropertiesPtr properties = communicator->getProperties();
    properties->setProperty("Ice.PrintAdapterReady", "0");
    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapterWithEndpoints("Hello", "default");
    adapter->setLocator(locator);
    
    Ice::Identity id;
    id.name = IceUtil::generateUUID();
    registry->addObject(adapter->add(new HelloI, id));
    adapter->activate();
    
    try
    {
        HelloPrx helloPrx = HelloPrx::checkedCast(communicator->stringToProxy(communicator->identityToString(id)));
        Ice::ConnectionPtr connection = helloPrx->ice_getConnection();
        test(false);
    }
    catch(const Ice::CollocationOptimizationException&)
    {
    }
    adapter->deactivate();
    tprintf("ok\n");
    
    tprintf("shutdown server manager... ");
    manager->shutdown();
    tprintf("ok\n");
}
