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

#ifndef __MessageDB_h__
#define __MessageDB_h__

#include <Freeze/Map.h>
#include <BridgeTypes.h>

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

namespace FIXBridge
{

class MessageDBKeyCodec
{
public:

    static void write(::Ice::Int, Freeze::Key&, const ::Ice::CommunicatorPtr&);
    static void read(::Ice::Int&, const Freeze::Key&, const ::Ice::CommunicatorPtr&);
    static const std::string& typeId();
};

class MessageDBValueCodec
{
public:

    static void write(const ::FIXBridge::Message&, Freeze::Value&, const ::Ice::CommunicatorPtr&);
    static void read(::FIXBridge::Message&, const Freeze::Value&, const ::Ice::CommunicatorPtr&);
    static const std::string& typeId();
};

typedef Freeze::Map< ::Ice::Int, ::FIXBridge::Message, MessageDBKeyCodec, MessageDBValueCodec, Freeze::IceEncodingCompare > MessageDB;

}

#endif
