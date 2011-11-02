// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_OBJECTS_TEST_I_H
#define TEST_OBJECTS_TEST_I_H

#include <ObjectsTest.h>

namespace Test
{

namespace Objects
{

class BI : public Test::Objects::B
{
public:
    
    BI();
    
    virtual bool postUnmarshalInvoked(const Ice::Current&);
    
    virtual void ice_preMarshal();
    virtual void ice_postUnmarshal();
    
private:
    
    bool _postUnmarshalInvoked;
};

class CI : public Test::Objects::C
{
public:
    
    CI();
    
    virtual bool postUnmarshalInvoked(const Ice::Current&);
    
    virtual void ice_preMarshal();
    virtual void ice_postUnmarshal();
    
private:
    
    bool _postUnmarshalInvoked;
};

class DI : public Test::Objects::D
{
public:
    
    DI();
    
    virtual bool postUnmarshalInvoked(const Ice::Current&);
    
    virtual void ice_preMarshal();
    virtual void ice_postUnmarshal();
    
private:
    
    bool _postUnmarshalInvoked;
};

class EI : public Test::Objects::E
{
public:
    
    EI();
    
    virtual bool checkValues(const Ice::Current&);
};

class FI : public Test::Objects::F
{
public:
    
    FI();
    FI(const Test::Objects::EPtr&);
    
    virtual bool checkValues(const Ice::Current&);
};

class II : public Test::Objects::I
{
};

class JI : public Test::Objects::J
{
};

class HI : public Test::Objects::H
{
};

class InitialI : public Test::Objects::Initial
{
public:
    
    InitialI(const Ice::ObjectAdapterPtr&);
    
    virtual void shutdown(const Ice::Current&);
    virtual Test::Objects::BPtr getB1(const Ice::Current&);
    virtual Test::Objects::BPtr getB2(const Ice::Current&);
    virtual Test::Objects::CPtr getC(const Ice::Current&);
    virtual Test::Objects::DPtr getD(const Ice::Current&);
    virtual Test::Objects::EPtr getE(const Ice::Current&);
    virtual Test::Objects::FPtr getF(const Ice::Current&);
    virtual void getAll(Test::Objects::BPtr&, Test::Objects::BPtr&, Test::Objects::CPtr&, Test::Objects::DPtr&, const Ice::Current&);
    virtual Test::Objects::IPtr getI(const Ice::Current&);
    virtual Test::Objects::IPtr getJ(const Ice::Current&);
    virtual Test::Objects::IPtr getH(const Ice::Current&);
    
    virtual void setI(const Test::Objects::IPtr&, const Ice::Current&);
    
private:
    
    Ice::ObjectAdapterPtr _adapter;
    Test::Objects::BPtr _b1;
    Test::Objects::BPtr _b2;
    Test::Objects::CPtr _c;
    Test::Objects::DPtr _d;
    Test::Objects::EPtr _e;
    Test::Objects::FPtr _f;
};

class UnexpectedObjectExceptionTestI : public Ice::Blobject
{
public:
    
    virtual bool ice_invoke(const std::vector<Ice::Byte>&, std::vector<Ice::Byte>&, const Ice::Current&);
};
typedef IceUtil::Handle<UnexpectedObjectExceptionTestI> UnexpectedObjectExceptionTestIPtr;

}

}

#endif
