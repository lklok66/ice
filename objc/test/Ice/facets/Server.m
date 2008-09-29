// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>
#import <TestCommon.h>

#import <Foundation/NSAutoreleasePool.h>

static int
run(id<ICECommunicator> communicator)
{
    [[communicator getProperties] setProperty:@"TestAdapter.Endpoints" value:@"default -p 12010 -t 10000"];

    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestAdapter"];
    ICEObject* d = [[[DI alloc] init] autorelease];
    [adapter add:d identity:[communicator stringToIdentity:@"d"]];
    [adapter addFacet:d identity:[communicator stringToIdentity:@"d"] facet:@"facetABCD"];
    ICEObject* f = [[[FI alloc] init] autorelease];
    [adapter addFacet:f identity:[communicator stringToIdentity:@"d"] facet:@"facetEF"];
    ICEObject* h = [[[HI alloc] init] autorelease];
    [adapter addFacet:h identity:[communicator stringToIdentity:@"d"] facet:@"facetGH"];

    [adapter activate];

    serverReady(communicator);

    [communicator waitForShutdown];

    return EXIT_SUCCESS;
}

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#  define main startServer
#endif

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    id<ICECommunicator> communicator;

    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultServerProperties(&argc, argv);
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
