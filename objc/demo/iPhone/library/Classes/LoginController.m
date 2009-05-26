// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <LoginController.h>
#import <MainController.h>
#import <WaitAlert.h>

#import <Ice/Ice.h>
#import <Library.h>
#import <Session.h>
#import <Glacier2Session.h>
#import <Glacier2/Router.h>

@interface LoginController()

@property (nonatomic, retain) UITextField* currentField;
@property (nonatomic, retain) NSString* oldFieldValue;

@property (nonatomic, retain) WaitAlert* waitAlert;
@property (nonatomic, retain) id<ICECommunicator> communicator;
@property (nonatomic, retain) id session;
@property (nonatomic, retain) id<DemoLibraryPrx> library;
@property (nonatomic, retain) id<Glacier2RouterPrx> router;

@end

@implementation LoginController

@synthesize currentField;
@synthesize oldFieldValue;
@synthesize waitAlert;
@synthesize communicator;
@synthesize session;
@synthesize library;
@synthesize router;

NSString* hostnameKey = @"hostnameKey";
NSString* glacier2Key = @"glacier2Key";
NSString* sslKey = @"sslKey";
NSString* usernameKey = @"usernameKey";
NSString* passwordKey = @"passwordKey";

+(void)initialize
{
    NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:@"demo2.zeroc.com", hostnameKey,
                                 @"", usernameKey,
                                 @"", passwordKey,
                                 @"YES", glacier2Key,
                                 @"YES", sslKey,
                                 nil];
	
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];    
}

-(void)viewDidLoad
{
    initData = [[ICEInitializationData initializationData] retain];
    initData.properties = [ICEUtil createProperties ];
    [initData.properties setProperty:@"Ice.ACM.Client" value:@"0"];
    [initData.properties setProperty:@"Ice.RetryIntervals" value:@"-1"];
    
    // Tracing properties.
    //[initData.properties setProperty:@"Ice.Trace.Network" value:@"1"];
    //[initData.properties setProperty:@"Ice.Trace.Protocol" value:@"1"];
    
    //[initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
    
    [initData.properties setProperty:@"IceSSL.TrustOnly.Client" value:@"11:DD:28:AD:13:44:76:47:4F:BE:3C:4D:AC:AD:5A:06:88:DA:52:DA"];
    [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
    
#if TARGET_IPHONE_SIMULATOR
    [initData.properties setProperty:@"IceSSL.Keychain" value:@"test"];
    [initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
#endif        
    
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    glacier2Switch.on = [defaults boolForKey:glacier2Key];
    sslSwitch.on = [defaults boolForKey:sslKey];
    
    queue = [[NSOperationQueue alloc] init];

    // When the user starts typing, show the clear button in the text field.
    hostnameField.clearButtonMode = UITextFieldViewModeWhileEditing;
    hostnameField.text = [defaults stringForKey:hostnameKey];
    usernameField.clearButtonMode = UITextFieldViewModeWhileEditing;
    usernameField.text = [defaults stringForKey:usernameKey];
    passwordField.clearButtonMode = UITextFieldViewModeWhileEditing;
    passwordField.text = [defaults stringForKey:passwordKey];

    loginButton.enabled = hostnameField.text.length > 0;
    
    mainController = [[MainController alloc] initWithNibName:@"MainView" bundle:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillTerminate) 
                                                 name:UIApplicationWillTerminateNotification
                                               object:nil]; 
}

-(void)applicationWillTerminate
{
    [communicator destroy];
    self.communicator = nil;
}

-(void)viewWillAppear:(BOOL)animated
{
    NSAssert(communicator == nil, @"communicator == nil");
    self.communicator = [[ICEUtil createCommunicator:initData] retain];
    
    [super viewWillAppear:animated];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

-(void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [currentField resignFirstResponder];
    currentField.text = oldFieldValue; 
    self.currentField = nil;
    [super touchesBegan:touches withEvent:event];
}

-(void)dealloc
{
    [hostnameField release];
    [usernameField release];
    [passwordField release];

    [loginButton release];
    [glacier2Switch release];
    [sslSwitch release];
    [mainController release];
    [queue release];
    [waitAlert release];
    [session release];
    [library release];
    [router release];

    [super dealloc];
}

-(void)glacier2Changed:(id)s
{
    UISwitch* sender = (UISwitch*)s;

    [[NSUserDefaults standardUserDefaults] setObject:(sender.isOn ? @"YES" : @"NO") forKey:glacier2Key];
}

-(void)sslChanged:(id)s
{
    UISwitch* sender = (UISwitch*)s;
    
    [[NSUserDefaults standardUserDefaults] setObject:(sender.isOn ? @"YES" : @"NO") forKey:sslKey];
}

#pragma mark UITextFieldDelegate

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
        NSString* s = [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000",
                       theTextField.text];
        @try
        {
            [communicator stringToProxy:s];
        }
        @catch(ICEEndpointParseException* ex)
        {
            UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Invalid Hostname"
                                                             message:@"The provided hostname is invalid."
                                                            delegate:self
                                                   cancelButtonTitle:@"OK"
                                                   otherButtonTitles:nil] autorelease];
            [alert show];
            return NO;
        }
        
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

    loginButton.enabled = hostnameField.text.length > 0;

    [theTextField resignFirstResponder];
    self.currentField = nil;
    
    return YES;
}

