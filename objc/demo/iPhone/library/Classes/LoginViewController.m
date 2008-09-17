// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <LoginViewController.h>
#import <MainViewController.h>
#import <AppDelegate.h>

#import <Ice/Ice.h>
#import <Library.h>
#import <Session.h>
#import <Glacier2Session.h>
#import <Glacier2/Router.h>

@interface LoginViewController()

@property (nonatomic, retain) UITextField* hostnameTextField;
@property (nonatomic, retain) UIButton* loginButton;
@property (nonatomic, retain) UISwitch* glacier2Switch;

@property (nonatomic, retain) NSString* hostname;

@property (nonatomic, retain) MainViewController* mainViewController;

@property (retain) id<DemoLibraryPrx> library;
@property (retain) id session;
@property int sessionTimeout;
@property (nonatomic, retain) NSOperationQueue* queue;

@end

@implementation LoginViewController

@synthesize hostnameTextField;
@synthesize loginButton;
@synthesize glacier2Switch;
@synthesize hostname;
@synthesize library;
@synthesize session;
@synthesize mainViewController;
@synthesize queue;
@synthesize sessionTimeout;

NSString* hostnameKey = @"hostnameKey";
NSString* glacier2Key = @"glacier2Key";

-(void)viewDidLoad
{
    // Initialize the application defaults.
    NSString* testValue = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    if (testValue == nil)
    {
        NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:@"127.0.0.1", hostnameKey, @"NO", glacier2Key, nil];
	
        [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    self.hostname = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    glacier2Switch.on = [[NSUserDefaults standardUserDefaults] boolForKey:glacier2Key];
    
    self.queue = [[[NSOperationQueue alloc] init] autorelease];

    // When the user starts typing, show the clear button in the text field.
    hostnameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
    hostnameTextField.text = hostname;
    loginButton.enabled = hostname.length > 0;
    showAlert = NO;
}

-(void)viewWillAppear:(BOOL)animated
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [appDelegate logout];
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
    if(hostnameTextField.editing)
    {
        // Dismiss the keyboard when the view outside the text field is touched.
        [hostnameTextField resignFirstResponder];
        
        // Revert the text field to the previous value.
        hostnameTextField.text = hostname; 
        loginButton.enabled = hostname.length > 0;
    }
    [super touchesBegan:touches withEvent:event];
}

-(void)dealloc
{
    [hostnameTextField release];
    [loginButton release];
    [glacier2Switch release];
    [hostname release];
    [mainViewController release];
    [session release];
    [library release];
    [queue release];
    [super dealloc];
}

-(MainViewController*)mainViewController
{
    // Instantiate the main view controller if necessary.
    if (mainViewController == nil)
    {
        mainViewController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
    }
    return mainViewController;
}

-(void)glacier2Changed:(id)s
{
    UISwitch* sender = (UISwitch*)s;

    [[NSUserDefaults standardUserDefaults] setObject:(sender.isOn ? @"YES" : @"NO") forKey:glacier2Key];
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

-(BOOL)textFieldShouldReturn:(UITextField*)theTextField
{
    // When the user presses return, take focus away from the text
    // field so that the keyboard is dismissed.
    if(theTextField == hostnameTextField)
    {
        NSString* s = [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000",
                       theTextField.text];
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
        
        [hostnameTextField resignFirstResponder];
        
        self.hostname = theTextField.text;
        loginButton.enabled = hostname.length > 0;
        
        [[NSUserDefaults standardUserDefaults] setObject:hostname forKey:hostnameKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    return YES;
}

#pragma mark Login

-(void)exception:(NSString*)s
{
    // Restart the login process in the delegate.
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [appDelegate logout];
    
    // Re-enable the login button.
    loginButton.enabled = YES;
    
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
   
    [appDelegate setSession:session timeout:self.sessionTimeout/2];
    
    MainViewController* controller = self.mainViewController;
    controller.library = library;
    [self.navigationController pushViewController:controller animated:YES];

    // Re-enable the login button.
    loginButton.enabled = YES;
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
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
        self.library = [sess getLibrary];
        self.sessionTimeout = [factory getSessionTimeout];
        
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:nil waitUntilDone:NO];
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
        id<Glacier2RouterPrx> router = [Glacier2RouterPrx uncheckedCast:proxy];
        
        id<Glacier2SessionPrx> glacier2session = [router createSession:@"dummy" password:@"none"];
        id<DemoGlacier2SessionPrx> sess = [DemoGlacier2SessionPrx uncheckedCast:glacier2session];

        self.session = sess;
        self.library = [sess getLibrary];
        self.sessionTimeout = [router getSessionTimeout];

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
    SEL loginSelector;

    @try
    {
        if(glacier2Switch.isOn)
        {
            NSString* s = [NSString stringWithFormat:@"DemoGlacier2/router:tcp -h %@ -p 4064 -t 30000", hostname];
            proxy = [appDelegate.communicator stringToProxy:s];
            id<ICERouterPrx> router = [ICERouterPrx uncheckedCast:proxy];
            
            // Configure the default router on the communicator.
            AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
            [appDelegate.communicator setDefaultRouter:router];
            
            loginSelector = @selector(doGlacier2Login:);
        }
        else
        {
            NSString* s = [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000 -t 30000", hostname];
            proxy = [appDelegate.communicator stringToProxy:s];

            loginSelector = @selector(doLogin:);
        }
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
                                 selector:loginSelector
                                 object:proxy];
    [queue addOperation:op];
    [op release];
    
}

@end
