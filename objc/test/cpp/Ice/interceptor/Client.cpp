// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <InterceptorTest.h>
#include <interceptor/MyObjectI.h>
#include <interceptor/InterceptorI.h>
#include <iostream>

using namespace std;
using namespace Test::Interceptor;

#if TARGET_OS_IPHONE
#define main runInterceptorClient
#endif

int
main(int argc, char* argv[])
{    
    int status;
    Ice::CommunicatorPtr communicator;
    
    try
    {
        //
        // Create OA and servants  
        // 
        Ice::InitializationData initData;
        initData.properties = defaultClientProperties(argc, argv);
        initData.properties->setProperty("Ice.Warn.Dispatch", "0");
        communicator = Ice::initialize(initData);
        Ice::ObjectAdapterPtr oa = communicator->createObjectAdapterWithEndpoints("MyOA", "tcp -h localhost");
    
        Ice::ObjectPtr servant = new MyObjectI;
        InterceptorIPtr interceptor = new InterceptorI(servant);
    
        Test::Interceptor::MyObjectPrx prx = Test::Interceptor::MyObjectPrx::uncheckedCast(oa->addWithUUID(interceptor));
    
        oa->activate();

        tprintf("Collocation optimization off");
        interceptor->clear();
        prx = Test::Interceptor::MyObjectPrx::uncheckedCast(prx->ice_collocationOptimized(false));
        tprintf("testing simple interceptor... ");
        test(interceptor->getLastOperation().empty());
        prx->ice_ping();
        test(interceptor->getLastOperation() == "ice_ping");
        test(interceptor->getLastStatus() == Ice::DispatchOK);
        string typeId = prx->ice_id();
        test(interceptor->getLastOperation() == "ice_id");
        test(interceptor->getLastStatus() == Ice::DispatchOK);
        test(prx->ice_isA(typeId));
        test(interceptor->getLastOperation() == "ice_isA");
        test(interceptor->getLastStatus() == Ice::DispatchOK);
        test(prx->add(33, 12) == 45);
        test(interceptor->getLastOperation() == "add");
        test(interceptor->getLastStatus() == Ice::DispatchOK);
        tprintf("ok\n");
        tprintf("testing retry... ");
        test(prx->addWithRetry(33, 12) == 45);
        test(interceptor->getLastOperation() == "addWithRetry");
        test(interceptor->getLastStatus() == Ice::DispatchOK);
        tprintf("ok\n");
        tprintf("testing user exception... ");
        try
        {
            prx->badAdd(33, 12);
            test(false);
        }
        catch(const Test::Interceptor::InvalidInputException&)
        {
            // expected
        }
        test(interceptor->getLastOperation() == "badAdd");
        test(interceptor->getLastStatus() == Ice::DispatchUserException);
        tprintf("ok\n");
        tprintf("testing ONE... ");
        
        interceptor->clear();
        try
        {
            prx->notExistAdd(33, 12);
            test(false);
        }
        catch(const Ice::ObjectNotExistException&)
        {
            // expected
        }
        test(interceptor->getLastOperation() == "notExistAdd");
        tprintf("ok\n");
        tprintf("testing system exception... ");
        interceptor->clear();
        try
        {
            prx->badSystemAdd(33, 12);
            test(false);
        }
        catch(const Ice::UnknownLocalException&)
        {
        }
        catch(...)
        {
            test(false);
        }
        test(interceptor->getLastOperation() == "badSystemAdd");
        tprintf("ok\n");

        status = EXIT_SUCCESS;
    }
    catch(const Ice::Exception& ex)
    {
        ostringstream os;
        os << ex << endl;
        tprintf(os.str().c_str());
        status = EXIT_FAILURE;
    }
    catch(const TestFailedException&)
    {
        status = EXIT_FAILURE;
    }
    
    if(communicator)
    {
        try
        {
            communicator->destroy();
        }
        catch(const Ice::Exception& ex)
        {
            ostringstream os;
            os << ex << endl;
            tprintf(os.str().c_str());
            status = EXIT_FAILURE;
        }
    }
    return status;
}
