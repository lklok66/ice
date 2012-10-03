// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <objects/TestI.h>

#import <Foundation/NSAutoreleasePool.h>
#ifdef ICE_OBJC_GC
  #import <Foundation/NSGarbageCollector.h>
#endif

@interface ServerMyObjectFactory : NSObject<ICEObjectFactory>
@end

@implementation ServerMyObjectFactory
// Note that the object factory must not autorelease the
// returned objects.
-(ICEObject*) create:(NSString*)type
{
    if([type isEqualToString:@"::Test::I"])
    {
        return [[TestObjectsII alloc] init];
    }
    else if([type isEqualToString:@"::Test::J"])
    {
        return [[TestObjectsJI alloc] init];
    }
    else if([type isEqualToString:@"::Test::H"])
    {
        return [[TestObjectsHI alloc] init];
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
    
    [[communicator getProperties] setProperty:@"TestAdapter.Endpoints" value:@"default -p 12010"];
    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestAdapter"];
    ICEObject* initial = [[[TestObjectsInitialI alloc] init] autorelease];
    [adapter add:initial identity:[communicator stringToIdentity:@"initial"]];
    ICEObject* uoet = [[[UnexpectedObjectExceptionTestI alloc] init] autorelease];
    [adapter add:uoet identity:[communicator stringToIdentity:@"uoet"]];
    [adapter activate];

    serverReady(communicator);

    [communicator waitForShutdown];
    return EXIT_SUCCESS;
}

#if TARGET_OS_IPHONE
#  define main objectsServer
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
#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestObjects", @"::Test", 
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
#ifdef ICE_OBJC_GC
    [[NSGarbageCollector defaultCollector] collectExhaustively];
#endif
    return status;
}
