// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <Test.h>

using namespace std;
using namespace Test;

void
allTests(const Ice::CommunicatorPtr& communicator, const string& ref)
{
    ServerManagerPrx manager = ServerManagerPrx::checkedCast(communicator->stringToProxy(ref));
    test(manager);

    cout << "testing stringToProxy... " << flush;
    Ice::ObjectPrx base = communicator->stringToProxy("test @ TestAdapter");
    Ice::ObjectPrx base2 = communicator->stringToProxy("test @ TestAdapter");
    Ice::ObjectPrx base3 = communicator->stringToProxy("test");
    Ice::ObjectPrx base4 = communicator->stringToProxy("ServerManager"); 
    Ice::ObjectPrx base5 = communicator->stringToProxy("test2");
    cout << "ok" << endl;

    cout << "starting server... " << flush;
    manager->startServer();
    cout << "ok" << endl;

    cout << "testing checked cast... " << flush;
    TestIntfPrx obj = TestIntfPrx::checkedCast(base);
    obj = TestIntfPrx::checkedCast(communicator->stringToProxy("test@TestAdapter"));
    obj = TestIntfPrx::checkedCast(communicator->stringToProxy("test   @TestAdapter"));
    obj = TestIntfPrx::checkedCast(communicator->stringToProxy("test@   TestAdapter"));
    test(obj);
    TestIntfPrx obj2 = TestIntfPrx::checkedCast(base2);
    test(obj2);
    TestIntfPrx obj3 = TestIntfPrx::checkedCast(base3);
    test(obj3);
    ServerManagerPrx obj4 = ServerManagerPrx::checkedCast(base4);
    test(obj4);
    TestIntfPrx obj5 = TestIntfPrx::checkedCast(base5);
    test(obj5);
    cout << "ok" << endl;
 
    cout << "testing id@AdapterId indirect proxy... " << flush;
    obj->shutdown();
    manager->startServer();
    try
    {
	obj2 = TestIntfPrx::checkedCast(base2);
	obj2->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    cout << "ok" << endl;    
    
    cout << "testing identity indirect proxy... " << flush;
    obj->shutdown();
    manager->startServer();
    try
    {
	obj3 = TestIntfPrx::checkedCast(base3);
	obj3->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    try
    {
	obj2 = TestIntfPrx::checkedCast(base2);
	obj2->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    obj->shutdown();
    manager->startServer();
    try
    {
	obj2 = TestIntfPrx::checkedCast(base2);
	obj2->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    try
    {
	obj3 = TestIntfPrx::checkedCast(base3);
	obj3->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    obj->shutdown();
    manager->startServer();

    try
    {
	obj2 = TestIntfPrx::checkedCast(base2);
	obj2->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    obj->shutdown();
    manager->startServer();
    try
    {
	obj3 = TestIntfPrx::checkedCast(base3);
	obj3->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    obj->shutdown();
    manager->startServer();
    try
    {
	obj2 = TestIntfPrx::checkedCast(base2);
	obj2->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    obj->shutdown();
    manager->startServer();

    try
    {
	obj5 = TestIntfPrx::checkedCast(base5);
	obj5->ice_ping();
    }
    catch(const Ice::LocalException&)
    {
	test(false);
    }
    cout << "ok" << endl;

    cout << "testing reference with unknown identity... " << flush;
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
    cout << "ok" << endl;

    cout << "testing reference with unknown adapter... " << flush;
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
    cout << "ok" << endl;

    cout << "testing object reference from server... " << flush;
    HelloPrx hello = obj->getHello();
    hello->sayHello();
    cout << "ok" << endl;

    cout << "testing object reference from server after shutdown... " << flush;
    obj->shutdown();
    manager->startServer();
    hello->sayHello();
    cout << "ok" << endl;

    cout << "testing object migration... " << flush;
    hello = HelloPrx::checkedCast(communicator->stringToProxy("hello"));
    obj->migrateHello();
    hello->sayHello();
    obj->migrateHello();
    hello->sayHello();
    obj->migrateHello();
    hello->sayHello();
    cout << "ok" << endl;

    cout << "shutdown server... " << flush;
    obj->shutdown();
    cout << "ok" << endl;

    cout << "testing whether server is gone... " << flush;
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
    cout << "ok" << endl;

    cout << "shutdown server manager... " << flush;
    manager->shutdown();
    cout << "ok" << endl;
}
