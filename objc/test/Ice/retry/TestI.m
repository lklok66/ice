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

@implementation RetryI
-(void) op:(BOOL)kill current:(ICECurrent*)current
{
   if(kill)
   {
       [[current con] close:YES];
   }
}

-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}
@end
