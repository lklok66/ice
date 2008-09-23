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
@class ICEException;
@class ICEInitializationData;

@interface AppDelegate : NSObject <UIApplicationDelegate>
{
@private
	
	IBOutlet UIWindow *window;
	IBOutlet UINavigationController *navigationController;
    id<ICECommunicator> communicator;
    ICEInitializationData* initData;
    NSTimer* refreshTimer;
    id session;
    BOOL fatal;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) UINavigationController *navigationController;

@property (readonly) id<ICECommunicator> communicator;

-(void)sessionRefreshException:(ICEException*)ex;
-(void)logout;
-(void)setSession:(id)session timeout:(int)timeout;

@end

