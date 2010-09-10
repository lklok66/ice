// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

#import <Voip.h>

@class WaitAlert;
@class ICEInitializationData;
@protocol ICECommunicator;
@protocol Glacier2RouterPrx;

@interface VoipViewController : UIViewController<UITextFieldDelegate,VoipControl> {
@private
    IBOutlet UITextField* hostnameField;
    IBOutlet UITextField* usernameField;
    IBOutlet UITextField* passwordField;
    IBOutlet UIButton* loginButton;
	IBOutlet UIButton* callButton;
    IBOutlet UISwitch* sslSwitch;
    IBOutlet UISlider* delaySlider;
	
    UITextField* currentField;
    NSString* oldFieldValue;
    WaitAlert* waitAlert;

    // Per session state.
    NSTimer* refreshTimer;
    id<ICECommunicator> communicator;
	id<VoipSessionPrx> session;
	id<Glacier2RouterPrx> router;
	int sessionTimeout;
}

-(IBAction)login:(id)sender;
-(IBAction)call:(id)sender;
-(IBAction)sslChanged:(id)sender;

@end

