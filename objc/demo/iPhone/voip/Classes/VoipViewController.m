// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <VoipViewController.h>

#import <Ice/Ice.h>
#import <Glacier2/Router.h>
#import <Voip.h>
#import <WaitAlert.h>

@interface VoipViewController()

@property (nonatomic, retain) UITextField* currentField;
@property (nonatomic, retain) NSString* oldFieldValue;
@property (nonatomic, retain) WaitAlert* waitAlert;
@property (nonatomic, retain) id<ICECommunicator> communicator;

@property (nonatomic, retain) NSString* category;
@property (nonatomic, retain) id<VoipSessionPrx> sess;
@property (nonatomic, retain) id<Glacier2RouterPrx> router;
@property (nonatomic, retain) id<VoipControlPrx> controlPrx;

@property (nonatomic, retain) NSTimer* refreshTimer;
@end

@implementation VoipViewController

@synthesize currentField;
@synthesize oldFieldValue;
@synthesize waitAlert;
@synthesize communicator;
@synthesize category;
@synthesize sess;
@synthesize router;
@synthesize controlPrx;
@synthesize refreshTimer;

static NSString* hostnameKey = @"hostnameKey";
static NSString* usernameKey = @"usernameKey";
static NSString* passwordKey = @"passwordKey";
static NSString* sslKey = @"sslKey";

+(void)initialize
{
    NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:@"demo.zeroc.com", hostnameKey,
                                 @"", usernameKey,
                                 @"", passwordKey,
                                 @"YES", sslKey,
                                 nil];
    
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
}

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


- (void)viewDidLoad {
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    queue = [[NSOperationQueue alloc] init];
	
    // Set the default values, and show the clear button in the text field.
    hostnameField.text = [defaults stringForKey:hostnameKey];
    hostnameField.clearButtonMode = UITextFieldViewModeWhileEditing;
    usernameField.text =  [defaults stringForKey:usernameKey];
    usernameField.clearButtonMode = UITextFieldViewModeWhileEditing;
    passwordField.text = [defaults stringForKey:passwordKey];
    passwordField.clearButtonMode = UITextFieldViewModeWhileEditing;
    
#if TARGET_IPHONE_SIMULATOR
    sslSwitch.userInteractionEnabled = NO;
    sslSwitch.on = NO;
#else    
    sslSwitch.on = [defaults boolForKey:sslKey];
#endif

	callButton.enabled = NO;

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillTerminate) 
                                                 name:UIApplicationWillTerminateNotification
                                               object:nil]; 
	
	[[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didEnterBackground) 
                                                 name:UIApplicationDidEnterBackgroundNotification
                                               object:nil]; 

	[[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(willEnterForeground) 
                                                 name:UIApplicationWillEnterForegroundNotification
                                               object:nil]; 
	
    [super viewDidLoad];
}

-(void)applicationWillTerminate
{
	NSLog(@"applicationWillTerminate");
	// TODO: Logout.
    [communicator destroy];
}

- (void)viewWillAppear:(BOOL)animated
{
    loginButton.enabled = hostnameField.text.length > 0 && usernameField.text.length > 0;
	[super viewWillAppear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)dealloc {
	[hostnameField release];
    [usernameField release];
    [passwordField release];
    [loginButton release];
	[callButton release];
    [sslSwitch release];
    
    [currentField release];
    [oldFieldValue release];
    [queue release];
    [communicator release];
	[router release];
	[controlPrx release];
	[sess release];
    [super dealloc];
}

#pragma mark State transition

- (void)didEnterBackground
{
	NSLog(@"applicationDidEnterBackground");
	// Disable the refresh timer.
	[refreshTimer invalidate];
    self.refreshTimer = nil;

	if(sess != nil) {
		// Timeout. Minimum is 601 seconds.
		int timeout = sessionTimeout/2;
		if(timeout <= 600) {
			timeout = 601;
		}
		// Setup the session refresh timer.
		[[UIApplication sharedApplication] setKeepAliveTimeout:sessionTimeout/2 handler:^{
			// Note that this is blocking.
			if(sess != nil)
			{
				@try
				{
					[sess refresh];
				}
				@catch(ICEException* ex)
				{
					// TODO: Refactor.
					// Kill the session and such.
					self.sess = nil;
					// 
					// TOOD: Pop warning dialog.
				}			
			}
		}];
	}
}

- (void)willEnterForeground
{
	NSLog(@"applicationWillEnterForeground");
	// Disable the keep alive timer.
	[[UIApplication sharedApplication] clearKeepAliveTimeout];

	if(sess != nil)
	{
		// Setup the session refresh timer.
		self.refreshTimer = [NSTimer timerWithTimeInterval:sessionTimeout/2
													target:self
												  selector:@selector(refreshSession:)
												  userInfo:nil
												   repeats:YES];
		[[NSRunLoop currentRunLoop] addTimer:refreshTimer forMode:NSDefaultRunLoopMode];
	}
}

#pragma mark Text Field support

-(BOOL)textFieldShouldBeginEditing:(UITextField*)field
{
    self.currentField = field;
    self.oldFieldValue = field.text;
    return YES;
}

-(BOOL)textFieldShouldReturn:(UITextField*)theTextField
{
    NSAssert(theTextField == currentField, @"theTextField == currentTextField");
    
    // When the user presses return, take focus away from the text
    // field so that the keyboard is dismissed.
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults]; 
    if(theTextField == hostnameField)
    {
        [defaults setObject:theTextField.text forKey:hostnameKey];
    }
    else if(theTextField == usernameField)
    {
        [defaults setObject:theTextField.text forKey:usernameKey];
    }
    else if(theTextField == passwordField)
    {
        [defaults setObject:theTextField.text forKey:passwordKey];
    }
    loginButton.enabled = hostnameField.text.length > 0 && usernameField.text.length > 0;
	
    [theTextField resignFirstResponder];
    self.currentField = nil;
	
    return YES;
}

