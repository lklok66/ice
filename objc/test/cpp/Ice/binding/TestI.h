// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_I_H
#define TEST_I_H

#include <BindingTest.h>

namespace Test
{
    
namespace Binding
{

class RemoteCommunicatorI : public Test::Binding::RemoteCommunicator
{
public:
    
    RemoteCommunicatorI();
    
    virtual Test::Binding::RemoteObjectAdapterPrx createObjectAdapter(const std::string&, const std::string&, 
                                                             const Ice::Current&);
    virtual void deactivateObjectAdapter(const Test::Binding::RemoteObjectAdapterPrx&, const Ice::Current&);
    virtual void shutdown(const Ice::Current&);
    
private:
    
    int _nextPort;
};

class RemoteObjectAdapterI : public Test::Binding::RemoteObjectAdapter
{
public:
    
    RemoteObjectAdapterI(const Ice::ObjectAdapterPtr&);
    
    virtual Test::Binding::TestIntfPrx getTestIntf(const Ice::Current&);
    virtual void deactivate(const Ice::Current&);
    
private:
    
    const Ice::ObjectAdapterPtr _adapter;
    const Test::Binding::TestIntfPrx _testIntf;
};

class TestI : public Test::Binding::TestIntf
{
public:
    
    virtual std::string getAdapterName(const Ice::Current&);
};

}
    
}

#endif
