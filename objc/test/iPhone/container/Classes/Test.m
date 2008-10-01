// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>
#include <dlfcn.h>

@implementation Test

@synthesize name;
@synthesize path;

-(id)initWithPath:(NSString*)p name:(NSString*)n
{
    if(self = [super init])
    {
        name = [n retain];
        path = [p retain];
    }
    return self;
}

+(id)testWithPath:(NSString*)path name:(NSString*)n
{
    return [[[Test alloc] initWithPath:path name:n] autorelease];
}

-(BOOL)open
{
    NSAssert(handle == 0, @"handle == 0");
    handle = dlopen([path UTF8String], RTLD_LOCAL);
    if(handle == 0)
    {
        NSLog(@"dlopen of %@ failed: %s", path, strerror(errno));
        return NO;
    }
    
    runServer = (int(*)(int,char**))dlsym(handle, "startServer");
    if(runServer == 0)
    {
        NSLog(@"dlsym of startServer failed");
    }
    
    runClient = (int(*)(int,char**))dlsym(handle, "startClient");
    if(runClient == 0)
    {
        NSLog(@"dlsym of startClient failed");
    }
    
    if(runServer == 0 || runClient == 0)
    {
        [self close];
        return NO;
    }
    
    return YES;
}

-(void)close
{
    if(handle != 0)
    {
        dlclose(handle);
        handle = 0;
    }
}

-(int)server
{
    NSAssert(runServer != 0, @"server != 0");
    int argc = 0;
    char** argv = 0;
    return (*runServer)(argc, argv);
}

-(int)client
{
    int argc = 0;
    char** argv = 0;

    NSAssert(runClient != 0, @"server != 0");
    return (*runClient)(argc, argv);
}

-(void)dealloc
{
    [name release];
    [path release];
    [super dealloc];
}

@end
