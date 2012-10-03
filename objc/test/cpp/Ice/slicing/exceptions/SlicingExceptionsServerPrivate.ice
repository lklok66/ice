// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef SERVERPRIVATE_ICE
#define SERVERPRIVATE_ICE

#include <SlicingExceptionsTest.ice>

module Test
{

module Slicing
{

module Exceptions
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

};

};

#endif