#pragma mark -

// A touch outside the keyboard dismisses the keyboard, and
// sets back the old field value.
-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    [currentField resignFirstResponder];
    currentField.text = oldFieldValue; 
    self.currentField = nil;
    [super touchesBegan:touches withEvent:event];
}

#pragma mark UI Actions

-(IBAction)sslChanged:(id)s
{
    UISwitch* sender = (UISwitch*)s;
    [[NSUserDefaults standardUserDefaults] setBool:sender.isOn forKey:sslKey];
}

#pragma mark Login

-(void)exception:(NSString*)s
{
    [waitAlert dismissWithClickedButtonIndex:0 animated:YES];
    self.waitAlert = nil;
	
    // We always create a new communicator each time
    // we try to login.
    [communicator destroy];
    self.communicator = nil;
	
    loginButton.enabled = hostnameField.text.length > 0 && usernameField.text.length > 0;
    
    // open an alert with just an OK button
    UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Error"
                                                     message:s
                                                    delegate:nil
                                           cancelButtonTitle:@"OK"
                                           otherButtonTitles:nil] autorelease];
    [alert show];       
}

-(void)loginComplete
{
    [waitAlert dismissWithClickedButtonIndex:0 animated:YES];
    self.waitAlert = nil;
	
	loginButton.titleLabel.text = @"Logout";
	callButton.enabled = YES;

	id<ICEObjectAdapter> adapter = [communicator createObjectAdapterWithRouter:@"VoipClient"
                                                                        router:router];
    [adapter activate];
    
    // Here we tie the chat view controller to the ChatRoomCallback servant.
    VoipControl* callbackImpl = [VoipControl objectWithDelegate:self];
    
    // This helper ensures that all methods are dispatched in the main thread.
    ICEObject* dispatchMainThread = [ICEMainThreadDispatch mainThreadDispatch:callbackImpl];
	
    ICEIdentity* callbackId = [ICEIdentity identity:[ICEUtil generateUUID] category:category];
	
    // The callback is registered in clear:, otherwise the callbacks can arrive
    // prior to the IBOutlet connections being setup.
    self.controlPrx = [VoipControlPrx uncheckedCast:[adapter add:dispatchMainThread identity:callbackId]];

    // Register the chat callback.
    [sess setControl_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                      response:nil
                     exception:@selector(exception:)
                            ctrl:controlPrx];

	// Setup the session refresh timer.
    self.refreshTimer = [NSTimer timerWithTimeInterval:sessionTimeout/2
                                                target:self
                                              selector:@selector(refreshSession:)
                                              userInfo:nil
                                               repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:refreshTimer forMode:NSDefaultRunLoopMode];
}

// Runs in a separate thread, called only by NSInvocationOperation.
-(void)doGlacier2Login
{
    @try
    {
		self.router = [Glacier2RouterPrx checkedCast:[communicator getDefaultRouter]];
        id<Glacier2SessionPrx> glacier2session = [router createSession:usernameField.text password:passwordField.text];
		self.sess = [VoipSessionPrx uncheckedCast:glacier2session];
		sessionTimeout = [router getSessionTimeout];
		self.category = [router getCategoryForClient];
		self.router = router;
		
        [self performSelectorOnMainThread:@selector(loginComplete) withObject:nil waitUntilDone:NO];
    }
    @catch(Glacier2CannotCreateSessionException* ex)
    {
        NSString* s = [NSString stringWithFormat:@"Session creation failed: %@", ex.reason_];
        [self performSelectorOnMainThread:@selector(exception:) withObject:s waitUntilDone:NO];
    }
    @catch(Glacier2PermissionDeniedException* ex)
    {
        NSString* s = [NSString stringWithFormat:@"Login failed: %@", ex.reason_];
        [self performSelectorOnMainThread:@selector(exception:) withObject:s waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:[ex description] waitUntilDone:NO];
    }
}

