// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
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