#pragma mark Login

-(void)exception:(NSString*)s
{
    [waitAlert dismissWithClickedButtonIndex:0 animated:YES];
    self.waitAlert = nil;

    // Restart the login process in the delegate.
    [communicator destroy];
    self.communicator = [[ICEUtil createCommunicator:initData] retain];    
    
    // open an alert with just an OK button
    UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Error"
                                                     message:s
                                                    delegate:self
                                           cancelButtonTitle:@"OK"
                                           otherButtonTitles:nil] autorelease];
    [alert show];
}

-(void)loginComplete
{
    [waitAlert dismissWithClickedButtonIndex:0 animated:YES];
    self.waitAlert = nil;

    [mainController activate:communicator
                     session:session
                      router:router
           sessionTimeout:sessionTimeout
                     library:library];

    // Clear internal state.
    self.communicator = nil;
    self.session = nil;
    self.library = nil;
    self.router = nil;
    
    [self.navigationController pushViewController:mainController animated:YES];
}

// Runs in a separate thread, called only by NSInvocationOperation.
-(void)doLogin:(id)proxy
{
    @try
    {
        id<DemoSessionFactoryPrx> factory = [DemoSessionFactoryPrx checkedCast:proxy];
        if(factory == nil)
        {
            [self performSelectorOnMainThread:@selector(exception:) withObject:@"Invalid proxy" waitUntilDone:NO];
            return;
        }

        id<DemoSessionPrx> sess = [factory create];

        self.session = sess;
        sessionTimeout = [factory getSessionTimeout];
        self.library = [sess getLibrary];
        
        [self performSelectorOnMainThread:@selector(loginComplete) withObject:nil waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:[ex description] waitUntilDone:NO];
    }
}

// Runs in a separate thread, called only by NSInvocationOperation.
-(void)doGlacier2Login:(id)proxy
{
    @try
    {
        id<Glacier2RouterPrx> glacier2router = [Glacier2RouterPrx uncheckedCast:proxy];
        
        id<Glacier2SessionPrx> glacier2session = [glacier2router createSession:usernameField.text password:passwordField.text];
        id<DemoGlacier2SessionPrx> sess = [DemoGlacier2SessionPrx uncheckedCast:glacier2session];

        self.session = sess;
        self.router = glacier2router;
        sessionTimeout = [glacier2router getSessionTimeout];
        self.library = [sess getLibrary];
        
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
    id<ICEObjectPrx> proxy;
    SEL loginSelector;

    NSString* hostname = hostnameField.text;
    @try
    {
        if(glacier2Switch.isOn)
        {
            NSString* s;
            if(sslSwitch.isOn)
            {
                s = [NSString stringWithFormat:@"DemoGlacier2/router:ssl -h %@ -p 4064 -t 10000", hostname];
            }
            else
            {
                s = [NSString stringWithFormat:@"DemoGlacier2/router:tcp -h %@ -p 4502 -t 10000", hostname];
            }
            proxy = [communicator stringToProxy:s];

            // Configure the default router on the communicator.
            id<ICERouterPrx> r = [ICERouterPrx uncheckedCast:proxy];
            [communicator setDefaultRouter:r];

            loginSelector = @selector(doGlacier2Login:);
        }
        else
        {
            NSString* s;
            if(sslSwitch.isOn)
            {
                s = [NSString stringWithFormat:@"SessionFactory:ssl -h %@ -p 10001 -t 10000", hostname];
            }
            else
            {
                s = [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000 -t 10000", hostname];
            }
            proxy = [communicator stringToProxy:s];

            loginSelector = @selector(doLogin:);
        }
    }
    @catch(ICEEndpointParseException* ex)
    {
        UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Invalid Hostname"
                                                         message:@"The provided hostname is invalid."
                                                        delegate:self
                                               cancelButtonTitle:@"OK"
                                               otherButtonTitles:nil] autorelease];
        [alert show];
        return;
    }
    
    // Show the wait alert.
    self.waitAlert = [[WaitAlert alloc] init];
    waitAlert.text = @"Connecting...";
    [waitAlert show];
    
    // Kick off the login process in a separate thread. This ensures that the UI is not blocked.
    NSInvocationOperation* op = [[[NSInvocationOperation alloc] initWithTarget:self
                                                                     selector:loginSelector
                                                                        object:proxy] autorelease];
    [queue addOperation:op];
}

@end
