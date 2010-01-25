// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <Test.h>

//
// We disable deprecation warning here, to allow clean compilation of
// test cases that uses the old stream api.  Once the old stream API
// is gone this could be removed.
//
#ifdef _MSC_VER
#   pragma warning( disable : 4996 )
#endif

using namespace std;

#if defined(_MSC_VER) && (_MSC_VER < 1300) // COMPILERBUG
//
// VC++ 6 compiler bugs doesn't allow using templates for the Stream API.
//
// see: http://support.microsoft.com/kb/240866
//      http://support.microsoft.com/kb/241569
//
#else
class TestObjectWriter : public Ice::ObjectWriter
{
public:

    TestObjectWriter(const Test::MyClassPtr& p)
    {
        obj = p;
        called = false;
    }

    virtual void
    write(const Ice::OutputStreamPtr& out) const
    {
        obj->__write(out);
        const_cast<TestObjectWriter*>(this)->called = true;
    }

    Test::MyClassPtr obj;
    bool called;
};
typedef IceUtil::Handle<TestObjectWriter> TestObjectWriterPtr;

class TestObjectReader : public Ice::ObjectReader
{
public:

    TestObjectReader()
    {
        called = false;
    }

    virtual void
    read(const Ice::InputStreamPtr& in, bool rid)
    {
        obj = new Test::MyClass;
        obj->__read(in, rid);
        called = true;
    }

    Test::MyClassPtr obj;
    bool called;
};
typedef IceUtil::Handle<TestObjectReader> TestObjectReaderPtr;

class TestObjectFactory : public Ice::ObjectFactory
{
public:

    virtual Ice::ObjectPtr
    create(const string& type)
    {
        assert(type == Test::MyClass::ice_staticId());
        return new TestObjectReader;
    }

    virtual void
    destroy()
    {
    }
};

class TestReadObjectCallback : public Ice::ReadObjectCallback
{
public:

    virtual void
    invoke(const Ice::ObjectPtr& p)
    {
        obj = p;
    }

    Ice::ObjectPtr obj;
};
typedef IceUtil::Handle<TestReadObjectCallback> TestReadObjectCallbackPtr;

class MyClassFactoryWrapper : public Ice::ObjectFactory
{
public:

    MyClassFactoryWrapper() : _factory(Test::MyClass::ice_factory())
    {
    }

    virtual Ice::ObjectPtr
    create(const string& type)
    {
        return _factory->create(type);
    }

    virtual void
    destroy()
    {
    }

    void
    setFactory(const Ice::ObjectFactoryPtr& factory)
    {
        if(!factory)
        {
            _factory = Test::MyClass::ice_factory();
        }
        else
        {
            _factory = factory;
        }
    }

private:

    Ice::ObjectFactoryPtr _factory;
};
typedef IceUtil::Handle<MyClassFactoryWrapper> MyClassFactoryWrapperPtr;

class MyInterfaceFactory : public Ice::ObjectFactory
{
public:

    virtual Ice::ObjectPtr
    create(const string& type)
    {
        return new Test::MyInterface;
    }

    virtual void
    destroy()
    {
    }
};
#endif

