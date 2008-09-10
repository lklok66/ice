// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <ServerPrivate.h>
#import <Forward.h>

@interface TestI : TestTestIntf
{
    TestB* oneElementCycle_;
    TestB* twoElementCycle_;
}
@end


