// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <EditController.h>

@implementation EditController

@synthesize textField;

-(void)viewDidLoad
{
    // Adjust the text field size and font.
    CGRect frame = textField.frame;
    frame.size.height += 10;
    textField.frame = frame;
    textField.font = [UIFont boldSystemFontOfSize:16];
    // Set the view background to match the grouped tables in the other views.
    self.view.backgroundColor = [UIColor groupTableViewBackgroundColor];
}

-(void)viewWillAppear:(BOOL)animated
{
    textField.placeholder = self.title;
    textField.enabled = YES;
    textField.text = value;
    
    [textField becomeFirstResponder];
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

-(IBAction)cancel:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

-(void)startEdit:(id)o selector:(SEL)sel name:(NSString*)name value:(NSString*)v
{
    obj = o;
    selector = sel;
    
    self.title = name;
    value = v;
}

-(IBAction)save:(id)sender
{
    [obj performSelector:selector withObject:textField.text];
    [self.navigationController popViewControllerAnimated:YES];
}

-(void)dealloc
{
    [value release];
    [textField release];
    
	[super dealloc];
}
@end
