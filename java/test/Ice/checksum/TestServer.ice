// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

#include <Ice/SliceChecksumDict.ice>

[["java:package:test.Ice.checksum.server"]]
module Test
{

interface Checksum
{
    idempotent Ice::SliceChecksumDict getSliceChecksums();
    
    void shutdown();
};

};

#endif
