// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AddViewController.h>

#import <Library.h>

@implementation AddViewController

@synthesize library;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
		// Initialization code
	}
	return self;
}

- (void)viewDidLoad
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

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationItem.rightBarButtonItem.enabled = (book.isbn && book.isbn.length > 0);
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
	[super dealloc];
}

- (IBAction)cancel:(id)sender
{
    [self.navigationController dismissModalViewControllerAnimated:YES];
}

- (IBAction)save:(id)sender
{
    // TODO: AMI
    [library createBook:book.isbn title:book.title authors:book.authors];

    // Dismiss the modal view to return to the main list
    [self.navigationController dismissModalViewControllerAnimated:YES];
}

@end