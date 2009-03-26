// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AppDelegate.h>
#import <LoginViewController.h>
#import <Ice/Ice.h>
#import <ChatSession.h>
#import <Glacier2/Session.h>

@interface SessionRefresh : NSObject
{
@private
    
    id session;
}

-(id) initWithSession:(id)session;
+(id) sessionRefreshWithSession:(id)session;

-(void) refresh:(NSTimer*)timer;

@property (nonatomic, retain) id session;
@end

@implementation SessionRefresh

@synthesize session;

-(id)initWithSession:(id)s
{
    if(self = [super init])
    {
        self.session = s;
    }
    return self;
}

+(id)sessionRefreshWithSession:(id)session;
{
    return [[[SessionRefresh alloc] initWithSession:session] autorelease];
}

-(void)refresh:(NSTimer*)timer
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [session
     ice_invoke_async:[ICECallbackOnMainThread callbackOnMainThread:appDelegate]
     response:nil
     exception:@selector(sessionRefreshException:)
     operation:@"ice_ping"
     mode:ICENonmutating
     inParams:nil];
}

-(void)dealloc
{
    [session release];
    [super dealloc];
}
@end

@interface AppDelegate()

@property (nonatomic, retain) ICEInitializationData* initData;
@property (nonatomic, retain) NSTimer* refreshTimer;

@end

@implementation AppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize communicator;
@synthesize initData;
@synthesize refreshTimer;

-(id)init
{
    if(self = [super init])
    {
        self.initData = [ICEInitializationData initializationData];
        self.initData.properties = [ICEUtil createProperties ];
        [self.initData.properties setProperty:@"Ice.ACM.Client" value:@"0"];
        [self.initData.properties setProperty:@"Ice.RetryIntervals" value:@"-1"];
        
        // Tracing properties.
        //[self.initData.properties setProperty:@"Ice.Trace.Network" value:@"1"];
        //[self.initData.properties setProperty:@"Ice.Trace.Protocol" value:@"1"];
        
        [self.initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
        [self.initData.properties setProperty:@"IceSSL.TrustOnly.Client" value:@"C2:E8:D3:33:D7:83:99:6E:08:F7:C2:34:31:F7:1E:8E:44:87:38:57"];
        [self.initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
        
#if TARGET_IPHONE_SIMULATOR
        [self.initData.properties setProperty:@"IceSSL.Keychain" value:@"test"];
        [self.initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
#endif
    }
    return self;
}
- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	// Configure and show the window
	[window addSubview:[navigationController view]];
	[window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [self logout];
    
    [communicator destroy];
}

- (void)dealloc
{
    [communicator release];
    
    [session release];
    [refreshTimer release];
    
    [navigationController release];
    [window release];
    
    [super dealloc];
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
    
    [communicator destroy];
    [communicator release];
    
    communicator = [[ICEUtil createCommunicator:initData] retain];    
}

-(void)setSession:(id)sess timeout:(int)timeout
{
    // We must not already have a session.
    NSAssert(session == nil, @"session == nil");
    
    // Save the new session, and create the refresh timer.
    session = [sess retain];
    SessionRefresh* refresh = [SessionRefresh sessionRefreshWithSession:session];
    // TODO: Use timeout.
    self.refreshTimer = [NSTimer
                         timerWithTimeInterval:timeout
                         target:refresh
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
