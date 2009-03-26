// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@class DemoBookDescription;
@class EditViewController;
@class ICEException;

@interface DetailViewController :
    UIViewController<UITableViewDelegate, UITableViewDataSource, UIActionSheetDelegate,UIAlertViewDelegate>
{
@protected
    DemoBookDescription* book;
    
    IBOutlet UITableView *tableView;
    NSIndexPath* selectedIndexPath;
}

@property (nonatomic, retain) DemoBookDescription* book;

+(EditViewController *)editViewController;
-(void)exception:(ICEException*)ex;

@end
