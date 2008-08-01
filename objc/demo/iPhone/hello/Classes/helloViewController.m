//
//  helloAppDelegate.m
//  hello
//
//  Created by Benoit Foucher on 7/30/08.
//  Copyright ZeroC, Inc. 2008. All rights reserved.
//

#import "helloViewController.h"

#import <IceObjC/IceObjC.h>

@implementation helloViewController

/*
 Implement loadView if you want to create a view hierarchy programmatically
- (void)loadView {
}
 */


- (void)viewDidLoad {
	[super viewDidLoad];
        communicator = [[Ice_Communicator create] retain];
}



- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}

- (void)sayHello:(id)sender {
        [[communicator stringToProxy:@"hello:tcp -h 192.168.5.100 -p 10000"] ice_ping];
}

- (void)dealloc {
        [communicator destroy];
	[communicator release];
	[super dealloc];
}

@end
