// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.0
// Generated from file `BridgeTypes.ice'

#ifndef __BridgeTypes_h__
#define __BridgeTypes_h__

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
#include <IceFIX/IceFIX.h>
#include <Ice/BuiltinSequences.h>
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

}

namespace IceInternal
{

}

namespace FIXBridge
{

struct Message
{
    ::Ice::Int seqNum;
    ::std::string data;
    ::Ice::StringSeq clients;
    ::Ice::StringSeq forwarded;

    bool operator==(const Message&) const;
    bool operator<(const Message&) const;
    bool operator!=(const Message& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const Message& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const Message& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const Message& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

struct RoutingRecord
{
    ::std::string id;
    ::Ice::Long timestamp;

    bool operator==(const RoutingRecord&) const;
    bool operator<(const RoutingRecord&) const;
    bool operator!=(const RoutingRecord& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const RoutingRecord& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const RoutingRecord& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const RoutingRecord& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

struct Client
{
    ::std::string id;
    ::IceFIX::QoS qos;
    ::IceFIX::ReporterPrx reporter;

    bool operator==(const Client&) const;
    bool operator<(const Client&) const;
    bool operator!=(const Client& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const Client& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const Client& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const Client& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

}

namespace IceProxy
{

}

namespace IceDelegate
{

}

namespace IceDelegateM
{

}

namespace IceDelegateD
{

}

#endif
