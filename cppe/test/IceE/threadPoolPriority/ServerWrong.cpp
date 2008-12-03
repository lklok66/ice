// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceE/IceE.h>
#include <TestCommon.h>
#include <TestI.h>
#include <TestApplication.h>
#include <iostream>

using namespace std;


class NullLogger : virtual public ::Ice::Logger
{
public:

    virtual void print(const ::std::string&){};
    virtual void trace(const ::std::string&, const ::std::string&){};
    virtual void warning(const ::std::string&){};
    virtual void error(const ::std::string&){};
};

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
    
    initData.properties->setProperty("Ice.ThreadPool.Server.ThreadPriority", "1024");
    loadConfig(initData.properties);
    initData.logger = new NullLogger();
    setCommunicator(Ice::initialize(argc, argv, initData));
    try
    {
        Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("TestAdapter");
        test(false);
    }
    catch(const IceUtil::ThreadSyscallException&)
    {
        //expected
    }
    catch(...)
    {
        test(false);
    }
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
