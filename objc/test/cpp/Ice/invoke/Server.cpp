// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <invoke/BlobjectI.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Invoke;

namespace
{

class ServantLocatorI : public Ice::ServantLocator
{   
public:
    
    ServantLocatorI()
    {
        _blobject = new BlobjectI();
    }
    
    virtual Ice::ObjectPtr
    locate(const Ice::Current&, Ice::LocalObjectPtr&)
    {
        return _blobject;
    }
    
    virtual void
    finished(const Ice::Current&, const Ice::ObjectPtr&, const Ice::LocalObjectPtr&)
    {
    }
    
    virtual void
    deactivate(const string&)
    {
    }
    
private:
    
    Ice::ObjectPtr _blobject;
};

}

#if TARGET_OS_IPHONE
#define main runInvokeServer
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
        communicator->getProperties()->setProperty("Ice.Trace.Protocol", "3");
        communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010:udp");
        Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
        adapter->addServantLocator(new ServantLocatorI(), "");
        adapter->activate();
        serverReady();
        communicator->waitForShutdown();
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
