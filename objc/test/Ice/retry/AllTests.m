// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <RetryTest.h>

#import <Foundation/Foundation.h>

@interface TestRetryCallback : NSObject
{
    BOOL called;
    NSCondition* cond;
}
-(void) check;
-(void) called;
@end

@implementation TestRetryCallback
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
-(void) check
{
    [cond lock];
    while(!called)
    {
        [cond wait];
    }
    called = NO;
    [cond unlock];
}
-(void) called
{
    [cond lock];
    called = YES;
    [cond signal];
    [cond unlock];
}
-(void) retryOpResponse
{
    [self called];
}
-(void) retryOpException:(ICEException*)ex
{
    test(NO);
}
-(void) killRetryOpResponse
{
    test(NO);
}

-(void) killRetryOpException:(ICEException*)ex
{
    test([ex isKindOfClass:[ICEConnectionLostException class]]);
    [self called];
};
@end

id<TestRetryRetryPrx>
retryAllTests(id<ICECommunicator> communicator)
{
    tprintf("testing stringToProxy... ");
    NSString* ref = @"retry:default -p 12010";
    id<ICEObjectPrx> base1 = [communicator stringToProxy:ref];
    test(base1);
    id<ICEObjectPrx> base2 = [communicator stringToProxy:ref];
    test(base2);
    tprintf("ok\n");

    tprintf("testing checked cast... ");
    id<TestRetryRetryPrx> retry1 = [TestRetryRetryPrx checkedCast:base1];
    test(retry1);
    test([retry1 isEqual:base1]);
    id<TestRetryRetryPrx> retry2 = [TestRetryRetryPrx checkedCast:base2];
    test(retry2);
    test([retry2 isEqual:base2]);
    tprintf("ok\n");

    tprintf("calling regular operation with first proxy... ");
    [retry1 op:NO];
    tprintf("ok\n");

    tprintf("calling operation to kill connection with second proxy... ");
    @try
    {
        [retry2 op:YES];
        test(NO);
    }
    @catch(ICEConnectionLostException*)
    {
        tprintf("ok\n");
    }

    tprintf("calling regular operation with first proxy again... ");
    [retry1 op:NO];
    tprintf("ok\n");

    TestRetryCallback* cb1 = [[[TestRetryCallback alloc] init] autorelease];
    TestRetryCallback* cb2 = [[[TestRetryCallback alloc] init] autorelease];

    tprintf("calling regular AMI operation with first proxy... ");
    [retry1 op_async:cb1 response:@selector(retryOpResponse) exception:@selector(retryOpException:) kill:NO];
    [cb1 check];
    tprintf("ok\n");

    tprintf("calling AMI operation to kill connection with second proxy... ");
    [retry2 op_async:cb2 response:@selector(killRetryOpResponse) exception:@selector(killRetryOpException:)  kill:YES];
    [cb2 check];
    tprintf("ok\n");

    tprintf("calling regular AMI operation with first proxy again... ");
    [retry1 op_async:cb1 response:@selector(retryOpResponse) exception:@selector(retryOpException:) kill:NO];
    [cb1 check];
    tprintf("ok\n");

    return retry1;
}
