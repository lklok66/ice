// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <retry/TestI.h>

@implementation TestRetryRetryI
-(void) op:(BOOL)kill current:(ICECurrent*)current
{
   if(kill)
   {
       [[current con] close:YES];
   }
}

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end
