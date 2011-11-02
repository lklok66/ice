// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <timeout/TestI.h>
#include <Ice/Ice.h>
#include <IceUtil/Thread.h>

using namespace std;
using namespace Ice;
using namespace Test;

class ActivateAdapterThread : public IceUtil::Thread
{
public:
    
    ActivateAdapterThread(const ObjectAdapterPtr& adapter, int timeout) :
    _adapter(adapter), _timeout(timeout)
    {
    }
    
    virtual void
    run()
    {
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(_timeout));
        _adapter->activate();
    }
    
private:
    
    ObjectAdapterPtr _adapter;
    int _timeout;
};

void
TimeoutI::op(const Ice::Current&)
{
}

void
TimeoutI::sendData(const Test::ByteSeq&, const Ice::Current&)
{
}

void
TimeoutI::sleep(Ice::Int to, const Ice::Current&)
{
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(to));
}

void
TimeoutI::holdAdapter(Ice::Int to, const Ice::Current& current)
{
    current.adapter->hold();
    IceUtil::ThreadPtr thread = new ActivateAdapterThread(current.adapter, to);
    thread->start();
}

void
TimeoutI::shutdown(const Ice::Current& current)
{
    current.adapter->getCommunicator()->shutdown();
}
