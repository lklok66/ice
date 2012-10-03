// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef INHERITANCE_TESTI_H
#define INHERITANCE_TESTI_H

#include <InheritanceTest.h>

namespace  Test
{

namespace Inheritance
{

class InitialI : public Test::Inheritance::Initial
{
public:
    
    InitialI(const Ice::ObjectAdapterPtr&);
    
    virtual void shutdown(const Ice::Current&);
    
    virtual Test::Inheritance::MA::CAPrx caop(const Ice::Current&);
    virtual Test::Inheritance::MB::CBPrx cbop(const Ice::Current&);
    virtual Test::Inheritance::MA::CCPrx ccop(const Ice::Current&);
    virtual Test::Inheritance::MA::CDPrx cdop(const Ice::Current&);
    virtual Test::Inheritance::MA::IAPrx iaop(const Ice::Current&);
    virtual Test::Inheritance::MB::IB1Prx ib1op(const Ice::Current&);
    virtual Test::Inheritance::MB::IB2Prx ib2op(const Ice::Current&);
    virtual Test::Inheritance::MA::ICPrx icop(const Ice::Current&);
    
private:
    
    Test::Inheritance::MA::CAPrx _ca;
    Test::Inheritance::MB::CBPrx _cb;
    Test::Inheritance::MA::CCPrx _cc;
    Test::Inheritance::MA::CDPrx _cd;
    Test::Inheritance::MA::IAPrx _ia;
    Test::Inheritance::MB::IB1Prx _ib1;
    Test::Inheritance::MB::IB2Prx _ib2;
    Test::Inheritance::MA::ICPrx _ic;
};

class CAI_ : virtual public Test::Inheritance::MA::CA
{
public:
    
    virtual Test::Inheritance::MA::CAPrx caop(const Test::Inheritance::MA::CAPrx&, const Ice::Current&);
};

class CBI : virtual public Test::Inheritance::MB::CB, virtual public CAI_
{
public:
    
    virtual Test::Inheritance::MB::CBPrx cbop(const Test::Inheritance::MB::CBPrx&, const Ice::Current&);
};

class CCI : virtual public Test::Inheritance::MA::CC, virtual public CBI
{
public:
    
    virtual Test::Inheritance::MA::CCPrx ccop(const Test::Inheritance::MA::CCPrx&, const Ice::Current&);
};

class IAI : virtual public Test::Inheritance::MA::IA
{
public:
    
    virtual Test::Inheritance::MA::IAPrx iaop(const Test::Inheritance::MA::IAPrx&, const Ice::Current&);
};

class IB1I : virtual public Test::Inheritance::MB::IB1, virtual public IAI
{
public:
    
    virtual Test::Inheritance::MB::IB1Prx ib1op(const Test::Inheritance::MB::IB1Prx&, const Ice::Current&);                  
};

class IB2I : virtual public Test::Inheritance::MB::IB2, virtual public IAI
{
public:
    
    virtual Test::Inheritance::MB::IB2Prx ib2op(const Test::Inheritance::MB::IB2Prx&, const Ice::Current&);
};

class ICI : virtual public Test::Inheritance::MA::IC, virtual public IB1I, virtual public IB2I
{
public:
    
    virtual Test::Inheritance::MA::ICPrx icop(const Test::Inheritance::MA::ICPrx&, const Ice::Current&);
};

class CDI : virtual public Test::Inheritance::MA::CD, virtual public CCI, virtual public IB1I, virtual public IB2I
{
public:
    
    virtual Test::Inheritance::MA::CDPrx cdop(const Test::Inheritance::MA::CDPrx&, const Ice::Current&);
};
    
}
    
}

#endif
