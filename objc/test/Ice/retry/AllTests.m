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

id<TestRetryPrx>
allTests(id<ICECommunicator> communicator)
{
    tprintf("testing stringToProxy... ");
    NSString* ref = @"retry:default -p 12010 -t 10000";
    id<ICEObjectPrx> base1 = [communicator stringToProxy:ref];
    test(base1);
    id<ICEObjectPrx> base2 = [communicator stringToProxy:ref];
    test(base2);
    tprintf("ok\n");

    tprintf("testing checked cast... ");
    id<TestRetryPrx> retry1 = [TestRetryPrx checkedCast:base1];
    test(retry1);
    test([retry1 isEqual:base1]);
    id<TestRetryPrx> retry2 = [TestRetryPrx checkedCast:base2];
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

    Callback* cb1 = [[Callback alloc] init];
    Callback* cb2 = [[Callback alloc] init];

    tprintf("calling regular AMI operation with first proxy... ");
    [retry1 op_async:cb1 response:@selector(retryOpResponse) exception:@selector(retryOpException:) kill:NO];
    test([cb1 check]);
    tprintf("ok\n");

    tprintf("calling AMI operation to kill connection with second proxy... ");
    [retry2 op_async:cb2 response:@selector(killRetryOpResponse) exception:@selector(killRetryOpException:)  kill:YES];
    test([cb2 check]);
    tprintf("ok\n");

    tprintf("calling regular AMI operation with first proxy again... ");
    [retry1 op_async:cb1 response:@selector(retryOpResponse) exception:@selector(retryOpException:) kill:NO];
    test([cb1 check]);
    tprintf("ok\n");

    [cb1 release];
    [cb2 release];

    return retry1;
}
