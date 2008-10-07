// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@class Test;

@interface TestViewController : UIViewController<UITableViewDataSource, UITableViewDelegate>
{
@private
    IBOutlet UITableView* output;
    IBOutlet UIActivityIndicatorView* activity;
    IBOutlet UIButton* nextButton;
    
    NSMutableString* currentMessage;
    NSMutableArray* messages;
    NSOperationQueue* queue;
    Test* test;
    int completed;
}

-(IBAction)next:(id)sender;

@end

