// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AppDelegate.h>
#import <TestViewController.h>
#import <Test.h>

struct TestCases
{
    NSString* name;
    int (*startServer)(int, char**);
    int (*startClient)(int, char**);
};
int adapterDeactivationServer(int, char**);
int adapterDeactivationClient(int, char**);
int bindingServer(int, char**);
int bindingClient(int, char**);
int defaultServantServer(int, char**);
int defaultServantClient(int, char**);
int exceptionsServer(int, char**);
int exceptionsClient(int, char**);
int facetsServer(int, char**);
int facetsClient(int, char**);
int holdServer(int, char**);
int holdClient(int, char**);
int inheritanceServer(int, char**);
int inheritanceClient(int, char**);
int interceptorServer(int, char**);
int interceptorClient(int, char**);
int locationServer(int, char**);
int locationClient(int, char**);
int objectsServer(int, char**);
int objectsClient(int, char**);
int operationsServer(int, char**);
int operationsClient(int, char**);
int proxyServer(int, char**);
int proxyClient(int, char**);
int retryServer(int, char**);
int retryClient(int, char**);
int timeoutServer(int, char**);
int timeoutClient(int, char**);
int slicingExceptionsServer(int, char**);
int slicingExceptionsClient(int, char**);

static const struct TestCases alltests[] =
{
{ @"adapterDeactivation", adapterDeactivationServer, adapterDeactivationClient },
{ @"binding", bindingServer, bindingClient },
{ @"defaultServant", defaultServantServer, defaultServantClient },
{ @"exceptions", exceptionsServer, exceptionsClient },
{ @"facets", facetsServer, facetsClient },
{ @"hold", holdServer, holdClient },
{ @"inheritance", inheritanceServer, inheritanceClient, },
{ @"interceptor", interceptorServer, interceptorClient },
{ @"location", locationServer, locationClient },
{ @"objects", objectsServer, objectsClient },
{ @"operations", operationsServer, operationsClient },
{ @"proxy", proxyServer, proxyClient },
{ @"retry",retryServer, retryClient },
{ @"timeout",timeoutServer, timeoutClient },
{ @"slicing/exceptions",slicingExceptionsServer, slicingExceptionsClient }
};

@implementation AppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize tests;
@synthesize currentTest;
@synthesize loop;

static NSString* currentTestKey = @"currentTestKey";
static NSString* sslKey = @"sslKey";

+(void)initialize
{
    NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
                                 @"0", currentTestKey,
                                 @"NO", sslKey,
                                 nil];
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
}

-(id)init
{
    if(self = [super init])
    {
        NSMutableArray* theTests = [NSMutableArray array];
        for(int i = 0; i < sizeof(alltests)/sizeof(alltests[0]); ++i)
        {
            Test* test = [Test testWithName:alltests[i].name server:alltests[i].startServer client:alltests[i].startClient];
            [theTests addObject:test];
        }
        tests = [[theTests copy] retain];
        
        // Initialize the application defaults.
        currentTest = [[NSUserDefaults standardUserDefaults] integerForKey:currentTestKey];
        if(currentTest < 0 || currentTest > tests.count)
        {
            currentTest = 0;
        }

#if TARGET_IPHONE_SIMULATOR
        ssl = NO;
#else    
        ssl = [[NSUserDefaults standardUserDefaults] boolForKey:sslKey];
#endif
        loop = NO;
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
}

-(BOOL)ssl
{
    return ssl;
}

-(void)setSsl:(BOOL)v
{
    ssl = v;
    
    [[NSUserDefaults standardUserDefaults] setBool:ssl forKey:sslKey];
}

-(void)testCompleted:(BOOL)success
{
    if(success)
    {
        self.currentTest = (currentTest+1) % tests.count;;
    }
}

@end
