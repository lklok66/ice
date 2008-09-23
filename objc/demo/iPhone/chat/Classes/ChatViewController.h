// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>
#import <Ice/Ice.h>

@protocol ChatChatSessionPrx;
@class UserViewController;

@interface ChatViewController : UIViewController<UITableViewDelegate, UITableViewDataSource,UITextFieldDelegate>
{
@private
    IBOutlet UITableView* chatView;
    IBOutlet UITextField* inputTextField;
    UserViewController* userViewController;
    NSMutableArray* messages;
    id<ChatChatSessionPrx> session;
}

@property (nonatomic, retain) id<ChatChatSessionPrx> session;

-(void)clear;
-(void)initUsers:(NSMutableArray *)users;
-(void)send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message;
-(void)join:(ICELong)timestamp name:(NSMutableString *)name;
-(void)leave:(ICELong)timestamp name:(NSMutableString *)name;

@end
