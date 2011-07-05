// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AddController.h>
#import <Library.h>
#import <WaitAlert.h>

@interface AddController ()

@property (nonatomic, retain) id<DemoLibraryPrx> library;

@end

@implementation AddController

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
    [waitAlert release];
    [library release];
	[super dealloc];
}

-(IBAction)cancel:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

-(IBAction)save:(id)sender
{
    NSAssert(waitAlert == nil, @"savingAlert == nil");
    self.waitAlert = [[WaitAlert alloc] init];
    [waitAlert show];
    
    [library begin_createBook:book.isbn
                        title:book.title
                      authors:book.authors
                     response:^(id<DemoBookPrx> prx) {
						 NSAssert(waitAlert != nil, @"savingAlert != nil");
						 [waitAlert dismissWithClickedButtonIndex:0 animated:NO];
						 self.waitAlert = nil;
						 
						 [self.navigationController popViewControllerAnimated:YES];
					 }
                    exception:^(ICEException* ex) { 
						NSAssert(waitAlert != nil, @"savingAlert != nil");
						[waitAlert dismissWithClickedButtonIndex:0 animated:NO];
						self.waitAlert = nil;
						
						if([ex isKindOfClass:[DemoBookExistsException class]])
						{
							// open an alert with just an OK button
							UIAlertView *alert = [[[UIAlertView alloc]
												   initWithTitle:@"Error" message:@"That ISBN number already exists"
												   delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] autorelease];
							[alert show];
							return;
						}
						
						[super exception:ex];
					}];
}

-(void)startEdit:(DemoBookDescription*)b library:(id<DemoLibraryPrx>)l
{
    self.book = b;
    self.library = l;
    [self setEditing:YES animated:NO];
}

@end