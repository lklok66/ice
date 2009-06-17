// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#import <AppDelegate.h>
#import <RootViewController.h>
#import <Ice/Ice.h>
#import <RouterI.h>
#import <Router.h>
#import <LoggingDelegate.h>

@implementation Logger

@synthesize delegate;

-(void) print:(NSString*)message
{
    if(delegate)
    {
        [delegate performSelectorOnMainThread:@selector(log:)
                                   withObject:[[LogEntry logEntryPrint:message] retain]
                                waitUntilDone:NO];
    }
}

-(void) trace:(NSString*)category message:(NSString*)message
{
    if(delegate)
    {
        [delegate performSelectorOnMainThread:@selector(log:)
                                   withObject:[[LogEntry logEntryTrace:message category:category] retain]
                                waitUntilDone:NO];
    }
}

-(void) warning:(NSString*)message
{
    if(delegate)
    {
        [delegate performSelectorOnMainThread:@selector(log:)
                                   withObject:[[LogEntry logEntryWarning:message] retain]
                                waitUntilDone:NO];
    }
}

-(void) error:(NSString*)message
{
    if(delegate)
    {
        [delegate performSelectorOnMainThread:@selector(log:)
                                   withObject:[[LogEntry logEntryError:message] retain]
                                waitUntilDone:NO];
    }
}

-(void)dealloc
{
    [delegate release];
    [super dealloc];
}
@end

@implementation AppDelegate

@synthesize window;
@synthesize rootViewController;
@synthesize communicator;
@synthesize router;
@synthesize logger;

-(id)init
{
    if (self = [super init])
    {
        NSString* testValue = [[NSUserDefaults standardUserDefaults] stringForKey:@"Ice.Trace.Network"];
        if (testValue == nil)
        {
            NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
                                         @"Ice.Trace.Network", @"0",
                                         @"Ice.Trace.Protocol", @"0",
                                         @"Trace.Router", @"1",
                                         nil];
            
            [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }
        
        ICEInitializationData* initData = [[ICEInitializationData initializationData] retain];
        initData.properties = [ICEUtil createProperties];
        
        // For the phone we want to target WiFi for the client endpoints, and
        // the 3g network for the server endpoints.
#if TARGET_IPHONE_SIMULATOR && !__IPHONE_3_0
        [initData.properties setProperty:@"Client.Endpoints" value:@"tcp -h 127.0.0.1 -p 12000"];
        [initData.properties setProperty:@"Server.Endpoints" value:@"tcp -h 127.0.0.1"];
#endif
        
        [initData.properties setProperty:@"RoutedServer.Endpoints" value:@""];

        [initData.properties setProperty:@"Ice.Trace.Network" value:[[NSUserDefaults standardUserDefaults] stringForKey:@"Ice.Trace.Network"]];
        [initData.properties setProperty:@"Ice.Trace.Protocol" value:[[NSUserDefaults standardUserDefaults] stringForKey:@"Ice.Trace.Protocol"]];
        
        [initData.properties setProperty:@"Ice.ThreadPool.Server.SizeMax" value:@"10"];

         [initData.properties setProperty:@"Trace.Router" value:[[NSUserDefaults standardUserDefaults] stringForKey:@"Trace.Router"]];

        //[initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
        //[initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
        //[initData.properties setProperty:@"IceSSL.CertFile" value:@"c_rsa1024.pfx"];
        //[initData.properties setProperty:@"IceSSL.Password" value:@"password"];
#if TARGET_IPHONE_SIMULATOR && !__IPHONE_3_0
        //[initData.properties setProperty:@"IceSSL.Keychain" value:@"test"];
        //[initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
#endif     
        self.logger = [[Logger alloc] init];
        initData.logger = logger;
        self.communicator = [ICEUtil createCommunicator:initData];
    }
    return self;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
    [window addSubview:[rootViewController view]];
    [window makeKeyAndVisible];
}

-(void)initializeRouter
{
    @try
    {
        id<ICEObjectAdapter> clientAdapter = [communicator createObjectAdapter:@"Client"];

        self.router = [[[RouterI alloc] initWithCommunicator:communicator] autorelease];
        [clientAdapter add:self.router identity:[communicator stringToIdentity:@"iPhoneRouter/Router"]];
        [clientAdapter addDefaultServant:router.clientBlobject category:@""];
        [clientAdapter activate];
    }
    @catch(ICELocalException* ex)
    {
        NSMutableString* s = [NSMutableString stringWithFormat:@"failed to initialize communicator:\n %@", ex];
        [s replaceOccurrencesOfString:@"\n" withString:@" " options:0 range:NSMakeRange(0, s.length)];
        NSLog(s);
    }
}

- (void)dealloc {
    [communicator release];
    [router release];
    [rootViewController release];
    [window release];
    [logger release];
    [super dealloc];
}

@end
