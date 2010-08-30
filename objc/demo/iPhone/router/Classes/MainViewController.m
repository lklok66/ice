// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#import <MainViewController.h>
#import <LogViewController.h>
#import <MainView.h>
#import <AppDelegate.h>
#import <RouterI.h>

@implementation MainViewController

/* TODO: Erase?
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
    }
    return self;
}
 */


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    AppDelegate* app = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    [app initializeRouter];
    [app.router setDelegate:self];
    [self refreshRoutingStatistics];
    [super viewDidLoad];
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

-(void)refreshRoutingStatistics
{
    AppDelegate* app = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    [statusTextField setText:app.router.status];
    [clientRequestsTextField setText:[NSString stringWithFormat:@"%d", app.router.clientRequests]];
    [clientExceptionsTextField setText:[NSString stringWithFormat:@"%d", app.router.clientExceptions]];
    [serverRequestsTextField setText:[NSString stringWithFormat:@"%d", app.router.serverRequests]];
    [serverExceptionsTextField setText:[NSString stringWithFormat:@"%d", app.router.serverExceptions]];
}

- (void)dealloc {
    [statusTextField release];
    [clientRequestsTextField release];
    [clientExceptionsTextField release];
    [serverRequestsTextField release];
    [serverExceptionsTextField release];
    
    [super dealloc];
}


@end
