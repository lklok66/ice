//
//  LoginViewController.h
//  library
//
//  Created by Matthew Newhook on 9/5/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

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
    BOOL showAlert_;

    NSString* hostname_;
    MainViewController *mainViewController_;
    
    id<DemoSessionPrx> session_;
    id<DemoLibraryPrx> library_;
    NSOperationQueue* queue_;
}

-(IBAction)login:(id)sender;

@end
