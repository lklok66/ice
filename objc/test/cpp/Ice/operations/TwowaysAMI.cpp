// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <OperationsTest.h>
#include <limits>

using namespace std;

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
    
    class AMI_MyClass_opVoidI : public Test::Operations::AMI_MyClass_opVoid, public CallbackBase
    {
    public:
        
        virtual void ice_response()
        {
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opVoidI> AMI_MyClass_opVoidIPtr;
    
    class AMI_MyClass_opVoidExI : public Test::Operations::AMI_MyClass_opVoid, public CallbackBase
    {
    public:
        
        virtual void ice_response()
        {
            test(false);
        }
        
        virtual void ice_exception(const ::Ice::Exception& ex)
        {
            test(dynamic_cast<const ::Ice::NoEndpointException*>(&ex));
            called();
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opVoidExI> AMI_MyClass_opVoidExIPtr;
    
    class AMI_MyClass_opByteI : public Test::Operations::AMI_MyClass_opByte, public CallbackBase
    {
    public:
        
        virtual void ice_response(::Ice::Byte r, ::Ice::Byte b)
        {
            test(b == Ice::Byte(0xf0));
            test(r == Ice::Byte(0xff));
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opByteI> AMI_MyClass_opByteIPtr;
    
    class AMI_MyClass_opByteExI : public Test::Operations::AMI_MyClass_opByte, public CallbackBase
    {
    public:
        
        virtual void ice_response(::Ice::Byte r, ::Ice::Byte b)
        {
            test(false);
        }
        
        virtual void ice_exception(const ::Ice::Exception& ex)
        {
            test(dynamic_cast<const ::Ice::NoEndpointException*>(&ex));
            called();
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opByteExI> AMI_MyClass_opByteExIPtr;
    
    
    class AMI_MyClass_opBoolI : public Test::Operations::AMI_MyClass_opBool, public CallbackBase
    {
    public:
        
        virtual void ice_response(bool r, bool b)
        {
            test(b);
            test(!r);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opBoolI> AMI_MyClass_opBoolIPtr;
    
    class AMI_MyClass_opShortIntLongI : public Test::Operations::AMI_MyClass_opShortIntLong, public CallbackBase
    {
    public:
        
        virtual void ice_response(::Ice::Long r, ::Ice::Short s, ::Ice::Int i, ::Ice::Long l)
        {
            test(s == 10);
            test(i == 11);
            test(l == 12);
            test(r == 12);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opShortIntLongI> AMI_MyClass_opShortIntLongIPtr;
    
    class AMI_MyClass_opFloatDoubleI : public Test::Operations::AMI_MyClass_opFloatDouble, public CallbackBase
    {
    public:
        
        virtual void ice_response(::Ice::Double r, ::Ice::Float f, ::Ice::Double d)
        {
            test(f == Ice::Float(3.14));
            test(d == Ice::Double(1.1E10));
            test(r == Ice::Double(1.1E10));
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opFloatDoubleI> AMI_MyClass_opFloatDoubleIPtr;
    
    class AMI_MyClass_opStringI : public Test::Operations::AMI_MyClass_opString, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::std::string& r, const ::std::string& s)
        {
            test(s == "world hello");
            test(r == "hello world");
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opStringI> AMI_MyClass_opStringIPtr;
    
    class AMI_MyClass_opMyEnumI : public Test::Operations::AMI_MyClass_opMyEnum, public CallbackBase
    {
    public:
        
        virtual void ice_response(::Test::Operations::MyEnum r, ::Test::Operations::MyEnum e)
        {
            test(e == Test::Operations::enum2);
            test(r == Test::Operations::enum3);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opMyEnumI> AMI_MyClass_opMyEnumIPtr;
    
    class AMI_MyClass_opMyClassI : public Test::Operations::AMI_MyClass_opMyClass, public CallbackBase
    {
    public:
        
        AMI_MyClass_opMyClassI(const Ice::CommunicatorPtr& communicator) :
        _communicator(communicator)
        {
        }
        
        virtual void ice_response(const ::Test::Operations::MyClassPrx& r, const ::Test::Operations::MyClassPrx& c1, const ::Test::Operations::MyClassPrx& c2)
        {
            test(c1->ice_getIdentity() == _communicator->stringToIdentity("test"));
            test(c2->ice_getIdentity() == _communicator->stringToIdentity("noSuchIdentity"));
            test(r->ice_getIdentity() == _communicator->stringToIdentity("test"));
            // We can't do the callbacks below in connection serialization mode.
            if(_communicator->getProperties()->getPropertyAsInt("Ice.ThreadPool.Client.Serialize"))
            {
                r->opVoid();
                c1->opVoid();
                try
                {
                    c2->opVoid();
                    test(false);
                }
                catch(const Ice::ObjectNotExistException&)
                {
                }
            }
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
        
    private:
        
        Ice::CommunicatorPtr _communicator;
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opMyClassI> AMI_MyClass_opMyClassIPtr;
    
    class AMI_MyClass_opStructI : public Test::Operations::AMI_MyClass_opStruct, public CallbackBase
    {
    public:
        
        AMI_MyClass_opStructI(const Ice::CommunicatorPtr& communicator) :
        _communicator(communicator)
        {
        }
        
        virtual void ice_response(const ::Test::Operations::Structure& rso, const ::Test::Operations::Structure& so)
        {
            test(rso.p == 0);
            test(rso.e == Test::Operations::enum2);
            test(rso.s.s == "def");
            test(so.e == Test::Operations::enum3);
            test(so.s.s == "a new string");
            // We can't do the callbacks below in connection serialization mode.
            if(_communicator->getProperties()->getPropertyAsInt("Ice.ThreadPool.Client.Serialize"))
            {
                so.p->opVoid();
            }
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
        
    private:
        
        Ice::CommunicatorPtr _communicator;
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opStructI> AMI_MyClass_opStructIPtr;
    
    class AMI_MyClass_opByteSI : public Test::Operations::AMI_MyClass_opByteS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::ByteS& rso, const ::Test::Operations::ByteS& bso)
        {
            test(bso.size() == 4);
            test(bso[0] == Ice::Byte(0x22));
            test(bso[1] == Ice::Byte(0x12));
            test(bso[2] == Ice::Byte(0x11));
            test(bso[3] == Ice::Byte(0x01));
            test(rso.size() == 8);
            test(rso[0] == Ice::Byte(0x01));
            test(rso[1] == Ice::Byte(0x11));
            test(rso[2] == Ice::Byte(0x12));
            test(rso[3] == Ice::Byte(0x22));
            test(rso[4] == Ice::Byte(0xf1));
            test(rso[5] == Ice::Byte(0xf2));
            test(rso[6] == Ice::Byte(0xf3));
            test(rso[7] == Ice::Byte(0xf4));
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opByteSI> AMI_MyClass_opByteSIPtr;
    
    class AMI_MyClass_opBoolSI : public Test::Operations::AMI_MyClass_opBoolS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::BoolS& rso, const ::Test::Operations::BoolS& bso)
        {
            test(bso.size() == 4);
            test(bso[0]);
            test(bso[1]);
            test(!bso[2]);
            test(!bso[3]);
            test(rso.size() == 3);
            test(!rso[0]);
            test(rso[1]);
            test(rso[2]);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opBoolSI> AMI_MyClass_opBoolSIPtr;
    
    class AMI_MyClass_opShortIntLongSI : public Test::Operations::AMI_MyClass_opShortIntLongS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::LongS& rso, const ::Test::Operations::ShortS& sso, const ::Test::Operations::IntS& iso,
                                  const ::Test::Operations::LongS& lso)
        {
            test(sso.size() == 3);
            test(sso[0] == 1);
            test(sso[1] == 2);
            test(sso[2] == 3);
            test(iso.size() == 4);
            test(iso[0] == 8);
            test(iso[1] == 7);
            test(iso[2] == 6);
            test(iso[3] == 5);
            test(lso.size() == 6);
            test(lso[0] == 10);
            test(lso[1] == 30);
            test(lso[2] == 20);
            test(lso[3] == 10);
            test(lso[4] == 30);
            test(lso[5] == 20);
            test(rso.size() == 3);
            test(rso[0] == 10);
            test(rso[1] == 30);
            test(rso[2] == 20);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opShortIntLongSI> AMI_MyClass_opShortIntLongSIPtr;
    
    class AMI_MyClass_opFloatDoubleSI : public Test::Operations::AMI_MyClass_opFloatDoubleS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::DoubleS& rso, const ::Test::Operations::FloatS& fso, const ::Test::Operations::DoubleS& dso)
        {
            test(fso.size() == 2);
            test(fso[0] == ::Ice::Float(3.14));
            test(fso[1] == ::Ice::Float(1.11));
            test(dso.size() == 3);
            test(dso[0] == ::Ice::Double(1.3E10));
            test(dso[1] == ::Ice::Double(1.2E10));
            test(dso[2] == ::Ice::Double(1.1E10));
            test(rso.size() == 5);
            test(rso[0] == ::Ice::Double(1.1E10));
            test(rso[1] == ::Ice::Double(1.2E10));
            test(rso[2] == ::Ice::Double(1.3E10));
            test(::Ice::Float(rso[3]) == ::Ice::Float(3.14));
            test(::Ice::Float(rso[4]) == ::Ice::Float(1.11));
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opFloatDoubleSI> AMI_MyClass_opFloatDoubleSIPtr;
    
    class AMI_MyClass_opStringSI : public Test::Operations::AMI_MyClass_opStringS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::StringS& rso, const ::Test::Operations::StringS& sso)
        {
            test(sso.size() == 4);
            test(sso[0] == "abc");
            test(sso[1] == "de");
            test(sso[2] == "fghi");
            test(sso[3] == "xyz");
            test(rso.size() == 3);
            test(rso[0] == "fghi");
            test(rso[1] == "de");
            test(rso[2] == "abc");
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opStringSI> AMI_MyClass_opStringSIPtr;
    
    class AMI_MyClass_opByteSSI : public Test::Operations::AMI_MyClass_opByteSS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::ByteSS& rso, const ::Test::Operations::ByteSS& bso)
        {
            test(bso.size() == 2);
            test(bso[0].size() == 1);
            test(bso[0][0] == Ice::Byte(0xff));
            test(bso[1].size() == 3);
            test(bso[1][0] == Ice::Byte(0x01));
            test(bso[1][1] == Ice::Byte(0x11));
            test(bso[1][2] == Ice::Byte(0x12));
            test(rso.size() == 4);
            test(rso[0].size() == 3);
            test(rso[0][0] == Ice::Byte(0x01));
            test(rso[0][1] == Ice::Byte(0x11));
            test(rso[0][2] == Ice::Byte(0x12));
            test(rso[1].size() == 1);
            test(rso[1][0] == Ice::Byte(0xff));
            test(rso[2].size() == 1);
            test(rso[2][0] == Ice::Byte(0x0e));
            test(rso[3].size() == 2);
            test(rso[3][0] == Ice::Byte(0xf2));
            test(rso[3][1] == Ice::Byte(0xf1));
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opByteSSI> AMI_MyClass_opByteSSIPtr;
    
    class AMI_MyClass_opBoolSSI : public Test::Operations::AMI_MyClass_opBoolSS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::BoolSS&, const ::Test::Operations::BoolSS& bso)
        {
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opBoolSSI> AMI_MyClass_opBoolSSIPtr;
    
    class AMI_MyClass_opShortIntLongSSI : public Test::Operations::AMI_MyClass_opShortIntLongSS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::LongSS&, const ::Test::Operations::ShortSS&, const ::Test::Operations::IntSS&,
                                  const ::Test::Operations::LongSS&)
        {
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opShortIntLongSSI> AMI_MyClass_opShortIntLongSSIPtr;
    
    class AMI_MyClass_opFloatDoubleSSI : public Test::Operations::AMI_MyClass_opFloatDoubleSS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::DoubleSS& rso, const ::Test::Operations::FloatSS& fso, const ::Test::Operations::DoubleSS& dso)
        {
            test(fso.size() == 3);
            test(fso[0].size() == 1);
            test(fso[0][0] == ::Ice::Float(3.14));
            test(fso[1].size() == 1);
            test(fso[1][0] == ::Ice::Float(1.11));
            test(fso[2].size() == 0);
            test(dso.size() == 1);
            test(dso[0].size() == 3);
            test(dso[0][0] == ::Ice::Double(1.1E10));
            test(dso[0][1] == ::Ice::Double(1.2E10));
            test(dso[0][2] == ::Ice::Double(1.3E10));
            test(rso.size() == 2);
            test(rso[0].size() == 3);
            test(rso[0][0] == ::Ice::Double(1.1E10));
            test(rso[0][1] == ::Ice::Double(1.2E10));
            test(rso[0][2] == ::Ice::Double(1.3E10));
            test(rso[1].size() == 3);
            test(rso[1][0] == ::Ice::Double(1.1E10));
            test(rso[1][1] == ::Ice::Double(1.2E10));
            test(rso[1][2] == ::Ice::Double(1.3E10));
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opFloatDoubleSSI> AMI_MyClass_opFloatDoubleSSIPtr;
    
    class AMI_MyClass_opStringSSI : public Test::Operations::AMI_MyClass_opStringSS, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::StringSS& rso, const ::Test::Operations::StringSS& sso)
        {
            test(sso.size() == 5);
            test(sso[0].size() == 1);
            test(sso[0][0] == "abc");
            test(sso[1].size() == 2);
            test(sso[1][0] == "de");
            test(sso[1][1] == "fghi");
            test(sso[2].size() == 0);
            test(sso[3].size() == 0);
            test(sso[4].size() == 1);
            test(sso[4][0] == "xyz");
            test(rso.size() == 3);
            test(rso[0].size() == 1);
            test(rso[0][0] == "xyz");
            test(rso[1].size() == 0);
            test(rso[2].size() == 0);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opStringSSI> AMI_MyClass_opStringSSIPtr;
    
    class AMI_MyClass_opByteBoolDI : public Test::Operations::AMI_MyClass_opByteBoolD, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::ByteBoolD& ro, const ::Test::Operations::ByteBoolD& _do)
        {
            Test::Operations::ByteBoolD di1;
            di1[10] = true;
            di1[100] = false;
            test(_do == di1);
            test(ro.size() == 4);
            test(ro.find(10) != ro.end());
            test(ro.find(10)->second == true);
            test(ro.find(11) != ro.end());
            test(ro.find(11)->second == false);
            test(ro.find(100) != ro.end());
            test(ro.find(100)->second == false);
            test(ro.find(101) != ro.end());
            test(ro.find(101)->second == true);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opByteBoolDI> AMI_MyClass_opByteBoolDIPtr;
    
    class AMI_MyClass_opShortIntDI : public Test::Operations::AMI_MyClass_opShortIntD, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::ShortIntD& ro, const ::Test::Operations::ShortIntD& _do)
        {
            Test::Operations::ShortIntD di1;
            di1[110] = -1;
            di1[1100] = 123123;
            test(_do == di1);
            test(ro.size() == 4);
            test(ro.find(110) != ro.end());
            test(ro.find(110)->second == -1);
            test(ro.find(111) != ro.end());
            test(ro.find(111)->second == -100);
            test(ro.find(1100) != ro.end());
            test(ro.find(1100)->second == 123123);
            test(ro.find(1101) != ro.end());
            test(ro.find(1101)->second == 0);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opShortIntDI> AMI_MyClass_opShortIntDIPtr;
    
    class AMI_MyClass_opLongFloatDI : public Test::Operations::AMI_MyClass_opLongFloatD, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::LongFloatD& ro, const ::Test::Operations::LongFloatD& _do)
        {
            Test::Operations::LongFloatD di1;
            di1[999999110] = Ice::Float(-1.1);
            di1[999999111] = Ice::Float(123123.2);
            test(_do == di1);
            test(ro.size() == 4);
            test(ro.find(999999110) != ro.end());
            test(ro.find(999999110)->second == Ice::Float(-1.1));
            test(ro.find(999999120) != ro.end());
            test(ro.find(999999120)->second == Ice::Float(-100.4));
            test(ro.find(999999111) != ro.end());
            test(ro.find(999999111)->second == Ice::Float(123123.2));
            test(ro.find(999999130) != ro.end());
            test(ro.find(999999130)->second == Ice::Float(0.5));
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opLongFloatDI> AMI_MyClass_opLongFloatDIPtr;
    
    class AMI_MyClass_opStringStringDI : public Test::Operations::AMI_MyClass_opStringStringD, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::StringStringD& ro, const ::Test::Operations::StringStringD& _do)
        {
            Test::Operations::StringStringD di1;
            di1["foo"] = "abc -1.1";
            di1["bar"] = "abc 123123.2";
            test(_do == di1);
            test(ro.size() == 4);
            test(ro.find("foo") != ro.end());
            test(ro.find("foo")->second == "abc -1.1");
            test(ro.find("FOO") != ro.end());
            test(ro.find("FOO")->second == "abc -100.4");
            test(ro.find("bar") != ro.end());
            test(ro.find("bar")->second == "abc 123123.2");
            test(ro.find("BAR") != ro.end());
            test(ro.find("BAR")->second == "abc 0.5");
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opStringStringDI> AMI_MyClass_opStringStringDIPtr;
    
    class AMI_MyClass_opStringMyEnumDI : public Test::Operations::AMI_MyClass_opStringMyEnumD, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::StringMyEnumD& ro, const ::Test::Operations::StringMyEnumD& _do)
        {
            Test::Operations::StringMyEnumD di1;
            di1["abc"] = Test::Operations::enum1;
            di1[""] = Test::Operations::enum2;
            test(_do == di1);
            test(ro.size() == 4);
            test(ro.find("abc") != ro.end());
            test(ro.find("abc")->second == Test::Operations::enum1);
            test(ro.find("qwerty") != ro.end());
            test(ro.find("qwerty")->second == Test::Operations::enum3);
            test(ro.find("") != ro.end());
            test(ro.find("")->second == Test::Operations::enum2);
            test(ro.find("Hello!!") != ro.end());
            test(ro.find("Hello!!")->second == Test::Operations::enum2);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opStringMyEnumDI> AMI_MyClass_opStringMyEnumDIPtr;
    
    class AMI_MyClass_opMyEnumStringDI : public Test::Operations::AMI_MyClass_opMyEnumStringD, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::MyEnumStringD& ro, const ::Test::Operations::MyEnumStringD& _do)
        {
            Test::Operations::MyEnumStringD di1;
            di1[Test::Operations::enum1] = "abc";
            test(_do == di1);
            test(ro.size() == 3);
            test(ro.find(Test::Operations::enum1) != ro.end());
            test(ro.find(Test::Operations::enum1)->second == "abc");
            test(ro.find(Test::Operations::enum2) != ro.end());
            test(ro.find(Test::Operations::enum2)->second == "Hello!!");
            test(ro.find(Test::Operations::enum3) != ro.end());
            test(ro.find(Test::Operations::enum3)->second == "qwerty");
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opMyEnumStringDI> AMI_MyClass_opMyEnumStringDIPtr;
    
    class AMI_MyClass_opMyStructMyEnumDI : public Test::Operations::AMI_MyClass_opMyStructMyEnumD, public CallbackBase
    {
    public:
        
        virtual void ice_response(const ::Test::Operations::MyStructMyEnumD& ro, const ::Test::Operations::MyStructMyEnumD& _do)
        {
            Test::Operations::MyStruct s11 = { 1, 1 };
            Test::Operations::MyStruct s12 = { 1, 2 };
            Test::Operations::MyStructMyEnumD di1;
            di1[s11] = Test::Operations::enum1;
            di1[s12] = Test::Operations::enum2;
            test(_do == di1);
            Test::Operations::MyStruct s22 = { 2, 2 };
            Test::Operations::MyStruct s23 = { 2, 3 };
            test(ro.size() == 4);
            test(ro.find(s11) != ro.end());
            test(ro.find(s11)->second == Test::Operations::enum1);
            test(ro.find(s12) != ro.end());
            test(ro.find(s12)->second == Test::Operations::enum2);
            test(ro.find(s22) != ro.end());
            test(ro.find(s22)->second == Test::Operations::enum3);
            test(ro.find(s23) != ro.end());
            test(ro.find(s23)->second == Test::Operations::enum2);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opMyStructMyEnumDI> AMI_MyClass_opMyStructMyEnumDIPtr;
    
    class AMI_MyClass_opIntSI : public Test::Operations::AMI_MyClass_opIntS, public CallbackBase
    {
    public:
        
        AMI_MyClass_opIntSI(int l) : _l(l) {}
        
        virtual void ice_response(const Test::Operations::IntS& r)
        {
            test(r.size() == static_cast<size_t>(_l));
            for(int j = 0; j < _l; ++j)
            {
                test(r[j] == -j);
            }
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
        
    private:
        
        int _l;
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opIntSI> AMI_MyClass_opIntSIPtr;
    
    class AMI_MyClass_opContextEqualI : public Test::Operations::AMI_MyClass_opContext, public CallbackBase
    {
    public:
        
        AMI_MyClass_opContextEqualI(const Test::Operations::StringStringD &d) : _d(d)
        {
        }
        
        virtual void ice_response(const Test::Operations::StringStringD& r)
        {
            test(r == _d);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
        
    private:
        
        Test::Operations::StringStringD _d;
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opContextEqualI> AMI_MyClass_opContextEqualIPtr;
    
    class AMI_MyClass_opContextNotEqualI : public Test::Operations::AMI_MyClass_opContext, public CallbackBase
    {
    public:
        
        AMI_MyClass_opContextNotEqualI(const Test::Operations::StringStringD &d) : _d(d)
        {
        }
        
        virtual void ice_response(const Test::Operations::StringStringD& r)
        {
            test(r != _d);
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
        
    private:
        
        Test::Operations::StringStringD _d;
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opContextNotEqualI> AMI_MyClass_opContextNotEqualIPtr;
    
    class AMI_MyDerivedClass_opDerivedI : public Test::Operations::AMI_MyDerivedClass_opDerived, public CallbackBase
    {
    public:
        
        virtual void ice_response()
        {
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyDerivedClass_opDerivedI> AMI_MyDerivedClass_opDerivedIPtr;
    
    class AMI_MyClass_opDoubleMarshalingI : public Test::Operations::AMI_MyClass_opDoubleMarshaling, public CallbackBase
    {
    public:
        
        virtual void ice_response()
        {
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception& ex)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opDoubleMarshalingI> AMI_MyClass_opDoubleMarshalingIPtr;
    
    class AMI_MyClass_opIdempotentI : public Test::Operations::AMI_MyClass_opIdempotent, public CallbackBase
    {
    public:
        
        virtual void ice_response()
        {
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opIdempotentI> AMI_MyClass_opIdempotentIPtr;
    
    class AMI_MyClass_opNonmutatingI : public Test::Operations::AMI_MyClass_opNonmutating, public CallbackBase
    {
    public:
        
        virtual void ice_response()
        {
            called();
        }
        
        virtual void ice_exception(const ::Ice::Exception&)
        {
            test(false);
        }
    };
    
    typedef IceUtil::Handle<AMI_MyClass_opNonmutatingI> AMI_MyClass_opNonmutatingIPtr;
    
}

void
twowaysAMI(const Ice::CommunicatorPtr& communicator, const Test::Operations::MyClassPrx& p)
{
    {
        // Check that a call to a void operation raises NoEndpointException
        // in the ice_exception() callback instead of at the point of call.
        Test::Operations::MyClassPrx indirect = Test::Operations::MyClassPrx::uncheckedCast(p->ice_adapterId("dummy"));
        AMI_MyClass_opVoidExIPtr cb = new AMI_MyClass_opVoidExI;
        try 
        {
            test(!indirect->opVoid_async(cb));
        }
        catch(const Ice::Exception&)
        {
            test(false);
        }
        cb->check();
    }
    
    {
        //
        // Check that a call to a twoway operation raises NoEndpointException
        // in the ice_exception() callback instead of at the point of call.
        //
        Test::Operations::MyClassPrx indirect = Test::Operations::MyClassPrx::uncheckedCast(p->ice_adapterId("dummy"));
        AMI_MyClass_opByteExIPtr cb = new AMI_MyClass_opByteExI;
        try
        {
            test(!indirect->opByte_async(cb, 0, 0));
        }
        catch(const Ice::Exception&)
        {
            test(false);
        }
        cb->check();
    }
    
    {
        AMI_MyClass_opVoidIPtr cb = new AMI_MyClass_opVoidI;
        p->opVoid_async(cb);
        cb->check();
        // Let's check if we can reuse the same callback object for another call.
        p->opVoid_async(cb);
        cb->check();
    }
    
    {
        //
        // Check that CommunicatorDestroyedException is raised directly.
        //
        Ice::InitializationData initData;
        initData.properties = communicator->getProperties()->clone();
        Ice::CommunicatorPtr ic = Ice::initialize(initData);
        Ice::ObjectPrx obj = ic->stringToProxy(p->ice_toString());
        Test::Operations::MyClassPrx p2 = Test::Operations::MyClassPrx::checkedCast(obj);
        
        ic->destroy();
        
        AMI_MyClass_opVoidIPtr cb = new AMI_MyClass_opVoidI;
        try
        {
            test(!p2->opVoid_async(cb));
            test(false);
        }
        catch(const Ice::CommunicatorDestroyedException&)
        {
            // Expected.
        }
    }
    
    {
        AMI_MyClass_opByteIPtr cb = new AMI_MyClass_opByteI;
        p->opByte_async(cb, Ice::Byte(0xff), Ice::Byte(0x0f));
        cb->check();
    }
    
    {
        AMI_MyClass_opBoolIPtr cb = new AMI_MyClass_opBoolI;
        p->opBool_async(cb, true, false);
        cb->check();
    }
    
    {
        AMI_MyClass_opShortIntLongIPtr cb = new AMI_MyClass_opShortIntLongI;
        p->opShortIntLong_async(cb, 10, 11, 12);
        cb->check();
    }
    
    {
        AMI_MyClass_opFloatDoubleIPtr cb = new AMI_MyClass_opFloatDoubleI;
        p->opFloatDouble_async(cb, Ice::Float(3.14), Ice::Double(1.1E10));
        cb->check();
        // Let's check if we can reuse the same callback object for another call.
        p->opFloatDouble_async(cb, Ice::Float(3.14), Ice::Double(1.1E10));
        cb->check();
    }
    
    {
        AMI_MyClass_opStringIPtr cb = new AMI_MyClass_opStringI;
        p->opString_async(cb, "hello", "world");
        cb->check();
    }
    
    {
        AMI_MyClass_opMyEnumIPtr cb = new AMI_MyClass_opMyEnumI;
        p->opMyEnum_async(cb, Test::Operations::enum2);
        cb->check();
    }
    
    {
        AMI_MyClass_opMyClassIPtr cb = new AMI_MyClass_opMyClassI(communicator);
        p->opMyClass_async(cb, p);
        cb->check();
    }
    
    {
        Test::Operations::Structure si1;
        si1.p = p;
        si1.e = Test::Operations::enum3;
        si1.s.s = "abc";
        Test::Operations::Structure si2;
        si2.p = 0;
        si2.e = Test::Operations::enum2;
        si2.s.s = "def";
        
        AMI_MyClass_opStructIPtr cb = new AMI_MyClass_opStructI(communicator);
        p->opStruct_async(cb, si1, si2);
        cb->check();
    }
    
    {
        Test::Operations::ByteS bsi1;
        Test::Operations::ByteS bsi2;
        
        bsi1.push_back(Ice::Byte(0x01));
        bsi1.push_back(Ice::Byte(0x11));
        bsi1.push_back(Ice::Byte(0x12));
        bsi1.push_back(Ice::Byte(0x22));
        
        bsi2.push_back(Ice::Byte(0xf1));
        bsi2.push_back(Ice::Byte(0xf2));
        bsi2.push_back(Ice::Byte(0xf3));
        bsi2.push_back(Ice::Byte(0xf4));
        
        AMI_MyClass_opByteSIPtr cb = new AMI_MyClass_opByteSI;
        p->opByteS_async(cb, bsi1, bsi2);
        cb->check();
    }
    
    {
        Test::Operations::BoolS bsi1;
        Test::Operations::BoolS bsi2;
        
        bsi1.push_back(true);
        bsi1.push_back(true);
        bsi1.push_back(false);
        
        bsi2.push_back(false);
        
        AMI_MyClass_opBoolSIPtr cb = new AMI_MyClass_opBoolSI;
        p->opBoolS_async(cb, bsi1, bsi2);
        cb->check();
    }
    
    {
        Test::Operations::ShortS ssi;
        Test::Operations::IntS isi;
        Test::Operations::LongS lsi;
        
        ssi.push_back(1);
        ssi.push_back(2);
        ssi.push_back(3);
        
        isi.push_back(5);
        isi.push_back(6);
        isi.push_back(7);
        isi.push_back(8);
        
        lsi.push_back(10);
        lsi.push_back(30);
        lsi.push_back(20);
        
        AMI_MyClass_opShortIntLongSIPtr cb = new AMI_MyClass_opShortIntLongSI;
        p->opShortIntLongS_async(cb, ssi, isi, lsi);
        cb->check();
    }
    
    {
        Test::Operations::FloatS fsi;
        Test::Operations::DoubleS dsi;
        
        fsi.push_back(Ice::Float(3.14));
        fsi.push_back(Ice::Float(1.11));
        
        dsi.push_back(Ice::Double(1.1E10));
        dsi.push_back(Ice::Double(1.2E10));
        dsi.push_back(Ice::Double(1.3E10));
        
        AMI_MyClass_opFloatDoubleSIPtr cb = new AMI_MyClass_opFloatDoubleSI;
        p->opFloatDoubleS_async(cb, fsi, dsi);
        cb->check();
    }
    
    {
        Test::Operations::StringS ssi1;
        Test::Operations::StringS ssi2;
        
        ssi1.push_back("abc");
        ssi1.push_back("de");
        ssi1.push_back("fghi");
        
        ssi2.push_back("xyz");
        
        AMI_MyClass_opStringSIPtr cb = new AMI_MyClass_opStringSI;
        p->opStringS_async(cb, ssi1, ssi2);
        cb->check();
    }
    
    {
        Test::Operations::ByteSS bsi1;
        bsi1.resize(2);
        Test::Operations::ByteSS bsi2;
        bsi2.resize(2);
        
        bsi1[0].push_back(Ice::Byte(0x01));
        bsi1[0].push_back(Ice::Byte(0x11));
        bsi1[0].push_back(Ice::Byte(0x12));
        bsi1[1].push_back(Ice::Byte(0xff));
        
        bsi2[0].push_back(Ice::Byte(0x0e));
        bsi2[1].push_back(Ice::Byte(0xf2));
        bsi2[1].push_back(Ice::Byte(0xf1));
        
        AMI_MyClass_opByteSSIPtr cb = new AMI_MyClass_opByteSSI;
        p->opByteSS_async(cb, bsi1, bsi2);
        cb->check();
    }
    
    {
        Test::Operations::FloatSS fsi;
        fsi.resize(3);
        Test::Operations::DoubleSS dsi;
        dsi.resize(1);
        
        fsi[0].push_back(Ice::Float(3.14));
        fsi[1].push_back(Ice::Float(1.11));
        
        dsi[0].push_back(Ice::Double(1.1E10));
        dsi[0].push_back(Ice::Double(1.2E10));
        dsi[0].push_back(Ice::Double(1.3E10));
        
        AMI_MyClass_opFloatDoubleSSIPtr cb = new AMI_MyClass_opFloatDoubleSSI;
        p->opFloatDoubleSS_async(cb, fsi, dsi);
        cb->check();
    }
    
    {
        Test::Operations::StringSS ssi1;
        ssi1.resize(2);
        Test::Operations::StringSS ssi2;
        ssi2.resize(3);
        
        ssi1[0].push_back("abc");
        ssi1[1].push_back("de");
        ssi1[1].push_back("fghi");
        
        ssi2[2].push_back("xyz");
        
        AMI_MyClass_opStringSSIPtr cb = new AMI_MyClass_opStringSSI;
        p->opStringSS_async(cb, ssi1, ssi2);
        cb->check();
    }
    
    {
        Test::Operations::ByteBoolD di1;
        di1[10] = true;
        di1[100] = false;
        Test::Operations::ByteBoolD di2;
        di2[10] = true;
        di2[11] = false;
        di2[101] = true;
        
        AMI_MyClass_opByteBoolDIPtr cb = new AMI_MyClass_opByteBoolDI;
        p->opByteBoolD_async(cb, di1, di2);
        cb->check();
    }
    
    {
        Test::Operations::ShortIntD di1;
        di1[110] = -1;
        di1[1100] = 123123;
        Test::Operations::ShortIntD di2;
        di2[110] = -1;
        di2[111] = -100;
        di2[1101] = 0;
        
        AMI_MyClass_opShortIntDIPtr cb = new AMI_MyClass_opShortIntDI;
        p->opShortIntD_async(cb, di1, di2);
        cb->check();
    }
    
    {
        Test::Operations::LongFloatD di1;
        di1[999999110] = Ice::Float(-1.1);
        di1[999999111] = Ice::Float(123123.2);
        Test::Operations::LongFloatD di2;
        di2[999999110] = Ice::Float(-1.1);
        di2[999999120] = Ice::Float(-100.4);
        di2[999999130] = Ice::Float(0.5);
        
        AMI_MyClass_opLongFloatDIPtr cb = new AMI_MyClass_opLongFloatDI;
        p->opLongFloatD_async(cb, di1, di2);
        cb->check();
    }
    
    {
        Test::Operations::StringStringD di1;
        di1["foo"] = "abc -1.1";
        di1["bar"] = "abc 123123.2";
        Test::Operations::StringStringD di2;
        di2["foo"] = "abc -1.1";
        di2["FOO"] = "abc -100.4";
        di2["BAR"] = "abc 0.5";
        
        AMI_MyClass_opStringStringDIPtr cb = new AMI_MyClass_opStringStringDI;
        p->opStringStringD_async(cb, di1, di2);
        cb->check();
    }
    
    {
        Test::Operations::StringMyEnumD di1;
        di1["abc"] = Test::Operations::enum1;
        di1[""] = Test::Operations::enum2;
        Test::Operations::StringMyEnumD di2;
        di2["abc"] = Test::Operations::enum1;
        di2["qwerty"] = Test::Operations::enum3;
        di2["Hello!!"] = Test::Operations::enum2;
        
        AMI_MyClass_opStringMyEnumDIPtr cb = new AMI_MyClass_opStringMyEnumDI;
        p->opStringMyEnumD_async(cb, di1, di2);
        cb->check();
    }
    
    {
        Test::Operations::MyEnumStringD di1;
        di1[Test::Operations::enum1] = "abc";
        Test::Operations::MyEnumStringD di2;
        di2[Test::Operations::enum2] = "Hello!!";
        di2[Test::Operations::enum3] = "qwerty";
        
        AMI_MyClass_opMyEnumStringDIPtr cb = new AMI_MyClass_opMyEnumStringDI;
        p->opMyEnumStringD_async(cb, di1, di2);
        cb->check();
    }
    
    {
        Test::Operations::MyStruct s11 = { 1, 1 };
        Test::Operations::MyStruct s12 = { 1, 2 };
        Test::Operations::MyStructMyEnumD di1;
        di1[s11] = Test::Operations::enum1;
        di1[s12] = Test::Operations::enum2;
        
        Test::Operations::MyStruct s22 = { 2, 2 };
        Test::Operations::MyStruct s23 = { 2, 3 };
        Test::Operations::MyStructMyEnumD di2;
        di2[s11] = Test::Operations::enum1;
        di2[s22] = Test::Operations::enum3;
        di2[s23] = Test::Operations::enum2;
        
        AMI_MyClass_opMyStructMyEnumDIPtr cb = new AMI_MyClass_opMyStructMyEnumDI;
        p->opMyStructMyEnumD_async(cb, di1, di2);
        cb->check();
    }
    
    {
        const int lengths[] = { 0, 1, 2, 126, 127, 128, 129, 253, 254, 255, 256, 257, 1000 };
        
        for(unsigned int l = 0; l != sizeof(lengths) / sizeof(*lengths); ++l)
        {
            Test::Operations::IntS s;
            for(int i = 0; i < lengths[l]; ++i)
            {
                s.push_back(i);
            }
            AMI_MyClass_opIntSIPtr cb = new AMI_MyClass_opIntSI(lengths[l]);
            p->opIntS_async(cb, s);
            cb->check();
        }
    }
    
    {
        Test::Operations::StringStringD ctx;
        ctx["one"] = "ONE";
        ctx["two"] = "TWO";
        ctx["three"] = "THREE";
        {
            test(p->ice_getContext().empty());
            AMI_MyClass_opContextNotEqualIPtr cb = new AMI_MyClass_opContextNotEqualI(ctx);
            p->opContext_async(cb);
            cb->check();
        }
        {
            test(p->ice_getContext().empty());
            AMI_MyClass_opContextEqualIPtr cb = new AMI_MyClass_opContextEqualI(ctx);
            p->opContext_async(cb, ctx);
            cb->check();
        }
        Test::Operations::MyClassPrx p2 = Test::Operations::MyClassPrx::checkedCast(p->ice_context(ctx));
        test(p2->ice_getContext() == ctx);
        {
            AMI_MyClass_opContextEqualIPtr cb = new AMI_MyClass_opContextEqualI(ctx);
            p2->opContext_async(cb);
            cb->check();
        }
        {
            Test::Operations::MyClassPrx p2 = Test::Operations::MyClassPrx::checkedCast(p->ice_context(ctx));
            AMI_MyClass_opContextEqualIPtr cb = new AMI_MyClass_opContextEqualI(ctx);
            p2->opContext_async(cb, ctx);
            cb->check();
        }
        
        {
            //
            // Test implicit context propagation
            //
            
            string impls[] = {"Shared", "PerThread"};
            for(int i = 0; i < 2; i++)
            {
                Ice::InitializationData initData;
                initData.properties = communicator->getProperties()->clone();
                initData.properties->setProperty("Ice.ImplicitContext", impls[i]);
                
                Ice::CommunicatorPtr ic = Ice::initialize(initData);
                
                Ice::Context ctx;
                ctx["one"] = "ONE";
                ctx["two"] = "TWO";
                ctx["three"] = "THREE";
                
                
                Test::Operations::MyClassPrx p = Test::Operations::MyClassPrx::uncheckedCast(
                                                                     ic->stringToProxy("test:default -p 12010"));
                
                
                ic->getImplicitContext()->setContext(ctx);
                test(ic->getImplicitContext()->getContext() == ctx);
                {
                    AMI_MyClass_opContextEqualIPtr cb = new AMI_MyClass_opContextEqualI(ctx);
                    p->opContext_async(cb);
                    cb->check();
                }
                
                ic->getImplicitContext()->put("zero", "ZERO");
                
                ctx = ic->getImplicitContext()->getContext();
                {
                    AMI_MyClass_opContextEqualIPtr cb = new AMI_MyClass_opContextEqualI(ctx);
                    p->opContext_async(cb);
                    cb->check();
                }
                
                Ice::Context prxContext;
                prxContext["one"] = "UN";
                prxContext["four"] = "QUATRE";
                
                Ice::Context combined = prxContext;
                combined.insert(ctx.begin(), ctx.end());
                test(combined["one"] == "UN");
                
                p = Test::Operations::MyClassPrx::uncheckedCast(p->ice_context(prxContext));
                
                ic->getImplicitContext()->setContext(Ice::Context());
                {
                    AMI_MyClass_opContextEqualIPtr cb = new AMI_MyClass_opContextEqualI(prxContext);
                    p->opContext_async(cb);
                    cb->check();
                }
                
                ic->getImplicitContext()->setContext(ctx);
                {
                    AMI_MyClass_opContextEqualIPtr cb = new AMI_MyClass_opContextEqualI(combined);
                    p->opContext_async(cb);
                    cb->check();
                }
                
                ic->getImplicitContext()->setContext(Ice::Context());
                ic->destroy();
            }
        }
    }
    
    {
        Ice::Double d = 1278312346.0 / 13.0;
        Test::Operations::DoubleS ds(5, d);
        AMI_MyClass_opDoubleMarshalingIPtr cb = new AMI_MyClass_opDoubleMarshalingI;
        p->opDoubleMarshaling_async(cb, d, ds);
        cb->check();
    }
    
    {
        AMI_MyClass_opIdempotentIPtr cb = new AMI_MyClass_opIdempotentI;
        p->opIdempotent_async(cb);
        cb->check();
    }
    
    {
        AMI_MyClass_opNonmutatingIPtr cb = new AMI_MyClass_opNonmutatingI;
        p->opNonmutating_async(cb);
        cb->check();
    }
    
    {
        Test::Operations::MyDerivedClassPrx derived = Test::Operations::MyDerivedClassPrx::checkedCast(p);
        test(derived);
        AMI_MyDerivedClass_opDerivedIPtr cb = new AMI_MyDerivedClass_opDerivedI;
        derived->opDerived_async(cb);
        cb->check();
    }
}
