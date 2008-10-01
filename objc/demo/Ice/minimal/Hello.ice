// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef HELLO_ICE
#define HELLO_ICE

module Demo
{

interface Hello
{
    ["cpp:const"] idempotent void sayHello();
};

};

#endif