int
run(int argc, char** argv, const Ice::CommunicatorPtr& communicator)
{
#if defined(_MSC_VER) && (_MSC_VER < 1300)
//
// VC++ 6 compiler bugs doesn't allow to write 
// the Stream API using c++ templates.
//
// see: http://support.microsoft.com/kb/240866
//      http://support.microsoft.com/kb/241569
//
#else
    MyClassFactoryWrapperPtr factoryWrapper = new MyClassFactoryWrapper;
    communicator->addObjectFactory(factoryWrapper, Test::MyClass::ice_staticId());
    communicator->addObjectFactory(new MyInterfaceFactory, Test::MyInterface::ice_staticId());
#endif
	
    Ice::InputStreamPtr in;
    Ice::OutputStreamPtr out;
    vector<Ice::Byte> data;

#if defined(_MSC_VER) && (_MSC_VER < 1300)
//
// VC++ 6 compiler bugs doesn't allow to write 
// the Stream API using c++ templates.
//
// see: http://support.microsoft.com/kb/240866
//      http://support.microsoft.com/kb/241569
//
#else
    //
    // Test the new stream api.
    //
    cout << "testing primitive types... " << flush;

    {
        vector<Ice::Byte> byte;
        in = Ice::createInputStream(communicator, byte);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->startEncapsulation();
        out->write(true);
        out->endEncapsulation();
        out->finished(data);
        out = 0;

        in = Ice::createInputStream(communicator, data);
        in->startEncapsulation();
        bool v;
        in->read(v);
        test(v);
        in->endEncapsulation();
    }

    {
        vector<Ice::Byte> byte;
        in = Ice::createInputStream(communicator, byte);
        try
        {
            bool v;
            in->read(v);
            test(false);
        }
        catch(const Ice::UnmarshalOutOfBoundsException&)
        {
        }
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write(true);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        bool v;
        in->read(v);
        test(v);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write((Ice::Byte)1);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Ice::Byte v;
        in->read(v);
        test(v == 1);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write((Ice::Short)2);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Ice::Short v;
        in->read(v);
        test(v == 2);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write((Ice::Int)3);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Ice::Int v;
        in->read(v);
        test(v == 3);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write((Ice::Long)4);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Ice::Long v;
        in->read(v);
        test(v == 4);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write((Ice::Float)5.0);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Ice::Float v;
        in->read(v);
        test(v == 5.0);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write((Ice::Double)6.0);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Ice::Double v;
        in->read(v);
        test(v == 6.0);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->write("hello world");
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        string v;
        in->read(v);
        test(v == "hello world");
    }

    cout << "ok" << endl;

    cout << "testing constructed types... " << flush;

    {
        out = Ice::createOutputStream(communicator);
        out->write(Test::enum3);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::MyEnum e;
        in->read(e);
        test(e == Test::enum3);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::SmallStruct s;
        s.bo = true;
        s.by = 1;
        s.sh = 2;
        s.i = 3;
        s.l = 4;
        s.f = 5.0;
        s.d = 6.0;
        s.str = "7";
        s.e = Test::enum2;
        s.p = Test::MyClassPrx::uncheckedCast(communicator->stringToProxy("test:default"));
        out->write(s);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::SmallStruct s2;
        in->read(s2);
        test(s2 == s);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::ClassStructPtr s = new Test::ClassStruct();
        s->i = 10;
        out->write(s);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ClassStructPtr s2 = new Test::ClassStruct();
        in->read(s2);
        test(s2->i == s->i);
    }

    {
        Test::BoolS arr;
        arr.push_back(true);
        arr.push_back(false);
        arr.push_back(true);
        arr.push_back(false);

        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::BoolS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::BoolSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::BoolS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::BoolSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::ByteS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);

        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ByteS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::ByteSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::ByteS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::ByteSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::ShortS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ShortS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::ShortSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::ShortS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::ShortSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::IntS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::IntS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::IntSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::IntS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::IntSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::LongS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::LongS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::LongSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::LongS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::LongSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::FloatS arr;
        arr.push_back(1);
        arr.push_back(2);
        arr.push_back(3);
        arr.push_back(4);
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::FloatS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::FloatSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::FloatS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::FloatSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::DoubleS arr;
        arr.push_back(1);
        arr.push_back(2);
        arr.push_back(3);
        arr.push_back(4);
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::DoubleS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::DoubleSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::DoubleS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::DoubleSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::StringS arr;
        arr.push_back("string1");
        arr.push_back("string2");
        arr.push_back("string3");
        arr.push_back("string4");
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::StringS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::StringSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::StringS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::StringSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::MyEnumS arr;
        arr.push_back(Test::enum3);
        arr.push_back(Test::enum2);
        arr.push_back(Test::enum1);
        arr.push_back(Test::enum2);

        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::MyEnumS arr2;
        in->read(arr2);
        test(arr2 == arr);

        Test::MyEnumSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::MyEnumS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::MyEnumSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::SmallStructS arr;
        for(int i = 0; i < 4; ++i)
        {
            Test::SmallStruct s;
            s.bo = true;
            s.by = 1;
            s.sh = 2;
            s.i = 3;
            s.l = 4;
            s.f = 5.0;
            s.d = 6.0;
            s.str = "7";
            s.e = Test::enum2;
            s.p = Test::MyClassPrx::uncheckedCast(communicator->stringToProxy("test:default"));
            arr.push_back(s);
        }
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->writePendingObjects();
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::SmallStructS arr2;
        in->read(arr2);
        in->readPendingObjects();
        test(arr2.size() == arr.size());
        for(Test::SmallStructS::size_type j = 0; j < arr2.size(); ++j)
        {
            test(arr[j] == arr2[j]);
        }

        Test::SmallStructSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::SmallStructS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::SmallStructSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::MyClassS arr;
        for(int i = 0; i < 4; ++i)
        {
            Test::MyClassPtr c = new Test::MyClass;
            c->c = c;
            c->o = c;
            c->s.e = Test::enum2;

            c->seq1.push_back(true);
            c->seq1.push_back(false);
            c->seq1.push_back(true);
            c->seq1.push_back(false);

            c->seq2.push_back(1);
            c->seq2.push_back(2);
            c->seq2.push_back(3);
            c->seq2.push_back(4);

            c->seq3.push_back(1);
            c->seq3.push_back(2);
            c->seq3.push_back(3);
            c->seq3.push_back(4);

            c->seq4.push_back(1);
            c->seq4.push_back(2);
            c->seq4.push_back(3);
            c->seq4.push_back(4);

            c->seq5.push_back(1);
            c->seq5.push_back(2);
            c->seq5.push_back(3);
            c->seq5.push_back(4);

            c->seq6.push_back(1);
            c->seq6.push_back(2);
            c->seq6.push_back(3);
            c->seq6.push_back(4);

            c->seq7.push_back(1);
            c->seq7.push_back(2);
            c->seq7.push_back(3);
            c->seq7.push_back(4);

            c->seq8.push_back("string1");
            c->seq8.push_back("string2");
            c->seq8.push_back("string3");
            c->seq8.push_back("string4");

            c->seq9.push_back(Test::enum3);
            c->seq9.push_back(Test::enum2);
            c->seq9.push_back(Test::enum1);

            c->d["hi"] = c;
        }
        out = Ice::createOutputStream(communicator);
        out->write(arr);
        out->writePendingObjects();
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::MyClassS arr2;
        in->read(arr2);
        in->readPendingObjects();
        test(arr2.size() == arr.size());
        for(Test::MyClassS::size_type j = 0; j < arr2.size(); ++j)
        {
            test(arr2[j]);
            test(arr2[j]->c == arr2[j]);
            test(arr2[j]->o == arr2[j]);
            test(arr2[j]->s.e == Test::enum2);
            test(arr2[j]->seq1 == arr[j]->seq1);
            test(arr2[j]->seq2 == arr[j]->seq2);
            test(arr2[j]->seq3 == arr[j]->seq3);
            test(arr2[j]->seq4 == arr[j]->seq4);
            test(arr2[j]->seq5 == arr[j]->seq5);
            test(arr2[j]->seq6 == arr[j]->seq6);
            test(arr2[j]->seq7 == arr[j]->seq7);
            test(arr2[j]->seq8 == arr[j]->seq8);
            test(arr2[j]->seq9 == arr[j]->seq9);
            test(arr2[j]->d["hi"] == arr2[j]);
        }

        Test::MyClassSS arrS;
        arrS.push_back(arr);
        arrS.push_back(Test::MyClassS());
        arrS.push_back(arr);

        out = Ice::createOutputStream(communicator);
        out->write(arrS);
        out->finished(data);
        
        in = Ice::createInputStream(communicator, data);
        Test::MyClassSS arr2S;
        in->read(arr2S);
        test(arr2S == arrS);
    }

    {
        Test::MyInterfacePtr i = new Test::MyInterface();
        out = Ice::createOutputStream(communicator);
        out->write(i);
        out->writePendingObjects();
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        i = 0;
        in->read(i);
        in->readPendingObjects();
        test(i);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::MyClassPtr obj = new Test::MyClass;
        obj->s.e = Test::enum2;
        TestObjectWriterPtr writer = new TestObjectWriter(obj);
        out->writeObject(writer);
        out->writePendingObjects();
        out->finished(data);
        test(writer->called);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::MyClassPtr obj = new Test::MyClass;
        obj->s.e = Test::enum2;
        TestObjectWriterPtr writer = new TestObjectWriter(obj);
        out->writeObject(writer);
        out->writePendingObjects();
        out->finished(data);
        test(writer->called);
        factoryWrapper->setFactory(new TestObjectFactory);
        in = Ice::createInputStream(communicator, data);
        TestReadObjectCallbackPtr cb = new TestReadObjectCallback;
        in->readObject(cb);
        in->readPendingObjects();
        test(cb->obj);
        TestObjectReaderPtr reader = TestObjectReaderPtr::dynamicCast(cb->obj);
        test(reader);
        test(reader->called);
        test(reader->obj);
        test(reader->obj->s.e == Test::enum2);
        factoryWrapper->setFactory(0);
    }
    
    {
        out = Ice::createOutputStream(communicator);
        Test::MyException ex;
        Test::MyClassPtr c = new Test::MyClass;
        c->c = c;
        c->o = c;
        c->s.e = Test::enum2;

        c->seq1.push_back(true);
        c->seq1.push_back(false);
        c->seq1.push_back(true);
        c->seq1.push_back(false);

        c->seq2.push_back(1);
        c->seq2.push_back(2);
        c->seq2.push_back(3);
        c->seq2.push_back(4);

        c->seq3.push_back(1);
        c->seq3.push_back(2);
        c->seq3.push_back(3);
        c->seq3.push_back(4);

        c->seq4.push_back(1);
        c->seq4.push_back(2);
        c->seq4.push_back(3);
        c->seq4.push_back(4);

        c->seq5.push_back(1);
        c->seq5.push_back(2);
        c->seq5.push_back(3);
        c->seq5.push_back(4);

        c->seq6.push_back(1);
        c->seq6.push_back(2);
        c->seq6.push_back(3);
        c->seq6.push_back(4);

        c->seq7.push_back(1);
        c->seq7.push_back(2);
        c->seq7.push_back(3);
        c->seq7.push_back(4);

        c->seq8.push_back("string1");
        c->seq8.push_back("string2");
        c->seq8.push_back("string3");
        c->seq8.push_back("string4");

        c->seq9.push_back(Test::enum3);
        c->seq9.push_back(Test::enum2);
        c->seq9.push_back(Test::enum1);

        ex.c = c;
        
        out->write(ex);
        out->finished(data);
 
        in = Ice::createInputStream(communicator, data);
        try
        {
            in->throwException();
            test(false);
        }
        catch(const Test::MyException& ex1)
        {
            test(ex1.c->s.e == c->s.e);
            test(ex1.c->seq1 == c->seq1);
            test(ex1.c->seq2 == c->seq2);
            test(ex1.c->seq3 == c->seq3);
            test(ex1.c->seq4 == c->seq4);
            test(ex1.c->seq5 == c->seq5);
            test(ex1.c->seq6 == c->seq6);
            test(ex1.c->seq7 == c->seq7);
            test(ex1.c->seq8 == c->seq8);
            test(ex1.c->seq9 == c->seq9);
        }
    }

    {
        Test::ByteBoolD dict;
        dict[0x04] = true;
        dict[0x01] = false;
        out = Ice::createOutputStream(communicator);
        out->write(dict);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ByteBoolD dict2;
        in->read(dict2);
        test(dict2 == dict);
    }

    {
        Test::ShortIntD dict;
        dict[1] = 9;
        dict[4] = 8;
        out = Ice::createOutputStream(communicator);
        out->write(dict);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ShortIntD dict2;
        in->read(dict2);
        test(dict2 == dict);
    }

    {
        Test::LongFloatD dict;
        dict[123809828] = 0.51f;
        dict[123809829] = 0.56f;
        out = Ice::createOutputStream(communicator);
        out->write(dict);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::LongFloatD dict2;
        in->read(dict2);
        test(dict2 == dict);
    }

    {
        Test::StringStringD dict;
        dict["key1"] = "value1";
        dict["key2"] = "value2";
        out = Ice::createOutputStream(communicator);
        out->write(dict);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::StringStringD dict2;
        in->read(dict2);
        test(dict2 == dict);
    }

    {
        Test::StringMyClassD dict;
        dict["key1"] = new Test::MyClass;
        dict["key1"]->s.e = Test::enum2;
        dict["key2"] = new Test::MyClass;
        dict["key2"]->s.e = Test::enum3;
        out = Ice::createOutputStream(communicator);
        out->write(dict);
        out->writePendingObjects();
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::StringMyClassD dict2;
        in->read(dict2);
        in->readPendingObjects();
        test(dict2.size() == dict.size());
        test(dict2["key1"] && (dict2["key1"]->s.e == Test::enum2));
        test(dict2["key2"] && (dict2["key2"]->s.e == Test::enum3));
    }

    cout << "ok" << endl;

