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

-(id)init
{
    if(self = [super init])
    {
        connectController = [[ConnectController alloc] init];
    }
    return self;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [connectController showWindow:self];
}

-(void)login:(id)sender
{
    [connectController showWindow:self];
}

-(void)setLibraryActive:(BOOL)active
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