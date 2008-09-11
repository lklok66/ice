//
//  LoginViewController.m
//  library
//
//  Created by Matthew Newhook on 9/5/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <LoginViewController.h>
#import <MainViewController.h>
#import <AppDelegate.h>

#include <Ice/Ice.h>
#include <Library.h>
#include <Session.h>


@interface LoginViewController()

@property (nonatomic, retain) UITextField* hostnameTextField;
@property (nonatomic, retain) UIButton* loginButton;

@property (nonatomic, retain) NSString* hostname_;

@property (nonatomic, retain) MainViewController* mainViewController_;

@property (retain) id<DemoLibraryPrx> library_;
@property (retain) id<DemoSessionPrx> session_;
@property (nonatomic, retain) NSOperationQueue* queue_;

@end

@implementation LoginViewController

@synthesize hostnameTextField;
@synthesize loginButton;
@synthesize hostname_;
@synthesize library_;
@synthesize session_;
@synthesize mainViewController_;
@synthesize queue_;

NSString* hostnameKey = @"hostnameKey";

-(MainViewController *)mainViewController_
{
    // Instantiate the main view controller if necessary.
    if (mainViewController_ == nil)
    {
        mainViewController_ = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
    }
    return mainViewController_;
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

    self.hostname_ = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    self.queue_ = [[[NSOperationQueue alloc] init] autorelease];

    // When the user starts typing, show the clear button in the text field.
    hostnameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
    hostnameTextField.text = hostname_;
    loginButton.enabled = hostname_.length > 0;
    showAlert_ = NO;
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
    showAlert_ = YES;
}

-(void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    showAlert_ = NO;
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
        
        self.hostname_ = theTextField.text;
        loginButton.enabled = hostname_.length > 0;
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
        hostnameTextField.text = hostname_; 
        loginButton.enabled = hostname_.length > 0;
    }
    [super touchesBegan:touches withEvent:event];
}

-(void)loginComplete:(id)data
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
   
    appDelegate.session = session_;
    
    MainViewController* controller = self.mainViewController_;
    controller.library = library_;
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
        self.session_ = [factory create];
        self.library_ = [session_ getLibrary];
        
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

    NSString* s = [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000", hostname_];
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
    [queue_ addOperation:op];
    [op release];
}

- (void)dealloc
{
    [queue_ release];
    [hostname_ release];
    [hostnameTextField release];
    [loginButton release];
    [super dealloc];
}

@end