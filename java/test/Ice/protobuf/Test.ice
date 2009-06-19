// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

module Test
{

["java:protobuf:test.TestPB.Message"] sequence<byte> Message;

["ami"] class MyClass
{
    void shutdown();

    Message opMessage(Message i, out Message o);

    ["amd"] Message opMessageAMD(Message i, out Message o);
};

// Remaining type definitions are there to verify that the generated
// code compiles correctly.

sequence<Message> SLS;
sequence<SLS> SLSS;
dictionary<int, Message> SLD;
dictionary<int, SLS> SLSD;
struct Foo
{
    Message SLmem;
    SLS SLSmem;
};

exception Bar
{
    Message SLmem;
    SLS SLSmem;
};

class Baz
{
    Message SLmem;
    SLS SLSmem;
};

};

#endif
