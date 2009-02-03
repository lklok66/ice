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

#ifndef __IceFIX_IceFIX_h__
#define __IceFIX_IceFIX_h__

#include <Ice/LocalObjectF.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/Outgoing.h>
#include <Ice/OutgoingAsync.h>
#include <Ice/Incoming.h>
#include <Ice/Direct.h>
#include <Ice/UserExceptionFactory.h>
#include <Ice/FactoryTable.h>
#include <Ice/StreamF.h>
#include <Ice/UndefSysMacros.h>

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

namespace IceProxy
{

namespace IceFIX
{

class Executor;

class Reporter;

class BridgeAdmin;

class Bridge;

}

}

namespace IceFIX
{

class Executor;
bool operator==(const Executor&, const Executor&);
bool operator<(const Executor&, const Executor&);

class Reporter;
bool operator==(const Reporter&, const Reporter&);
bool operator<(const Reporter&, const Reporter&);

class BridgeAdmin;
bool operator==(const BridgeAdmin&, const BridgeAdmin&);
bool operator<(const BridgeAdmin&, const BridgeAdmin&);

class Bridge;
bool operator==(const Bridge&, const Bridge&);
bool operator<(const Bridge&, const Bridge&);

}

namespace IceInternal
{

::Ice::Object* upCast(::IceFIX::Executor*);
::IceProxy::Ice::Object* upCast(::IceProxy::IceFIX::Executor*);

::Ice::Object* upCast(::IceFIX::Reporter*);
::IceProxy::Ice::Object* upCast(::IceProxy::IceFIX::Reporter*);

::Ice::Object* upCast(::IceFIX::BridgeAdmin*);
::IceProxy::Ice::Object* upCast(::IceProxy::IceFIX::BridgeAdmin*);

::Ice::Object* upCast(::IceFIX::Bridge*);
::IceProxy::Ice::Object* upCast(::IceProxy::IceFIX::Bridge*);

}

namespace IceFIX
{

typedef ::IceInternal::Handle< ::IceFIX::Executor> ExecutorPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::IceFIX::Executor> ExecutorPrx;

void __read(::IceInternal::BasicStream*, ExecutorPrx&);
void __patch__ExecutorPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::IceFIX::Reporter> ReporterPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::IceFIX::Reporter> ReporterPrx;

void __read(::IceInternal::BasicStream*, ReporterPrx&);
void __patch__ReporterPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::IceFIX::BridgeAdmin> BridgeAdminPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::IceFIX::BridgeAdmin> BridgeAdminPrx;

void __read(::IceInternal::BasicStream*, BridgeAdminPrx&);
void __patch__BridgeAdminPtr(void*, ::Ice::ObjectPtr&);

typedef ::IceInternal::Handle< ::IceFIX::Bridge> BridgePtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::IceFIX::Bridge> BridgePrx;

void __read(::IceInternal::BasicStream*, BridgePrx&);
void __patch__BridgePtr(void*, ::Ice::ObjectPtr&);

}

namespace IceFIX
{

class ExecuteException : public ::Ice::UserException
{
public:

    ExecuteException() {}
    explicit ExecuteException(const ::std::string&);
    virtual ~ExecuteException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    ::std::string reason;

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

static ExecuteException __ExecuteException_init;

class RegistrationException : public ::Ice::UserException
{
public:

    RegistrationException() {}
    explicit RegistrationException(const ::std::string&);
    virtual ~RegistrationException() throw();

    virtual ::std::string ice_name() const;
    virtual ::Ice::Exception* ice_clone() const;
    virtual void ice_throw() const;

    static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();

    ::std::string reason;

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

struct ClientInfo
{
    ::std::string id;
    bool isConnected;

    bool operator==(const ClientInfo&) const;
    bool operator<(const ClientInfo&) const;
    bool operator!=(const ClientInfo& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const ClientInfo& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const ClientInfo& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const ClientInfo& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

typedef ::std::vector< ::IceFIX::ClientInfo> ClientInfoSeq;
void __writeClientInfoSeq(::IceInternal::BasicStream*, const ::IceFIX::ClientInfo*, const ::IceFIX::ClientInfo*);
void __readClientInfoSeq(::IceInternal::BasicStream*, ClientInfoSeq&);

typedef ::std::map< ::std::string, ::std::string> QoS;
void __writeQoS(::IceInternal::BasicStream*, const QoS&);
void __readQoS(::IceInternal::BasicStream*, QoS&);

enum BridgeStatus
{
    BridgeStatusNotActive,
    BridgeStatusActive,
    BridgeStatusLoggedOn
};

void __write(::IceInternal::BasicStream*, BridgeStatus);
void __read(::IceInternal::BasicStream*, BridgeStatus&);

}

namespace IceFIX
{

class AMI_Reporter_message : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response() = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::IceFIX::ReporterPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::IceFIX::AMI_Reporter_message> AMI_Reporter_messagePtr;

}

namespace IceProxy
{

namespace IceFIX
{

class Executor : virtual public ::IceProxy::Ice::Object
{
public:

