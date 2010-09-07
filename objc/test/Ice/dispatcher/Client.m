// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <DispatcherTest.h>

#include <dispatch/dispatch.h>

#import <Foundation/NSAutoreleasePool.h>
#ifndef ICE_OBJC_NOGC
  #import <objc/objc-auto.h>
  #import <Foundation/NSGarbageCollector.h>
#endif

static int
run(id<ICECommunicator> communicator)
{
    TestDispatcherTestIntfPrx* dispatcherAllTests(id<ICECommunicator>);
    TestDispatcherTestIntfPrx* dispatcher = dispatcherAllTests(communicator);
    [dispatcher shutdown];
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main dispatcherClient
#endif

int
main(int argc, char* argv[])
{
#ifndef ICE_OBJC_NOGC
    objc_startCollectorThread();
#endif
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    id<ICECommunicator> communicator = nil;

    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultClientProperties(&argc, argv);
        initData.dispatcher = ^(id<ICEDispatcherCall> call, id<ICEConnection> con) {
            dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^ { [call run]; });
        };
#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestDispatcher", @"::Test", 
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
#ifndef ICE_OBJC_NOGC
    [[NSGarbageCollector defaultCollector] collectExhaustively];
#endif
    return status;
}
