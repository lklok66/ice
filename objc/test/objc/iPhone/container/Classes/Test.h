// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>


@interface Test : NSObject
{
@private
    int (*server)(int,char**);
    int (*client)(int,char**);
    NSString* name;
}

+(id)testWithName:(NSString*)name
           server:(int (*)(int, char**))server
           client:(int (*)(int, char**))client;

-(BOOL)hasServer;
-(int)server;
-(int)client;

@property (readonly) NSString* name;

@end
