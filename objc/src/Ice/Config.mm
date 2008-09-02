// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>
#import <Foundation/NSNull.h>

BOOL ICEisNil(id p)
{
    return p == nil || p == [NSNull null];
}

