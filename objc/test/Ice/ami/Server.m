// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <ami/TestI.h>

#import <TestCommon.h>

#import <Foundation/NSAutoreleasePool.h>
#ifndef ICE_OBJC_NOGC
  #import <objc/objc-auto.h>
  #import <Foundation/NSGarbageCollector.h>
#endif
 
static int
run(id<ICECommunicator> communicator)
{
    [[communicator getProperties] setProperty:@"TestAMIAdapter.Endpoints" value:@"default -p 12010:udp"];
    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestAMIAdapter"];
    [adapter add:[[[TestAMITestIntfI alloc] init] autorelease] identity:[communicator stringToIdentity:@"test"]];
    [adapter activate];

    serverReady(communicator);

    [communicator waitForShutdown];
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main amiServer
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
        initData.properties = defaultServerProperties(&argc, argv);

        //
        // Its possible to have batch oneway requests dispatched after
        // the adapter is deactivated due to thread scheduling so we
        // supress this warning.
        //
        [initData.properties setProperty:@"Ice.Warn.Dispatch" value:@"0"];

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
