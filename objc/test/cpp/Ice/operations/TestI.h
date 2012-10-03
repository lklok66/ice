// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_OPERATIONS_TEST_I_H
#define TEST_OPERATIONS_TEST_I_H

#include <OperationsTest.h>

namespace Test
{

namespace Operations
{

class MyDerivedClassI : public Test::Operations::MyDerivedClass
{
public:
    
    //
    // Override the Object "pseudo" operations to verify the operation mode.
    //
    virtual bool ice_isA(const std::string&, const Ice::Current&) const;
    virtual void ice_ping(const Ice::Current&) const;
    virtual std::vector<std::string> ice_ids(const Ice::Current&) const;
    virtual const std::string& ice_id(const Ice::Current&) const;
    
    virtual void shutdown(const Ice::Current&);
    
    virtual void delay(Ice::Int, const Ice::Current&);
    
    virtual void opVoid(const Ice::Current&);
    
    virtual Ice::Byte opByte(Ice::Byte,
                             Ice::Byte,
                             Ice::Byte&,
                             const Ice::Current&);
    
    virtual bool opBool(bool,
                        bool,
                        bool&,
                        const Ice::Current&);
    
    virtual Ice::Long opShortIntLong(Ice::Short,
                                     Ice::Int,
                                     Ice::Long,
                                     Ice::Short&,
                                     Ice::Int&,
                                     Ice::Long&,
                                     const Ice::Current&);
    
    virtual Ice::Double opFloatDouble(Ice::Float,
                                      Ice::Double,
                                      Ice::Float&,
                                      Ice::Double&,
                                      const Ice::Current&);
    
    virtual std::string opString(const std::string&,
                                 const std::string&,
                                 std::string&,
                                 const Ice::Current&);
    
    virtual Test::Operations::MyEnum opMyEnum(Test::Operations::MyEnum,
                                  Test::Operations::MyEnum&,
                                  const Ice::Current&);
    
    virtual Test::Operations::MyClassPrx opMyClass(const Test::Operations::MyClassPrx&,
                                       Test::Operations::MyClassPrx&, Test::Operations::MyClassPrx&,
                                       const Ice::Current&);
    
    virtual Test::Operations::Structure opStruct(const Test::Operations::Structure&, const Test::Operations::Structure&,
                                     Test::Operations::Structure&,
                                     const Ice::Current&);
    
    virtual Test::Operations::ByteS opByteS(const Test::Operations::ByteS&,
                                const Test::Operations::ByteS&,
                                Test::Operations::ByteS&,
                                const Ice::Current&);
    
    virtual Test::Operations::BoolS opBoolS(const Test::Operations::BoolS&,
                                const Test::Operations::BoolS&,
                                Test::Operations::BoolS&,
                                const Ice::Current&);
    
    virtual Test::Operations::LongS opShortIntLongS(const Test::Operations::ShortS&,
                                        const Test::Operations::IntS&,
                                        const Test::Operations::LongS&,
                                        Test::Operations::ShortS&,
                                        Test::Operations::IntS&,
                                        Test::Operations::LongS&,
                                        const Ice::Current&);
    
    virtual Test::Operations::DoubleS opFloatDoubleS(const Test::Operations::FloatS&,
                                         const Test::Operations::DoubleS&,
                                         Test::Operations::FloatS&,
                                         Test::Operations::DoubleS&,
                                         const Ice::Current&);
    
    virtual Test::Operations::StringS opStringS(const Test::Operations::StringS&,
                                    const Test::Operations::StringS&,
                                    Test::Operations::StringS&,
                                    const Ice::Current&);
    
    virtual Test::Operations::ByteSS opByteSS(const Test::Operations::ByteSS&,
                                  const Test::Operations::ByteSS&,
                                  Test::Operations::ByteSS&,
                                  const Ice::Current&);
    
    virtual Test::Operations::BoolSS opBoolSS(const Test::Operations::BoolSS&,
                                  const Test::Operations::BoolSS&,
                                  Test::Operations::BoolSS&,
                                  const Ice::Current&);
    
    virtual Test::Operations::LongSS opShortIntLongSS(const Test::Operations::ShortSS&,
                                          const Test::Operations::IntSS&,
                                          const Test::Operations::LongSS&,
                                          Test::Operations::ShortSS&,
                                          Test::Operations::IntSS&,
                                          Test::Operations::LongSS&,
                                          const Ice::Current&);
    
    virtual Test::Operations::DoubleSS opFloatDoubleSS(const Test::Operations::FloatSS&,
                                           const Test::Operations::DoubleSS&,
                                           Test::Operations::FloatSS&,
                                           Test::Operations::DoubleSS&,
                                           const Ice::Current&);
    
    virtual Test::Operations::StringSS opStringSS(const Test::Operations::StringSS&,
                                      const Test::Operations::StringSS&,
                                      Test::Operations::StringSS&,
                                      const Ice::Current&);
    
    virtual Test::Operations::StringSSS opStringSSS(const Test::Operations::StringSSS&,
                                        const Test::Operations::StringSSS&,
                                        Test::Operations::StringSSS&,
                                        const ::Ice::Current&);
    
    virtual Test::Operations::ByteBoolD opByteBoolD(const Test::Operations::ByteBoolD&, const Test::Operations::ByteBoolD&, 
                                        Test::Operations::ByteBoolD&,
                                        const Ice::Current&);
    
    virtual Test::Operations::ShortIntD opShortIntD(const Test::Operations::ShortIntD&, const Test::Operations::ShortIntD&,
                                        Test::Operations::ShortIntD&,
                                        const Ice::Current&);
    
    virtual Test::Operations::LongFloatD opLongFloatD(const Test::Operations::LongFloatD&, const Test::Operations::LongFloatD&,
                                          Test::Operations::LongFloatD&,
                                          const Ice::Current&);
    
    virtual Test::Operations::StringStringD opStringStringD(const Test::Operations::StringStringD&, const Test::Operations::StringStringD&,
                                                Test::Operations::StringStringD&,
                                                const Ice::Current&);
    
    virtual Test::Operations::StringMyEnumD opStringMyEnumD(const Test::Operations::StringMyEnumD&, const Test::Operations::StringMyEnumD&,
                                                Test::Operations::StringMyEnumD&,
                                                const Ice::Current&);
    
    virtual Test::Operations::MyEnumStringD opMyEnumStringD(const Test::Operations::MyEnumStringD&, const Test::Operations::MyEnumStringD&,
                                                Test::Operations::MyEnumStringD&,
                                                const Ice::Current&);
    
    virtual Test::Operations::MyStructMyEnumD opMyStructMyEnumD(const Test::Operations::MyStructMyEnumD&, const Test::Operations::MyStructMyEnumD&,
                                                    Test::Operations::MyStructMyEnumD&,
                                                    const Ice::Current&);
    
    virtual Test::Operations::IntS opIntS(const Test::Operations::IntS&, const Ice::Current&);
    
    virtual void opByteSOneway(const Test::Operations::ByteS&, const Ice::Current&);
    
    virtual Ice::Context opContext(const Ice::Current&);
    
    virtual void opDoubleMarshaling(Ice::Double, const Test::Operations::DoubleS&, const Ice::Current&);
    
    virtual void opIdempotent(const Ice::Current&);
    
    virtual void opNonmutating(const Ice::Current&);
    
    virtual void opDerived(const Ice::Current&);
};

}
    
}

#endif
