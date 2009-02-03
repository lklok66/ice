// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.0
// Generated from file `IceFIX.ice'

#include <IceFIX/IceFIX.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/BasicStream.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/ScopedArray.h>
#include <IceUtil/DisableWarnings.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 303
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 0
#       error Ice patch level mismatch!
#   endif
#endif

static const ::std::string __IceFIX__Executor__execute_name = "execute";

static const ::std::string __IceFIX__Executor__destroy_name = "destroy";

static const ::std::string __IceFIX__Reporter__message_name = "message";

static const ::std::string __IceFIX__BridgeAdmin__activate_name = "activate";

static const ::std::string __IceFIX__BridgeAdmin__deactivate_name = "deactivate";

static const ::std::string __IceFIX__BridgeAdmin__clean_name = "clean";

static const ::std::string __IceFIX__BridgeAdmin__getStatus_name = "getStatus";

static const ::std::string __IceFIX__BridgeAdmin__unregister_name = "unregister";

static const ::std::string __IceFIX__BridgeAdmin__register_name = "register";

static const ::std::string __IceFIX__BridgeAdmin__registerWithId_name = "registerWithId";

static const ::std::string __IceFIX__BridgeAdmin__getClients_name = "getClients";

static const ::std::string __IceFIX__Bridge__connect_name = "connect";

::Ice::Object* IceInternal::upCast(::IceFIX::Executor* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::IceFIX::Executor* p) { return p; }

::Ice::Object* IceInternal::upCast(::IceFIX::Reporter* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::IceFIX::Reporter* p) { return p; }

::Ice::Object* IceInternal::upCast(::IceFIX::BridgeAdmin* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::IceFIX::BridgeAdmin* p) { return p; }

::Ice::Object* IceInternal::upCast(::IceFIX::Bridge* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::IceFIX::Bridge* p) { return p; }

void
IceFIX::__read(::IceInternal::BasicStream* __is, ::IceFIX::ExecutorPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::IceFIX::Executor;
        v->__copyFrom(proxy);
    }
}

void
IceFIX::__read(::IceInternal::BasicStream* __is, ::IceFIX::ReporterPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::IceFIX::Reporter;
        v->__copyFrom(proxy);
    }
}

void
IceFIX::__read(::IceInternal::BasicStream* __is, ::IceFIX::BridgeAdminPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::IceFIX::BridgeAdmin;
        v->__copyFrom(proxy);
    }
}

void
IceFIX::__read(::IceInternal::BasicStream* __is, ::IceFIX::BridgePrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::IceFIX::Bridge;
        v->__copyFrom(proxy);
    }
}

IceFIX::ExecuteException::ExecuteException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    UserException(),
#else
    ::Ice::UserException(),
#endif
    reason(__ice_reason)
{
}

IceFIX::ExecuteException::~ExecuteException() throw()
{
}

static const char* __IceFIX__ExecuteException_name = "IceFIX::ExecuteException";

::std::string
IceFIX::ExecuteException::ice_name() const
{
    return __IceFIX__ExecuteException_name;
}

::Ice::Exception*
IceFIX::ExecuteException::ice_clone() const
{
    return new ExecuteException(*this);
}

void
IceFIX::ExecuteException::ice_throw() const
{
    throw *this;
}

void
IceFIX::ExecuteException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::IceFIX::ExecuteException"), false);
    __os->startWriteSlice();
    __os->write(reason);
    __os->endWriteSlice();
}

void
IceFIX::ExecuteException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->read(reason);
    __is->endReadSlice();
}

void
IceFIX::ExecuteException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception IceFIX::ExecuteException was not generated with stream support";
    throw ex;
}

void
IceFIX::ExecuteException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception IceFIX::ExecuteException was not generated with stream support";
    throw ex;
}

struct __F__IceFIX__ExecuteException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::IceFIX::ExecuteException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__IceFIX__ExecuteException__Ptr = new __F__IceFIX__ExecuteException;

const ::IceInternal::UserExceptionFactoryPtr&
IceFIX::ExecuteException::ice_factory()
{
    return __F__IceFIX__ExecuteException__Ptr;
}

class __F__IceFIX__ExecuteException__Init
{
public:

    __F__IceFIX__ExecuteException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::IceFIX::ExecuteException", ::IceFIX::ExecuteException::ice_factory());
    }

    ~__F__IceFIX__ExecuteException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::IceFIX::ExecuteException");
    }
};

