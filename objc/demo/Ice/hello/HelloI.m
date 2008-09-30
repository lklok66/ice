// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <HelloI.h>

#import <Foundation/NSThread.h>

#import <stdio.h>

@implementation HelloI

-(void) sayHello:(int)delay current:(ICECurrent*)c
{
    if(delay != 0)
    {
        [NSThread sleepForTimeInterval:delay / 1000.f];
    }
    printf("Hello World!\n");
}

-(void) shutdown:(ICECurrent*)c
{
    printf("Shutting down...\n");
    [[c.adapter getCommunicator] shutdown];
}

@end
