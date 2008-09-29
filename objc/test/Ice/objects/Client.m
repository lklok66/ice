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

@interface ClientMyObjectFactory : NSObject<ICEObjectFactory>
@end

@implementation ClientMyObjectFactory
-(ICEObject*) create:(NSString*)type
{
    if([type isEqualToString:@"::Test::B"])
    {
        return [[BI alloc] init];
    }
    else if([type isEqualToString:@"::Test::C"])
    {
        return [[CI alloc] init];
    }
    else if([type isEqualToString:@"::Test::D"])
    {
        return [[DI alloc] init];
    }
    else if([type isEqualToString:@"::Test::E"])
    {
        return [[EI alloc] init];
    }
    else if([type isEqualToString:@"::Test::F"])
    {
        return [[FI alloc] init];
    }
    else if([type isEqualToString:@"::Test::I"])
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
    id<ICEObjectFactory> factory = [[[ClientMyObjectFactory alloc] init] autorelease];
    [communicator addObjectFactory:factory sliceId:@"::Test::B"];
    [communicator addObjectFactory:factory sliceId:@"::Test::C"];
    [communicator addObjectFactory:factory sliceId:@"::Test::D"];
    [communicator addObjectFactory:factory sliceId:@"::Test::E"];
    [communicator addObjectFactory:factory sliceId:@"::Test::F"];
    [communicator addObjectFactory:factory sliceId:@"::Test::I"];
    [communicator addObjectFactory:factory sliceId:@"::Test::J"];
    [communicator addObjectFactory:factory sliceId:@"::Test::H"];

    id<TestInitialPrx> allTests(id<ICECommunicator>, bool);
    id<TestInitialPrx> initial = allTests(communicator, NO);
    [initial shutdown];
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
    id<ICECommunicator> communicator;

    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultClientProperties(&argc, argv);
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
