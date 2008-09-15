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

-(IBAction)save:(id)sender
{
    [cb save:textField.text];
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)dealloc
{
    [textField release];
    [cb release];
    
	[super dealloc];
}
@end
