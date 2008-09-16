// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@class DemoBookDescription;
@class EditViewController;
@class ICEException;

@interface DetailViewController :
    UIViewController<UITableViewDelegate, UITableViewDataSource, UIActionSheetDelegate,UIAlertViewDelegate>
{
    DemoBookDescription* book;
@protected
    IBOutlet UITableView *tableView;
    NSIndexPath* selectedIndexPath;
}

@property (nonatomic, retain) DemoBookDescription* book;

+(EditViewController *)editViewController;
-(void)exception:(ICEException*)ex;

@end