    void execute(const ::std::string& data)
    {
        execute(data, 0);
    }
    void execute(const ::std::string& data, const ::Ice::Context& __ctx)
    {
        execute(data, &__ctx);
    }
    
private:

    void execute(const ::std::string&, const ::Ice::Context*);
    
public:

    void destroy()
    {
        destroy(0);
    }
    void destroy(const ::Ice::Context& __ctx)
    {
        destroy(&__ctx);
    }
    
private:

    void destroy(const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<Executor> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Executor> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Executor*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<Executor*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class Reporter : virtual public ::IceProxy::Ice::Object
{
public:

    void message(const ::std::string& data)
    {
        message(data, 0);
    }
    void message(const ::std::string& data, const ::Ice::Context& __ctx)
    {
        message(data, &__ctx);
    }
    
private:

    void message(const ::std::string&, const ::Ice::Context*);
    
public:
    bool message_async(const ::IceFIX::AMI_Reporter_messagePtr&, const ::std::string&);
    bool message_async(const ::IceFIX::AMI_Reporter_messagePtr&, const ::std::string&, const ::Ice::Context&);
    
    ::IceInternal::ProxyHandle<Reporter> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Reporter> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Reporter*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<Reporter*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class BridgeAdmin : virtual public ::IceProxy::Ice::Object
{
public:

    void activate()
    {
        activate(0);
    }
    void activate(const ::Ice::Context& __ctx)
    {
        activate(&__ctx);
    }
    
private:

    void activate(const ::Ice::Context*);
    
public:

    void deactivate()
    {
        deactivate(0);
    }
    void deactivate(const ::Ice::Context& __ctx)
    {
        deactivate(&__ctx);
    }
    
private:

    void deactivate(const ::Ice::Context*);
    
public:

    void clean(::Ice::Long timeout)
    {
        clean(timeout, 0);
    }
    void clean(::Ice::Long timeout, const ::Ice::Context& __ctx)
    {
        clean(timeout, &__ctx);
    }
    
private:

    void clean(::Ice::Long, const ::Ice::Context*);
    
public:

    ::IceFIX::BridgeStatus getStatus()
    {
        return getStatus(0);
    }
    ::IceFIX::BridgeStatus getStatus(const ::Ice::Context& __ctx)
    {
        return getStatus(&__ctx);
    }
    
private:

    ::IceFIX::BridgeStatus getStatus(const ::Ice::Context*);
    
public:

    void unregister(const ::std::string& id)
    {
        unregister(id, 0);
    }
    void unregister(const ::std::string& id, const ::Ice::Context& __ctx)
    {
        unregister(id, &__ctx);
    }
    
private:

    void unregister(const ::std::string&, const ::Ice::Context*);
    
public:

    ::std::string _cpp_register(const ::IceFIX::QoS& clientQoS)
    {
        return _cpp_register(clientQoS, 0);
    }
    ::std::string _cpp_register(const ::IceFIX::QoS& clientQoS, const ::Ice::Context& __ctx)
    {
        return _cpp_register(clientQoS, &__ctx);
    }
    
private:

    ::std::string _cpp_register(const ::IceFIX::QoS&, const ::Ice::Context*);
    
public:

    void registerWithId(const ::std::string& id, const ::IceFIX::QoS& clientQoS)
    {
        registerWithId(id, clientQoS, 0);
    }
    void registerWithId(const ::std::string& id, const ::IceFIX::QoS& clientQoS, const ::Ice::Context& __ctx)
    {
        registerWithId(id, clientQoS, &__ctx);
    }
    
private:

    void registerWithId(const ::std::string&, const ::IceFIX::QoS&, const ::Ice::Context*);
    
public:

    ::IceFIX::ClientInfoSeq getClients()
    {
        return getClients(0);
    }
    ::IceFIX::ClientInfoSeq getClients(const ::Ice::Context& __ctx)
    {
        return getClients(&__ctx);
    }
    
private:

    ::IceFIX::ClientInfoSeq getClients(const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<BridgeAdmin> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<BridgeAdmin*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<BridgeAdmin*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

class Bridge : virtual public ::IceProxy::Ice::Object
{
public:

    void connect(const ::std::string& id, const ::IceFIX::ReporterPrx& cb, ::IceFIX::ExecutorPrx& exec)
    {
        connect(id, cb, exec, 0);
    }
    void connect(const ::std::string& id, const ::IceFIX::ReporterPrx& cb, ::IceFIX::ExecutorPrx& exec, const ::Ice::Context& __ctx)
    {
        connect(id, cb, exec, &__ctx);
    }
    
private:

    void connect(const ::std::string&, const ::IceFIX::ReporterPrx&, ::IceFIX::ExecutorPrx&, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<Bridge> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Bridge> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Bridge*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<Bridge*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

}

}

namespace IceDelegate
{

namespace IceFIX
{

class Executor : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void execute(const ::std::string&, const ::Ice::Context*) = 0;

    virtual void destroy(const ::Ice::Context*) = 0;
};

class Reporter : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void message(const ::std::string&, const ::Ice::Context*) = 0;
};

class BridgeAdmin : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void activate(const ::Ice::Context*) = 0;

    virtual void deactivate(const ::Ice::Context*) = 0;

    virtual void clean(::Ice::Long, const ::Ice::Context*) = 0;

    virtual ::IceFIX::BridgeStatus getStatus(const ::Ice::Context*) = 0;

    virtual void unregister(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::std::string _cpp_register(const ::IceFIX::QoS&, const ::Ice::Context*) = 0;

    virtual void registerWithId(const ::std::string&, const ::IceFIX::QoS&, const ::Ice::Context*) = 0;

    virtual ::IceFIX::ClientInfoSeq getClients(const ::Ice::Context*) = 0;
};

class Bridge : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void connect(const ::std::string&, const ::IceFIX::ReporterPrx&, ::IceFIX::ExecutorPrx&, const ::Ice::Context*) = 0;
};

}

}

namespace IceDelegateM
{

namespace IceFIX
{

class Executor : virtual public ::IceDelegate::IceFIX::Executor,
                 virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void execute(const ::std::string&, const ::Ice::Context*);

    virtual void destroy(const ::Ice::Context*);
};

class Reporter : virtual public ::IceDelegate::IceFIX::Reporter,
                 virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void message(const ::std::string&, const ::Ice::Context*);
};

class BridgeAdmin : virtual public ::IceDelegate::IceFIX::BridgeAdmin,
                    virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void activate(const ::Ice::Context*);

    virtual void deactivate(const ::Ice::Context*);

    virtual void clean(::Ice::Long, const ::Ice::Context*);

    virtual ::IceFIX::BridgeStatus getStatus(const ::Ice::Context*);

    virtual void unregister(const ::std::string&, const ::Ice::Context*);

    virtual ::std::string _cpp_register(const ::IceFIX::QoS&, const ::Ice::Context*);

    virtual void registerWithId(const ::std::string&, const ::IceFIX::QoS&, const ::Ice::Context*);

    virtual ::IceFIX::ClientInfoSeq getClients(const ::Ice::Context*);
};

class Bridge : virtual public ::IceDelegate::IceFIX::Bridge,
               virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void connect(const ::std::string&, const ::IceFIX::ReporterPrx&, ::IceFIX::ExecutorPrx&, const ::Ice::Context*);
};

}

}

namespace IceDelegateD
{

namespace IceFIX
{

class Executor : virtual public ::IceDelegate::IceFIX::Executor,
                 virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void execute(const ::std::string&, const ::Ice::Context*);

