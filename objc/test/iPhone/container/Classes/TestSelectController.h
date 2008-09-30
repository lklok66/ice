// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>


@interface TestSelectController : UIViewController<UIPickerViewDataSource, UIPickerViewDelegate>
{
@private
    IBOutlet UIPickerView* pickerView;
    IBOutlet UISwitch* sslSwitch;
    NSArray* tests;
    NSEnumerator* testsEnum;
}

-(IBAction)runTest:(id)sender;

@end
