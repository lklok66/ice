// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <RootViewController.h>
#import <MainViewController.h>
#import <FlipsideViewController.h>
#import <LogViewController.h>
#import <LogZoomViewController.h>

@implementation RootViewController

@synthesize infoButton;
@synthesize logButton;
@synthesize mainViewController;

@synthesize flipsideNavigationBar;
@synthesize flipsideViewController;

@synthesize logViewController;
@synthesize logNavigationBar;

@synthesize logZoomViewController;
@synthesize logZoomNavigationBar;

- (void)viewDidLoad {
    
    [super viewDidLoad];
    MainViewController *viewController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
    self.mainViewController = viewController;
    [viewController release];

    // Force the log view controller to load.
    LogViewController* controller = self.logViewController;
    controller = nil; // Get rid of warning.

    [self.view insertSubview:mainViewController.view belowSubview:infoButton];
    //[self.view insertSubview:mainViewController.view belowSubview:logButton];
}

- (IBAction)toggleView
{
    /*
     This method is called when the info or Done button is pressed.
     It flips the displayed view from the main view to the flipside view and vice-versa.
     */
    UIView *mainView = mainViewController.view;
    UIView *flipsideView = self.flipsideViewController.view;
    
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1];
    [UIView setAnimationTransition:([mainView superview] ? UIViewAnimationTransitionFlipFromRight : UIViewAnimationTransitionFlipFromLeft) forView:self.view cache:YES];
    
    if ([mainView superview] != nil) {
        [flipsideViewController viewWillAppear:YES];
        [mainViewController viewWillDisappear:YES];
        [mainView removeFromSuperview];
        [infoButton removeFromSuperview];
        [logButton removeFromSuperview];
        [self.view addSubview:flipsideView];
        [self.view insertSubview:flipsideNavigationBar aboveSubview:flipsideView];
        [mainViewController viewDidDisappear:YES];
        [flipsideViewController viewDidAppear:YES];

    } else {
        [mainViewController viewWillAppear:YES];
        [flipsideViewController viewWillDisappear:YES];
        [flipsideView removeFromSuperview];
        [flipsideNavigationBar removeFromSuperview];
        [self.view addSubview:mainView];
        [self.view insertSubview:infoButton aboveSubview:mainViewController.view];
        [self.view insertSubview:logButton aboveSubview:mainViewController.view];
        [flipsideViewController viewDidDisappear:YES];
        [mainViewController viewDidAppear:YES];
    }
    [UIView commitAnimations];
}

- (IBAction)toggleLogView
{
    UIView *mainView = mainViewController.view;
    UIView *logView = self.logViewController.view;
    
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1];
    [UIView setAnimationTransition:([mainView superview] ? UIViewAnimationTransitionFlipFromRight : UIViewAnimationTransitionFlipFromLeft) forView:self.view cache:YES];
    
    if ([mainView superview] != nil) {
        [logViewController viewWillAppear:YES];
        [mainViewController viewWillDisappear:YES];
        [mainView removeFromSuperview];
        [infoButton removeFromSuperview];
        [logButton removeFromSuperview];
        [self.view addSubview:logView];
        [self.view insertSubview:logNavigationBar aboveSubview:logView];
        [mainViewController viewDidDisappear:YES];
        [logViewController viewDidAppear:YES];
        
    } else {
        [mainViewController viewWillAppear:YES];
        [logViewController viewWillDisappear:YES];
        [logView removeFromSuperview];
        [logNavigationBar removeFromSuperview];
        [self.view addSubview:mainView];
        [self.view insertSubview:infoButton aboveSubview:mainViewController.view];
        [self.view insertSubview:logButton aboveSubview:mainViewController.view];
        [logViewController viewDidDisappear:YES];
        [mainViewController viewDidAppear:YES];
    }
    [UIView commitAnimations];
}

