// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef WSTRING_ICE
#define WSTRING_ICE

["cpp:type:wstring"] module Test1
{

sequence<string> WstringSeq;

dictionary<string, string> WstringWStringDict;

struct WstringStruct
{
    string s;
};

exception WstringException
{
    string reason;
};

class WstringClass
{
    string opString(string s1, out string s2);

    WstringStruct opStruct(WstringStruct s1, out WstringStruct s2);

    void throwExcept(string reason)
        throws WstringException;

    string s;
};

};

module Test2
{

sequence<["cpp:type:wstring"] string> WstringSeq;

dictionary<["cpp:type:wstring"] string, ["cpp:type:wstring"] string> WstringWStringDict;

["cpp:type:wstring"] struct WstringStruct
{
    string s;
};

["cpp:type:wstring"] exception WstringException
{
    string reason;
};

["cpp:type:wstring"] class WstringClass
{
    string opString(string s1, out string s2);

    WstringStruct opStruct(WstringStruct s1, out WstringStruct s2);

    void throwExcept(string reason)
        throws WstringException;

    string s;
};

};

#endif
