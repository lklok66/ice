// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>
#import <Ice/Ice.h>

#import <ChatSession.h>

@protocol ChatChatSessionPrx;
@class UserViewController;

@interface ChatViewController : UIViewController<UITableViewDelegate, UITableViewDataSource,UITextFieldDelegate,ChatChatRoomCallback>
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
-(void)init:(NSMutableArray *)users current:(ICECurrent*)current;
-(void)send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message current:(ICECurrent*)current;
-(void)join:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent*)current;
-(void)leave:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent*)current;

@end
