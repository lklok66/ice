// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <Test.h>

#import <Foundation/Foundation.h>

@interface Condition : NSObject
{
    BOOL value;
}
-(id) init:(BOOL)value;
-(void) set:(BOOL)value;
-(BOOL) value;
@end

@implementation Condition
-(id) init:(BOOL)v
{
    if(![super init])
    {
        return nil;
    }
    value = v;
    return self;
}
-(void) set:(BOOL)v
{
    @synchronized(self)
    {
        value = v;
    }
}

-(BOOL) value
{
    @synchronized(self)
    {
        return value;
    }
    return value;
}
@end

@interface AMICheckSetValue : NSObject
{
    Condition* condition;
    ICEInt expected;
    BOOL sent;
    NSCondition* cond;
}
-(id)init:(Condition*)cond expected:(ICEInt)expected;
-(void) waitForSent;
@end

@implementation AMICheckSetValue
-(id)init:(Condition*)c expected:(ICEInt)e
{
    if(![super init])
    {
        return nil;
    }
    cond = [[NSCondition alloc] init];
    condition = [c retain];
    expected = e;
    return self;
}
-(void) release
{
    [cond release];
    [condition release];
    [super release];
}
-(void) ice_response:(ICEInt)value
{
    if(value != expected)
    {
        [condition set:NO];
    }
}
-(void) ice_sent
{
    [cond lock];
    sent = YES;
    [cond signal];
    [cond unlock];
}
-(void) waitForSent
{
    [cond lock];
    while(!sent)
    {
        [cond wait];
    }
    [cond unlock];
}
@end

void
allTests(id<ICECommunicator> communicator)
{
    tprintf("testing stringToProxy... ");
    NSString* ref = @"hold:default -p 12010 -t 30000";
    id<ICEObjectPrx> base = [communicator stringToProxy:ref];
    test(base);
    NSString* refSerialized = @"hold:default -p 12011 -t 60000";
    id<ICEObjectPrx> baseSerialized = [communicator stringToProxy:refSerialized];
    test(base);
    tprintf("ok\n");

    tprintf("testing checked cast... ");
    id<TestHoldHoldPrx> hold = [TestHoldHoldPrx checkedCast:base];
    test(hold);
    test([hold isEqual:base]);
    id<TestHoldHoldPrx> holdSerialized = [TestHoldHoldPrx checkedCast:baseSerialized];
    test(holdSerialized);
    test([holdSerialized isEqual:baseSerialized]);
    tprintf("ok\n");

    tprintf("changing state between active and hold rapidly... ");
    int i;
    for(i = 0; i < 100; ++i)
    {
        [hold putOnHold:0];
    }
    for(i = 0; i < 100; ++i)
    {
        [[hold ice_oneway] putOnHold:0];
    }
    for(i = 0; i < 100; ++i)
    {
        [holdSerialized putOnHold:0];
    }
    for(i = 0; i < 100; ++i)
    {
        [[holdSerialized ice_oneway] putOnHold:0];
    }
    tprintf("ok\n");

    tprintf("testing without serialize mode... ");
    {
        Condition* cond = [(Condition*)[Condition alloc] init:YES];
        int value = 0;
        AMICheckSetValue* cb;
        while([cond value])
        {
            cb = [[AMICheckSetValue alloc] init:cond expected:value];
            if([hold set_async:cb response:@selector(ice_response:) exception:nil sent:@selector(ice_sent) 
                     value:++value delay:(random() % 5 + 1)])
            {
                cb = 0;
            }
            if(value % 100 == 0)
            {
                if(cb)
                {
                    [cb waitForSent];
                    cb = 0;
                }
            }
        }
        if(cb)
        {
            [cb waitForSent];
            cb = 0;
        }
    }
    tprintf("ok\n");

    tprintf("testing with serialize mode... ");
    {
        Condition* cond = [(Condition*)[Condition alloc] init:YES];
        int value = 0;
        AMICheckSetValue* cb;
        while(value < 3000 && [cond value])
        {
            cb = [[AMICheckSetValue alloc] init:cond expected:value];
            if([holdSerialized set_async:cb response:@selector(ice_response:) exception:nil sent:@selector(ice_sent)
                               value:++value delay:0])
            {
                cb = 0;
            }
            if(value % 100 == 0)
            {
                if(cb)
                {
                    [cb waitForSent];
                    cb = 0;
                }
            }
        }
        if(cb)
        {
            [cb waitForSent];
            cb = 0;
        }
        test([cond value]);
        int i;
        for(i = 0; i < 20000; ++i)
        {
            [[holdSerialized ice_oneway] setOneway:(value + 1) expected:value];
            ++value;
            if((i % 100) == 0)
            {
                [[holdSerialized ice_oneway] putOnHold:1];
            }
        }
    }
    tprintf("ok\n");

    tprintf("changing state to hold and shutting down server... ");
    [hold shutdown];
    tprintf("ok\n");
}
