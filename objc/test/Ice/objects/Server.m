// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <TestI.h>

#import <Foundation/NSAutoreleasePool.h>

@interface ServerMyObjectFactory : NSObject<ICEObjectFactory>
@end

@implementation ServerMyObjectFactory
-(ICEObject*) create:(NSString*)type
{
    if([type isEqualToString:@"::Test::I"])
    {
        return [[II alloc] init];
    }
    else if([type isEqualToString:@"::Test::J"])
    {
        return [[JI alloc] init];
    }
    else if([type isEqualToString:@"::Test::H"])
    {
        return [[HI alloc] init];
    }
    
    test(NO); // Should never be reached
    return 0;
}

-(void) destroy
{
    // Nothing to do
}
@end

static int
run(id<ICECommunicator> communicator)
{
    id<ICEObjectFactory> factory = [[[ServerMyObjectFactory alloc] init] autorelease];
    [communicator addObjectFactory:factory sliceId:@"::Test::I"];
    [communicator addObjectFactory:factory sliceId:@"::Test::J"];
    [communicator addObjectFactory:factory sliceId:@"::Test::H"];
    
    [[communicator getProperties] setProperty:@"TestAdapter.Endpoints" value:@"default -p 12010 -t 10000"];
    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestAdapter"];
    ICEObject* initial = [[[InitialI alloc] init] autorelease];
    [adapter add:initial identity:[communicator stringToIdentity:@"initial"]];
    ICEObject* uoet = [[[UnexpectedObjectExceptionTestI alloc] init] autorelease];
    [adapter add:uoet identity:[communicator stringToIdentity:@"uoet"]];
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
