// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef SERVERPRIVATE_ICE
#define SERVERPRIVATE_ICE

#include <Test.ice>

["objc:prefix:TestSlicingExceptions"]
module Test
{

exception UnknownDerived extends Base
{
    string ud;
};

exception UnknownIntermediate extends Base
{
   string ui;
};

exception UnknownMostDerived1 extends KnownIntermediate
{
   string umd1;
};

exception UnknownMostDerived2 extends UnknownIntermediate
{
   string umd2;
};

};

#endif
