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
        @try
        {
            ICEInitializationData* initData = [ICEInitializationData initializationData];
            initData.properties = [ICEUtil createProperties];
            [initData.properties setProperty:@"Ice.Plugin.IceSSL" value:@"createIceSSL"];
            [initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
            [initData.properties setProperty:@"IceSSL.Password" value:@"password"];
            [initData.properties setProperty:@"IceSSL.CertFile" value:@"c_rsa1024.pfx"];
            [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.pem"];
            self.communicator = [ICEUtil createCommunicator:initData];
        }
        @catch(ICELocalException* ex)
        {
            NSLog(@"failed to initialize communicator:\n%@", ex);
            return nil;
        }
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
