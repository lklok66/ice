// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.0

// Freeze types in this file:
// name="FIXBridge::RoutingRecordDB", key="string", value="FIXBridge::RoutingRecord"

#include <Ice/BasicStream.h>
#include <RoutingRecordDB.h>

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

void
FIXBridge::RoutingRecordDBKeyCodec::write(const ::std::string& v, Freeze::Key& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.write(v);
    ::std::vector<Ice::Byte>(stream.b.begin(), stream.b.end()).swap(bytes);
}

void
FIXBridge::RoutingRecordDBKeyCodec::read(::std::string& v, const Freeze::Key& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.b.resize(bytes.size());
    ::memcpy(&stream.b[0], &bytes[0], bytes.size());
    stream.i = stream.b.begin();
    stream.read(v);
}

static const ::std::string __FIXBridge__RoutingRecordDBKeyCodec_typeId = "string";

const ::std::string&
FIXBridge::RoutingRecordDBKeyCodec::typeId()
{
    return __FIXBridge__RoutingRecordDBKeyCodec_typeId;
}

void
FIXBridge::RoutingRecordDBValueCodec::write(const ::FIXBridge::RoutingRecord& v, Freeze::Value& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.startWriteEncaps();
    v.__write(&stream);
    stream.endWriteEncaps();
    ::std::vector<Ice::Byte>(stream.b.begin(), stream.b.end()).swap(bytes);
}

void
FIXBridge::RoutingRecordDBValueCodec::read(::FIXBridge::RoutingRecord& v, const Freeze::Value& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.b.resize(bytes.size());
    ::memcpy(&stream.b[0], &bytes[0], bytes.size());
    stream.i = stream.b.begin();
    stream.startReadEncaps();
    v.__read(&stream);
    stream.endReadEncaps();
}

static const ::std::string __FIXBridge__RoutingRecordDBValueCodec_typeId = "::FIXBridge::RoutingRecord";

const ::std::string&
FIXBridge::RoutingRecordDBValueCodec::typeId()
{
    return __FIXBridge__RoutingRecordDBValueCodec_typeId;
}
