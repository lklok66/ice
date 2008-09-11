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

@interface LoginViewController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate>
{
    IBOutlet UITextField* hostnameTextField;
    IBOutlet UIButton* loginButton;

@private
    BOOL showAlert;

    NSString* hostname;
    MainViewController *mainViewController;
    
    id<DemoSessionPrx> session;
    id<DemoLibraryPrx> library;
    NSOperationQueue* queue;
}

-(IBAction)login:(id)sender;

@end
