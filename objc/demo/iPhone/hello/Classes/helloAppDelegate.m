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
            [initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
            [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
	    [initData.properties setProperty:@"IceSSL.CertFile" value:@"c_rsa1024.pfx"];
	    [initData.properties setProperty:@"IceSSL.Password" value:@"password"];
	    [initData.properties setProperty:@"IceSSL.TrustOnly" value:@"75:FA:B7:3C:6B:1C:F8:FA:69:4B:75:A0:22:51:B2:AC:11:54:A7:E7"];
#if TARGET_IPHONE_SIMULATOR
            [initData.properties setProperty:@"IceSSL.Keychain" value:@"test"];
            [initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
#endif     
            self.communicator = [ICEUtil createCommunicator:initData];
        }
        @catch(ICELocalException* ex)
        {
            NSMutableString* s = [NSMutableString stringWithFormat:@"failed to initialize communicator:\n %@", ex];
            [s replaceOccurrencesOfString:@"\n" withString:@" " options:0 range:NSMakeRange(0, s.length)];
            NSLog(s);
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
