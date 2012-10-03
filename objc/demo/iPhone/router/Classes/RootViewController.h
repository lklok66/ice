// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@class MainViewController;
@class FlipsideViewController;
@class LogViewController;
@class LogZoomViewController;

@interface RootViewController : UIViewController {

    UIButton *infoButton;
    UIButton *logButton;
    MainViewController *mainViewController;

    FlipsideViewController *flipsideViewController;
    UINavigationBar *flipsideNavigationBar;
    
    UINavigationBar *logNavigationBar;
    LogViewController *logViewController;

    UINavigationBar *logZoomNavigationBar;
    LogZoomViewController *logZoomViewController;
}

@property (nonatomic, retain) IBOutlet UIButton *infoButton;
@property (nonatomic, retain) IBOutlet UIButton *logButton;

@property (nonatomic, retain) MainViewController *mainViewController;

@property (nonatomic, retain) UINavigationBar *flipsideNavigationBar;
@property (nonatomic, retain) FlipsideViewController *flipsideViewController;

@property (nonatomic, retain) UINavigationBar *logNavigationBar;
@property (nonatomic, retain) LogViewController *logViewController;

@property (nonatomic, retain) UINavigationBar *logZoomNavigationBar;
@property (nonatomic, retain) LogZoomViewController *logZoomViewController;

- (IBAction)toggleView;
- (IBAction)toggleLogView;
- (IBAction)toggleLogZoomView;

@end
