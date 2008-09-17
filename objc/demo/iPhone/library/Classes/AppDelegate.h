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
@class ICEInitializationData;

@interface AppDelegate : NSObject <UIApplicationDelegate>
{
    id<ICECommunicator> communicator;
    id session;
    BOOL fatal;
    
@private

    IBOutlet UIWindow *window;
    IBOutlet UINavigationController *navigationController;

    NSTimer* refreshTimer;
    ICEInitializationData* initData;
}

@property (nonatomic, retain) id<ICECommunicator> communicator;
@property (nonatomic, readonly) id session;
@property (nonatomic) BOOL fatal;

-(void)sessionRefreshException:(ICEException*)ex;
-(void)logout;
-(void)setSession:(id)session timeout:(int)timeout;

@end

