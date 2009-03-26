// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************
#import <UIKit/UIKit.h>

@class ChatViewController;
@protocol ChatChatSessionPrx;

@interface LoginViewController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate>
{
@private
    IBOutlet UITextField* hostnameTextField;
    IBOutlet UITextField* usernameTextField;
    IBOutlet UITextField* passwordTextField;
    IBOutlet UIButton* loginButton;
    IBOutlet UISwitch* sslSwitch;

    NSString* hostname;
    NSString* username;
    NSString* password;
    UITextField* currentTextField;
    NSString* oldTextFieldValue;
    ChatViewController* chatViewController;
    NSOperationQueue* queue;
    id<ChatChatSessionPrx> session;
    int sessionTimeout;
    BOOL showAlert;
}

-(IBAction)login:(id)sender;
-(IBAction)sslChanged:(id)sender;

@end
