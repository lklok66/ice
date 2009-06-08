// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>
#include <dlfcn.h>

@implementation Test

@synthesize name;

+(id)testWithName:(NSString*)name
           server:(int (*)(int, char**))server
           client:(int (*)(int, char**))client;
{
    Test* t = [[Test alloc] init];
    if(t != nil)
    {
        t->name = name;
        t->server = server;
        t->client = client;
    }
    return t;
}

-(int)server
{
    NSAssert(server != 0, @"server != 0");
    int argc = 0;
    char** argv = 0;
    return (*server)(argc, argv);
}

-(int)client
{
    int argc = 0;
    char** argv = 0;

    NSAssert(client != 0, @"server != 0");
    return (*client)(argc, argv);
}

-(void)dealloc
{
    [name release];
    [super dealloc];
}

@end
