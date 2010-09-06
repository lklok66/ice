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
#import <InvokeTest.h>

#import <Foundation/Foundation.h>

static NSString* testString = @"This is a test string";

@interface TestInvokeCallback : NSObject
{
    BOOL called;
    NSCondition* cond;
    id<ICECommunicator> communicator;
}
-(BOOL) check;
-(void) called;
@end

@implementation TestInvokeCallback
-(id) initWithCommunicator:(id<ICECommunicator>)c
{
    if(![super init])
    {
        return nil;
    }
    cond = [[NSCondition alloc] init];
    communicator = c;
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
-(void) opString:(BOOL)ok outParams:(NSMutableData*)outParams
{
    if(ok)
    {
        id<ICEInputStream> inS = [ICEUtil createInputStream:communicator data:outParams];
        NSString* s;
        s = [inS readString];
        test([s isEqualToString:testString]);
        s = [inS readString];
        test([s isEqualToString:testString]);
        [self called];
    }
    else
    {
        test(NO);
    }
}

-(void) opException:(BOOL)ok outParams:(NSMutableData*)outParams
{
    if(ok)
    {
        test(NO);
    }
    else
    {
        id<ICEInputStream> inS = [ICEUtil createInputStream:communicator data:outParams];
        @try
        {
            [inS throwException];
        }
        @catch(TestInvokeMyException*)
        {
            [self called];
        }
        @catch(NSException*)
        {
            test(NO);
        }
    }
}
@end

id<TestInvokeMyClassPrx>
invokeAllTests(id<ICECommunicator> communicator)
{
    NSString* ref = @"test:default -p 12010";
    id<ICEObjectPrx> base = [communicator stringToProxy:ref];
    test(base);

    id<TestInvokeMyClassPrx> cl = [TestInvokeMyClassPrx checkedCast:base];
    test(cl);

    id<TestInvokeMyClassPrx> onewayCl = [cl ice_oneway];

    tprintf("testing ice_invoke... ");

    {
        NSData* inParams = [NSData data];
        NSMutableData* outParams;
        if(![onewayCl ice_invoke:@"opOneway" mode:ICENormal inParams:inParams outParams:&outParams])
        {
            test(NO);
        }

        id<ICEOutputStream> outS = [ICEUtil createOutputStream:communicator];
        [outS writeString:testString];
        inParams = [outS finished];

        // ice_invoke
        if([cl ice_invoke:@"opString" mode:ICENormal inParams:inParams outParams:&outParams])
        {
            id<ICEInputStream> inS = [ICEUtil createInputStream:communicator data:outParams];
            NSString* s;
            s = [inS readString];
            test([s isEqualToString:testString]);
            s = [inS readString];
            test([s isEqualToString:testString]);
        }
        else
        {
            test(NO);
        }
    }

    {
        NSData* inParams = [NSData data];
        NSMutableData* outParams;
        if([cl ice_invoke:@"opException" mode:ICENormal inParams:inParams outParams:&outParams])
        {
            test(NO);
        }
        else
        {
            id<ICEInputStream> inS = [ICEUtil createInputStream:communicator data:outParams];
            @try
            {
                [inS throwException];
            }
            @catch(TestInvokeMyException*)
            {
            }
            @catch(NSException*)
            {
                test(NO);
            }
        }
    }

    tprintf("ok\n");

    tprintf("testing asynchronous ice_invoke... ");

    {
        NSData* inParams = [NSData data];
        NSMutableData* outParams;
        ICEAsyncResult* result = [onewayCl begin_ice_invoke:@"opOneway" mode:ICENormal inParams:inParams];
        if(![onewayCl end_ice_invoke:&outParams result:result])
        {
            test(NO);
        }
        
        id<ICEOutputStream> outS = [ICEUtil createOutputStream:communicator];
        [outS writeString:testString];
        inParams = [outS finished];

        // begin_ice_invoke with no callback
        result = [cl begin_ice_invoke:@"opString" mode:ICENormal inParams:inParams];
        if([cl end_ice_invoke:&outParams result:result])
        {
            id<ICEInputStream> inS = [ICEUtil createInputStream:communicator data:outParams];
            NSString* s;
            s = [inS readString];
            test([s isEqualToString:testString]);
            s = [inS readString];
            test([s isEqualToString:testString]);
        }
        else
        {
            test(NO);
        };

        TestInvokeCallback* cb = [[TestInvokeCallback alloc] initWithCommunicator:communicator];
        [cl begin_ice_invoke:@"opString" mode:ICENormal inParams:inParams 
            response:^(BOOL ok, NSMutableData* outParams) { [cb opString:ok outParams:outParams]; }
            exception:^(ICEException* ex) { test(NO); }];
        [cb check];
        [cb release];
    }

    {
        NSData* inParams = [NSData data];
        NSMutableData* outParams;

        // begin_ice_invoke with no callback
        ICEAsyncResult* result = [cl begin_ice_invoke:@"opException" mode:ICENormal inParams:inParams];
        if([cl end_ice_invoke:&outParams result:result])
        {
            test(NO);
        }
        else
        {
            id<ICEInputStream> inS = [ICEUtil createInputStream:communicator data:outParams];
            @try
            {
                [inS throwException];
            }
            @catch(TestInvokeMyException*)
            {
            }
            @catch(NSException*)
            {
                test(NO);
            }
        }

        // begin_ice_invoke with Callback_Object_ice_invoke
        TestInvokeCallback* cb = [[TestInvokeCallback alloc] initWithCommunicator:communicator];
        [cl begin_ice_invoke:@"opException" mode:ICENormal inParams:inParams 
            response:^(BOOL ok, NSMutableData* outP) { [cb opException:ok outParams:outP]; }
            exception:^(ICEException* ex) { test(NO); }];
        [cb check];
        [cb release];
    }

    tprintf("ok\n");

    return cl;

}
