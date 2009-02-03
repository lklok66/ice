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

#include <BridgeTypes.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/BasicStream.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/ScopedArray.h>

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

bool
FIXBridge::Message::operator==(const Message& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(seqNum != __rhs.seqNum)
    {
        return false;
    }
    if(data != __rhs.data)
    {
        return false;
    }
    if(clients != __rhs.clients)
    {
        return false;
    }
    if(forwarded != __rhs.forwarded)
    {
        return false;
    }
    return true;
}

bool
FIXBridge::Message::operator<(const Message& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(seqNum < __rhs.seqNum)
    {
        return true;
    }
    else if(__rhs.seqNum < seqNum)
    {
        return false;
    }
    if(data < __rhs.data)
    {
        return true;
    }
    else if(__rhs.data < data)
    {
        return false;
    }
    if(clients < __rhs.clients)
    {
        return true;
    }
    else if(__rhs.clients < clients)
    {
        return false;
    }
    if(forwarded < __rhs.forwarded)
    {
        return true;
    }
    else if(__rhs.forwarded < forwarded)
    {
        return false;
    }
    return false;
}

void
FIXBridge::Message::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(seqNum);
    __os->write(data);
    if(clients.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        __os->write(&clients[0], &clients[0] + clients.size());
    }
    if(forwarded.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        __os->write(&forwarded[0], &forwarded[0] + forwarded.size());
    }
}

void
FIXBridge::Message::__read(::IceInternal::BasicStream* __is)
{
    __is->read(seqNum);
    __is->read(data);
    __is->read(clients);
    __is->read(forwarded);
}

bool
FIXBridge::RoutingRecord::operator==(const RoutingRecord& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(id != __rhs.id)
    {
        return false;
    }
    if(timestamp != __rhs.timestamp)
    {
        return false;
    }
    return true;
}

bool
FIXBridge::RoutingRecord::operator<(const RoutingRecord& __rhs) const
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
    if(timestamp < __rhs.timestamp)
    {
        return true;
    }
    else if(__rhs.timestamp < timestamp)
    {
        return false;
    }
    return false;
}

void
FIXBridge::RoutingRecord::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(id);
    __os->write(timestamp);
}

void
FIXBridge::RoutingRecord::__read(::IceInternal::BasicStream* __is)
{
    __is->read(id);
    __is->read(timestamp);
}

bool
FIXBridge::Client::operator==(const Client& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(id != __rhs.id)
    {
        return false;
    }
    if(qos != __rhs.qos)
    {
        return false;
    }
    if(reporter != __rhs.reporter)
    {
        return false;
    }
    return true;
}

bool
FIXBridge::Client::operator<(const Client& __rhs) const
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
    if(qos < __rhs.qos)
    {
        return true;
    }
    else if(__rhs.qos < qos)
    {
        return false;
    }
    if(reporter < __rhs.reporter)
    {
        return true;
    }
    else if(__rhs.reporter < reporter)
    {
        return false;
    }
    return false;
}

void
FIXBridge::Client::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(id);
    ::IceFIX::__writeQoS(__os, qos);
    __os->write(::Ice::ObjectPrx(::IceInternal::upCast(reporter.get())));
}

void
FIXBridge::Client::__read(::IceInternal::BasicStream* __is)
{
    __is->read(id);
    ::IceFIX::__readQoS(__is, qos);
    ::IceFIX::__read(__is, reporter);
}
