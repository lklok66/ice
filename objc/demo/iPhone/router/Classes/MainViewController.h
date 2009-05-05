// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>
#import <RouterDelegate.h>

@class LogViewController;

@interface MainViewController : UIViewController<RouterDelegate> {
    IBOutlet UITextField* statusTextField;
    IBOutlet UITextField* clientRequestsTextField;
    IBOutlet UITextField* clientExceptionsTextField;
    IBOutlet UITextField* serverRequestsTextField;
    IBOutlet UITextField* serverExceptionsTextField;
}

@end
