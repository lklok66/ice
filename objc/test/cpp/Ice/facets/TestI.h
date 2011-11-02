// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef FACETS_TEST_I_H
#define FACETS_TEST_I_H

#include <FacetsTest.h>
namespace Test
{
    
namespace Facets
{

class AI : virtual public Test::Facets::A
{
public:
    
    virtual std::string callA(const Ice::Current&);
};

class BI : virtual public Test::Facets::B, virtual public AI
{
public:
    
    virtual std::string callB(const Ice::Current&);
};

class CI : virtual public Test::Facets::C, virtual public AI
{
public:
    
    virtual std::string callC(const Ice::Current&);
};

class DI : virtual public Test::Facets::D, virtual public BI, virtual public CI
{
public:
    
    virtual std::string callD(const Ice::Current&);
};

class EI : virtual public Test::Facets::E
{
public:
    
    virtual std::string callE(const Ice::Current&);
};

class FI : virtual public Test::Facets::F, virtual public EI
{
public:
    
    virtual std::string callF(const Ice::Current&);
};

class GI : virtual public Test::Facets::G
{
public:
    
    GI(const Ice::CommunicatorPtr&);
    virtual void shutdown(const Ice::Current&);
    virtual std::string callG(const Ice::Current&);
    
private:
    
    Ice::CommunicatorPtr _communicator;
};

class HI : virtual public Test::Facets::H, virtual public GI
{
public:
    
    HI(const Ice::CommunicatorPtr&);
    virtual std::string callH(const Ice::Current&);
};
    
}
    
}

#endif