    virtual void destroy(const ::Ice::Context*);
};

class Reporter : virtual public ::IceDelegate::IceFIX::Reporter,
                 virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void message(const ::std::string&, const ::Ice::Context*);
};

class BridgeAdmin : virtual public ::IceDelegate::IceFIX::BridgeAdmin,
                    virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void activate(const ::Ice::Context*);

    virtual void deactivate(const ::Ice::Context*);

    virtual void clean(::Ice::Long, const ::Ice::Context*);

    virtual ::IceFIX::BridgeStatus getStatus(const ::Ice::Context*);

    virtual void unregister(const ::std::string&, const ::Ice::Context*);

    virtual ::std::string _cpp_register(const ::IceFIX::QoS&, const ::Ice::Context*);

    virtual void registerWithId(const ::std::string&, const ::IceFIX::QoS&, const ::Ice::Context*);

    virtual ::IceFIX::ClientInfoSeq getClients(const ::Ice::Context*);
};

class Bridge : virtual public ::IceDelegate::IceFIX::Bridge,
               virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void connect(const ::std::string&, const ::IceFIX::ReporterPrx&, ::IceFIX::ExecutorPrx&, const ::Ice::Context*);
};

}

}

namespace IceFIX
{

class Executor : virtual public ::Ice::Object
{
public:

    typedef ExecutorPrx ProxyType;
    typedef ExecutorPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void execute(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___execute(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void destroy(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___destroy(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class Reporter : virtual public ::Ice::Object
{
public:

    typedef ReporterPrx ProxyType;
    typedef ReporterPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void message(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___message(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class BridgeAdmin : virtual public ::Ice::Object
{
public:

    typedef BridgeAdminPrx ProxyType;
    typedef BridgeAdminPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void activate(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___activate(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void deactivate(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___deactivate(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void clean(::Ice::Long, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___clean(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::IceFIX::BridgeStatus getStatus(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___getStatus(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void unregister(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___unregister(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::std::string _cpp_register(const ::IceFIX::QoS&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___register(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void registerWithId(const ::std::string&, const ::IceFIX::QoS&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___registerWithId(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::IceFIX::ClientInfoSeq getClients(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___getClients(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

class Bridge : virtual public ::Ice::Object
{
public:

    typedef BridgePrx ProxyType;
    typedef BridgePtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void connect(const ::std::string&, const ::IceFIX::ReporterPrx&, ::IceFIX::ExecutorPrx&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___connect(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

}

#endif
