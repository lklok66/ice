// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@protocol DemoLibraryPrx;
@class MainController;
@class WaitAlert;
@protocol Glacier2RouterPrx;
@protocol ICECommunicator;

@interface LoginController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate>
{
@private
    IBOutlet UITextField* hostnameField;
    IBOutlet UITextField* usernameField;
    IBOutlet UITextField* passwordField;
    IBOutlet UIButton* loginButton;
    IBOutlet UISwitch* glacier2Switch;
    IBOutlet UISwitch* sslSwitch;
    
    UITextField* currentField;
    NSString* oldFieldValue;
    
    MainController *mainController;
    
    NSOperationQueue* queue;
    WaitAlert* waitAlert;
    
    id<ICECommunicator> communicator;
    id session;
    id<Glacier2RouterPrx> router;
    int sessionTimeout;
    id<DemoLibraryPrx> library;
}

-(IBAction)login:(id)sender;
-(void)glacier2Changed:(id)sender;
-(void)sslChanged:(id)sender;

@end
