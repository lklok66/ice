// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <facets/TestI.h>
#import <TestCommon.h>

#import <Foundation/NSAutoreleasePool.h>
#if ICE_OBJC_GC
  #import <objc/objc-auto.h>
  #import <Foundation/NSGarbageCollector.h>
#endif

static int
run(id<ICECommunicator> communicator)
{
    [[communicator getProperties] setProperty:@"TestAdapter.Endpoints" value:@"default -p 12010"];

    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestAdapter"];
    ICEObject* d = [[[TestFacetsDI alloc] init] autorelease];
    [adapter add:d identity:[communicator stringToIdentity:@"d"]];
    [adapter addFacet:d identity:[communicator stringToIdentity:@"d"] facet:@"facetABCD"];
    ICEObject* f = [[[TestFacetsFI alloc] init] autorelease];
    [adapter addFacet:f identity:[communicator stringToIdentity:@"d"] facet:@"facetEF"];
    ICEObject* h = [[[TestFacetsHI alloc] init] autorelease];
    [adapter addFacet:h identity:[communicator stringToIdentity:@"d"] facet:@"facetGH"];

    [adapter activate];

    serverReady(communicator);

    [communicator waitForShutdown];

    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main facetsServer
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
        initData.properties = defaultServerProperties(&argc, argv);
#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestFacets", @"::Test", 
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
#if ICE_OBJC_GC
    [[NSGarbageCollector defaultCollector] collectExhaustively];
#endif
    return status;
}
