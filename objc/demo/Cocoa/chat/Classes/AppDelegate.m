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

@synthesize connectController;

-(id)init
{
    if(self = [super init])
    {
        connectController = [[ConnectController alloc] init];
    }
    return self;
}

-(void)switchController:(NSViewController*)controller
{
    NSApplication* app = [NSApplication sharedApplication];
    NSWindow* window = app.mainWindow;
    
    // Compute the new window frame size.
    NSSize currentSize = [window.contentView frame].size;
    NSSize newSize = [controller.view frame].size; 
    float deltaWidth = newSize.width - currentSize.width; 
    float deltaHeight = newSize.height - currentSize.height; 
    NSRect windowFrame = [window frame]; 
    windowFrame.size.height += deltaHeight; 
    windowFrame.origin.y -= deltaHeight; 
    windowFrame.size.width += deltaWidth; 

    // Clear the box for resizing
    [window setContentView:nil];
    [window setFrame:windowFrame display:YES animate:YES]; 
    
    window.delegate = controller;
    window.contentView = controller.view;
}

- (void)windowDidBecomeMain:(NSNotification *)notification
{
    NSLog(@"- (void)windowDidBecomeMain:(NSNotification *)notification");
    [self switchController:connectController];
}

@end