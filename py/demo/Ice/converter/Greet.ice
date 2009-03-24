// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef GREET_ICE
#define GREET_ICE

module Demo
{

interface Greet
{
    ["cpp:const"] idempotent string exchangeGreeting(string s);
    void shutdown();
};

};

#endif
