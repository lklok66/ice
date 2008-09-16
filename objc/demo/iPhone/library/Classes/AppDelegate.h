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
@protocol DemoSessionPrx;
@class ICEException;

@interface AppDelegate : NSObject <UIApplicationDelegate> {
	
    IBOutlet UIWindow *window;
    IBOutlet UINavigationController *navigationController;
    id<ICECommunicator> communicator;
    id<DemoSessionPrx> session;

    BOOL fatal;
    
@private
    NSTimer* refreshTimer;

}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) UINavigationController *navigationController;
@property (nonatomic, retain) id<ICECommunicator> communicator;
@property (nonatomic, retain) id<DemoSessionPrx> session;
@property (nonatomic) BOOL fatal;

-(void)sessionRefreshException:(ICEException*)ex;

@end

