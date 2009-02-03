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

#ifndef __ClientDB_h__
#define __ClientDB_h__

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

class ClientDBKeyCodec
{
public:

    static void write(const ::std::string&, Freeze::Key&, const ::Ice::CommunicatorPtr&);
    static void read(::std::string&, const Freeze::Key&, const ::Ice::CommunicatorPtr&);
    static const std::string& typeId();
};

class ClientDBValueCodec
{
public:

    static void write(const ::FIXBridge::Client&, Freeze::Value&, const ::Ice::CommunicatorPtr&);
    static void read(::FIXBridge::Client&, const Freeze::Value&, const ::Ice::CommunicatorPtr&);
    static const std::string& typeId();
};

typedef Freeze::Map< ::std::string, ::FIXBridge::Client, ClientDBKeyCodec, ClientDBValueCodec, Freeze::IceEncodingCompare > ClientDB;

}

#endif
