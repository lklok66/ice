// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <TestCommon.h>

#include <stdarg.h>
#include <stdlib.h>

#import <Ice/Ice.h>

@implementation TestFailedException
-(id)init
{
    return [super initWithName:@"TestFailedException" reason:nil userInfo:nil];
}
@end

id<ICEProperties>
defaultServerProperties(int *argc, char** argv)
{
    id<ICEProperties> properties = [ICEUtil createProperties];
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
    static NSString* defaults[] =
    {
        @"Ice.NullHandleAbort", @"1",
        @"Ice.Warn.Connections", @"1",
        @"Ice.ThreadPool.Server.Size", @"1",
        @"Ice.ThreadPool.Server.SizeMax", @"3",
        @"Ice.ThreadPool.Server.SizeWarn", @"0",
        @"Ice.PrintAdapterReady", @"1",
        @"Ice.ServerIdleTime", @"30",
        @"Ice.Default.Host", @"127.0.0.1"
    };
    int i;
    for(i = 0; i < sizeof(defaults)/sizeof(defaults[0]); i += 2)
    {
        [properties setProperty:defaults[i] value:defaults[i+1]];
    }
#endif

    NSArray* args = [properties parseIceCommandLineOptions:[ICEUtil argsToStringSeq:*argc argv:argv]];
    [ICEUtil stringSeqToArgs:args argc:argc argv:argv];

    return properties;
}

id<ICEProperties>
defaultClientProperties(int* argc, char** argv)
{
    id<ICEProperties> properties = [ICEUtil createProperties];
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
    static NSString* defaults[] =
    {
        @"Ice.NullHandleAbort", @"1",
        @"Ice.Warn.Connections", @"1",
        @"Ice.Default.Host", @"127.0.0.1"
    };
    int i;
    for(i = 0; i < sizeof(defaults)/sizeof(defaults[0]); i += 2)
    {
        [properties setProperty:defaults[i] value:defaults[i+1]];
    }
#endif

    NSArray* args = [properties parseIceCommandLineOptions:[ICEUtil argsToStringSeq:*argc argv:argv]];
    [ICEUtil stringSeqToArgs:args argc:argc argv:argv];

    return properties;
}

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)

#import <Foundation/NSString.h>
#import <Foundation/NSObject.h>
#import <Foundation/NSThread.h>

#include <Ice/Ice.h>

static id outputTarget;
static SEL readySelector;
static SEL outputSelector;
static id<ICECommunicator> communicator;

void
tprintInit(id target, SEL output, SEL ready)
{
    outputTarget = [target retain];
    outputSelector = output;
    readySelector = ready;
}

void
serverReady(id<ICECommunicator> c)
{
    communicator = [c retain];
    [outputTarget
     performSelectorOnMainThread:readySelector
     withObject:nil
     waitUntilDone:NO];
}

void
serverStop()
{
    [communicator shutdown];
}

void
tprintf(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    NSString* s = [[[NSString alloc] initWithFormat:[NSString stringWithCString:fmt encoding:NSUTF8StringEncoding]
                             arguments:va] autorelease];
    va_end(va);
    [outputTarget performSelectorOnMainThread:outputSelector withObject:s waitUntilDone:NO];
}

void
testFailed(const char* expr, const char* file, unsigned int line)
{
    tprintf("failed!\n");
    tprintf("%s:%u: assertion `%s' failed\n", file, line, expr);

    @throw [[TestFailedException alloc] init];
}

#else

#include <stdio.h>

void
tprintf(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    NSString* s = [[[NSString alloc] initWithFormat:[NSString stringWithCString:fmt encoding:NSUTF8StringEncoding]
                             arguments:va] autorelease];
    va_end(va);
    fputs([s UTF8String], stdout);
    fflush(stdout);
}

void
serverReady(id<ICECommunicator> c)
{
}

void
testFailed(const char* expr, const char* file, unsigned int line)
{
    tprintf("failed!\n");
    tprintf("%s:%u: assertion `%s' failed\n", file, line, expr);
    abort();
}
#endif
