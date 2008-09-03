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

#import <Foundation/Foundation.h>

@interface Callback : NSObject
{
    BOOL called;
    NSCondition* cond;
}
-(BOOL) check;
-(void) called;
@end

@implementation Callback
-(id) init
{
    if(![super init])
    {
        return nil;
    }
    cond = [[NSCondition alloc] init];
    return self;
}
-(void) dealloc
{
    [cond release];
    [super dealloc];
}
-(BOOL) check
{
    [cond lock];
    while(!called)
    {
        if(![cond waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:50]])
        {
            return NO;
        }
    }
    called = NO;
    [cond unlock];
    return YES;
}
-(void) called
{
    [cond lock];
    called = YES;
    [cond signal];
    [cond unlock];
}
-(void) sendDataResponse
{
    [self called];
}
-(void) sendDataException:(ICEException*)ex
{
    test(NO);
}
-(void) sendDataExResponse
{
    test(NO);
}
-(void) sendDataExException:(ICEException*)ex
{
    test([ex isKindOfClass:[ICETimeoutException class]]);
    [self called];
}
-(void) sleepResponse
{
    [self called];
}
-(void) sleepException:(ICEException*)ex
{
    test(NO);
}
-(void) sleepExResponse
{
    test(NO);
}
-(void) sleepExException:(ICEException*)ex
{
    test([ex isKindOfClass:[ICETimeoutException class]]);
    [self called];
}
@end

id<TestTimeoutPrx>
allTests(id<ICECommunicator> communicator)
{
    NSString* sref = @"timeout:default -p 12010 -t 10000";
    id<ICEObjectPrx> obj = [communicator stringToProxy:sref];
    test(obj);

    id<TestTimeoutPrx> timeout = [TestTimeoutPrx checkedCast:obj];
    test(timeout);

    tprintf("testing connect timeout... ");
    {
        //
        // Expect ConnectTimeoutException.
        //
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:250]];
        [to holdAdapter:750];
        [[to ice_getConnection] close:YES]; // Force a reconnect.
        @try
        {
            [to op];
            test(NO);
        }
        @catch(ICEConnectTimeoutException*)
        {
            // Expected.
        }
    }
    {
        //
        // Expect success.
        //
        [timeout op]; // Ensure adapter is active.
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:1000]];
        [to holdAdapter:500];
        [[to ice_getConnection] close:YES]; // Force a reconnect.
        @try
        {
            [to op];
        }
        @catch(ICEConnectTimeoutException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("testing read timeout... ");
    {
        //
        // Expect TimeoutException.
        //
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:500]];
        @try
        {
            [to sleep:750];
            test(NO);
        }
        @catch(ICETimeoutException*)
        {
            // Expected.
        }
    }
    {
        //
        // Expect success.
        //
        [timeout op]; // Ensure adapter is active.
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:1000]];
        @try
        {
            [to sleep:500];
        }
        @catch(ICETimeoutException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("testing write timeout... ");
    {
        //
        // Expect TimeoutException.
        //
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:500]];
        [to holdAdapter:2000];
        @try
        {
            TestByteSeq* seq = [TestMutableByteSeq dataWithLength:10000];
            [to sendData:seq];
            test(NO);
        }
        @catch(ICETimeoutException*)
        {
            // Expected.
        }
    }
    {
        //
        // Expect success.
        //
        [timeout op]; // Ensure adapter is active.
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:1000]];
        [to holdAdapter:500];
        @try
        {
            TestByteSeq* seq = [TestMutableByteSeq dataWithLength:10000];
            [to sendData:seq];
        }
        @catch(ICETimeoutException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("testing AMI read timeout... ");
    {
        //
        // Expect TimeoutException.
        //
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:500]];
        Callback* cb = [[Callback alloc] init];
        [to sleep_async:cb response:@selector(sleepExResponse) exception:@selector(sleepExException:) to:2000];
        test([cb check]);
        [cb release];
    }
    {
        //
        // Expect success.
        //
        [timeout op]; // Ensure adapter is active.
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:1000]];
        Callback* cb = [[Callback alloc] init];
        [to sleep_async:cb response:@selector(sleepResponse) exception:@selector(sleepException:) to:500];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("testing AMI write timeout... ");
    {
        //
        // Expect TimeoutException.
        //
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:500]];
        [to holdAdapter:2000];
        TestByteSeq* seq = [TestMutableByteSeq dataWithLength:10000];
        Callback* cb = [[Callback alloc] init];
        [to sendData_async:cb response:@selector(sendDataExResponse) exception:@selector(sendDataExException:) seq:seq];
        test([cb check]);
        [cb release];
    }
    {
        //
        // Expect success.
        //
        [timeout op]; // Ensure adapter is active.
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[obj ice_timeout:1000]];
        [to holdAdapter:500];
        TestByteSeq* seq = [TestMutableByteSeq dataWithLength:10000];
        Callback* cb = [[Callback alloc] init];
        [to sendData_async:cb response:@selector(sendDataResponse) exception:@selector(sendDataException:) seq:seq];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("testing timeout overrides... ");
    {
        //
        // Test Ice.Override.Timeout. This property overrides all
        // endpoint timeouts.
        //
        ICEInitializationData* initData = [[ICEInitializationData alloc] init];
        [initData setProperties:[[communicator getProperties] clone]];
        [[initData properties] setProperty:@"Ice.Override.Timeout" value:@"500"];
        id<ICECommunicator> comm = [ICEUtil createCommunicator:initData];
        id<TestTimeoutPrx> to = [TestTimeoutPrx checkedCast:[comm stringToProxy:sref]];
        @try
        {
            [to sleep:750];
            test(NO);
        }
        @catch(ICETimeoutException*)
        {
            // Expected.
        }
        //
        // Calling ice_timeout() should have no effect.
        //
        [timeout op]; // Ensure adapter is active.
        to = [TestTimeoutPrx checkedCast:[to ice_timeout:1000]];
        @try
        {
            [to sleep:750];
            test(NO);
        }
        @catch(ICETimeoutException*)
        {
            // Expected.
        }
        [comm destroy];
    }
    {
        //
        // Test Ice.Override.ConnectTimeout.
        //
        ICEInitializationData* initData = [[ICEInitializationData alloc] init];
        [initData setProperties:[[communicator getProperties] clone]];
        [[initData properties] setProperty:@"Ice.Override.ConnectTimeout" value:@"750"];
        id<ICECommunicator> comm = [ICEUtil createCommunicator:initData];
        [timeout holdAdapter:1000];
        id<TestTimeoutPrx> to = [TestTimeoutPrx uncheckedCast:[comm stringToProxy:sref]];
        @try
        {
            [to op];
            test(NO);
        }
        @catch(ICEConnectTimeoutException*)
        {
            // Expected.
        }
        //
        // Calling ice_timeout() should have no effect on the connect timeout.
        //
        [timeout op]; // Ensure adapter is active.
        [timeout holdAdapter:1000];
        to = [TestTimeoutPrx uncheckedCast:[to ice_timeout:1250]];
        @try
        {
            [to op];
            test(NO);
        }
        @catch(ICEConnectTimeoutException*)
        {
            // Expected.
        }
        //
        // Verify that timeout set via ice_timeout() is still used for requests.
        //
        [to op]; // Force connection.
        @try
        {
            [to sleep:1500];
            test(NO);
        }
        @catch(ICETimeoutException*)
        {
            // Expected.
        }
        [comm destroy];
    }
    tprintf("ok\n");

    return timeout;
}
