// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AppDelegate.h>

@implementation AppDelegate

-(void)applicationDidFinishLaunching:(UIApplication *)application
{
    // Configure and show the window
    [window addSubview:navigationController.view];
    [window makeKeyAndVisible];
}

-(void)dealloc
{
    [navigationController release];
    [window release];

    [super dealloc];
}

@end
