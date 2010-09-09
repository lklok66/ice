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
#import <AMITest.h>

#import <Foundation/NSAutoreleasePool.h>
#ifdef ICE_OBJC_GC
  #import <objc/objc-auto.h>
  #import <Foundation/NSGarbageCollector.h>
#endif

static int
run(id<ICECommunicator> communicator)
{
    void amiAllTests(id<ICECommunicator>);
    amiAllTests(communicator);
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main amiClient
#endif

int
main(int argc, char* argv[])
{
#ifdef ICE_OBJC_GC
    objc_startCollectorThread();
#endif
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    id<ICECommunicator> communicator = nil;

    @try
    {

        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultClientProperties(&argc, argv);
        //
        // In this test, we need at least two threads in the
        // client side thread pool for nested AMI.
        //
        [initData.properties setProperty:@"Ice.ThreadPool.Client.Size" value:@"2"];
        [initData.properties setProperty:@"Ice.ThreadPool.Client.SizeWarn" value:@"0"];
        [initData.properties setProperty:@"Ice.Warn.AMICallback" value:@"0"];

        //
        // We must set MessageSizeMax to an explicit values, because
        // we run tests to check whether Ice.MemoryLimitException is
        // raised as expected.
        //
        [initData.properties setProperty:@"Ice.MessageSizeMax" value:@"100"];

#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestAMI", @"::Test", 
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
#ifdef ICE_OBJC_GC
    [[NSGarbageCollector defaultCollector] collectExhaustively];
#endif
    return status;
}
