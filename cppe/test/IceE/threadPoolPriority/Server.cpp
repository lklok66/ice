// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceE/IceE.h>
#include <TestI.h>
#include <TestApplication.h>
#include <iostream>

using namespace std;

class PriorityTestApplication : public TestApplication
{
public:
    PriorityTestApplication() :
        TestApplication("priority server")
    {
    }

int
run(int argc, char** argv)
{
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    initData.properties->setProperty("TestAdapter.Endpoints", "default -p 12010 -t 10000");
#ifdef _WIN32_WCE
    initData.properties->setProperty("Ice.ThreadPool.Server.ThreadPriority", "0");
#elif defined _WIN32
    initData.properties->setProperty("Ice.ThreadPool.Server.ThreadPriority", "1");
#else
    initData.properties->setProperty("Ice.ThreadPool.Server.ThreadPriority", "50");
#endif
    loadConfig(initData.properties);
    initData.logger = getLogger();
    setCommunicator(Ice::initialize(argc, argv, initData));
    
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("TestAdapter");
    adapter->add(new PriorityI(adapter), communicator()->stringToIdentity("test"));
    adapter->activate();
#ifndef _WIN32_WCE
        communicator()->waitForShutdown();
#endif
    return EXIT_SUCCESS;
}

};
#ifdef _WIN32_WCE

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    PriorityTestApplication app;
    return app.main(hInstance);
}

#else

int
main(int argc, char* argv[])
{
    PriorityTestApplication app;
    return app.main(argc, argv);
}

#endif
