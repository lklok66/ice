// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <LoginViewController.h>
#import <ChatViewController.h>
#import <AppDelegate.h>

#import <ChatRoomCallbackI.h>

#import <Ice/Ice.h>
#import <ChatSession.h>
#import <Glacier2/Router.h>

@interface LoginViewController()

@property (nonatomic, retain) UITextField* hostnameTextField;
@property (nonatomic, retain) UITextField* usernameTextField;
@property (nonatomic, retain) UITextField* passwordTextField;
@property (nonatomic, retain) UIButton* loginButton;
@property (nonatomic, retain) UISwitch* sslSwitch;

@property (nonatomic, retain) NSString* hostname;
@property (retain) NSString* username;
@property (retain) NSString* password;

@property (nonatomic, retain) UITextField* currentTextField;
@property (nonatomic, retain) NSString* oldTextFieldValue;

@property (retain) ChatViewController* chatViewController;

@property (nonatomic, retain) NSOperationQueue* queue;

@property (nonatomic, retain)  id<ChatChatSessionPrx> session;
@property (nonatomic)  int sessionTimeout;

@end

@implementation LoginViewController

@synthesize hostnameTextField;
@synthesize usernameTextField;
@synthesize passwordTextField;
@synthesize loginButton;
@synthesize sslSwitch;
@synthesize hostname;
@synthesize username;
@synthesize password;

@synthesize currentTextField;
@synthesize oldTextFieldValue;
@synthesize chatViewController;
@synthesize queue;
@synthesize session;
@synthesize sessionTimeout;

NSString* hostnameKey = @"hostnameKey";
NSString* usernameKey = @"usernameKey";
NSString* passwordKey = @"passwordKey";
NSString* sslKey = @"sslKey";

