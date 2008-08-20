// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>

#import <TestI.h>
#import <TestCommon.h>

@implementation MyDerivedClassI
-(void) opVoid:(ICECurrent*)current
{
}

-(void) opDerived:(ICECurrent*)current
{
}

-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 current:(ICECurrent*)current
{
    *p3 = p1 ^ p2;
    return p1;
}

-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}
@end
