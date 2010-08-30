// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <LoginController.h>
#import <ChatController.h>
#import <WaitAlert.h>

#import <Ice/Ice.h>
#import <ChatSession.h>
#import <Glacier2/Router.h>

@interface LoginController()

@property (nonatomic, retain) UITextField* currentField;
@property (nonatomic, retain) NSString* oldFieldValue;
@property (nonatomic, retain) WaitAlert* waitAlert;
@property (nonatomic, retain) id<ICECommunicator> communicator;

@end

@implementation LoginController

@synthesize currentField;
@synthesize oldFieldValue;
@synthesize waitAlert;
@synthesize communicator;

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

- (void)viewDidLoad
{
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
    
    chatController = [[ChatController alloc] initWithNibName:@"ChatView" bundle:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillTerminate) 
                                                 name:UIApplicationWillTerminateNotification
                                               object:nil]; 
}

-(void)applicationWillTerminate
{
    [communicator destroy];
}

- (void)viewWillAppear:(BOOL)animated
{
    loginButton.enabled = hostnameField.text.length > 0 && usernameField.text.length > 0;
	[super viewWillAppear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	// Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}

- (void)dealloc
{
    [hostnameField release];
    [usernameField release];
    [passwordField release];
    [loginButton release];
    [sslSwitch release];
    
    [currentField release];
    [oldFieldValue release];
    [chatController release];
    [queue release];
    [communicator release];
    
	[super dealloc];
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

    // The communicator is now owned by the ChatController.
    self.communicator = nil;

    [chatController activate:hostnameField.text];
    [self.navigationController pushViewController:chatController animated:YES];
}

// Runs in a separate thread, called only by NSInvocationOperation.
-(void)doGlacier2Login
{
    @try
    {
        id<Glacier2RouterPrx> router = [Glacier2RouterPrx checkedCast:[communicator getDefaultRouter]];
        id<Glacier2SessionPrx> glacier2session = [router createSession:usernameField.text password:passwordField.text];
        id<ChatChatSessionPrx> sess = [ChatChatSessionPrx uncheckedCast:glacier2session];
        [chatController setup:communicator
                      session:sess
               sessionTimeout:[router getSessionTimeout]
                       router:router
                     category:[router getCategoryForClient]];
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
    ICEInitializationData* initData = [ICEInitializationData initializationData];
    
    initData.properties = [ICEUtil createProperties];
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
            s = [NSString stringWithFormat:@"Glacier2/router:tcp -p 4502 -h %@ -t 10000", hostnameField.text];
        }
        id<ICEObjectPrx> proxy = [communicator stringToProxy:s];
        id<ICERouterPrx> router = [ICERouterPrx uncheckedCast:proxy];
        
        // Configure the default router on the communicator.
        [communicator setDefaultRouter:router];
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

@end

