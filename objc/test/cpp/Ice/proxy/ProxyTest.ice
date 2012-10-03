// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

#include <Ice/Current.ice>

module Test
{

module Proxy
{

["ami"] class MyClass
{
    void shutdown();

    Ice::Context getContext();
};

class MyDerivedClass extends MyClass
{
    Object* echo(Object* obj);
};

};

};

#endif
