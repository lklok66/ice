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
// name="FIXBridge::MessageDB", key="int", value="FIXBridge::Message"

#include <Ice/BasicStream.h>
#include <MessageDB.h>

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
FIXBridge::MessageDBKeyCodec::write(::Ice::Int v, Freeze::Key& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.write(v);
    ::std::vector<Ice::Byte>(stream.b.begin(), stream.b.end()).swap(bytes);
}

void
FIXBridge::MessageDBKeyCodec::read(::Ice::Int& v, const Freeze::Key& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.b.resize(bytes.size());
    ::memcpy(&stream.b[0], &bytes[0], bytes.size());
    stream.i = stream.b.begin();
    stream.read(v);
}

static const ::std::string __FIXBridge__MessageDBKeyCodec_typeId = "int";

const ::std::string&
FIXBridge::MessageDBKeyCodec::typeId()
{
    return __FIXBridge__MessageDBKeyCodec_typeId;
}

void
FIXBridge::MessageDBValueCodec::write(const ::FIXBridge::Message& v, Freeze::Value& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.startWriteEncaps();
    v.__write(&stream);
    stream.endWriteEncaps();
    ::std::vector<Ice::Byte>(stream.b.begin(), stream.b.end()).swap(bytes);
}

void
FIXBridge::MessageDBValueCodec::read(::FIXBridge::Message& v, const Freeze::Value& bytes, const ::Ice::CommunicatorPtr& communicator)
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

static const ::std::string __FIXBridge__MessageDBValueCodec_typeId = "::FIXBridge::Message";

const ::std::string&
FIXBridge::MessageDBValueCodec::typeId()
{
    return __FIXBridge__MessageDBValueCodec_typeId;
}
