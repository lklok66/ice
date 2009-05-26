// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Cocoa/Cocoa.h>

@protocol ICECommunicator;

@interface LoginController : NSWindowController
{
@private
    IBOutlet NSTextField* chatServerField;
    IBOutlet NSTextField* usernameField;
    IBOutlet NSTextField* passwordField;
    IBOutlet NSButton* sslField;
    IBOutlet NSButton* routerField;
    IBOutlet NSTextField* routerServerField;
    IBOutlet NSWindow* advancedSheet;
    IBOutlet NSWindow* connectingSheet;
    IBOutlet NSProgressIndicator* progress;
    id<ICECommunicator> communicator;
    id<ICECommunicator> validationCommunicator;
    NSOperationQueue* queue;
}

-(void)doLogin:(id)sender;
-(void)routerChanged:(id)sender;
-(void)showAdvancedSheet:(id)sender;
-(void)closeAdvancedSheet:(id)sender;

@end
