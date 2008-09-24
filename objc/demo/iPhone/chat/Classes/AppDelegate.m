// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
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

@property (nonatomic, retain) NSTimer* refreshTimer;
@property (nonatomic, retain) ICEInitializationData* initData;

@end

@implementation AppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize communicator;
@synthesize refreshTimer;
@synthesize initData;

- (id)init
{
	if((self = [super init]))
    {
        self.initData = [ICEInitializationData initializationData];
        self.initData.properties = [ICEUtil createProperties ];
        [self.initData.properties setProperty:@"Ice.ACM.Client" value:@"0"];
        [self.initData.properties setProperty:@"Ice.RetryIntervals" value:@"-1"];
        
        // Tracing properties.
        //[self.initData.properties setProperty:@"Ice.Trace.Network" value:@"1"];
        //[self.initData.properties setProperty:@"Ice.Trace.Protocol" value:@"1"];

        //initData.properties.setProperty("Ice.Plugin.IceSSL", "IceSSL.PluginFactory");
        //initData.properties.setProperty("IceSSL.TrustOnly.Client", "CN=Glacier2");
        //initData.properties.setProperty("Ice.Default.Router", routerEndpoints);
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
    [initData release];
    
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
    
    // Recreate the communicator each time the user logs out.
    [communicator destroy];
    [communicator release];
    
    communicator = [[ICEUtil createCommunicator:self.initData] retain];
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
