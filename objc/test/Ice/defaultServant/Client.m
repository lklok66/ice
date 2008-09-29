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
#import <Test.h>
#import <MyObjectI.h>

#import <Foundation/NSAutoreleasePool.h>

static int
run(id<ICECommunicator> communicator)
{
    //
    // Create OA
    //  
    id<ICEObjectAdapter> oa = [communicator createObjectAdapterWithEndpoints:@"MyOA" endpoints:@"tcp -h localhost"];
    [oa activate];

    ICEObject* servant = [[MyObjectI alloc] init];
    [servant autorelease];

    //
    // Register default servant with category "foo"
    //
    [oa addDefaultServant:servant category:@"foo"];
    
    //
    // Start test
    //
    tprintf("testing single category... ");

    ICEObject* r = [oa findDefaultServant:@"foo"];
    test(r == servant);
    
    r = [oa findDefaultServant:@"bar"];
    test(r == nil);

    ICEIdentity* identity = [ICEIdentity identity:@"" category:@"foo"];

    NSArray* stringArray = [NSArray arrayWithObjects:@"foo", @"bar", @"x", @"y", @"abcdefg", nil];

    for(NSString* name in stringArray)
    {
        [identity setName:name]; 
        id<TestMyObjectPrx> prx = [TestMyObjectPrx uncheckedCast:[oa createProxy:identity]];
        [prx ice_ping];
        test([[prx getName] isEqualToString:name]);
    }

    [identity setName:@"ObjectNotExist"]; 
    id<TestMyObjectPrx> prx = [TestMyObjectPrx uncheckedCast:[oa createProxy:identity]];
    @try
    {
        [prx ice_ping];
        test(NO);
    }
    @catch(ICEObjectNotExistException*)
    {
        // expected
    }
    
    @try
    {
        [prx getName];
        test(NO);
    }
    @catch(ICEObjectNotExistException*)
    {
        // expected
    }

    [identity setName:@"FacetNotExist"]; 
    prx = [TestMyObjectPrx uncheckedCast:[oa createProxy:identity]];

     @try
    {
        [prx ice_ping];
        test(NO);
    }
    @catch(ICEFacetNotExistException*)
    {
        // expected
    }
    
    @try
    {
        [prx getName];
        test(NO);
    }
    @catch(ICEFacetNotExistException*)
    {
        // expected
    }

    [identity setCategory:@"bar"];
    for(NSString* name in stringArray)
    {
        [identity setName:name]; 
        id<TestMyObjectPrx> prx = [TestMyObjectPrx uncheckedCast:[oa createProxy:identity]];

        @try
        {
            [prx ice_ping];
            test(NO);
        }
        @catch(ICEObjectNotExistException*)
        {
        }

        @try
        {
            [prx getName];
            test(NO);
        }
        @catch(ICEObjectNotExistException*)
        {
        }
    }
    tprintf("ok\n");
    
    tprintf("testing default category... ");
    
    [oa addDefaultServant:servant category:@""];
    
    r = [oa findDefaultServant:@"bar"];
    test(r == nil);

    r = [oa findDefaultServant:@""];
    test(r == servant);
    
    for(NSString* name in stringArray)
    {
        [identity setName:name]; 
        id<TestMyObjectPrx> prx = [TestMyObjectPrx uncheckedCast:[oa createProxy:identity]];
        [prx ice_ping];
        test([[prx getName] isEqualToString:name]);
    }
    tprintf("ok\n");
  
    return 0;
}

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#  define main startClient

int
startServer(int argc, char* argv[])
{
    serverReady(nil);
    return 0;
}
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

