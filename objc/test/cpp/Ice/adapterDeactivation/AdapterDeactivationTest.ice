// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

module Test
{

module AdapterDeactivation
{

interface TestIntf
{
    void transient();

    void deactivate();
};

local class Cookie
{
    ["cpp:const"] string message();
};

};

};

#endif
