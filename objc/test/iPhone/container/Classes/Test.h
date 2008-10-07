// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>


@interface Test : NSObject
{
@private
    void* handle;
    int (*runServer)(int,char**);
    int (*runClient)(int,char**);
    NSString* name;
    NSString* path;
}

-(id)initWithPath:(NSString*)path name:(NSString*)n;
+(id)testWithPath:(NSString*)path name:(NSString*)n;
-(BOOL)open;
-(void)close;

-(int)server;
-(int)client;

@property (readonly) NSString* name;
@property (readonly) NSString* path;

@end
