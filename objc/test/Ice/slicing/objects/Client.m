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
#import <TestClient.h>

#import <Foundation/NSAutoreleasePool.h>

static int
run(id<ICECommunicator> communicator)
{
    id<TestSlicingObjectsClientTestIntfPrx> allTests(id<ICECommunicator>);
    id<TestSlicingObjectsClientTestIntfPrx> TestSlicingObjectsClient = allTests(communicator);
    [TestSlicingObjectsClient shutdown];
    return EXIT_SUCCESS;
}

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
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
#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestSlicingObjectsClient", @"::Test", 
                                  @"TestSlicingObjectsShared", @"::TestShared", 
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
