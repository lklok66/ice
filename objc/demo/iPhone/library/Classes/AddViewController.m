// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AddViewController.h>

#import <Library.h>

@implementation AddViewController

@synthesize library;

-(void)viewDidLoad
{
    self.title = @"New Book";
    self.navigationItem.leftBarButtonItem = [[[UIBarButtonItem alloc]
                                              initWithBarButtonSystemItem:UIBarButtonSystemItemCancel 
                                              target:self
                                              action:@selector(cancel:)] autorelease];
    self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc]
                                               initWithBarButtonSystemItem:UIBarButtonSystemItemSave 
                                               target:self
                                               action:@selector(save:)] autorelease];
    tableView.allowsSelectionDuringEditing = YES;
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationItem.leftBarButtonItem.enabled = YES;
    self.navigationItem.rightBarButtonItem.enabled = (book.isbn && book.isbn.length > 0);
    [self setEditing:YES animated:NO];
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

-(void)dealloc
{
    [library release];
	[super dealloc];
}

-(IBAction)cancel:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

-(IBAction)save:(id)sender
{
    // Disable the navigation buttons. The user is locked on this page
    // until the save has completed.
    self.navigationItem.leftBarButtonItem.enabled = NO;
    self.navigationItem.rightBarButtonItem.enabled = NO;
    [self setEditing:NO animated:NO];
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;

    [library
     createBook_async:[ICECallbackOnMainThread callbackOnMainThread:self]
     response:@selector(createResponse)
     exception:@selector(exception:)
     isbn:book.isbn
     title:book.title
     authors:book.authors];
}

#pragma mark AMI callbacks

-(void)createResponse
{
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
  
    [self.navigationController popViewControllerAnimated:YES];
}

-(void)exception:(ICEException*)ex
{
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    if([ex isKindOfClass:[DemoBookExistsException class]])
    {
        // open an alert with just an OK button
        UIAlertView *alert = [[UIAlertView alloc]
                              initWithTitle:@"Error" message:@"That ISBN number already exists"
                              delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
        
        self.navigationItem.leftBarButtonItem.enabled = YES;
        self.navigationItem.rightBarButtonItem.enabled = YES;
        [self setEditing:YES animated:NO];
        
        return;
    }

    [super exception:ex];
}

@end