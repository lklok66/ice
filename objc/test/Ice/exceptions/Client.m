// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <ExceptionsTest.h>

#import <Foundation/NSAutoreleasePool.h>
#if ICE_OBJC_GC
  #import <objc/objc-auto.h>
  #import <Foundation/NSGarbageCollector.h>
#endif
 
static int
run(id<ICECommunicator> communicator)
{
    TestExceptionsThrowerPrx* exceptionsAllTests(id<ICECommunicator>, BOOL);
    TestExceptionsThrowerPrx* thrower = exceptionsAllTests(communicator, NO);
    [thrower shutdown];
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main exceptionsClient
#endif

int
main(int argc, char* argv[])
{
#if ICE_OBJC_GC
    objc_startCollectorThread();
#endif
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    id<ICECommunicator> communicator = nil;

    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultClientProperties(&argc, argv);
#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestExceptions", @"::Test", 
                                  @"TestExceptionsMod", @"::Test::Mod", 
                                nil];
#endif
        communicator = [ICEUtil createCommunicator:&argc argv:argv initData:initData];
        status = run(communicator);
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

    if(communicator)
    {
        @try
        {
            [communicator destroy];
        }
        @catch(ICEException* ex)
        {
	    tprintf("%@\n", ex);
            status = EXIT_FAILURE;
        }
    }
    
    [pool release];
#if ICE_OBJC_GC
    [[NSGarbageCollector defaultCollector] collectExhaustively];
#endif
    return status;
}