static __F__IceFIX__ExecuteException__Init __F__IceFIX__ExecuteException__i;

#ifdef __APPLE__
extern "C" { void __F__IceFIX__ExecuteException__initializer() {} }
#endif

IceFIX::RegistrationException::RegistrationException(const ::std::string& __ice_reason) :
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    UserException(),
#else
    ::Ice::UserException(),
#endif
    reason(__ice_reason)
{
}

IceFIX::RegistrationException::~RegistrationException() throw()
{
}

static const char* __IceFIX__RegistrationException_name = "IceFIX::RegistrationException";

::std::string
IceFIX::RegistrationException::ice_name() const
{
    return __IceFIX__RegistrationException_name;
}

::Ice::Exception*
IceFIX::RegistrationException::ice_clone() const
{
    return new RegistrationException(*this);
}

void
IceFIX::RegistrationException::ice_throw() const
{
    throw *this;
}

void
IceFIX::RegistrationException::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(::std::string("::IceFIX::RegistrationException"), false);
    __os->startWriteSlice();
    __os->write(reason);
    __os->endWriteSlice();
}

void
IceFIX::RegistrationException::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->read(myId, false);
    }
    __is->startReadSlice();
    __is->read(reason);
    __is->endReadSlice();
}

void
IceFIX::RegistrationException::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception IceFIX::RegistrationException was not generated with stream support";
    throw ex;
}

void
IceFIX::RegistrationException::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "exception IceFIX::RegistrationException was not generated with stream support";
    throw ex;
}

struct __F__IceFIX__RegistrationException : public ::IceInternal::UserExceptionFactory
{
    virtual void
    createAndThrow()
    {
        throw ::IceFIX::RegistrationException();
    }
};

static ::IceInternal::UserExceptionFactoryPtr __F__IceFIX__RegistrationException__Ptr = new __F__IceFIX__RegistrationException;

const ::IceInternal::UserExceptionFactoryPtr&
IceFIX::RegistrationException::ice_factory()
{
    return __F__IceFIX__RegistrationException__Ptr;
}

class __F__IceFIX__RegistrationException__Init
{
public:

    __F__IceFIX__RegistrationException__Init()
    {
        ::IceInternal::factoryTable->addExceptionFactory("::IceFIX::RegistrationException", ::IceFIX::RegistrationException::ice_factory());
    }

    ~__F__IceFIX__RegistrationException__Init()
    {
        ::IceInternal::factoryTable->removeExceptionFactory("::IceFIX::RegistrationException");
    }
};

static __F__IceFIX__RegistrationException__Init __F__IceFIX__RegistrationException__i;

#ifdef __APPLE__
extern "C" { void __F__IceFIX__RegistrationException__initializer() {} }
#endif

bool
IceFIX::ClientInfo::operator==(const ClientInfo& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(id != __rhs.id)
    {
        return false;
    }
    if(isConnected != __rhs.isConnected)
    {
        return false;
    }
    return true;
}

bool
IceFIX::ClientInfo::operator<(const ClientInfo& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(id < __rhs.id)
    {
        return true;
    }
    else if(__rhs.id < id)
    {
        return false;
    }
    if(isConnected < __rhs.isConnected)
    {
        return true;
    }
    else if(__rhs.isConnected < isConnected)
    {
        return false;
    }
    return false;
}

void
IceFIX::ClientInfo::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(id);
    __os->write(isConnected);
}

void
IceFIX::ClientInfo::__read(::IceInternal::BasicStream* __is)
{
    __is->read(id);
    __is->read(isConnected);
}

void
IceFIX::__writeClientInfoSeq(::IceInternal::BasicStream* __os, const ::IceFIX::ClientInfo* begin, const ::IceFIX::ClientInfo* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        begin[i].__write(__os);
    }
}

void
IceFIX::__readClientInfoSeq(::IceInternal::BasicStream* __is, ::IceFIX::ClientInfoSeq& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 2);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        v[i].__read(__is);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

void
IceFIX::__writeQoS(::IceInternal::BasicStream* __os, const ::IceFIX::QoS& v)
{
    __os->writeSize(::Ice::Int(v.size()));
    ::IceFIX::QoS::const_iterator p;
    for(p = v.begin(); p != v.end(); ++p)
    {
        __os->write(p->first);
        __os->write(p->second);
    }
}

