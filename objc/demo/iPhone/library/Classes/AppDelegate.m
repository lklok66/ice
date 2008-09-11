// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AppDelegate.h>

#import <Ice/Ice.h>
#import <Session.h>

@interface SessionRefresh : NSObject
{
@private
    
    id<ICELogger> logger_;
    id<DemoSessionPrx> session_;
}
-(id) initWithLogger:(id<ICELogger>)logger session:(id<DemoSessionPrx>)session;
+(id) sessionRefreshWithLogger:(id<ICELogger>)logger session:(id<DemoSessionPrx>)session;

-(void) refresh:(NSTimer*)timer;

@property (nonatomic, retain) id<ICELogger> logger_;
@property (nonatomic, retain) id<DemoSessionPrx> session_;
@end

@implementation SessionRefresh

@synthesize logger_;
@synthesize session_;

-(id) initWithLogger:(id<ICELogger>)logger session:(id<DemoSessionPrx>)session
{
    if(![super init])
    {
        return nil;
    }
    
    self.logger_ = logger;
    self.session_ = session;
    
    return self;
}

+(id) sessionRefreshWithLogger:(id<ICELogger>)logger session:(id<DemoSessionPrx>)session;
{
    return [[[SessionRefresh alloc] initWithLogger:logger session:session] autorelease];
}

-(void) refresh:(NSTimer*)timer
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [session_ refresh_async:[ICECallbackOnMainThread callbackOnMainThread:appDelegate]
              response:nil
              exception:@selector(sessionRefreshException:)];
}

-(void) dealloc
{
    [session_ release];
    [logger_ release];
    [super dealloc];
}
@end

@interface AppDelegate()

@property (nonatomic, retain) NSTimer* refreshTimer_;

@end

@implementation AppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize communicator;
@synthesize refreshTimer_;
@dynamic session;

-(id)init
{
    if (self = [super init])
    {
        self.communicator = [ICEUtil createCommunicator];
    }
    return self;
}

-(void)applicationDidFinishLaunching:(UIApplication *)application
{
    // Configure and show the window
    [window addSubview:navigationController.view];
    [window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    self.session = nil;
    [communicator destroy];
}

- (void)dealloc
{
    [communicator release];
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
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];    
}

-(void)setSession:(id<DemoSessionPrx>)sess
{
    // Destroy the old session, and invalidate the refresh timer.
    if(session != nil)
    {
        [refreshTimer_ invalidate];
        [session destroy_async:nil response:nil exception:nil];
        self.refreshTimer_ = nil;
        [session release];
        session = nil;
    }

    // Save the new session, and create the refresh timer.
    session = [sess retain];
    if(session != nil)
    {
        SessionRefresh* refresh = [SessionRefresh sessionRefreshWithLogger:[communicator getLogger] session:session];
        self.refreshTimer_ = [NSTimer timerWithTimeInterval:5
                                                     target:refresh
                                                   selector:@selector(refresh:)
                                                   userInfo:nil
                                                    repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:refreshTimer_ forMode:NSDefaultRunLoopMode];
    }
}

-(id<DemoSessionPrx>)session
{
    return [[session retain] autorelease];
}
@end
