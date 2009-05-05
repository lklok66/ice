// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#import <UIKit/UIKit.h>
#import <LoggingDelegate.h>

@class LogZoomViewController;

@interface LogViewController : UIViewController<LoggingDelegate, UITableViewDelegate, UITableViewDataSource>
{
    IBOutlet UITableView* messagesTableView;
    NSMutableArray* messages;
    LogZoomViewController* logZoomViewController;
}

@property (nonatomic, retain) UITableView* messagesTableView;
@property (nonatomic, retain) NSMutableArray* messages;
@property (nonatomic, retain) LogZoomViewController* logZoomViewController;

@end
