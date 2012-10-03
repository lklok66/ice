// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>


@interface LogZoomViewController : UIViewController {
    UITextView* text;
    NSArray* messages;
    int current;
    UIButton* nextButton;
    UIButton* prevButton;
    NSDateFormatter *dateFormatter;
}

@property (nonatomic, retain) IBOutlet UITextView* text;
@property (nonatomic, retain) IBOutlet UIButton* nextButton;
@property (nonatomic, retain) IBOutlet UIButton* prevButton;

@property (nonatomic, retain) NSArray* messages;
@property (nonatomic) int current;

@property (nonatomic, retain) NSDateFormatter* dateFormatter;

-(IBAction)next:(id)sender;
-(IBAction)prev:(id)sender;

@end