- (void)viewDidLoad
{
    // Initialize the application defaults.
    NSString* testValue = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    if(testValue == nil)
    {
        NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:@"demo.zeroc.com", hostnameKey,
                                     @"", usernameKey,
                                     @"", passwordKey,
                                     @"YES", sslKey,
                                     nil];
        
        [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    
    self.queue = [[[NSOperationQueue alloc] init] autorelease];

    hostnameTextField.text = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    hostnameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;

    usernameTextField.text =  [[NSUserDefaults standardUserDefaults] stringForKey:usernameKey];
    usernameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;

    passwordTextField.text = [[NSUserDefaults standardUserDefaults] stringForKey:passwordKey];
    passwordTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
    
    sslSwitch.on =[[NSUserDefaults standardUserDefaults] boolForKey:sslKey]; 

    showAlert = NO;
}

- (void)viewWillAppear:(BOOL)animated
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [appDelegate logout];

    ChatViewController* controller = chatViewController;
    [controller clear];

    loginButton.enabled = hostnameTextField.text.length > 0 && usernameTextField.text.length > 0;

	[super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    [currentTextField resignFirstResponder];
    currentTextField.text = oldTextFieldValue; 
    self.currentTextField = nil;
    [super touchesBegan:touches withEvent:event];
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
    [hostnameTextField release];
    [usernameTextField release];
    [passwordTextField release];
    [loginButton release];
    [sslSwitch release];
    
    [hostname release];
    [username release];
    [password release];
    [currentTextField release];
    [oldTextFieldValue release];
    [chatViewController release];
    [queue release];
    [session release];
	[super dealloc];
}

-(ChatViewController*)chatViewController
{
    // Instantiate the main view controller if necessary.
    if (chatViewController == nil)
    {
        chatViewController = [[ChatViewController alloc] initWithNibName:@"ChatView" bundle:nil];
    }
    return chatViewController;
}

-(IBAction)sslChanged:(id)sender
{
    [[NSUserDefaults standardUserDefaults] setObject:(sslSwitch.isOn ? @"YES" : @"NO") forKey:sslKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

#pragma mark UIAlertViewDelegate

-(void)didPresentAlertView:(UIAlertView *)alertView
{
    showAlert = YES;
}

-(void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    showAlert = NO;
}

#pragma mark UITextFieldDelegate

-(BOOL)textFieldShouldBeginEditing:(UITextField*)field
{
    self.currentTextField = field;
    self.oldTextFieldValue = field.text;
    return YES;
}

-(BOOL)textFieldShouldReturn:(UITextField*)theTextField
{
    NSAssert(theTextField == currentTextField, @"theTextField == currentTextField");
    
    // When the user presses return, take focus away from the text
    // field so that the keyboard is dismissed.
    if(theTextField == hostnameTextField)
    {
        // The exact string doesn't matter as long as the hostname validates as correct.
        NSString* s = [NSString stringWithFormat:@"Glacier2/router:tcp -p 4064 -h %@ -t 10000", theTextField.text];
        @try
        {
            AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
            [appDelegate.communicator stringToProxy:s];
        }
        @catch(ICEEndpointParseException* ex)
        {
            UIAlertView *alert = [[UIAlertView alloc]
                                  initWithTitle:@"Invalid Hostname"
                                  message:@"The provided hostname is invalid."
                                  delegate:self cancelButtonTitle:@"OK"
                                  otherButtonTitles:nil];
            [alert show];
            [alert release];
            return NO;
        }
        
        [[NSUserDefaults standardUserDefaults] setObject:theTextField.text forKey:hostnameKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    else if(theTextField == usernameTextField)
    {
        [[NSUserDefaults standardUserDefaults] setObject:theTextField.text forKey:usernameKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    else if(theTextField == passwordTextField)
    {
        [[NSUserDefaults standardUserDefaults] setObject:theTextField.text forKey:passwordKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    loginButton.enabled = hostnameTextField.text.length > 0 && usernameTextField.text.length > 0;

    [theTextField resignFirstResponder];
    self.currentTextField = nil;

    return YES;
}

#pragma mark Login

-(void)exception:(NSString*)s
{
    // Restart the login process in the delegate.
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [appDelegate logout];
 
    loginButton.enabled = hostnameTextField.text.length > 0 && usernameTextField.text.length > 0;
    
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];
}

-(void)loginComplete:(id)data
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    [appDelegate setSession:session timeout:sessionTimeout/2];
    
    ChatViewController* controller = chatViewController;
    controller.session = session;
    [self.navigationController pushViewController:controller animated:YES];
    
    // Re-enable the login button.
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
}

// Runs in a separate thread, called only by NSInvocationOperation.
-(void)doGlacier2Login:(id)proxy
{
    @try
    {
        id<Glacier2RouterPrx> router = [Glacier2RouterPrx uncheckedCast:proxy];
        id<Glacier2SessionPrx> glacier2session = [router createSession:username password:password];
        id<ChatChatSessionPrx> sess = [ChatChatSessionPrx uncheckedCast:glacier2session];
        
        self.session = sess;
        self.sessionTimeout = [router getSessionTimeout];

        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        id<ICEObjectAdapter> adapter = [appDelegate.communicator
                                        createObjectAdapterWithRouter:@"ChatDemo.Client"
                                        router:router];
        
        ICEIdentity* callbackId = [ICEIdentity identity:[ICEUtil generateUUID] category:[router getCategoryForClient]];
        id<ICEObjectPrx> proxy = [adapter
                                  add:[ChatRoomCallbackI chatRoomCallbackWithTarget:self.chatViewController]
                                  identity:callbackId];

        [session setCallback:[ChatChatRoomCallbackPrx uncheckedCast:proxy]];
        
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:nil waitUntilDone:NO];
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
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    id<ICEObjectPrx> proxy;
    
    // Save the text field properties. We do that here since the text fields could be
    // changed during the login process.
    self.hostname = hostnameTextField.text;
    self.username = usernameTextField.text;
    self.password = passwordTextField.text;

    @try
    {
        NSString* s;
        if(sslSwitch.isOn)
        {
            s = [NSString stringWithFormat:@"Glacier2/router:ssl -p 4064 -h %@ -t 10000", hostname];
        }
        else
        {
            s = [NSString stringWithFormat:@"Glacier2/router:tcp -p 4502 -h %@ -t 10000", hostname];
        }
        proxy = [appDelegate.communicator stringToProxy:s];
        id<ICERouterPrx> router = [ICERouterPrx uncheckedCast:proxy];
        
        // Configure the default router on the communicator.
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.communicator setDefaultRouter:router];
    }
    @catch(ICEEndpointParseException* ex)
    {
        UIAlertView *alert = [[UIAlertView alloc]
                              initWithTitle:@"Invalid Hostname"
                              message:@"The provided hostname is invalid."
                              delegate:self cancelButtonTitle:@"OK"
                              otherButtonTitles:nil];
        [alert show];
        [alert release];
    }    
    
    // Disable the login button.
    loginButton.enabled = NO;
    
    // Kick off the login process in a separate thread. This ensures that the UI is not blocked.
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;
    NSInvocationOperation* op = [[NSInvocationOperation alloc]
                                 initWithTarget:self
                                 selector:@selector(doGlacier2Login:)
                                 object:proxy];
    [queue addOperation:op];
    [op release];
}

@end

