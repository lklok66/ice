// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <RetryTest.h>

using namespace std;
using namespace Test;

namespace 
{

class CallbackBase : public IceUtil::Monitor<IceUtil::Mutex>
{
public:
    
    CallbackBase() :
    _called(false)
    {
    }
    
    virtual ~CallbackBase()
    {
    }
    
    void check()
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        while(!_called)
        {
            wait();
        }
        _called = false;
    }
    
protected:
    
    void called()
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        assert(!_called);
        _called = true;
        notify();
    }
    
private:
    
    bool _called;
};

class CallbackSuccess : public IceUtil::Shared, public CallbackBase
{
public:
    
    void response()
    {
        called();
    }
    
    void exception(const ::Ice::Exception&)
    {
        test(false);
    }
};
typedef IceUtil::Handle<CallbackSuccess> CallbackSuccessPtr;

class CallbackFail : public IceUtil::Shared, public CallbackBase
{
public:
    
    void response()
    {
        test(false);
    }
    
    void exception(const ::Ice::Exception& ex)
    {
        test(dynamic_cast<const Ice::ConnectionLostException*>(&ex));
        called();
    }
};
typedef IceUtil::Handle<CallbackFail> CallbackFailPtr;
    
}

RetryPrx
retryAllTests(const Ice::CommunicatorPtr& communicator)
{
    tprintf("testing stringToProxy... ");
    string ref = "retry:default -p 12010";
    Ice::ObjectPrx base1 = communicator->stringToProxy(ref);
    test(base1);
    Ice::ObjectPrx base2 = communicator->stringToProxy(ref);
    test(base2);
    tprintf("ok\n");
    
    tprintf("testing checked cast... ");
    RetryPrx retry1 = RetryPrx::checkedCast(base1);
    test(retry1);
    test(retry1 == base1);
    RetryPrx retry2 = RetryPrx::checkedCast(base2);
    test(retry2);
    test(retry2 == base2);
    tprintf("ok\n");
    
    tprintf("calling regular operation with first proxy... ");
    retry1->op(false);
    tprintf("ok\n");
    
    tprintf("calling operation to kill connection with second proxy... ");
    try
    {
        retry2->op(true);
        test(false);
    }
    catch(Ice::ConnectionLostException)
    {
        tprintf("ok\n");
    }
    
    tprintf("calling regular operation with first proxy again... ");
    retry1->op(false);
    tprintf("ok\n");
    
    CallbackSuccessPtr cb1 = new CallbackSuccess();
    CallbackFailPtr cb2 = new CallbackFail();
    
    tprintf("calling regular AMI operation with first proxy... ");
    retry1->begin_op(false, newCallback_Retry_op(cb1, &CallbackSuccess::response, &CallbackSuccess::exception));
    cb1->check();
    tprintf("ok\n");
    
    tprintf("calling AMI operation to kill connection with second proxy... ");
    retry2->begin_op(true, newCallback_Retry_op(cb2, &CallbackFail::response, &CallbackFail::exception));
    cb2->check();
    tprintf("ok\n");
    
    tprintf("calling regular AMI operation with first proxy again... ");
    retry1->begin_op(false, newCallback_Retry_op(cb1, &CallbackSuccess::response, &CallbackSuccess::exception));
    cb1->check();
    tprintf("ok\n");
    
    return retry1;
}
