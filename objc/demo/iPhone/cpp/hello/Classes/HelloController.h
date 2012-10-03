// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@protocol ICECommunicator;

@interface HelloController : UIViewController<UITextFieldDelegate, UIAlertViewDelegate, UIPickerViewDataSource,
                                                  UIPickerViewDelegate >
{
@private
    IBOutlet UIButton* flushButton;
    IBOutlet UITextField* hostnameTextField;
    IBOutlet UILabel* statusLabel;
    IBOutlet UIPickerView* modePicker;
    IBOutlet UISlider* timeoutSlider;
    IBOutlet UISlider* delaySlider;
    IBOutlet UIActivityIndicatorView* activity;

    BOOL showAlert;
    id<ICECommunicator> communicator;
}

- (void)startAnimating;
- (void)stopAnimating;
- (void)requestSend;
- (void)flushBatchSend;
- (void)waitingForResponse;
- (void)sendingRequest;
- (void)ready;
- (void)queuedRequest;
- (void)sayHello:(id)sender;
- (void)flushBatch:(id) sender;
- (void)shutdown: (id)sender;
- (void)exception:(NSString*)s;

@end

