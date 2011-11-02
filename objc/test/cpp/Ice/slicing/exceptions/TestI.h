// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_SLICING_EXCEPTIONS_TEST_I_H
#define TEST_SLICING_EXCEPTIONS_TEST_I_H

#include <SlicingExceptionsServerPrivate.h>

namespace Test
{

namespace Slicing
{
    
namespace Exceptions
{

class TestI : virtual public Test::Slicing::Exceptions::TestIntf
{
public:
    
    TestI();
    virtual void baseAsBase(const ::Ice::Current&);
    virtual void unknownDerivedAsBase(const ::Ice::Current&);
    virtual void knownDerivedAsBase(const ::Ice::Current&);
    virtual void knownDerivedAsKnownDerived(const ::Ice::Current&);
    
    virtual void unknownIntermediateAsBase(const ::Ice::Current&);
    virtual void knownIntermediateAsBase(const ::Ice::Current&);
    virtual void knownMostDerivedAsBase(const ::Ice::Current&);
    virtual void knownIntermediateAsKnownIntermediate(const ::Ice::Current&);
    virtual void knownMostDerivedAsKnownIntermediate(const ::Ice::Current&);
    virtual void knownMostDerivedAsKnownMostDerived(const ::Ice::Current&);
    
    virtual void unknownMostDerived1AsBase(const ::Ice::Current&);
    virtual void unknownMostDerived1AsKnownIntermediate(const ::Ice::Current&);
    virtual void unknownMostDerived2AsBase(const ::Ice::Current&);
    
    virtual void shutdown(const ::Ice::Current&);
};
    
}
    
}
    
}

#endif
