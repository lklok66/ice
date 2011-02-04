// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

#include <Ice/Current.ice>

module Test
{


["amd"] class MyClass
{
    void shutdown();

    Ice::Context getContext();
};

["amd"] class MyDerivedClass extends MyClass
{
    Object* echo(Object* obj);
};

};

#endif
