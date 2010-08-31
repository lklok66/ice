// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Ice/Locator.h>
#import <TestCommon.h>
#import <AMITest.h>
 
#import <Foundation/Foundation.h>

@interface TestAMICallback : NSObject
{
    BOOL called;
    NSCondition* cond;
}
-(BOOL) check;
-(void) called;
@end

@implementation TestAMICallback
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
@end

void
amiAllTests(id<ICECommunicator> communicator)
{
    NSString* ref = @"test:default -p 12010";
    id<ICEObjectPrx> base = [communicator stringToProxy:(ref)];
    id<TestAMITestIntfPrx> p = [TestAMITestIntfPrx checkedCast:base];
    test(p);

    tprintf("testing begin/end invocation... ");
    {
        ICEContext* ctx = [ICEContext dictionary];
        ICEAsyncResult* result;

        result = [p begin_ice_isA:[TestAMITestIntfPrx ice_staticId]];
        test([p end_ice_isA:result]);
        result = [p begin_ice_isA:[TestAMITestIntfPrx ice_staticId] context:ctx];
        test([p end_ice_isA:result]);

        result = [p begin_ice_ping];
        [p end_ice_ping:result];
        result = [p begin_ice_ping:ctx];
        [p end_ice_ping:result];

        result = [p begin_ice_id];
        test([[p end_ice_id:result] isEqualToString:[TestAMITestIntfPrx ice_staticId]]);
        result = [p begin_ice_id:ctx];
        test([[p end_ice_id:result] isEqualToString:[TestAMITestIntfPrx ice_staticId]]);

        result = [p begin_ice_ids];
        test([[p end_ice_ids:result] count] == 2);
        result = [p begin_ice_ids:ctx];
        test([[p end_ice_ids:result] count] == 2);
    }
    tprintf("ok\n");

    tprintf("testing response callback... ");
    {
        TestAMICallback* cb = [[TestAMICallback alloc] init];
        ICEContext* ctx = [NSDictionary dictionary];

 	void (^isACB)(BOOL) = [[ ^(BOOL ret) { test(ret); [cb called]; } copy] autorelease ];
        [p begin_ice_isA:[TestAMITestIntfPrx ice_staticId] response:isACB exception:nil];
        [cb check];
        [p begin_ice_isA:[TestAMITestIntfPrx ice_staticId] response:isACB exception:nil context:ctx];
        [cb check];

 	void (^pingCB)() = [[ ^ { [cb called]; } copy] autorelease ];
        [p begin_ice_ping:pingCB exception:nil];
        [cb check];
        [p begin_ice_ping:pingCB exception:nil context:ctx];
        [cb check];

        void (^idCB)(NSString* typeId) = [[ ^(NSString* typeId) 
        { 
            test([typeId isEqualToString:[TestAMITestIntfPrx ice_staticId]]); 
            [cb called]; 
        } copy] autorelease ];

        [p begin_ice_id:idCB exception:nil];
        [cb check];
        [p begin_ice_id:idCB exception:nil context:ctx];
        [cb check];

        void (^idsCB)(NSArray* types) = [[ ^(NSArray* types) 
        { 
            test([types count] == 2); 
            [cb called]; 
        } copy] autorelease ];

        [p begin_ice_ids:idsCB exception:nil];
        [cb check];
        [p begin_ice_ids:idsCB exception:nil context:ctx];
        [cb check];

        [cb release];
    }
    tprintf("ok\n");

    tprintf("testing local exceptions... ");
    {
        id<TestAMITestIntfPrx> i = [TestAMITestIntfPrx uncheckedCast:[p ice_adapterId:@"dummy"]];
        TestAMICallback* cb = [[TestAMICallback alloc] init];
        ICEContext* ctx = [NSDictionary dictionary];

        void (^exCB)(ICEException*) = [[^ (ICEException* ex) {
            test([ex isKindOfClass:[ICENoEndpointException class]]);
            [cb called];
        } copy] autorelease];

        void (^isACB)(BOOL) = [[ ^(BOOL ret) { test(NO); } copy ] autorelease];
        [i begin_ice_isA:@"dummy" response:isACB exception:exCB];
        [cb check];
        [i begin_ice_isA:@"dummy" response:isACB exception:exCB context:ctx];
        [cb check];

	void (^pingCB)() = [[ ^ { test(NO); } copy ] autorelease];
        [i begin_ice_ping:pingCB exception:exCB];
        [cb check];
        [i begin_ice_ping:pingCB exception:exCB context:ctx];
        [cb check];

        void (^idCB)(NSString*) = [[ ^(NSString* ret) { test(NO); } copy ] autorelease];
        [i begin_ice_id:idCB exception:exCB];
        [cb check];
        [i begin_ice_id:idCB exception:exCB context:ctx];
        [cb check];

        void (^idsCB)(NSArray*) = [[ ^(NSArray* ret) { test(NO); } copy ] autorelease];
        [i begin_ice_ids:idsCB exception:exCB];
        [cb check];
        [i begin_ice_ids:idsCB exception:exCB context:ctx];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("testing sent callback... ");
    {
        TestAMICallback* cb = [[TestAMICallback alloc] init];
        ICEContext* ctx = [NSDictionary dictionary];
        
        void (^sentCB)(BOOL) = [[ ^ { [cb called]; } copy] autorelease];

        [p begin_ice_isA:@"test" response:nil exception:nil sent:sentCB];
        [cb check];
        [p begin_ice_isA:@"test" response:nil exception:nil sent:sentCB context:ctx];
        [cb check];

        [p begin_ice_ping:nil exception:nil sent:sentCB];
        [cb check];
        [p begin_ice_ping:nil exception:nil sent:sentCB context:ctx];
        [cb check];

        [p begin_ice_id:nil exception:nil sent:sentCB];
        [cb check];
        [p begin_ice_id:nil exception:nil sent:sentCB context:ctx];
        [cb check];

        [p begin_ice_ids:nil exception:nil sent:sentCB];
        [cb check];
        [p begin_ice_ids:nil exception:nil sent:sentCB context:ctx];
        [cb check];
    }
    tprintf("ok\n");

    [p shutdown];
}