-(IBAction)login:(id)sender
{
	if(communicator != nil)
	{
		//		[refreshTimer invalidate];
		//		self.refreshTimer = nil;
		
		// Destroy the old session, and invalidate the refresh timer.
		[router destroySession_async:self response:nil exception:nil];
		self.router = nil;
		self.sess = nil;
		
		[communicator destroy];
		self.communicator = nil;
		
		callButton.enabled = NO;
		
		return;
	}
	
    ICEInitializationData* initData = [ICEInitializationData initializationData];
    
    initData.properties = [ICEUtil createProperties];
	[initData.properties setProperty:@"Ice.Voip" value:@"1"];
    [initData.properties setProperty:@"Ice.ACM.Client" value:@"0"];
    [initData.properties setProperty:@"Ice.RetryIntervals" value:@"-1"];
    
    // Tracing properties.
    //[self.initData.properties setProperty:@"Ice.Trace.Network" value:@"1"];
    //[self.initData.properties setProperty:@"Ice.Trace.Protocol" value:@"1"];
    
    [initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
    [initData.properties setProperty:@"IceSSL.TrustOnly.Client" value:@"C2:E8:D3:33:D7:83:99:6E:08:F7:C2:34:31:F7:1E:8E:44:87:38:57"];
    [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
    
#if TARGET_IPHONE_SIMULATOR && !__IPHONE_3_0
    [initData.properties setProperty:@"IceSSL.Keychain" value:@"test"];
    [initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
#endif
    
    NSAssert(communicator == nil, @"communicator == nil");
    self.communicator = [ICEUtil createCommunicator:initData];
	
    @try
    {
        NSString* s;
        if(sslSwitch.isOn)
        {
            s = [NSString stringWithFormat:@"Glacier2/router:ssl -p 4064 -h %@ -t 10000", hostnameField.text];
        }
        else
        {
            s = [NSString stringWithFormat:@"Glacier2/router:tcp -p 4063 -h %@ -t 10000", hostnameField.text];
        }
        id<ICEObjectPrx> proxy = [communicator stringToProxy:s];
        id<ICERouterPrx> r = [ICERouterPrx uncheckedCast:proxy];
        
        // Configure the default router on the communicator.
        [communicator setDefaultRouter:r];
    }
    @catch(ICEEndpointParseException* ex)
    {
        UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Invalid Hostname"
                                                         message:@"The provided hostname is invalid."
                                                        delegate:nil
                                               cancelButtonTitle:@"OK"
                                               otherButtonTitles:nil] autorelease];
        [alert show];
		
        [communicator destroy];
        self.communicator = nil;
        return;
    }
    
    // Show the wait alert.
    self.waitAlert = [[WaitAlert alloc] init];
    waitAlert.text = @"Connecting...";
    [waitAlert show];
    
    // Kick off the login process in a separate thread. This ensures that the UI is not blocked.
    NSInvocationOperation* op = [[[NSInvocationOperation alloc] initWithTarget:self
                                                                      selector:@selector(doGlacier2Login)
                                                                        object:nil] autorelease];
    [queue addOperation:op];
}

-(void)refreshException:(ICEException*)ex
{
	[self exception:[ex description]];
}

-(void)refreshSession:(NSTimer*)timer
{
	if(sess != nil)
	{
		[sess refresh_async:[ICECallbackOnMainThread callbackOnMainThread:self]
				   response:nil
				  exception:@selector(refreshException:)];
	}
}

#pragma mark Call

-(IBAction)call:(id)sender
{
	[sess simulateCall_async:[ICECallbackOnMainThread callbackOnMainThread:self] response:nil exception:@selector(exception:)];
}

#pragma mark VoipControl

-(void)incomingCall:(ICECurrent*)current
{
	NSLog(@"incoming call");
	UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    if (localNotif != nil)
	{
		NSDate* now = [NSDate date];
		localNotif.fireDate = [now dateByAddingTimeInterval:1];
		localNotif.timeZone = [NSTimeZone defaultTimeZone];
	
		localNotif.alertBody = [NSString stringWithFormat:@"Incoming call at %@.", now];
		localNotif.alertAction = @"View Details";
	
		localNotif.soundName = UILocalNotificationDefaultSoundName;
		localNotif.applicationIconBadgeNumber = 1;
	
		[[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
		[localNotif release];
	}
}
@end
