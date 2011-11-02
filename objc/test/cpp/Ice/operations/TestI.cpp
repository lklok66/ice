// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <operations/TestI.h>
#include <TestCommon.h>
#include <functional>
#include <iterator>

using namespace Test::Operations;

bool
MyDerivedClassI::ice_isA(const std::string& id, const Ice::Current& current) const
{
    test(current.mode == Ice::Nonmutating);
    return Test::Operations::MyDerivedClass::ice_isA(id, current);
}

void
MyDerivedClassI::ice_ping(const Ice::Current& current) const
{
    test(current.mode == Ice::Nonmutating);
    Test::Operations::MyDerivedClass::ice_ping(current);
}

std::vector<std::string>
MyDerivedClassI::ice_ids(const Ice::Current& current) const
{
    test(current.mode == Ice::Nonmutating);
    return Test::Operations::MyDerivedClass::ice_ids(current);
}

const std::string&
MyDerivedClassI::ice_id(const Ice::Current& current) const
{
    test(current.mode == Ice::Nonmutating);
    return Test::Operations::MyDerivedClass::ice_id(current);
}

void
MyDerivedClassI::shutdown(const Ice::Current& current)
{
    current.adapter->getCommunicator()->shutdown();
}

void
MyDerivedClassI::delay(Ice::Int ms, const Ice::Current& current)
{
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(ms));
}

void
MyDerivedClassI::opVoid(const Ice::Current& current)
{
    test(current.mode == Ice::Normal);
}

Ice::Byte
MyDerivedClassI::opByte(Ice::Byte p1,
                        Ice::Byte p2,
                        Ice::Byte& p3,
                        const Ice::Current&)
{
    p3 = p1 ^ p2;
    return p1;
}

bool
MyDerivedClassI::opBool(bool p1,
                        bool p2,
                        bool& p3,
                        const Ice::Current&)
{
    p3 = p1;
    return p2;
}

Ice::Long
MyDerivedClassI::opShortIntLong(Ice::Short p1,
                                Ice::Int p2,
                                Ice::Long p3,
                                Ice::Short& p4,
                                Ice::Int& p5,
                                Ice::Long& p6,
                                const Ice::Current&)
{
    p4 = p1;
    p5 = p2;
    p6 = p3;
    return p3;
}

Ice::Double
MyDerivedClassI::opFloatDouble(Ice::Float p1,
                               Ice::Double p2,
                               Ice::Float& p3,
                               Ice::Double& p4,
                               const Ice::Current&)
{
    p3 = p1;
    p4 = p2;
    return p2;
}

std::string
MyDerivedClassI::opString(const std::string& p1,
                          const std::string& p2,
                          std::string& p3,
                          const Ice::Current&)
{
    p3 = p2 + " " + p1;
    return p1 + " " + p2;
}

Test::Operations::MyEnum
MyDerivedClassI::opMyEnum(Test::Operations::MyEnum p1,
                          Test::Operations::MyEnum& p2,
                          const Ice::Current&)
{
    p2 = p1;
    return Test::Operations::enum3;
}

Test::Operations::MyClassPrx
MyDerivedClassI::opMyClass(const Test::Operations::MyClassPrx& p1,
                           Test::Operations::MyClassPrx& p2,
                           Test::Operations::MyClassPrx& p3,
                           const Ice::Current& current)
{
    p2 = p1;
    p3 = Test::Operations::MyClassPrx::uncheckedCast(current.adapter->createProxy(
                                                                      current.adapter->getCommunicator()->stringToIdentity("noSuchIdentity")));
    return Test::Operations::MyClassPrx::uncheckedCast(current.adapter->createProxy(current.id));
}

Test::Operations::Structure
MyDerivedClassI::opStruct(const Test::Operations::Structure& p1,
                          const ::Test::Operations::Structure& p2,
                          ::Test::Operations::Structure& p3,
                          const Ice::Current&)
{
    p3 = p1;
    p3.s.s = "a new string";
    return p2;
}

