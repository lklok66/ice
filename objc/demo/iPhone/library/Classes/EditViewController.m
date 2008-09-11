// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <EditViewController.h>

#import <Ice/Ice.h>

@implementation EditViewController

@synthesize textField, cb;

- (void)viewDidLoad
{
    // Adjust the text field size and font.
    CGRect frame = textField.frame;
    frame.size.height += 10;
    textField.frame = frame;
    textField.font = [UIFont boldSystemFontOfSize:16];
    // Set the view background to match the grouped tables in the other views.
    self.view.backgroundColor = [UIColor groupTableViewBackgroundColor];
}

- (void)viewWillAppear:(BOOL)animated
{
    NSString *capitalizedKey = cb.fieldName;
    self.title = capitalizedKey;
    textField.placeholder = capitalizedKey;
    textField.enabled = YES;
    textField.text = cb.textValue;
    [textField becomeFirstResponder];
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

-(IBAction)cancel:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

-(void)saveSuccess
{
    cb.textValue = textField.text;
    [self.navigationController popViewControllerAnimated:YES];
}

-(void)exception:(ICEException*)ex
{
    textField.enabled = YES;
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    
    NSString* s = [NSString stringWithFormat:@"%@", ex];
    
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];    
}

-(IBAction)save:(id)sender
{
    textField.enabled = NO;
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    [cb save:textField.text object:self response:@selector(saveSuccess) exception:@selector(exception:)];
}

- (void)dealloc
{
    [textField release];
    [cb release];
    
	[super dealloc];
}
@end
