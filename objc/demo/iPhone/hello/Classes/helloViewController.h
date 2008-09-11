// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@protocol ICECommunicator;
@class DemoHelloPrx;

@interface helloViewController : UIViewController<UITextFieldDelegate,UIAlertViewDelegate> {
    IBOutlet UIButton* flushButton;
    IBOutlet UITextField* hostnameTextField;
    IBOutlet UILabel* statusLabel;
    IBOutlet UISwitch* batchSwitch;
    IBOutlet UISwitch* secureSwitch;
    IBOutlet UISlider* timeoutSlider;
    IBOutlet UISlider* delaySlider;
    IBOutlet UIActivityIndicatorView* activity;

@private
    NSString* hostname;
    enum DeliveryMode
    {
        DeliveryModeTwoway,
        DeliveryModeOneway,
        DeliveryModeDatagram,
        DeliveryModeBatchOneway,
        DeliveryModeBatchDatagram
    }
    deliveryMode;
    BOOL showAlert;
    BOOL batch;
    int delay;
    int timeout;
    DemoHelloPrx* hello;
}

@property (nonatomic, retain) UIButton* flushButton;
@property (nonatomic, retain) UITextField* hostnameTextField;
@property (nonatomic, retain) UILabel* statusLabel;
@property (nonatomic, retain) UISwitch* batchSwitch;
@property (nonatomic, retain) UISwitch* secureSwitch;
@property (nonatomic, retain) UISlider* timeoutSlider;
@property (nonatomic, retain) UISlider* delaySlider;
@property (nonatomic, retain) UIActivityIndicatorView* activity;

@property (nonatomic, copy) NSString* hostname;
@property (nonatomic, retain) DemoHelloPrx* hello;

- (void)modeChanged:(id)sender;
- (void)batchChanged:(id)sender;
- (void)delayChanged:(id)sender;
- (void)timeoutChanged:(id)sender;

- (void)sayHello:(id)sender;
- (void)flushBatch:(id) sender;

- (void)shutdown: (id)sender;

@end