#endif

    //
    // Test the old stream api.
    //

    cout << "testing primitive types... " << flush;

    {
        vector<Ice::Byte> byte;
        in = Ice::createInputStream(communicator, byte);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->startEncapsulation();
        out->writeBool(true);
        out->endEncapsulation();
        out->finished(data);
        out = 0;

        in = Ice::createInputStream(communicator, data);
        in->startEncapsulation();
        test(in->readBool());
        in->endEncapsulation();
    }

    {
        vector<Ice::Byte> byte;
        in = Ice::createInputStream(communicator, byte);
        try
        {
            in->readBool();
            test(false);
        }
        catch(const Ice::UnmarshalOutOfBoundsException&)
        {
        }
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeBool(true);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readBool());
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeByte(1);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readByte() == 1);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeShort(2);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readShort() == 2);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeInt(3);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readInt() == 3);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeLong(4);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readLong() == 4);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeFloat(5.0);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readFloat() == 5.0);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeDouble(6.0);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readDouble() == 6.0);
    }

    {
        out = Ice::createOutputStream(communicator);
        out->writeString("hello world");
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        test(in->readString() == "hello world");
    }

    cout << "ok" << endl;
#if defined(_MSC_VER) && (_MSC_VER < 1300)
//
// VC++ 6 compiler bugs doesn't allow to write 
// the Stream API using c++ templates.
//
// see: http://support.microsoft.com/kb/240866
//      http://support.microsoft.com/kb/241569
//
#else
    cout << "testing constructed types... " << flush;

    {
        out = Ice::createOutputStream(communicator);
        Test::ice_writeMyEnum(out, Test::enum3);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::MyEnum e;
        Test::ice_readMyEnum(in, e);
        test(e == Test::enum3);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::SmallStruct s;
        s.bo = true;
        s.by = 1;
        s.sh = 2;
        s.i = 3;
        s.l = 4;
        s.f = 5.0;
        s.d = 6.0;
        s.str = "7";
        s.e = Test::enum2;
        s.p = Test::MyClassPrx::uncheckedCast(communicator->stringToProxy("test:default"));
        Test::ice_writeSmallStruct(out, s);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::SmallStruct s2;
        Test::ice_readSmallStruct(in, s2);
        test(s2 == s);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::ClassStructPtr s = new Test::ClassStruct();
        s->i = 10;
        Test::ice_writeClassStruct(out, s);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ClassStructPtr s2 = new Test::ClassStruct();
        Test::ice_readClassStruct(in, s2);
        test(s2->i == s->i);
    }
