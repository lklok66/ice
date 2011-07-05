// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <HelloI.h>

#include <stdio.h>

@implementation HelloI
-(void) sayHello:(ICECurrent*)current
{
    printf("Hello world!\n");
}
@end