Test::Operations::ByteS
MyDerivedClassI::opByteS(const Test::Operations::ByteS& p1,
                         const Test::Operations::ByteS& p2,
                         Test::Operations::ByteS& p3,
                         const Ice::Current&)
{
    p3.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), p3.begin());
    Test::Operations::ByteS r = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(r));
    return r;
}

Test::Operations::BoolS
MyDerivedClassI::opBoolS(const Test::Operations::BoolS& p1,
                         const Test::Operations::BoolS& p2,
                         Test::Operations::BoolS& p3,
                         const Ice::Current&)
{
    p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::Operations::BoolS r;
    r.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), r.begin());
    return r;
}

Test::Operations::LongS
MyDerivedClassI::opShortIntLongS(const Test::Operations::ShortS& p1,
                                 const Test::Operations::IntS& p2,
                                 const Test::Operations::LongS& p3,
                                 Test::Operations::ShortS& p4,
                                 Test::Operations::IntS& p5,
                                 Test::Operations::LongS& p6,
                                 const Ice::Current&)
{
    p4 = p1;
    p5.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p5.begin());
    p6 = p3;
    std::copy(p3.begin(), p3.end(), std::back_inserter(p6));
    return p3;
}

Test::Operations::DoubleS
MyDerivedClassI::opFloatDoubleS(const Test::Operations::FloatS& p1,
                                const Test::Operations::DoubleS& p2,
                                Test::Operations::FloatS& p3,
                                Test::Operations::DoubleS& p4,
                                const Ice::Current&)
{
    p3 = p1;
    p4.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p4.begin());
    Test::Operations::DoubleS r = p2;
    std::copy(p1.begin(), p1.end(), std::back_inserter(r));
    return r;
}

Test::Operations::StringS
MyDerivedClassI::opStringS(const Test::Operations::StringS& p1,
                           const Test::Operations::StringS& p2,
                           Test::Operations::StringS& p3,
                           const Ice::Current&)
{
    p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::Operations::StringS r;
    r.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), r.begin());
    return r;
}

Test::Operations::ByteSS
MyDerivedClassI::opByteSS(const Test::Operations::ByteSS& p1,
                          const Test::Operations::ByteSS& p2,
                          Test::Operations::ByteSS& p3,
                          const Ice::Current&)
{
    p3.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), p3.begin());
    Test::Operations::ByteSS r = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(r));
    return r;
}

Test::Operations::BoolSS
MyDerivedClassI::opBoolSS(const Test::Operations::BoolSS& p1,
                          const Test::Operations::BoolSS& p2,
                          Test::Operations::BoolSS& p3,
                          const Ice::Current&)
{
    p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::Operations::BoolSS r;
    r.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), r.begin());
    return r;
}

Test::Operations::LongSS
MyDerivedClassI::opShortIntLongSS(const Test::Operations::ShortSS& p1,
                                  const Test::Operations::IntSS& p2,
                                  const Test::Operations::LongSS& p3,
                                  Test::Operations::ShortSS& p4,
                                  Test::Operations::IntSS& p5,
                                  Test::Operations::LongSS& p6,
                                  const Ice::Current&)
{
    p4 = p1;
    p5.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p5.begin());
    p6 = p3;
    std::copy(p3.begin(), p3.end(), std::back_inserter(p6));
    return p3;
}

Test::Operations::DoubleSS
MyDerivedClassI::opFloatDoubleSS(const Test::Operations::FloatSS& p1,
                                 const Test::Operations::DoubleSS& p2,
                                 Test::Operations::FloatSS& p3,
                                 Test::Operations::DoubleSS& p4,
                                 const Ice::Current&)
{
    p3 = p1;
    p4.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p4.begin());
    Test::Operations::DoubleSS r = p2;
    std::copy(p2.begin(), p2.end(), std::back_inserter(r));
    return r;
}

Test::Operations::StringSS
MyDerivedClassI::opStringSS(const Test::Operations::StringSS& p1,
                            const Test::Operations::StringSS& p2,
                            Test::Operations::StringSS& p3,
                            const Ice::Current&)
{
    p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::Operations::StringSS r;
    r.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), r.begin());
    return r;
}

