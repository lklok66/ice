// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>

#import <TestCommon.h>

#import <Foundation/NSAutoreleasePool.h>
 
static int
run(id<ICECommunicator> communicator)
{
    [[communicator getProperties] setProperty:@"TestOperationsAdapter.Endpoints" value:@"default -p 12010:udp"];
    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestOperationsAdapter"];
    [adapter add:[[[TestOperationsMyDerivedClassI alloc] init] autorelease]
             identity:[communicator stringToIdentity:@"test"]];
    [adapter activate];

    serverReady(communicator);

    [communicator waitForShutdown];
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main startServer
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
        initData.properties = defaultServerProperties(&argc, argv);

        //
        // Its possible to have batch oneway requests dispatched after
        // the adapter is deactivated due to thread scheduling so we
        // supress this warning.
        //
        [initData.properties setProperty:@"Ice.Warn.Dispatch" value:@"0"];

#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestOperations", @"::Test", 
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
