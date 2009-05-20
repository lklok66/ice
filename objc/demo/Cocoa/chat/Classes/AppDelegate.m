// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AppDelegate.h>
#import <ConnectController.h>

@implementation AppDelegate

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    connectController = [[ConnectController alloc] init];
    [connectController showWindow:self];
}

-(void)login:(id)sender
{
    [connectController showWindow:self];
}

-(void)setChatActive:(BOOL)active
{
    chatActive = active;
}

- (BOOL)validateMenuItem:(NSMenuItem *)item
{
    if ([item action] == @selector(login:))
    {
        return !chatActive && !connectController.window.isVisible;
    }
    return YES;
}

@end
