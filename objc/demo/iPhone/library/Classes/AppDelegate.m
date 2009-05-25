// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AppDelegate.h>

#import <Ice/Ice.h>
#import <Session.h>

@interface AppDelegate()

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) UINavigationController *navigationController;

@property (nonatomic, retain) NSTimer* refreshTimer;
@property (nonatomic, retain) ICEInitializationData* initData;

@end

@implementation AppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize communicator;
@synthesize refreshTimer;
@synthesize initData;

@dynamic fatal;

-(id)init
{
    if (self = [super init])
    {
        self.initData = [ICEInitializationData initializationData];
        self.initData.properties = [ICEUtil createProperties ];
        [self.initData.properties setProperty:@"Ice.ACM.Client" value:@"0"];
        [self.initData.properties setProperty:@"Ice.RetryIntervals" value:@"-1"];
        
        // Tracing properties.
        //[self.initData.properties setProperty:@"Ice.Trace.Network" value:@"1"];
        //[self.initData.properties setProperty:@"Ice.Trace.Protocol" value:@"1"];
        
        //[self.initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
        
        [self.initData.properties setProperty:@"IceSSL.TrustOnly.Client" value:@"11:DD:28:AD:13:44:76:47:4F:BE:3C:4D:AC:AD:5A:06:88:DA:52:DA"];
        [self.initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
        
#if TARGET_IPHONE_SIMULATOR
        [self.initData.properties setProperty:@"IceSSL.Keychain" value:@"test"];
        [self.initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
#endif        
    }
    return self;
}

-(void)applicationDidFinishLaunching:(UIApplication *)application
{
    // Configure and show the window
    [window addSubview:navigationController.view];
    [window makeKeyAndVisible];
}

-(void)applicationWillTerminate:(UIApplication *)application
{
    [self logout];

    [communicator destroy];
}

-(void)dealloc
{
    [communicator release];
    [initData release];

    [session release];
    [refreshTimer release];
    
    [navigationController release];
    [window release];

    [super dealloc];
}

-(BOOL)fatal
{
    return fatal;
}

-(void)setFatal:(BOOL)value
{
    if(value)
    {
        [self logout];
    }
    fatal = value;
}

-(void)logout
{
    // Destroy the old session, and invalidate the refresh timer.
    if(session != nil)
    {
        [refreshTimer invalidate];
        [session destroy_async:nil response:nil exception:nil];
        self.refreshTimer = nil;
        [session release];
        session = nil;
    }
    self.fatal = NO;

    // Recreate the communicator each time the user logs out.
    [communicator destroy];
    [communicator release];
    
    communicator = [[ICEUtil createCommunicator:self.initData] retain];
}

-(void)sessionRefreshException:(ICEException*)ex
{
    // Go back to the login view. This triggers the viewWillAppear on the
    // LoginViewController, which will invalidate the session.
    [navigationController popToRootViewControllerAnimated:YES];
    
    NSString* s = [NSString stringWithFormat:@"Lost connection with session!\n%@", ex];
    
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];    
}

-(void)refresh:(NSTimer*)timer
{
    [session
     refresh_async:[ICECallbackOnMainThread callbackOnMainThread:self]
     response:nil
     exception:@selector(sessionRefreshException:)];
}

-(void)setSession:(id)sess timeout:(int)timeout
{
    // We must not already have a session.
    NSAssert(session == nil, @"session == nil");

    // Save the new session, and create the refresh timer.
    session = [sess retain];
    self.refreshTimer = [NSTimer
                         timerWithTimeInterval:timeout
                         target:self
                         selector:@selector(refresh:)
                         userInfo:nil
                         repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:refreshTimer forMode:NSDefaultRunLoopMode];
}

-(id)session
{
    return [[session retain] autorelease];
}
@end
