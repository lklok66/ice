// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@class helloViewController;

@interface helloAppDelegate : NSObject <UIApplicationDelegate> {
    IBOutlet UIWindow *window;
    IBOutlet helloViewController *viewController;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) helloViewController *viewController;

@end