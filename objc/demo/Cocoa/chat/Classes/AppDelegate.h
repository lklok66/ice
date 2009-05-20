// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Cocoa/Cocoa.h>

@class ConnectController;

@interface AppDelegate : NSObject
{
    ConnectController* connectController;
    BOOL chatActive;
}

-(void)login:(id)sender;
-(void)setChatActive:(BOOL)active;

@end
