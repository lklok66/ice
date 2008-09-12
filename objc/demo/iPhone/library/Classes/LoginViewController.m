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

#include <Ice/Ice.h>
#include <Library.h>
#include <Session.h>


@interface LoginViewController()

@property (nonatomic, retain) UITextField* hostnameTextField;
@property (nonatomic, retain) UIButton* loginButton;

@property (nonatomic, retain) NSString* hostname;

@property (nonatomic, retain) MainViewController* mainViewController;

@property (retain) id<DemoLibraryPrx> library;
@property (retain) id<DemoSessionPrx> session;
@property (nonatomic, retain) NSOperationQueue* queue;

@end

@implementation LoginViewController

@synthesize hostnameTextField;
@synthesize loginButton;
@synthesize hostname;
@synthesize library;
@synthesize session;
@synthesize mainViewController;
@synthesize queue;

NSString* hostnameKey = @"hostnameKey";

-(MainViewController *)mainViewController
{
    // Instantiate the main view controller if necessary.
    if (mainViewController == nil)
    {
        mainViewController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
    }
    return mainViewController;
}

-(void)handleException:(ICEException*)ex
{
    // Re-enable the login button.
    loginButton.enabled = YES;

    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    
    NSString* s = [NSString stringWithFormat:@"%@", ex];

    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];
}

/*
 Implement loadView if you want to create a view hierarchy programmatically
- (void)loadView {
}
 */

- (void)viewDidLoad
{
    // Initialize the application defaults.
    NSString* testValue = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    if (testValue == nil)
    {
        NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:@"127.0.0.1", hostnameKey, nil];
	
        [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    self.hostname = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
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
    appDelegate.session = nil;
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

-(void)didPresentAlertView:(UIAlertView *)alertView
{
    showAlert = YES;
}

-(void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    showAlert = NO;
}

-(BOOL)textFieldShouldReturn:(UITextField *)theTextField
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

-(void)loginComplete:(id)data
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
   
    appDelegate.session = session;
    
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
     
        // This doesn't call appDelegate.session directly as we want the refresh
        // to run in the main thread.
        self.session = [factory create];
        self.library = [session getLibrary];
        
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:nil waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(handleException:) withObject:ex waitUntilDone:NO];
    }
}

-(IBAction)login:(id)sender
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];

    NSString* s = [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000", hostname];
    id<ICEObjectPrx> proxy;
    @try
    {
        proxy = [ICEObjectPrx uncheckedCast:[appDelegate.communicator stringToProxy:s]];
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
        return;
    }

    // Disable the login button.
    loginButton.enabled = NO;

    // Kick off the login process in a separate thread. This ensures that the UI is not blocked.
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;
    NSInvocationOperation* op = [[NSInvocationOperation alloc]
                                 initWithTarget:self selector:@selector(doLogin:) object:proxy];
    [queue addOperation:op];
    [op release];
}

- (void)dealloc
{
    [hostnameTextField release];
    [loginButton release];
    [hostname release];
    [mainViewController release];
    [session release];
    [library release];
    [queue release];
    [super dealloc];
}

@end
