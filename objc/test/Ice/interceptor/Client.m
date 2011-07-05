// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <InterceptorTest.h>
#import <interceptor/MyObjectI.h>
#import <interceptor/InterceptorI.h>

#import <Foundation/NSAutoreleasePool.h>
#ifdef ICE_OBJC_GC
  #import <objc/objc-auto.h>
  #import <Foundation/NSGarbageCollector.h>
#endif

static int
run(id<ICECommunicator> communicator)
{
    //
    // Create OA and servants  
    //  
    id<ICEObjectAdapter> oa = [communicator createObjectAdapterWithEndpoints:@"MyOA" endpoints:@"tcp -h localhost"];
    
    ICEObject* servant = [[[TestInterceptorMyObjectI alloc] init] autorelease];
    InterceptorI* interceptor = [[[InterceptorI alloc] init:servant] autorelease];
    
    id<TestInterceptorMyObjectPrx> prx = [TestInterceptorMyObjectPrx uncheckedCast:[oa addWithUUID:interceptor]];
    
    [oa activate];
       
    tprintf("testing simple interceptor... ");
    test([[interceptor getLastOperation] length] == 0);
    [prx ice_ping];
    test([[interceptor getLastOperation] isEqualToString:@"ice_ping"]);
    test([interceptor getLastStatus]);
    NSString* typeId = [prx ice_id];
    test([[interceptor getLastOperation] isEqualToString:@"ice_id"]);
    test([interceptor getLastStatus]);
    test([prx ice_isA:typeId]);
    test([[interceptor getLastOperation] isEqualToString:@"ice_isA"]);
    test([interceptor getLastStatus]);
    test([prx add:33 y:12] == 45);
    test([[interceptor getLastOperation] isEqualToString:@"add"]);
    test([interceptor getLastStatus]);
    tprintf("ok\n");

    tprintf("testing retry... ");
    test([prx addWithRetry:33 y:12] == 45);
    test([[interceptor getLastOperation] isEqualToString:@"addWithRetry"]);
    test([interceptor getLastStatus]);
    tprintf("ok\n");
   
    tprintf("testing user exception... ");
    @try
    {
        [prx badAdd:33 y:12];
        test(NO);
    }
    @catch(TestInterceptorInvalidInputException*)
    {
        // expected
    }
    test([[interceptor getLastOperation] isEqualToString:@"badAdd"]);
    test([interceptor getLastStatus] == NO);
    tprintf("ok\n");
    tprintf("testing ONE... ");
    
    [interceptor clear];
    @try
    {
        [prx notExistAdd:33 y:12];
        test(NO);
    }
    @catch(ICEObjectNotExistException*)
    {
        // expected
    }
    test([[interceptor getLastOperation] isEqualToString:@"notExistAdd"]);
    tprintf("ok\n");
    tprintf("testing system exception... ");
    [interceptor clear];
    @try
    {
        [prx badSystemAdd:33 y:12];
        test(NO);
    }
    @catch(ICEUnknownLocalException*)
    {
    }
    @catch(NSException*)
    {
        test(NO);
    }
    test([[interceptor getLastOperation] isEqualToString:@"badSystemAdd"]);
    tprintf("ok\n");
    
    return 0;
}

#if TARGET_OS_IPHONE
#  define main interceptorClient

int
interceptorServer(int argc, char* argv[])
{
    serverReady(nil);
    return 0;
}
#endif

int
main(int argc, char* argv[])
{
#ifdef ICE_OBJC_GC
    objc_startCollectorThread();
#endif
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    id<ICECommunicator> communicator = nil;

    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = defaultClientProperties(&argc, argv);
#if TARGET_OS_IPHONE
        initData.prefixTable__ = [NSDictionary dictionaryWithObjectsAndKeys:
                                  @"TestInterceptor", @"::Test", 
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
#ifdef ICE_OBJC_GC
    [[NSGarbageCollector defaultCollector] collectExhaustively];
#endif
    return status;
}

