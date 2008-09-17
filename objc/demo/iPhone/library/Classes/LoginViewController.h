// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@protocol DemoSessionPrx;
@protocol DemoLibraryPrx;
@protocol DemoSessionFactoryPrx;
@class MainViewController;
@protocol SessionAdapter;

@interface LoginViewController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate>
{
@private
    IBOutlet UITextField* hostnameTextField;
    IBOutlet UIButton* loginButton;
    IBOutlet UISwitch* glacier2Switch;
    
    BOOL showAlert;

    NSString* hostname;
    MainViewController *mainViewController;
    
    id session;
    id<DemoLibraryPrx> library;
    int sessionTimeout;
    NSOperationQueue* queue;
}

-(IBAction)login:(id)sender;
-(void)glacier2Changed:(id)sender;

@end