void
IceFIX::__readQoS(::IceInternal::BasicStream* __is, ::IceFIX::QoS& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    while(sz--)
    {
        ::std::pair<const  ::std::string, ::std::string> pair;
        __is->read(const_cast< ::std::string&>(pair.first));
        ::IceFIX::QoS::iterator __i = v.insert(v.end(), pair);
        __is->read(__i->second);
    }
}

void
IceFIX::__write(::IceInternal::BasicStream* __os, ::IceFIX::BridgeStatus v)
{
    __os->write(static_cast< ::Ice::Byte>(v), 3);
}

void
IceFIX::__read(::IceInternal::BasicStream* __is, ::IceFIX::BridgeStatus& v)
{
    ::Ice::Byte val;
    __is->read(val, 3);
    v = static_cast< ::IceFIX::BridgeStatus>(val);
}

bool
IceFIX::AMI_Reporter_message::__invoke(const ::IceFIX::ReporterPrx& __prx, const ::std::string& data, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prepare(__prx, __IceFIX__Reporter__message_name, ::Ice::Normal, __ctx);
        __os->write(data);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
IceFIX::AMI_Reporter_message::__response(bool __ok)
{
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->skipEmptyEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response();
    __releaseCallback();
}

void
IceProxy::IceFIX::Executor::execute(const ::std::string& data, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__IceFIX__Executor__execute_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::Executor* __del = dynamic_cast< ::IceDelegate::IceFIX::Executor*>(__delBase.get());
            __del->execute(data, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

void
IceProxy::IceFIX::Executor::destroy(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::Executor* __del = dynamic_cast< ::IceDelegate::IceFIX::Executor*>(__delBase.get());
            __del->destroy(__ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

const ::std::string&
IceProxy::IceFIX::Executor::ice_staticId()
{
    return ::IceFIX::Executor::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::IceFIX::Executor::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::IceFIX::Executor);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::IceFIX::Executor::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::IceFIX::Executor);
}

::IceProxy::Ice::Object*
IceProxy::IceFIX::Executor::__newInstance() const
{
    return new Executor;
}

void
IceProxy::IceFIX::Reporter::message(const ::std::string& data, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::Reporter* __del = dynamic_cast< ::IceDelegate::IceFIX::Reporter*>(__delBase.get());
            __del->message(data, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::IceFIX::Reporter::message_async(const ::IceFIX::AMI_Reporter_messagePtr& __cb, const ::std::string& data)
{
    return __cb->__invoke(this, data, 0);
}

bool
IceProxy::IceFIX::Reporter::message_async(const ::IceFIX::AMI_Reporter_messagePtr& __cb, const ::std::string& data, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, data, &__ctx);
}

const ::std::string&
IceProxy::IceFIX::Reporter::ice_staticId()
{
    return ::IceFIX::Reporter::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::IceFIX::Reporter::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::IceFIX::Reporter);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::IceFIX::Reporter::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::IceFIX::Reporter);
}

::IceProxy::Ice::Object*
IceProxy::IceFIX::Reporter::__newInstance() const
{
    return new Reporter;
}

void
IceProxy::IceFIX::BridgeAdmin::activate(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            __del->activate(__ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

void
IceProxy::IceFIX::BridgeAdmin::deactivate(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            __del->deactivate(__ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

void
IceProxy::IceFIX::BridgeAdmin::clean(::Ice::Long timeout, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            __del->clean(timeout, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

::IceFIX::BridgeStatus
IceProxy::IceFIX::BridgeAdmin::getStatus(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__IceFIX__BridgeAdmin__getStatus_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            return __del->getStatus(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

void
IceProxy::IceFIX::BridgeAdmin::unregister(const ::std::string& id, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__IceFIX__BridgeAdmin__unregister_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            __del->unregister(id, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

::std::string
IceProxy::IceFIX::BridgeAdmin::_cpp_register(const ::IceFIX::QoS& clientQoS, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__IceFIX__BridgeAdmin__register_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            return __del->_cpp_register(clientQoS, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

void
IceProxy::IceFIX::BridgeAdmin::registerWithId(const ::std::string& id, const ::IceFIX::QoS& clientQoS, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__IceFIX__BridgeAdmin__registerWithId_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            __del->registerWithId(id, clientQoS, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

::IceFIX::ClientInfoSeq
IceProxy::IceFIX::BridgeAdmin::getClients(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__IceFIX__BridgeAdmin__getClients_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::BridgeAdmin* __del = dynamic_cast< ::IceDelegate::IceFIX::BridgeAdmin*>(__delBase.get());
            return __del->getClients(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

const ::std::string&
IceProxy::IceFIX::BridgeAdmin::ice_staticId()
{
    return ::IceFIX::BridgeAdmin::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::IceFIX::BridgeAdmin::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::IceFIX::BridgeAdmin);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::IceFIX::BridgeAdmin::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::IceFIX::BridgeAdmin);
}

::IceProxy::Ice::Object*
IceProxy::IceFIX::BridgeAdmin::__newInstance() const
{
    return new BridgeAdmin;
}

void
IceProxy::IceFIX::Bridge::connect(const ::std::string& id, const ::IceFIX::ReporterPrx& cb, ::IceFIX::ExecutorPrx& exec, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__IceFIX__Bridge__connect_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::IceFIX::Bridge* __del = dynamic_cast< ::IceDelegate::IceFIX::Bridge*>(__delBase.get());
            __del->connect(id, cb, exec, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

const ::std::string&
IceProxy::IceFIX::Bridge::ice_staticId()
{
    return ::IceFIX::Bridge::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::IceFIX::Bridge::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::IceFIX::Bridge);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::IceFIX::Bridge::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::IceFIX::Bridge);
}

::IceProxy::Ice::Object*
IceProxy::IceFIX::Bridge::__newInstance() const
{
    return new Bridge;
}

void
IceDelegateM::IceFIX::Executor::execute(const ::std::string& data, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__Executor__execute_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(data);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::IceFIX::ExecuteException&)
            {
                throw;
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::IceFIX::Executor::destroy(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__Executor__destroy_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

void
IceDelegateM::IceFIX::Reporter::message(const ::std::string& data, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__Reporter__message_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(data);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

void
IceDelegateM::IceFIX::BridgeAdmin::activate(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__activate_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

void
IceDelegateM::IceFIX::BridgeAdmin::deactivate(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__deactivate_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

void
IceDelegateM::IceFIX::BridgeAdmin::clean(::Ice::Long timeout, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__clean_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(timeout);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

::IceFIX::BridgeStatus
IceDelegateM::IceFIX::BridgeAdmin::getStatus(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__getStatus_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceFIX::BridgeStatus __ret;
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        ::IceFIX::__read(__is, __ret);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::IceFIX::BridgeAdmin::unregister(const ::std::string& id, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__unregister_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(id);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::IceFIX::RegistrationException&)
            {
                throw;
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::std::string
IceDelegateM::IceFIX::BridgeAdmin::_cpp_register(const ::IceFIX::QoS& clientQoS, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__register_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::IceFIX::__writeQoS(__os, clientQoS);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::IceFIX::RegistrationException&)
            {
                throw;
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::std::string __ret;
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __is->read(__ret);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::IceFIX::BridgeAdmin::registerWithId(const ::std::string& id, const ::IceFIX::QoS& clientQoS, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__registerWithId_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(id);
        ::IceFIX::__writeQoS(__os, clientQoS);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::IceFIX::RegistrationException&)
            {
                throw;
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::IceFIX::ClientInfoSeq
IceDelegateM::IceFIX::BridgeAdmin::getClients(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__BridgeAdmin__getClients_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceFIX::ClientInfoSeq __ret;
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        ::IceFIX::__readClientInfoSeq(__is, __ret);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::IceFIX::Bridge::connect(const ::std::string& id, const ::IceFIX::ReporterPrx& cb, ::IceFIX::ExecutorPrx& exec, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __IceFIX__Bridge__connect_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(id);
        __os->write(::Ice::ObjectPrx(::IceInternal::upCast(cb.get())));
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::IceFIX::RegistrationException&)
            {
                throw;
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        ::IceFIX::__read(__is, exec);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateD::IceFIX::Executor::execute(const ::std::string& data, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::std::string& data, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_data(data)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::Executor* servant = dynamic_cast< ::IceFIX::Executor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            try
            {
                servant->execute(_m_data, _current);
                return ::Ice::DispatchOK;
            }
            catch(const ::Ice::UserException& __ex)
            {
                setUserException(__ex);
                return ::Ice::DispatchUserException;
            }
        }
        
    private:
        
        const ::std::string& _m_data;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__Executor__execute_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(data, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::IceFIX::ExecuteException&)
    {
        throw;
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

void
IceDelegateD::IceFIX::Executor::destroy(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::Executor* servant = dynamic_cast< ::IceFIX::Executor*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->destroy(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__Executor__destroy_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(__current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

void
IceDelegateD::IceFIX::Reporter::message(const ::std::string& data, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::std::string& data, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_data(data)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::Reporter* servant = dynamic_cast< ::IceFIX::Reporter*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->message(_m_data, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        const ::std::string& _m_data;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__Reporter__message_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(data, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

void
IceDelegateD::IceFIX::BridgeAdmin::activate(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->activate(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__activate_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(__current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

void
IceDelegateD::IceFIX::BridgeAdmin::deactivate(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->deactivate(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__deactivate_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(__current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

void
IceDelegateD::IceFIX::BridgeAdmin::clean(::Ice::Long timeout, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::Ice::Long timeout, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_timeout(timeout)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->clean(_m_timeout, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::Ice::Long _m_timeout;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__clean_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(timeout, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

::IceFIX::BridgeStatus
IceDelegateD::IceFIX::BridgeAdmin::getStatus(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::IceFIX::BridgeStatus& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->getStatus(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::IceFIX::BridgeStatus& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__getStatus_name, ::Ice::Normal, __context);
    ::IceFIX::BridgeStatus __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

void
IceDelegateD::IceFIX::BridgeAdmin::unregister(const ::std::string& id, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::std::string& id, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_id(id)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            try
            {
                servant->unregister(_m_id, _current);
                return ::Ice::DispatchOK;
            }
            catch(const ::Ice::UserException& __ex)
            {
                setUserException(__ex);
                return ::Ice::DispatchUserException;
            }
        }
        
    private:
        
        const ::std::string& _m_id;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__unregister_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(id, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::IceFIX::RegistrationException&)
    {
        throw;
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

::std::string
IceDelegateD::IceFIX::BridgeAdmin::_cpp_register(const ::IceFIX::QoS& clientQoS, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::std::string& __result, const ::IceFIX::QoS& clientQoS, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_clientQoS(clientQoS)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            try
            {
                _result = servant->_cpp_register(_m_clientQoS, _current);
                return ::Ice::DispatchOK;
            }
            catch(const ::Ice::UserException& __ex)
            {
                setUserException(__ex);
                return ::Ice::DispatchUserException;
            }
        }
        
    private:
        
        ::std::string& _result;
        const ::IceFIX::QoS& _m_clientQoS;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__register_name, ::Ice::Normal, __context);
    ::std::string __result;
    try
    {
        _DirectI __direct(__result, clientQoS, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::IceFIX::RegistrationException&)
    {
        throw;
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

void
IceDelegateD::IceFIX::BridgeAdmin::registerWithId(const ::std::string& id, const ::IceFIX::QoS& clientQoS, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::std::string& id, const ::IceFIX::QoS& clientQoS, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_id(id),
            _m_clientQoS(clientQoS)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            try
            {
                servant->registerWithId(_m_id, _m_clientQoS, _current);
                return ::Ice::DispatchOK;
            }
            catch(const ::Ice::UserException& __ex)
            {
                setUserException(__ex);
                return ::Ice::DispatchUserException;
            }
        }
        
    private:
        
        const ::std::string& _m_id;
        const ::IceFIX::QoS& _m_clientQoS;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__registerWithId_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(id, clientQoS, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::IceFIX::RegistrationException&)
    {
        throw;
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

::IceFIX::ClientInfoSeq
IceDelegateD::IceFIX::BridgeAdmin::getClients(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::IceFIX::ClientInfoSeq& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::BridgeAdmin* servant = dynamic_cast< ::IceFIX::BridgeAdmin*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->getClients(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::IceFIX::ClientInfoSeq& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__BridgeAdmin__getClients_name, ::Ice::Normal, __context);
    ::IceFIX::ClientInfoSeq __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

void
IceDelegateD::IceFIX::Bridge::connect(const ::std::string& id, const ::IceFIX::ReporterPrx& cb, ::IceFIX::ExecutorPrx& exec, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::std::string& id, const ::IceFIX::ReporterPrx& cb, ::IceFIX::ExecutorPrx& exec, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _m_id(id),
            _m_cb(cb),
            _m_exec(exec)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::IceFIX::Bridge* servant = dynamic_cast< ::IceFIX::Bridge*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            try
            {
                servant->connect(_m_id, _m_cb, _m_exec, _current);
                return ::Ice::DispatchOK;
            }
            catch(const ::Ice::UserException& __ex)
            {
                setUserException(__ex);
                return ::Ice::DispatchUserException;
            }
        }
        
    private:
        
        const ::std::string& _m_id;
        const ::IceFIX::ReporterPrx& _m_cb;
        ::IceFIX::ExecutorPrx& _m_exec;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __IceFIX__Bridge__connect_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(id, cb, exec, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::IceFIX::RegistrationException&)
    {
        throw;
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

::Ice::ObjectPtr
IceFIX::Executor::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __IceFIX__Executor_ids[2] =
{
    "::Ice::Object",
    "::IceFIX::Executor"
};

bool
IceFIX::Executor::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__IceFIX__Executor_ids, __IceFIX__Executor_ids + 2, _s);
}

::std::vector< ::std::string>
IceFIX::Executor::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__IceFIX__Executor_ids[0], &__IceFIX__Executor_ids[2]);
}

const ::std::string&
IceFIX::Executor::ice_id(const ::Ice::Current&) const
{
    return __IceFIX__Executor_ids[1];
}

const ::std::string&
IceFIX::Executor::ice_staticId()
{
    return __IceFIX__Executor_ids[1];
}

::Ice::DispatchStatus
IceFIX::Executor::___execute(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string data;
    __is->read(data);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    try
    {
        execute(data, __current);
    }
    catch(const ::IceFIX::ExecuteException& __ex)
    {
        __os->write(__ex);
        return ::Ice::DispatchUserException;
    }
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::Executor::___destroy(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    destroy(__current);
    return ::Ice::DispatchOK;
}

static ::std::string __IceFIX__Executor_all[] =
{
    "destroy",
    "execute",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};

::Ice::DispatchStatus
IceFIX::Executor::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__IceFIX__Executor_all, __IceFIX__Executor_all + 6, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __IceFIX__Executor_all)
    {
        case 0:
        {
            return ___destroy(in, current);
        }
        case 1:
        {
            return ___execute(in, current);
        }
        case 2:
        {
            return ___ice_id(in, current);
        }
        case 3:
        {
            return ___ice_ids(in, current);
        }
        case 4:
        {
            return ___ice_isA(in, current);
        }
        case 5:
        {
            return ___ice_ping(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
IceFIX::Executor::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
IceFIX::Executor::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
IceFIX::Executor::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::Executor was not generated with stream support";
    throw ex;
}

void
IceFIX::Executor::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::Executor was not generated with stream support";
    throw ex;
}

void 
IceFIX::__patch__ExecutorPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::IceFIX::ExecutorPtr* p = static_cast< ::IceFIX::ExecutorPtr*>(__addr);
    assert(p);
    *p = ::IceFIX::ExecutorPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::IceFIX::Executor::ice_staticId(), v->ice_id());
    }
}

bool
IceFIX::operator==(const ::IceFIX::Executor& l, const ::IceFIX::Executor& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
IceFIX::operator<(const ::IceFIX::Executor& l, const ::IceFIX::Executor& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

::Ice::ObjectPtr
IceFIX::Reporter::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __IceFIX__Reporter_ids[2] =
{
    "::Ice::Object",
    "::IceFIX::Reporter"
};

bool
IceFIX::Reporter::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__IceFIX__Reporter_ids, __IceFIX__Reporter_ids + 2, _s);
}

::std::vector< ::std::string>
IceFIX::Reporter::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__IceFIX__Reporter_ids[0], &__IceFIX__Reporter_ids[2]);
}

const ::std::string&
IceFIX::Reporter::ice_id(const ::Ice::Current&) const
{
    return __IceFIX__Reporter_ids[1];
}

const ::std::string&
IceFIX::Reporter::ice_staticId()
{
    return __IceFIX__Reporter_ids[1];
}

::Ice::DispatchStatus
IceFIX::Reporter::___message(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string data;
    __is->read(data);
    __is->endReadEncaps();
    message(data, __current);
    return ::Ice::DispatchOK;
}

static ::std::string __IceFIX__Reporter_all[] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "message"
};

::Ice::DispatchStatus
IceFIX::Reporter::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__IceFIX__Reporter_all, __IceFIX__Reporter_all + 5, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __IceFIX__Reporter_all)
    {
        case 0:
        {
            return ___ice_id(in, current);
        }
        case 1:
        {
            return ___ice_ids(in, current);
        }
        case 2:
        {
            return ___ice_isA(in, current);
        }
        case 3:
        {
            return ___ice_ping(in, current);
        }
        case 4:
        {
            return ___message(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
IceFIX::Reporter::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
IceFIX::Reporter::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
IceFIX::Reporter::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::Reporter was not generated with stream support";
    throw ex;
}

void
IceFIX::Reporter::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::Reporter was not generated with stream support";
    throw ex;
}

void 
IceFIX::__patch__ReporterPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::IceFIX::ReporterPtr* p = static_cast< ::IceFIX::ReporterPtr*>(__addr);
    assert(p);
    *p = ::IceFIX::ReporterPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::IceFIX::Reporter::ice_staticId(), v->ice_id());
    }
}

bool
IceFIX::operator==(const ::IceFIX::Reporter& l, const ::IceFIX::Reporter& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
IceFIX::operator<(const ::IceFIX::Reporter& l, const ::IceFIX::Reporter& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

::Ice::ObjectPtr
IceFIX::BridgeAdmin::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __IceFIX__BridgeAdmin_ids[2] =
{
    "::Ice::Object",
    "::IceFIX::BridgeAdmin"
};

bool
IceFIX::BridgeAdmin::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__IceFIX__BridgeAdmin_ids, __IceFIX__BridgeAdmin_ids + 2, _s);
}

::std::vector< ::std::string>
IceFIX::BridgeAdmin::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__IceFIX__BridgeAdmin_ids[0], &__IceFIX__BridgeAdmin_ids[2]);
}

const ::std::string&
IceFIX::BridgeAdmin::ice_id(const ::Ice::Current&) const
{
    return __IceFIX__BridgeAdmin_ids[1];
}

const ::std::string&
IceFIX::BridgeAdmin::ice_staticId()
{
    return __IceFIX__BridgeAdmin_ids[1];
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___activate(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    activate(__current);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___deactivate(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    deactivate(__current);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___clean(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Long timeout;
    __is->read(timeout);
    __is->endReadEncaps();
    clean(timeout, __current);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___getStatus(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::IceFIX::BridgeStatus __ret = getStatus(__current);
    ::IceFIX::__write(__os, __ret);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___unregister(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string id;
    __is->read(id);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    try
    {
        unregister(id, __current);
    }
    catch(const ::IceFIX::RegistrationException& __ex)
    {
        __os->write(__ex);
        return ::Ice::DispatchUserException;
    }
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___register(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::IceFIX::QoS clientQoS;
    ::IceFIX::__readQoS(__is, clientQoS);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    try
    {
        ::std::string __ret = _cpp_register(clientQoS, __current);
        __os->write(__ret);
    }
    catch(const ::IceFIX::RegistrationException& __ex)
    {
        __os->write(__ex);
        return ::Ice::DispatchUserException;
    }
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___registerWithId(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string id;
    ::IceFIX::QoS clientQoS;
    __is->read(id);
    ::IceFIX::__readQoS(__is, clientQoS);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    try
    {
        registerWithId(id, clientQoS, __current);
    }
    catch(const ::IceFIX::RegistrationException& __ex)
    {
        __os->write(__ex);
        return ::Ice::DispatchUserException;
    }
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
IceFIX::BridgeAdmin::___getClients(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::IceFIX::ClientInfoSeq __ret = getClients(__current);
    if(__ret.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        ::IceFIX::__writeClientInfoSeq(__os, &__ret[0], &__ret[0] + __ret.size());
    }
    return ::Ice::DispatchOK;
}

static ::std::string __IceFIX__BridgeAdmin_all[] =
{
    "activate",
    "clean",
    "deactivate",
    "getClients",
    "getStatus",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "register",
    "registerWithId",
    "unregister"
};

::Ice::DispatchStatus
IceFIX::BridgeAdmin::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__IceFIX__BridgeAdmin_all, __IceFIX__BridgeAdmin_all + 12, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __IceFIX__BridgeAdmin_all)
    {
        case 0:
        {
            return ___activate(in, current);
        }
        case 1:
        {
            return ___clean(in, current);
        }
        case 2:
        {
            return ___deactivate(in, current);
        }
        case 3:
        {
            return ___getClients(in, current);
        }
        case 4:
        {
            return ___getStatus(in, current);
        }
        case 5:
        {
            return ___ice_id(in, current);
        }
        case 6:
        {
            return ___ice_ids(in, current);
        }
        case 7:
        {
            return ___ice_isA(in, current);
        }
        case 8:
        {
            return ___ice_ping(in, current);
        }
        case 9:
        {
            return ___register(in, current);
        }
        case 10:
        {
            return ___registerWithId(in, current);
        }
        case 11:
        {
            return ___unregister(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
IceFIX::BridgeAdmin::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
IceFIX::BridgeAdmin::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
IceFIX::BridgeAdmin::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::BridgeAdmin was not generated with stream support";
    throw ex;
}

void
IceFIX::BridgeAdmin::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::BridgeAdmin was not generated with stream support";
    throw ex;
}

void 
IceFIX::__patch__BridgeAdminPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::IceFIX::BridgeAdminPtr* p = static_cast< ::IceFIX::BridgeAdminPtr*>(__addr);
    assert(p);
    *p = ::IceFIX::BridgeAdminPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::IceFIX::BridgeAdmin::ice_staticId(), v->ice_id());
    }
}

bool
IceFIX::operator==(const ::IceFIX::BridgeAdmin& l, const ::IceFIX::BridgeAdmin& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
IceFIX::operator<(const ::IceFIX::BridgeAdmin& l, const ::IceFIX::BridgeAdmin& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

::Ice::ObjectPtr
IceFIX::Bridge::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __IceFIX__Bridge_ids[2] =
{
    "::Ice::Object",
    "::IceFIX::Bridge"
};

bool
IceFIX::Bridge::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__IceFIX__Bridge_ids, __IceFIX__Bridge_ids + 2, _s);
}

::std::vector< ::std::string>
IceFIX::Bridge::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__IceFIX__Bridge_ids[0], &__IceFIX__Bridge_ids[2]);
}

const ::std::string&
IceFIX::Bridge::ice_id(const ::Ice::Current&) const
{
    return __IceFIX__Bridge_ids[1];
}

const ::std::string&
IceFIX::Bridge::ice_staticId()
{
    return __IceFIX__Bridge_ids[1];
}

::Ice::DispatchStatus
IceFIX::Bridge::___connect(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string id;
    ::IceFIX::ReporterPrx cb;
    __is->read(id);
    ::IceFIX::__read(__is, cb);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::IceFIX::ExecutorPrx exec;
    try
    {
        connect(id, cb, exec, __current);
        __os->write(::Ice::ObjectPrx(::IceInternal::upCast(exec.get())));
    }
    catch(const ::IceFIX::RegistrationException& __ex)
    {
        __os->write(__ex);
        return ::Ice::DispatchUserException;
    }
    return ::Ice::DispatchOK;
}

static ::std::string __IceFIX__Bridge_all[] =
{
    "connect",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};

::Ice::DispatchStatus
IceFIX::Bridge::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__IceFIX__Bridge_all, __IceFIX__Bridge_all + 5, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __IceFIX__Bridge_all)
    {
        case 0:
        {
            return ___connect(in, current);
        }
        case 1:
        {
            return ___ice_id(in, current);
        }
        case 2:
        {
            return ___ice_ids(in, current);
        }
        case 3:
        {
            return ___ice_isA(in, current);
        }
        case 4:
        {
            return ___ice_ping(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
IceFIX::Bridge::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
IceFIX::Bridge::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
IceFIX::Bridge::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::Bridge was not generated with stream support";
    throw ex;
}

void
IceFIX::Bridge::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type IceFIX::Bridge was not generated with stream support";
    throw ex;
}

void 
IceFIX::__patch__BridgePtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::IceFIX::BridgePtr* p = static_cast< ::IceFIX::BridgePtr*>(__addr);
    assert(p);
    *p = ::IceFIX::BridgePtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::IceFIX::Bridge::ice_staticId(), v->ice_id());
    }
}

bool
IceFIX::operator==(const ::IceFIX::Bridge& l, const ::IceFIX::Bridge& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
IceFIX::operator<(const ::IceFIX::Bridge& l, const ::IceFIX::Bridge& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}
