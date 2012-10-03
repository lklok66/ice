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

["objc:prefix:TestInvoke"]
module Test
{

exception MyException
{
};

class MyClass
{
    void opOneway();

    string opString(string s1, out string s2);

    void opException() throws MyException;

    void shutdown();
};

};

#endif
