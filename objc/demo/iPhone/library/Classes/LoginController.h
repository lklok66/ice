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
@class ICEInitializationData;
@protocol ICECommunicator;

@interface LoginController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate>
{
@private
    IBOutlet UITextField* hostnameTextField;
    IBOutlet UIButton* loginButton;
    IBOutlet UISwitch* glacier2Switch;
    IBOutlet UISwitch* sslSwitch;
    
    NSString* hostname;
    MainController *mainController;
    
    NSOperationQueue* queue;
    WaitAlert* waitAlert;
    
    ICEInitializationData* initData;
    id<ICECommunicator> communicator;
    id session;
    int sessionTimeout;
    id<DemoLibraryPrx> library;
}

-(IBAction)login:(id)sender;
-(void)glacier2Changed:(id)sender;
-(void)sslChanged:(id)sender;

@end
