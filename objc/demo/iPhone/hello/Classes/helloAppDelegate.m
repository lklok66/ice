//
//  helloAppDelegate.m
//  hello
//
//  Created by Benoit Foucher on 7/30/08.
//  Copyright ZeroC, Inc. 2008. All rights reserved.
//

#import "helloAppDelegate.h"
#import "helloViewController.h"

#import <IceObjC/IceObjC.h>

@implementation helloAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {

    communicator = [Communicator initializeCommunicator];

    // Override point for customization after app launch	
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {

    [communicator shutdown];
    [communicator destroy];
    [communicator release];

    [viewController release];
    [window release];
    [super dealloc];
}


@end