- (IBAction)toggleLogZoomView
{
    UIView *logView = logViewController.view;
    UIView *logZoomView = self.logZoomViewController.view;
    
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:1];
    [UIView setAnimationTransition:([logView superview] ? UIViewAnimationTransitionFlipFromRight : UIViewAnimationTransitionFlipFromLeft) forView:self.view cache:YES];
    
    if ([logView superview] != nil) {
        [logZoomViewController viewWillAppear:YES];
        [logViewController viewWillDisappear:YES];
        [logView removeFromSuperview];
        [logNavigationBar removeFromSuperview];
        [infoButton removeFromSuperview];
        [logButton removeFromSuperview];
        [self.view addSubview:logZoomView];
        [self.view insertSubview:logZoomNavigationBar aboveSubview:logZoomView];
        [logViewController viewDidDisappear:YES];
        [logZoomViewController viewDidAppear:YES];
        
    } else {
        [logViewController viewWillAppear:YES];
        [logZoomViewController viewWillDisappear:YES];
        [logZoomView removeFromSuperview];
        [logZoomNavigationBar removeFromSuperview];
        [self.view addSubview:logView];
        [self.view insertSubview:logNavigationBar aboveSubview:logView];
        [logZoomViewController viewDidDisappear:YES];
        [logViewController viewDidAppear:YES];
    }
    [UIView commitAnimations];
}


-(FlipsideViewController*)flipsideViewController
{
    if(flipsideViewController == nil)
    {
        FlipsideViewController *viewController = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
        self.flipsideViewController = viewController;
        [viewController release];
        
        // Set up the navigation bar
        UINavigationBar *aNavigationBar = [[UINavigationBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 44.0)];
        aNavigationBar.barStyle = UIBarStyleBlackOpaque;
        self.flipsideNavigationBar = aNavigationBar;
        [aNavigationBar release];
        
        UIBarButtonItem *buttonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(toggleView)];
        UINavigationItem *navigationItem = [[UINavigationItem alloc] initWithTitle:@"Settings"];
        navigationItem.rightBarButtonItem = buttonItem;
        [flipsideNavigationBar pushNavigationItem:navigationItem animated:NO];
        [navigationItem release];
        [buttonItem release];
    }
    return flipsideViewController;
}

-(LogViewController*)logViewController
{
    if(logViewController == nil)
    {
        LogViewController* viewController = [[LogViewController alloc] initWithNibName:@"LogView" bundle:nil];        
        self.logViewController = viewController;
        [viewController release];
        
        // Set up the navigation bar
        UINavigationBar *aNavigationBar = [[UINavigationBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 44.0)];
        aNavigationBar.barStyle = UIBarStyleBlackOpaque;
        self.logNavigationBar = aNavigationBar;
        [aNavigationBar release];
        
        UIBarButtonItem *buttonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(toggleLogView)];
        UINavigationItem *navigationItem = [[UINavigationItem alloc] initWithTitle:@"Log"];
        navigationItem.rightBarButtonItem = buttonItem;
        [logNavigationBar pushNavigationItem:navigationItem animated:NO];
        [navigationItem release];
        [buttonItem release];
    }
    return logViewController;
}

-(LogZoomViewController*)logZoomViewController
{
    // Instantiate the main view controller if necessary.
    if (logZoomViewController == nil)
    {
        LogZoomViewController* viewController = [[LogZoomViewController alloc] initWithNibName:@"LogZoomView" bundle:nil];
        self.logZoomViewController = viewController;
        [viewController release];
        
        // Set up the navigation bar
        UINavigationBar *aNavigationBar = [[UINavigationBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 44.0)];
        aNavigationBar.barStyle = UIBarStyleBlackOpaque;
        self.logZoomNavigationBar = aNavigationBar;
        [aNavigationBar release];
        
        UIBarButtonItem *buttonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(toggleLogZoomView)];
        UINavigationItem *navigationItem = [[UINavigationItem alloc] initWithTitle:@"Log Item"];
        navigationItem.rightBarButtonItem = buttonItem;
        [logZoomNavigationBar pushNavigationItem:navigationItem animated:NO];
        [navigationItem release];
        [buttonItem release];
    }
    return logZoomViewController;
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


- (void)dealloc {
    [infoButton release];
    [logButton release];
    [mainViewController release];

    [flipsideNavigationBar release];
    [flipsideViewController release];

    [logNavigationBar release];
    [logViewController release];

    [logZoomNavigationBar release];
    [logZoomViewController release];
    
    [super dealloc];
}


@end
