// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@class ChatController;
@class WaitAlert;
@class ICEInitializationData;
@protocol ICECommunicator;

@interface LoginController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate>
{
@private
    IBOutlet UITextField* hostnameField;
    IBOutlet UITextField* usernameField;
    IBOutlet UITextField* passwordField;
    IBOutlet UIButton* loginButton;

    UITextField* currentField;
    NSString* oldFieldValue;
    ChatController* chatController;
    WaitAlert* waitAlert;

    id<ICECommunicator> communicator;
}

-(IBAction)login:(id)sender;

@end
