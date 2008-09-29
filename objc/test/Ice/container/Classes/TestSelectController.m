// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <TestSelectController.h>
#import <Test.h>
#import <AppDelegate.h>
#import <TestViewController.h>

@interface TestSelectController()

@property (nonatomic, retain) NSArray* tests;
@property (nonatomic, retain) UIPickerView* pickerView;
@end

@implementation TestSelectController

@synthesize tests;
@synthesize pickerView;

- (void)viewDidLoad
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    self.tests = appDelegate.tests;
    [pickerView selectRow:appDelegate.currentTest inComponent:0 animated:NO];
    if(appDelegate.autoLaunch)
    {
        [self runTest:pickerView];
    }
    [super viewDidLoad];
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

#pragma mark -

-(IBAction)runTest:(id)sender
{
    int row = [pickerView selectedRowInComponent:0];
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    appDelegate.currentTest = row;
    
    TestViewController* controller = [[[TestViewController alloc] initWithNibName:@"TestView" bundle:nil] autorelease];
    controller.test = (Test*)[tests objectAtIndex:row];
    
    UINavigationController *navController = [[[UINavigationController alloc] initWithRootViewController:controller] autorelease];
    [self.navigationController presentModalViewController:navController animated:YES];
}

#pragma mark UIPickerViewDelegate

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    Test* test = (Test*)[tests objectAtIndex:row];
    return test.name;
}

#pragma mark UIPickerViewDataSource

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)componen
{
    return tests.count;
}
@end
