// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <TestServer.h>
#import <Forward.h>

@interface TestSlicingObjectsServerI : TestSlicingObjectsServerTestIntf
{
    TestSlicingObjectsServerB* oneElementCycle_;
    TestSlicingObjectsServerB* twoElementCycle_;
}
@end


