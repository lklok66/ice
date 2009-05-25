// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>
#import <ChatSession.h>

@class UserController;

@interface ChatController : UIViewController<UITableViewDelegate, UITableViewDataSource, UITextFieldDelegate,
                                             ChatChatRoomCallback>
{
@private
    IBOutlet UITableView* chatView;
    IBOutlet UITextField* inputField;
    UserController* userController;
    NSMutableArray* messages;
    NSTimer* refreshTimer;

    id<ICECommunicator> communicator;
    id<ChatChatSessionPrx>  session;
    id<ChatChatRoomCallbackPrx> callbackProxy;
    int sessionTimeout;
}

// Called to setup the session.
-(void)  setup:(id<ICECommunicator>)communicator
       session:(id<ChatChatSessionPrx>)session
sessionTimeout:(int)timeout
        router:(id<ICERouterPrx>)router
      category:(NSString*)category;
// Called just prior to being shown.
-(void)activate:(NSString*)title;

// ChatChatRoomCallback methods.
-(void)init:(NSMutableArray *)users current:(ICECurrent*)current;
-(void)send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message
    current:(ICECurrent*)current;
-(void)join:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent*)current;
-(void)leave:(ICELong)timestamp name:(NSMutableString *)name current:(ICECurrent*)current;

@end
