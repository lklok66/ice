// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <DefaultValueTest.h>

#import <Foundation/NSAutoreleasePool.h>
#ifdef ICE_OBJC_GC
  #import <Foundation/NSGarbageCollector.h>
#endif

static int
run()
{
    void defaultValueAllTests();
    defaultValueAllTests();
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main defaultValueClient
#endif

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    @try
    {
        status = run();
    }
    @catch(ICEException* ex)
    {
        tprintf("%@\n", ex);
        status = EXIT_FAILURE;
    }
    @catch(TestFailedException* ex)
    {
        status = EXIT_FAILURE;
    }
    [pool release];
#ifdef ICE_OBJC_GC
    [[NSGarbageCollector defaultCollector] collectExhaustively];
#endif
    return status;
}
