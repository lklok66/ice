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

#ifndef __RoutingRecordDB_h__
#define __RoutingRecordDB_h__

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

class RoutingRecordDBKeyCodec
{
public:

    static void write(const ::std::string&, Freeze::Key&, const ::Ice::CommunicatorPtr&);
    static void read(::std::string&, const Freeze::Key&, const ::Ice::CommunicatorPtr&);
    static const std::string& typeId();
};

class RoutingRecordDBValueCodec
{
public:

    static void write(const ::FIXBridge::RoutingRecord&, Freeze::Value&, const ::Ice::CommunicatorPtr&);
    static void read(::FIXBridge::RoutingRecord&, const Freeze::Value&, const ::Ice::CommunicatorPtr&);
    static const std::string& typeId();
};

typedef Freeze::Map< ::std::string, ::FIXBridge::RoutingRecord, RoutingRecordDBKeyCodec, RoutingRecordDBValueCodec, Freeze::IceEncodingCompare > RoutingRecordDB;

}

#endif
