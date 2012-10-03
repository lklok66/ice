// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <hold/TestI.h>
#include <TestCommon.h>

using namespace std;

#if TARGET_OS_IPHONE
#define main runHoldServer
#endif

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;
    
    try
    {
        Ice::InitializationData initData;
        initData.properties = defaultServerProperties(argc, argv);
        communicator = Ice::initialize(initData);
#if TARGET_OS_IPHONE
        PrivateServerImplementation::communicator = communicator;
#endif
        IceUtil::TimerPtr timer = new IceUtil::Timer();
        
        communicator->getProperties()->setProperty("TestAdapter1.Endpoints", "default -p 12010:udp");
        communicator->getProperties()->setProperty("TestAdapter1.ThreadPool.Size", "5"); 
        communicator->getProperties()->setProperty("TestAdapter1.ThreadPool.SizeMax", "5"); 
        communicator->getProperties()->setProperty("TestAdapter1.ThreadPool.SizeWarn", "0"); 
        communicator->getProperties()->setProperty("TestAdapter1.ThreadPool.Serialize", "0");
        Ice::ObjectAdapterPtr adapter1 = communicator->createObjectAdapter("TestAdapter1");
        adapter1->add(new HoldI(timer, adapter1), communicator->stringToIdentity("hold"));
        
        communicator->getProperties()->setProperty("TestAdapter2.Endpoints", "default -p 12011:udp");
        communicator->getProperties()->setProperty("TestAdapter2.ThreadPool.Size", "5"); 
        communicator->getProperties()->setProperty("TestAdapter2.ThreadPool.SizeMax", "5"); 
        communicator->getProperties()->setProperty("TestAdapter2.ThreadPool.SizeWarn", "0"); 
        communicator->getProperties()->setProperty("TestAdapter2.ThreadPool.Serialize", "1");
        Ice::ObjectAdapterPtr adapter2 = communicator->createObjectAdapter("TestAdapter2");
        adapter2->add(new HoldI(timer, adapter2), communicator->stringToIdentity("hold"));
        
        adapter1->activate();
        adapter2->activate();
        serverReady();
        communicator->waitForShutdown();
        
        timer->destroy();
        
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
#if TARGET_OS_IPHONE
            PrivateServerImplementation::communicator = 0;
#endif
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
