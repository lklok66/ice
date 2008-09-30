// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import "bounceAppDelegate.h"

//
// Avoid warning for undocumented method.
//
@interface UIApplication(UndocumentedAPI)
-(void)launchApplicationWithIdentifier:(NSString*)id suspended:(BOOL)flag;
@end

@implementation bounceAppDelegate

@synthesize window;

-(void)launch
{
    [[UIApplication sharedApplication] launchApplicationWithIdentifier:@"com.zeroc.container" suspended:NO];

}

- (void)applicationDidFinishLaunching:(UIApplication *)application {    

    // Override point for customization after application launch
    [window makeKeyAndVisible];
    // Cannot launch directly. Use a timer to re-launch.
    NSTimer* timer = [NSTimer
                         timerWithTimeInterval:.1f
                         target:self
                         selector:@selector(launch)
                         userInfo:nil
                         repeats:NO];
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    
}


- (void)dealloc {
    [window release];
    [super dealloc];
}


@end
