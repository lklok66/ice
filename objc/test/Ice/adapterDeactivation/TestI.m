// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>

#import <Foundation/NSThread.h>

@implementation TestAdapterDeactivationI

-(void) transient:(ICECurrent*)current
{
    id<ICECommunicator> communicator = [current.adapter getCommunicator];
    id<ICEObjectAdapter> adapter =
        [communicator createObjectAdapterWithEndpoints:@"TransientTestAdapter" endpoints:@"default -p 9999"];
    [adapter activate];
    [adapter destroy];
}

-(void) deactivate:(ICECurrent*)current
{
    [current.adapter deactivate];
    [NSThread sleepForTimeInterval:1];
}
@end
