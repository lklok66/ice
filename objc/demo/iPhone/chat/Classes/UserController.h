// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@interface UserController : UITableViewController
{
    IBOutlet UITableView* usersTableView;
    NSMutableArray* users;
}

@property (nonatomic, retain) UITableView* usersTableView;
@property (nonatomic, retain) NSMutableArray* users;

@end
