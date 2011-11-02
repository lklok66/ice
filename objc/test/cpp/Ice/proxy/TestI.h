// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_PROXY_TEST_I_H
#define TEST_PROXY_TEST_I_H

#include <ProxyTest.h>

namespace Test
{

namespace Proxy
{

class MyDerivedClassI : public Test::Proxy::MyDerivedClass
{
public:
    
    MyDerivedClassI();
    
    virtual Ice::ObjectPrx echo(const Ice::ObjectPrx&, const Ice::Current&);
    virtual void shutdown(const Ice::Current&);
    virtual Ice::Context getContext(const Ice::Current&);
    virtual bool ice_isA(const std::string&, const Ice::Current&) const;
    
private:
    
    mutable Ice::Context _ctx;
};
    
}
    
}

#endif
