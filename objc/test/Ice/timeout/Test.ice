// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

["objc:prefix:TestTimeout"]
module Test
{

sequence<byte> ByteSeq;

interface Timeout
{
    ["ami"] void op();
    ["ami"] void sendData(ByteSeq seq);
    ["ami"] void sleep(int to);

    void holdAdapter(int to);

    void shutdown();
};

};

#endif
