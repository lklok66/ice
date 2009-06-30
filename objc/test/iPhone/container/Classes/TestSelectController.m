// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <TestSelectController.h>
#import <Test.h>
#import <AppDelegate.h>
#import <TestViewController.h>

@interface TestSelectController()

@property (nonatomic, retain) NSArray* tests;
@property (nonatomic, retain) UIPickerView* pickerView;
@property (nonatomic, retain) UISwitch* sslSwitch;
@property (nonatomic, retain) TestViewController* testViewController;

@end

@implementation TestSelectController

@synthesize tests;
@synthesize pickerView;
@synthesize sslSwitch;
@synthesize testViewController;

- (void)viewDidLoad
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    self.tests = appDelegate.tests;
    [self.sslSwitch setOn:appDelegate.ssl animated:TRUE];
    [self.sslSwitch setOn:NO animated:TRUE];

#if TARGET_IPHONE_SIMULATOR
    sslSwitch.userInteractionEnabled = NO;
#endif
    
    [super viewDidLoad];
}

-(void)viewWillAppear:(BOOL)animated
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [pickerView selectRow:appDelegate.currentTest inComponent:0 animated:NO];
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
    [tests release];
    [pickerView release];
    [sslSwitch release];
    [loopSwitch release];
    [testViewController release];
    
    [super dealloc];
}

-(TestViewController*)testViewController
{
    // Instantiate the test view controller if necessary.
    if (testViewController == nil)
    {
        testViewController = [[TestViewController alloc] initWithNibName:@"TestView" bundle:nil];
    }
    return testViewController;
}

#pragma mark -

-(IBAction)runTest:(id)sender
{
    int row = [pickerView selectedRowInComponent:0];
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    appDelegate.currentTest = row;
    appDelegate.ssl = sslSwitch.isOn;
    appDelegate.loop = loopSwitch.isOn;
    
    TestViewController* controller = self.testViewController;
    [self.navigationController pushViewController:controller animated:YES];
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
