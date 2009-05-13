// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject
{
    NSViewController* connectController;
}

@property (nonatomic, readonly) NSViewController* connectController;

-(void)switchController:(NSViewController*)controller;

@end
