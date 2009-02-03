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
// name="FIXBridge::ClientDB", key="string", value="FIXBridge::Client"

#include <Ice/BasicStream.h>
#include <ClientDB.h>

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
FIXBridge::ClientDBKeyCodec::write(const ::std::string& v, Freeze::Key& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.write(v);
    ::std::vector<Ice::Byte>(stream.b.begin(), stream.b.end()).swap(bytes);
}

void
FIXBridge::ClientDBKeyCodec::read(::std::string& v, const Freeze::Key& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.b.resize(bytes.size());
    ::memcpy(&stream.b[0], &bytes[0], bytes.size());
    stream.i = stream.b.begin();
    stream.read(v);
}

static const ::std::string __FIXBridge__ClientDBKeyCodec_typeId = "string";

const ::std::string&
FIXBridge::ClientDBKeyCodec::typeId()
{
    return __FIXBridge__ClientDBKeyCodec_typeId;
}

void
FIXBridge::ClientDBValueCodec::write(const ::FIXBridge::Client& v, Freeze::Value& bytes, const ::Ice::CommunicatorPtr& communicator)
{
    IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);
    IceInternal::BasicStream stream(instance.get());
    stream.startWriteEncaps();
    v.__write(&stream);
    stream.endWriteEncaps();
    ::std::vector<Ice::Byte>(stream.b.begin(), stream.b.end()).swap(bytes);
}

void
FIXBridge::ClientDBValueCodec::read(::FIXBridge::Client& v, const Freeze::Value& bytes, const ::Ice::CommunicatorPtr& communicator)
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

static const ::std::string __FIXBridge__ClientDBValueCodec_typeId = "::FIXBridge::Client";

const ::std::string&
FIXBridge::ClientDBValueCodec::typeId()
{
    return __FIXBridge__ClientDBValueCodec_typeId;
}
