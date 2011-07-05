// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <VoipViewController.h>

#import <Voip.h>
#import <Ice/Ice.h>
#import <Glacier2/Router.h>
#import <WaitAlert.h>

@interface VoipViewController()

@property (nonatomic, retain) UITextField* currentField;
@property (nonatomic, retain) NSString* oldFieldValue;
@property (nonatomic, retain) WaitAlert* waitAlert;
@property (nonatomic, retain) id<ICECommunicator> communicator;

@property (nonatomic, retain) id<VoipSessionPrx> session;
@property (nonatomic, retain) id<Glacier2RouterPrx> router;

@property (nonatomic, retain) NSTimer* refreshTimer;
@end

@implementation VoipViewController

@synthesize currentField;
@synthesize oldFieldValue;
@synthesize waitAlert;
@synthesize communicator;
@synthesize session;
@synthesize router;
@synthesize refreshTimer;

static NSString* hostnameKey = @"hostnameKey";
static NSString* usernameKey = @"usernameKey";
static NSString* passwordKey = @"passwordKey";
static NSString* sslKey = @"sslKey";
static NSString* defaultHost = @"demo2.zeroc.com";

+(void)initialize
{
    NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:defaultHost, hostnameKey,
                                 @"", usernameKey,
                                 @"", passwordKey,
                                 @"YES", sslKey,
                                 nil];
    
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
}

- (void)viewDidLoad {
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	
    // Set the default values, and show the clear button in the text field.
    hostnameField.text = [defaults stringForKey:hostnameKey];
    hostnameField.clearButtonMode = UITextFieldViewModeWhileEditing;
    usernameField.text =  [defaults stringForKey:usernameKey];
    usernameField.clearButtonMode = UITextFieldViewModeWhileEditing;
    passwordField.text = [defaults stringForKey:passwordKey];
    passwordField.clearButtonMode = UITextFieldViewModeWhileEditing;
    
    sslSwitch.on = [defaults boolForKey:sslKey];

	callButton.enabled = NO;
    [callButton setAlpha:0.5];

	[[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didEnterBackground) 
                                                 name:UIApplicationDidEnterBackgroundNotification
                                               object:nil]; 

	[[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(willEnterForeground) 
                                                 name:UIApplicationWillEnterForegroundNotification
                                               object:nil]; 
	
    // This generates a compile time warning, but does actually work!
    [delaySlider setShowValue:YES];
    
    loginButton.enabled = hostnameField.text.length > 0 && usernameField.text.length > 0;
    [loginButton setAlpha:loginButton.enabled ? 1.0 : 0.5];

    [super viewDidLoad];
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

- (void)dealloc {
	[hostnameField release];
    [usernameField release];
    [passwordField release];
    [loginButton release];
	[callButton release];
    [sslSwitch release];
    [delaySlider release];
    
    [currentField release];
    [oldFieldValue release];
    [waitAlert release];

    [refreshTimer release];
    [communicator release];
	[session release];
	[router release];

    [super dealloc];
}

#pragma mark Session Management

-(void)destroySession
{
    UIApplication* app = [UIApplication sharedApplication];
    if(app.applicationState ==  UIApplicationStateBackground)
    {
        [app clearKeepAliveTimeout];
    }
    else
    {
        // Invalidate the refresh timer.
        [refreshTimer invalidate];
        self.refreshTimer = nil;        
    }
    self.session = nil;

    callButton.enabled = NO;
    [callButton setAlpha:0.5];
    loginButton.enabled = hostnameField.text.length > 0 && usernameField.text.length > 0;
    [loginButton setAlpha:loginButton.enabled ? 1.0 : 0.5];
    [loginButton setTitle:@"Login" forState:UIControlStateNormal];
    
    // Destroy the session and destroy the communicator from another thread since these
    // calls block.
    id<ICECommunicator> c = [communicator retain];
    id<Glacier2RouterPrx> r = [router retain];
    self.router = nil;
    self.communicator = nil;
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^ {
        @try
        {
            [r destroySession];
        }
        @catch (ICEException* ex) {
        }
        
        @try
        {            
            [c destroy];
        }
        @catch (ICEException* ex) {
        }
        [r release];
        [c release];
    });
}

-(void)exception:(NSString*)s
{
    [waitAlert dismissWithClickedButtonIndex:0 animated:YES];
    self.waitAlert = nil;
    
    [self destroySession];

    UIApplication* app = [UIApplication sharedApplication];
    if(app.applicationState != UIApplicationStateBackground)
    {
        // open an alert with just an OK button
        UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Error"
                                                         message:s
                                                        delegate:nil
                                               cancelButtonTitle:@"OK"
                                               otherButtonTitles:nil] autorelease];
        [alert show];
    }
}

