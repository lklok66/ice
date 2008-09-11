// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <helloAppDelegate.h>
#import <helloViewController.h>
#import <Ice/Ice.h>

@implementation helloAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize communicator;

-(id)init
{
    if (self = [super init])
    {
        self.communicator = [ICEUtil createCommunicator];
    }
    return self;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    // Override point for customization after app launch	
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [communicator destroy];
}

- (void)dealloc
{
    [communicator release];
    [viewController release];
    [window release];
    [super dealloc];
}

@end
