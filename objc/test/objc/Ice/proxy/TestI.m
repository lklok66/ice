// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <proxy/TestI.h>
#import <TestCommon.h>

@implementation TestProxyMyDerivedClassI
-(void) dealloc
{
    [_ctx release];
    [super dealloc];
}

-(id<ICEObjectPrx>) echo:(id<ICEObjectPrx>)obj current:(ICECurrent*)current
{
    return obj;
}

-(void) shutdown:(ICECurrent*)c
{
    [[[c adapter] getCommunicator] shutdown];
}

-(ICEContext*)getContext:(ICECurrent*)c
{
    return [[_ctx retain] autorelease];
}

-(BOOL) ice_isA:(NSString*)s current:(ICECurrent*)current
{
    [_ctx release];
    _ctx = [[current ctx] retain];
    return [super ice_isA:s current:current];
}

@end
