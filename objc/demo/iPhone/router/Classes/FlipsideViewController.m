// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#import <FlipsideViewController.h>
#import <AppDelegate.h>

@implementation FlipsideViewController

@synthesize network;
@synthesize protocol;
@synthesize router;

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor viewFlipsideBackgroundColor];

    network.selectedSegmentIndex = [[NSUserDefaults standardUserDefaults] integerForKey:@"Ice.Trace.Network"];
    protocol.on = [[NSUserDefaults standardUserDefaults] integerForKey:@"Ice.Trace.Protocol"];
    router.on = [[NSUserDefaults standardUserDefaults] integerForKey:@"Trace.Router"];
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
    [network release];
    [protocol release];
    [router release];
    [super dealloc];
}


- (void)networkChanged:(id)thesender
{
    UISegmentedControl* sender = thesender;
    int val = sender.selectedSegmentIndex;

    [[NSUserDefaults standardUserDefaults] setInteger:val forKey:@"Ice.Trace.Network"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)protocolChanged:(id)thesender
{
    UISwitch* sender = thesender;
    int val = (int)sender.isOn;
    [[NSUserDefaults standardUserDefaults] setInteger:val forKey:@"Ice.Trace.Protocol"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)routerChanged:(id)thesender
{
    UISwitch* sender = thesender;
    int val = (int)sender.isOn;
    [[NSUserDefaults standardUserDefaults] setInteger:val forKey:@"Trace.Router"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

@end
