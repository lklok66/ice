// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#import <UIKit/UIKit.h>
#import <Ice/Ice.h>

@class RouterI;
@class RootViewController;
@protocol LoggingDelegate;

@interface Logger : NSObject<ICELogger>
{
    NSObject<LoggingDelegate>* delegate;
}

@property (retain) NSObject<LoggingDelegate>* delegate;

@end

@interface AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    RootViewController *rootViewController;
    id<ICECommunicator> communicator;
    RouterI* router;
    Logger* logger;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet RootViewController *rootViewController;
@property (nonatomic, retain) id<ICECommunicator> communicator;
@property (nonatomic, retain) RouterI* router;
@property (nonatomic, retain) Logger* logger;

-(void)initializeRouter;

@end

