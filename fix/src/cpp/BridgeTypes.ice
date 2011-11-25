// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice FIX is licensed to you under the terms described in the
// ICE_FIX_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceFIX/IceFIX.ice>
#include <Ice/BuiltinSequences.ice>

module FIXBridge
{

struct Message
{
    string data; // Message data
    Ice::StringSeq clients; // The list of clients that will receive this message.
    Ice::StringSeq forwarded; // Clients that have received the message. 
};

struct RoutingRecord
{
    string id; // The client id
    long timestamp;
};

struct Client
{
    string id;
    IceFIX::QoS qos;
    IceFIX::Reporter* reporter;
};

};