Test::Operations::StringSSS
MyDerivedClassI::opStringSSS(const Test::Operations::StringSSS& p1,
                             const Test::Operations::StringSSS& p2,
                             Test::Operations::StringSSS& p3,
                             const ::Ice::Current&)
{
    p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::Operations::StringSSS r;
    r.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), r.begin());
    return r;
}

Test::Operations::ByteBoolD
MyDerivedClassI::opByteBoolD(const Test::Operations::ByteBoolD& p1,
                             const Test::Operations::ByteBoolD& p2,
                             Test::Operations::ByteBoolD& p3,
                             const Ice::Current&)
{
    p3 = p1;
    Test::Operations::ByteBoolD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    return r;
}

Test::Operations::ShortIntD
MyDerivedClassI::opShortIntD(const Test::Operations::ShortIntD& p1,
                             const Test::Operations::ShortIntD& p2,
                             Test::Operations::ShortIntD& p3,
                             const Ice::Current&)
{
    p3 = p1;
    Test::Operations::ShortIntD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    return r;
}

Test::Operations::LongFloatD
MyDerivedClassI::opLongFloatD(const Test::Operations::LongFloatD& p1,
                              const Test::Operations::LongFloatD& p2,
                              Test::Operations::LongFloatD& p3,
                              const Ice::Current&)
{
    p3 = p1;
    Test::Operations::LongFloatD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    return r;
}

Test::Operations::StringStringD
MyDerivedClassI::opStringStringD(const Test::Operations::StringStringD& p1,
                                 const Test::Operations::StringStringD& p2,
                                 Test::Operations::StringStringD& p3,
                                 const Ice::Current&)
{
    p3 = p1;
    Test::Operations::StringStringD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    return r;
}

Test::Operations::StringMyEnumD
MyDerivedClassI::opStringMyEnumD(const Test::Operations::StringMyEnumD& p1,
                                 const Test::Operations::StringMyEnumD& p2,
                                 Test::Operations::StringMyEnumD& p3,
                                 const Ice::Current&)
{
    p3 = p1;
    Test::Operations::StringMyEnumD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    return r;
}

Test::Operations::MyEnumStringD
MyDerivedClassI::opMyEnumStringD(const Test::Operations::MyEnumStringD& p1,
                                 const Test::Operations::MyEnumStringD& p2,
                                 Test::Operations::MyEnumStringD& p3,
                                 const Ice::Current&)
{
    p3 = p1;
    Test::Operations::MyEnumStringD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    return r;
}

Test::Operations::MyStructMyEnumD
MyDerivedClassI::opMyStructMyEnumD(const Test::Operations::MyStructMyEnumD& p1,
                                   const Test::Operations::MyStructMyEnumD& p2,
                                   Test::Operations::MyStructMyEnumD& p3,
                                   const Ice::Current&)
{
    p3 = p1;
    Test::Operations::MyStructMyEnumD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    return r;
}

Test::Operations::IntS
MyDerivedClassI::opIntS(const Test::Operations::IntS& s, const Ice::Current&)
{
    Test::Operations::IntS r;
    std::transform(s.begin(), s.end(), std::back_inserter(r), std::negate<int>());
    return r;
}

void
MyDerivedClassI::opByteSOneway(const Test::Operations::ByteS&, const Ice::Current&)
{
}

Test::Operations::StringStringD
MyDerivedClassI::opContext(const Ice::Current& c)
{
    return c.ctx;
}

void 
MyDerivedClassI::opDoubleMarshaling(Ice::Double p1, const Test::Operations::DoubleS& p2, const Ice::Current&)
{
    Ice::Double d = 1278312346.0 / 13.0;
    test(p1 == d);
    for(unsigned int i = 0; i < p2.size(); ++i)
    {
        test(p2[i] == d);
    }
}

void
MyDerivedClassI::opIdempotent(const Ice::Current& current)
{
    test(current.mode == Ice::Idempotent);
}

void
MyDerivedClassI::opNonmutating(const Ice::Current& current)
{
    test(current.mode == Ice::Nonmutating);
}

void
MyDerivedClassI::opDerived(const Ice::Current&)
{
}
