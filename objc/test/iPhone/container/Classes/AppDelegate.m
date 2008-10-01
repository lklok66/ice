// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AppDelegate.h>
#import <TestViewController.h>
#import <Test.h>

@implementation AppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize tests;
@synthesize currentTest;
@synthesize autoLaunch;

NSString* currentTestKey = @"currentTestKey";
NSString* autoLaunchKey = @"autoLaunchKey";
NSString* sslKey = @"sslKey";

-(id)init
{
    if(self = [super init])
    {
        NSMutableArray* theTests = [NSMutableArray array];
        NSBundle* bundle = [NSBundle mainBundle];
        
        NSFileManager* manager = [NSFileManager defaultManager];
        NSArray* frameworks = [manager directoryContentsAtPath:bundle.privateFrameworksPath];
        int i;
        for(i = 0; i < frameworks.count; ++i)
        {
            NSString* test = [frameworks objectAtIndex:i];
            NSRange r = [test rangeOfString:@"Test"];
            if(r.location == 0)
            {
                [theTests addObject:[Test testWithPath:[bundle.privateFrameworksPath stringByAppendingPathComponent:test] name:test]];
            }
        }
        tests = [[theTests copy] retain];
        
        // Initialize the application defaults.
        NSString* testValue = [[NSUserDefaults standardUserDefaults] stringForKey:currentTestKey];
        if(testValue == nil)
        {
            NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:@"0", currentTestKey,
                                         @"NO", autoLaunchKey,
                                         @"NO", sslKey,
                                         nil];
            
            [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }
        
        currentTest = [[NSUserDefaults standardUserDefaults] integerForKey:currentTestKey];
        if(currentTest < 0 || currentTest > tests.count)
        {
            currentTest = 0;
        }

        autoLaunch = [[NSUserDefaults standardUserDefaults] boolForKey:autoLaunchKey];
        // If auto-launch is set, then disable immediately.
        if(autoLaunch)
        {
            [[NSUserDefaults standardUserDefaults] setObject:@"NO" forKey:autoLaunchKey];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }
        ssl = [[NSUserDefaults standardUserDefaults] boolForKey:sslKey];
    }
    return self;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    
    // Override point for customization after app launch    
    [window addSubview:navigationController.view];
    [window makeKeyAndVisible];
}

- (void)dealloc
{
    [tests release];
    [navigationController release];
    [window release];
    [super dealloc];
}

-(int)currentTest
{
    return currentTest;
}

-(void)setCurrentTest:(int)test
{
    currentTest = test;

    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInt:currentTest] forKey:currentTestKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

-(BOOL)ssl
{
    return ssl;
}

-(void)setSsl:(BOOL)v
{
    ssl = v;
    
    [[NSUserDefaults standardUserDefaults] setObject:(v ? @"YES" : @"NO") forKey:sslKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

-(void)testCompleted:(BOOL)success
{
    if(success)
    {
        self.currentTest = currentTest+1;
    }
}

-(void)setAutoLaunch
{
    [[NSUserDefaults standardUserDefaults] setObject:@"YES" forKey:autoLaunchKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

@end