-(void)refreshSession:(NSTimer*)timer
{
	if(session != nil)
	{
		[session begin_refresh:nil exception:^(ICEException* ex) { [self exception:[ex description]]; }];
	}
}

-(void)setupRefresh:(BOOL)background
{
    NSAssert(session != nil, @"sess is nil");
    
    // Timeout. Minimum is 601 seconds.
    int timeout = sessionTimeout/2;
    if(timeout <= 600) {
        timeout = 601;
    }
    
    UIApplication* app = [UIApplication sharedApplication];
    if(background)
    {
        // Setup the session refresh timer.
		[app setKeepAliveTimeout:sessionTimeout/2 handler:^{
			// Note that this is blocking.
			if(session != nil)
			{
				@try
				{
					[session refresh];
				}
				@catch(ICEException* ex)
				{
                    [self destroySession];

                    NSDate* now = [NSDate date];
                    UILocalNotification *localNotif = [[[UILocalNotification alloc] init] autorelease];
                    if (localNotif != nil)
                    {
                        localNotif.fireDate = [now dateByAddingTimeInterval:1];
                        localNotif.timeZone = [NSTimeZone defaultTimeZone];
                        
                        localNotif.alertBody = [NSString stringWithFormat:@"Lost connection: %@.", [ex description]];
                        
                        localNotif.soundName = UILocalNotificationDefaultSoundName;
                        
                        [app scheduleLocalNotification:localNotif];
                    }
                }			
			}
		}];
    }
    else
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

#pragma mark State transition

- (void)didEnterBackground
{
	// Disable the refresh timer.
	[refreshTimer invalidate];
    self.refreshTimer = nil;

    if(session != nil)
    {
        [self setupRefresh:YES];
    }
}

- (void)willEnterForeground
{
	// Disable the keep alive timer.
	[[UIApplication sharedApplication] clearKeepAliveTimeout];
    if(session != nil)
    {
        [self setupRefresh:NO];
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
    [loginButton setAlpha:loginButton.enabled ? 1.0 : 0.5];
	
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

-(IBAction)login:(id)sender
{
    // Logout.
	if(communicator != nil)
	{
        [self destroySession];		
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
    
    NSString *hostname = [hostnameField.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    
    // Setup the SSL certificates depending on the which server host we are
    // connecting with.
    int portOffset = 0;
    if([hostname caseInsensitiveCompare:@"demo2.zeroc.com"] == NSOrderedSame)
    {
        portOffset = 100;
        [initData.properties setProperty:@"IceSSL.TrustOnly.Client"
                                   value:@"11:DD:28:AD:13:44:76:47:4F:BE:3C:4D:AC:AD:5A:06:88:DA:52:DA"];
        [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
    }
    else
    {
        [initData.properties setProperty:@"IceSSL.TrustOnly.Client"
                                   value:@"75:FA:B7:3C:6B:1C:F8:FA:69:4B:75:A0:22:51:B2:AC:11:54:A7:E7"];
        [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"democacert.der"];
    }
        
    initData.dispatcher = ^(id<ICEDispatcherCall> call, id<ICEConnection> con) {
        dispatch_sync(dispatch_get_main_queue(), ^ { [call run]; });
    };
    
    NSAssert(communicator == nil, @"communicator == nil");
    self.communicator = [ICEUtil createCommunicator:initData];
    
    @try
    {
        NSString* s;
        if(sslSwitch.isOn)
        {
            s = [NSString stringWithFormat:@"Glacier2/router:ssl -p %d -h %@ -t 10000", 4064 + portOffset, hostname];
        }
        else
        {
            s = [NSString stringWithFormat:@"Glacier2/router:tcp -p %d -h %@ -t 10000", 4063 + portOffset, hostname];
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
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^ {
        @try
        {
            id<Glacier2RouterPrx> r = [Glacier2RouterPrx checkedCast:[communicator getDefaultRouter]];
            id<Glacier2SessionPrx> glacier2session = [r createSession:usernameField.text password:passwordField.text];
            id<VoipSessionPrx> s = [VoipSessionPrx uncheckedCast:glacier2session];
            int timeout = [r getSessionTimeout];

            id<ICEObjectAdapter> adapter = [communicator createObjectAdapterWithRouter:@"VoipClient"
                                                                                router:r];
            [adapter activate];
            
            VoipControl* callbackImpl = [VoipControl objectWithDelegate:self];
            
            ICEIdentity* callbackId = [ICEIdentity identity:[ICEUtil generateUUID] category:[r getCategoryForClient]];
            
            // Register the chat callback.
            [s setControl:[VoipControlPrx uncheckedCast:[adapter add:callbackImpl identity:callbackId]]];
            
            dispatch_async(dispatch_get_main_queue(), ^ {

                self.router = r;
                self.session = s;
                sessionTimeout = timeout;

                [waitAlert dismissWithClickedButtonIndex:0 animated:YES];
                self.waitAlert = nil;
                
                [loginButton setTitle:@"Logout" forState:UIControlStateNormal];
                callButton.enabled = YES;
                [callButton setAlpha:1.0];
                
                [self setupRefresh:[UIApplication sharedApplication].applicationState == UIApplicationStateBackground];
            });
        }
        @catch(Glacier2CannotCreateSessionException* ex)
        {
            NSString* s = [NSString stringWithFormat:@"Session creation failed: %@", ex.reason_];
            dispatch_async(dispatch_get_main_queue(), ^ { [self exception:s]; });
        }
        @catch(Glacier2PermissionDeniedException* ex)
        {
            NSString* s = [NSString stringWithFormat:@"Login failed: %@", ex.reason_];
            dispatch_async(dispatch_get_main_queue(), ^ { [self exception:s]; });
        }
        @catch(ICEException* ex)
        {
            dispatch_async(dispatch_get_main_queue(), ^ { [self exception:[ex description]]; });
        }        
    });
}


#pragma mark Call

-(IBAction)call:(id)sender
{
    if(session != nil)
    {
        int delay = (int)(delaySlider.value * 1000.0f); // Convert to ms.

        [session begin_simulateCall:delay response:nil exception:^(ICEException* ex) { [self exception:[ex description]]; }];
    }
}

#pragma mark VoipControl

-(void)incomingCall:(ICECurrent*)current
{
    NSDate* now = [NSDate date];

    UIApplication* app = [UIApplication sharedApplication];
    if(app.applicationState ==  UIApplicationStateBackground)
    {
        UILocalNotification *localNotif = [[[UILocalNotification alloc] init] autorelease];
        if (localNotif != nil)
        {
            localNotif.fireDate = [now dateByAddingTimeInterval:1];
            localNotif.timeZone = [NSTimeZone defaultTimeZone];
        
            localNotif.alertBody = [NSString stringWithFormat:@"Incoming call at %@.", now];
        
            localNotif.soundName = UILocalNotificationDefaultSoundName;
        
            [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
        }
    }
    else
    {
        // open an alert with just an OK button
        UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Call"
                                                         message:[NSString stringWithFormat:@"Incoming call at %@.", now]
                                                        delegate:nil
                                               cancelButtonTitle:@"OK"
                                               otherButtonTitles:nil] autorelease];
        [alert show];
    }
}
@end
