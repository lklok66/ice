// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <Test.h>

#import <Foundation/Foundation.h>

@interface OnewayCallback : NSObject
{
    BOOL called;
    NSCondition* cond;
}
-(BOOL) check;
-(void) called;
@end

@implementation OnewayCallback
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
-(void) opVoidResponse
{
    [self called];
}
-(void) opVoidException:(ICEException*)ex
{
    test(NO);
}
-(void) opVoidExResponse
{
    test(NO);
}
-(void) opVoidExException:(ICEException*)ex
{
    test([ex isKindOfClass:[ICENoEndpointException class]]);
    [self called];
}
-(void) opByteEx:(ICEException*)ex
{
    test(NO);
}
-(void) opByteExException:(ICEException*)ex
{
    test([ex isKindOfClass:[ICETwowayOnlyException class]]);
    [self called];
}
@end

void
onewaysAMI(id<ICECommunicator> communicator, id<TestOperationsMyClassPrx> proxy)
{
    id<TestOperationsMyClassPrx> p = [TestOperationsMyClassPrx uncheckedCast:[proxy ice_oneway]];

    {
        OnewayCallback* cb = [[OnewayCallback alloc] init];
        [p opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)];

        // Let's check if we can reuse the same callback object for another call.
        [p opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)];
        [cb release];
    }

    {
        // Check that a call to a void operation raises NoEndpointException
        // in the ice_exception() callback instead of at the point of call.
        id<TestOperationsMyClassPrx> indirect = [TestOperationsMyClassPrx uncheckedCast:[p ice_adapterId:@"dummy"]];
        OnewayCallback* cb = [[OnewayCallback alloc] init];
        @try
        {
            [indirect opVoid_async:cb response:@selector(opVoidExResponse) exception:@selector(opVoidExException:)];
        }
        @catch(ICEException*)
        {
            test(NO);
        }
        test([cb check]);
        [cb release];
    }

    {
        OnewayCallback* cb = [[OnewayCallback alloc] init];
        @try
        {
            [p opByte_async:cb  response:@selector(opByteExResponse) exception:@selector(opByteExException:) p1:0 p2:0];
            test([cb check]);
        }
        @catch(ICETwowayOnlyException* ex)
        {
            // TODO: fix once we decide how.
        }
        [cb release];
    }
}
