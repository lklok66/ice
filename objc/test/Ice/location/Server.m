// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <ServerLocator.h>
#import <TestI.h>
#import <TestCommon.h>

#import <Foundation/NSAutoreleasePool.h>

static int
run(id<ICECommunicator> communicator, ICEInitializationData* initData)
{
    //
    // Register the server manager. The server manager creates a new
    // 'server' (a server isn't a different process, it's just a new
    // communicator and object adapter).
    //
    [[communicator getProperties] setProperty:@"Ice.ThreadPool.Server.Size" value:@"2"];
    [[communicator getProperties] setProperty:@"ServerManager.Endpoints" value:@"default -p 12010:udp"];

    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"ServerManager"];

    //
    // We also register a sample server locator which implements the
    // locator interface, this locator is used by the clients and the
    // 'servers' created with the server manager interface.
    //
    ServerLocatorRegistry* registry = [[[ServerLocatorRegistry alloc] init] autorelease];
    [registry addObject:[adapter createProxy:[communicator stringToIdentity:@"ServerManager"]]];
    ServerManagerI* serverManager = [[[ServerManagerI alloc] init:registry initData:initData] autorelease];
    [adapter add:serverManager identity:[communicator stringToIdentity:@"ServerManager"]];

    id<ICELocatorRegistryPrx> registryPrx = 
        [ICELocatorRegistryPrx uncheckedCast:[adapter add:registry 
                                                      identity:[communicator stringToIdentity:@"registry"]]];

    ICELocator* locator = [[[ServerLocator alloc] init:registry proxy:registryPrx] autorelease];
    [adapter add:locator identity:[communicator stringToIdentity:@"locator"]];

    [adapter activate];

    serverReady(communicator);

    [communicator waitForShutdown];
    [serverManager terminate];

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
    id<ICECommunicator> communicator = nil;

    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultServerProperties(&argc, argv);
        communicator = [ICEUtil createCommunicator:&argc argv:argv initData:initData];
        status = run(communicator, initData);
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
