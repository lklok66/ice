// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef FORWARD_ICE
#define FORWARD_ICE

["objc:prefix:TestSlicingObjectsShared"]
module TestShared
{

class Forward;

class Hidden
{
    Forward f;
};

class Forward
{
    Hidden h;
};

};

#endif
