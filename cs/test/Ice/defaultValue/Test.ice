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

module Test
{

enum Color { red, green, blue };

//
// Struct1 maps to a C# class because it contains string members.
//
struct Struct1
{
    bool boolFalse = false;
    bool boolTrue = true;
    byte b = 254;
    short s = 16000;
    int i = 3;
    long l = 4;
    float f = 5.0;
    double d = 6.0;
    string str = "foo bar";
    Color c = red;
    string noDefault;
};

//
// Struct2 would normally map to a C# struct because we have omitted
// the string members, but we use metadata to force it to be mapped
// to a class.
//
["clr:class"]
struct Struct2
{
    bool boolFalse = false;
    bool boolTrue = true;
    byte b = 1;
    short s = 2;
    int i = 3;
    long l = 4;
    float f = 5.0;
    double d = 6.0;
    //string str = "foo bar";
    Color c = blue;
    //string noDefault;
};

class Base
{
    bool boolFalse = false;
    bool boolTrue = true;
    byte b = 1;
    short s = 2;
    int i = 3;
    long l = 4;
    float f = 5.0;
    double d = 6.0;
    string str = "foo bar";
    string noDefault;
};

class Derived extends Base
{
    Color c = green;
};

exception BaseEx
{
    bool boolFalse = false;
    bool boolTrue = true;
    byte b = 1;
    short s = 2;
    int i = 3;
    long l = 4;
    float f = 5.0;
    double d = 6.0;
    string str = "foo bar";
    string noDefault;
};

exception DerivedEx extends BaseEx
{
    Color c = green;
};

};

#endif