#endif
    {
        Test::BoolS arr;
        arr.push_back(true);
        arr.push_back(false);
        arr.push_back(true);
        arr.push_back(false);

        out = Ice::createOutputStream(communicator);
        out->writeBoolSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::BoolS arr2 = in->readBoolSeq();
        test(arr2 == arr);
    }

    {
        Test::ByteS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);

        out = Ice::createOutputStream(communicator);
        out->writeByteSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ByteS arr2 = in->readByteSeq();
        test(arr2 == arr);
    }

    {
        Test::ShortS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);
        out = Ice::createOutputStream(communicator);
        out->writeShortSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::ShortS arr2 = in->readShortSeq();
        test(arr2 == arr);
    }

    {
        Test::IntS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);
        out = Ice::createOutputStream(communicator);
        out->writeIntSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::IntS arr2 = in->readIntSeq();
        test(arr2 == arr);
    }

    {
        Test::LongS arr;
        arr.push_back(0x01);
        arr.push_back(0x11);
        arr.push_back(0x12);
        arr.push_back(0x22);
        out = Ice::createOutputStream(communicator);
        out->writeLongSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::LongS arr2 = in->readLongSeq();
        test(arr2 == arr);
    }

    {
        Test::FloatS arr;
        arr.push_back(1);
        arr.push_back(2);
        arr.push_back(3);
        arr.push_back(4);
        out = Ice::createOutputStream(communicator);
        out->writeFloatSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::FloatS arr2 = in->readFloatSeq();
        test(arr2 == arr);
    }

    {
        Test::DoubleS arr;
        arr.push_back(1);
        arr.push_back(2);
        arr.push_back(3);
        arr.push_back(4);
        out = Ice::createOutputStream(communicator);
        out->writeDoubleSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::DoubleS arr2 = in->readDoubleSeq();
        test(arr2 == arr);
    }

    {
        Test::StringS arr;
        arr.push_back("string1");
        arr.push_back("string2");
        arr.push_back("string3");
        arr.push_back("string4");
        out = Ice::createOutputStream(communicator);
        out->writeStringSeq(arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::StringS arr2 = in->readStringSeq();
        test(arr2 == arr);
    }
#if defined(_MSC_VER) && (_MSC_VER < 1300)
//
// VC++ 6 compiler bugs doesn't allow to write 
// the Stream API using c++ templates.
//
// see: http://support.microsoft.com/kb/240866
//      http://support.microsoft.com/kb/241569
//
#else
    {
        Test::MyEnumS arr;
        arr.push_back(Test::enum3);
        arr.push_back(Test::enum2);
        arr.push_back(Test::enum1);
        arr.push_back(Test::enum2);

        out = Ice::createOutputStream(communicator);
        Test::ice_writeMyEnumS(out, arr);
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::MyEnumS arr2;
        Test::ice_readMyEnumS(in, arr2);
        test(arr2 == arr);
    }

    {
        Test::MyClassS arr;
        for(int i = 0; i < 4; ++i)
        {
            Test::MyClassPtr c = new Test::MyClass;
            c->c = c;
            c->o = c;
            c->s.e = Test::enum2;

            c->seq1.push_back(true);
            c->seq1.push_back(false);
            c->seq1.push_back(true);
            c->seq1.push_back(false);

            c->seq2.push_back(1);
            c->seq2.push_back(2);
            c->seq2.push_back(3);
            c->seq2.push_back(4);

            c->seq3.push_back(1);
            c->seq3.push_back(2);
            c->seq3.push_back(3);
            c->seq3.push_back(4);

            c->seq4.push_back(1);
            c->seq4.push_back(2);
            c->seq4.push_back(3);
            c->seq4.push_back(4);

            c->seq5.push_back(1);
            c->seq5.push_back(2);
            c->seq5.push_back(3);
            c->seq5.push_back(4);

            c->seq6.push_back(1);
            c->seq6.push_back(2);
            c->seq6.push_back(3);
            c->seq6.push_back(4);

            c->seq7.push_back(1);
            c->seq7.push_back(2);
            c->seq7.push_back(3);
            c->seq7.push_back(4);

            c->seq8.push_back("string1");
            c->seq8.push_back("string2");
            c->seq8.push_back("string3");
            c->seq8.push_back("string4");

            c->seq9.push_back(Test::enum3);
            c->seq9.push_back(Test::enum2);
            c->seq9.push_back(Test::enum1);

            c->d["hi"] = c;
        }
        out = Ice::createOutputStream(communicator);
        Test::ice_writeMyClassS(out, arr);
        out->writePendingObjects();
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        Test::MyClassS arr2;
        Test::ice_readMyClassS(in, arr2);
        in->readPendingObjects();
        test(arr2.size() == arr.size());
        for(Test::MyClassS::size_type j = 0; j < arr2.size(); ++j)
        {
            test(arr2[j]);
            test(arr2[j]->c == arr2[j]);
            test(arr2[j]->o == arr2[j]);
            test(arr2[j]->s.e == Test::enum2);
            test(arr2[j]->seq1 == arr[j]->seq1);
            test(arr2[j]->seq2 == arr[j]->seq2);
            test(arr2[j]->seq3 == arr[j]->seq3);
            test(arr2[j]->seq4 == arr[j]->seq4);
            test(arr2[j]->seq5 == arr[j]->seq5);
            test(arr2[j]->seq6 == arr[j]->seq6);
            test(arr2[j]->seq7 == arr[j]->seq7);
            test(arr2[j]->seq8 == arr[j]->seq8);
            test(arr2[j]->seq9 == arr[j]->seq9);
            test(arr2[j]->d["hi"] == arr2[j]);
        }
    }

    {
        Test::MyInterfacePtr i = new Test::MyInterface();
        out = Ice::createOutputStream(communicator);
        Test::ice_writeMyInterface(out, i);
        out->writePendingObjects();
        out->finished(data);
        in = Ice::createInputStream(communicator, data);
        i = 0;
        Test::ice_readMyInterface(in, i);
        in->readPendingObjects();
        test(i);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::MyClassPtr obj = new Test::MyClass;
        obj->s.e = Test::enum2;
        TestObjectWriterPtr writer = new TestObjectWriter(obj);
        out->writeObject(writer);
        out->writePendingObjects();
        out->finished(data);
        test(writer->called);
    }

    {
        out = Ice::createOutputStream(communicator);
        Test::MyClassPtr obj = new Test::MyClass;
        obj->s.e = Test::enum2;
        TestObjectWriterPtr writer = new TestObjectWriter(obj);
        out->writeObject(writer);
        out->writePendingObjects();
        out->finished(data);
        test(writer->called);
        factoryWrapper->setFactory(new TestObjectFactory);
        in = Ice::createInputStream(communicator, data);
        TestReadObjectCallbackPtr cb = new TestReadObjectCallback;
        in->readObject(cb);
        in->readPendingObjects();
        test(cb->obj);
        TestObjectReaderPtr reader = TestObjectReaderPtr::dynamicCast(cb->obj);
        test(reader);
        test(reader->called);
        test(reader->obj);
        test(reader->obj->s.e == Test::enum2);
        factoryWrapper->setFactory(0);
    }
    cout << "ok" << endl;
#endif
    return 0;
}

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;

    try
    {
        communicator = Ice::initialize(argc, argv);
        status = run(argc, argv, communicator);
    }
    catch(const Ice::Exception& ex)
    {
        cerr << ex << endl;
        status = EXIT_FAILURE;
    }

    if(communicator)
    {
        try
        {
            communicator->destroy();
        }
        catch(const Ice::Exception& ex)
        {
            cerr << ex << endl;
            status = EXIT_FAILURE;
        }
    }

    return status;
}
