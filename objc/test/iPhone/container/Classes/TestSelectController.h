// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@class TestViewController;

@interface TestSelectController : UIViewController<UIPickerViewDataSource, UIPickerViewDelegate>
{
@private
    IBOutlet UIPickerView* pickerView;
    IBOutlet UISwitch* sslSwitch;
    NSArray* tests;
    NSEnumerator* testsEnum;
    TestViewController *testViewController;
}

-(IBAction)runTest:(id)sender;

@end
