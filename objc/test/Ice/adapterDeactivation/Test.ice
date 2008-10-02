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

["objc:prefix:TestAdapterDeactivation"]
module Test
{

interface TestIntf
{
    void transient();

    void deactivate();
};

};

#endif
