// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <Test.h>

#import <Foundation/NSAutoreleasePool.h>

static int
run(id<ICECommunicator> communicator)
{
    id<TestTimeoutTimeoutPrx> allTests(id<ICECommunicator>);
    id<TestTimeoutTimeoutPrx> timeout = allTests(communicator);
    [timeout shutdown];
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main startClient
#endif

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    id<ICECommunicator> communicator = nil;

    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultClientProperties(&argc, argv);

        //
        // For this test, we want to disable retries.
        //
        [initData.properties setProperty:@"Ice.RetryIntervals" value:@"-1"];

        //
        // This test kills connections, so we don't want warnings.
        //
        [initData.properties setProperty:@"Ice.Warn.Connections" value:@"0"];

#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestTimeout", @"::Test", 
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
    return status;
}
