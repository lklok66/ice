// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <helloAppDelegate.h>
#import <helloViewController.h>

@implementation helloAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    // Override point for customization after app launch	
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}

- (void)dealloc
{
    [viewController release];
    [window release];
    [super dealloc];
}

@end
