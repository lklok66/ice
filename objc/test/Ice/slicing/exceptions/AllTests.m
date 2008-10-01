// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
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

-(void) response
{
    test(NO);
}

-(void) baseAsBaseException:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestBase* b)
    {
	test([b.b isEqualToString:@"Base.b"]);
	test([[b ice_name] isEqualToString:@"Test::Base"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) unknownDerivedAsBaseException:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestBase* b)
    {
	test([b.b isEqualToString:@"UnknownDerived.b"]);
	test([[b ice_name] isEqualToString:@"Test::Base"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) knownDerivedException:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestKnownDerived* k)
    {
	test([k.b isEqualToString:@"KnownDerived.b"]);
	test([k.kd isEqualToString:@"KnownDerived.kd"]);
	test([[k ice_name] isEqualToString:@"Test::KnownDerived"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) unknownIntermediateAsBaseException:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestBase* b)
    {
	test([b.b isEqualToString:@"UnknownIntermediate.b"]);
	test([[b ice_name] isEqualToString:@"Test::Base"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) knownIntermediateAsBaseException:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestKnownIntermediate* ki)
    {
	test([ki.b isEqualToString:@"KnownIntermediate.b"]);
	test([ki.ki isEqualToString:@"KnownIntermediate.ki"]);
	test([[ki ice_name] isEqualToString:@"Test::KnownIntermediate"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) knownMostDerivedException:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestKnownMostDerived* kmd)
    {
	test([kmd.b isEqualToString:@"KnownMostDerived.b"]);
	test([kmd.ki isEqualToString:@"KnownMostDerived.ki"]);
	test([kmd.kmd isEqualToString:@"KnownMostDerived.kmd"]);
	test([[kmd ice_name] isEqualToString:@"Test::KnownMostDerived"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) knownIntermediateAsKnownIntermediateException:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestKnownIntermediate* ki)
    {
	test([ki.b isEqualToString:@"KnownIntermediate.b"]);
	test([ki.ki isEqualToString:@"KnownIntermediate.ki"]);
	test([[ki ice_name] isEqualToString:@"Test::KnownIntermediate"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) unknownMostDerived1Exception:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestKnownIntermediate* ki)
    {
	test([ki.b isEqualToString:@"UnknownMostDerived1.b"]);
	test([ki.ki isEqualToString:@"UnknownMostDerived1.ki"]);
	test([[ki ice_name] isEqualToString:@"Test::KnownIntermediate"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}

-(void) unknownMostDerived2Exception:(ICEException*)exc
{
    @try
    {
	@throw exc;
    }
    @catch(TestBase* b)
    {
	test([b.b isEqualToString:@"UnknownMostDerived2.b"]);
	test([[b ice_name] isEqualToString:@"Test::Base"]);
    }
    @catch(...)
    {
	test(NO);
    }
    [self called];
}
@end

id<TestTestIntfPrx>
allTests(id<ICECommunicator> communicator)
{
    id<ICEObjectPrx> obj = [communicator stringToProxy:@"Test:default -p 12010"];
    id<TestTestIntfPrx> test = [TestTestIntfPrx checkedCast:obj];

    tprintf("base... ");
    {
        @try
        {
            [test baseAsBase];
            test(NO);
        }
        @catch(TestBase* b)
        {
            test([b.b isEqual:@"Base.b"]);
            test([[b ice_name] isEqualToString:@"Test::Base"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("base (AMI)... ");
    {
        Callback *cb = [[Callback alloc] init];
        [test baseAsBase_async:cb response:@selector(response) exception:@selector(baseAsBaseException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown derived... ");
    {
        @try
        {
            [test unknownDerivedAsBase];
            test(NO);
        }
        @catch(TestBase* b)
        {
            test([b.b isEqualToString:@"UnknownDerived.b"]);
            test([[b ice_name] isEqualToString:@"Test::Base"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("slicing of unknown derived (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test unknownDerivedAsBase_async:cb response:@selector(response) exception:@selector(unknownDerivedAsBaseException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known derived as base... ");
    {
        @try
        {
            [test knownDerivedAsBase];
            test(NO);
        }
        @catch(TestKnownDerived* k)
        {
            test([k.b isEqualToString:@"KnownDerived.b"]);
            test([k.kd isEqualToString:@"KnownDerived.kd"]);
            test([[k ice_name] isEqualToString:@"Test::KnownDerived"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("non-slicing of known derived as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test knownDerivedAsBase_async:cb response:@selector(response) exception:@selector(knownDerivedException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known derived as derived... ");
    {
        @try
        {
            [test knownDerivedAsKnownDerived];
            test(NO);
        }
        @catch(TestKnownDerived* k)
        {
            test([k.b isEqualToString:@"KnownDerived.b"]);
            test([k.kd isEqualToString:@"KnownDerived.kd"]);
            test([[k ice_name] isEqualToString:@"Test::KnownDerived"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("non-slicing of known derived as derived (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test knownDerivedAsKnownDerived_async:cb response:@selector(response) exception:@selector(knownDerivedException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown intermediate as base... ");
    {
        @try
        {
            [test unknownIntermediateAsBase];
            test(NO);
        }
        @catch(TestBase* b)
        {
            test([b.b isEqualToString:@"UnknownIntermediate.b"]);
            test([[b ice_name] isEqualToString:@"Test::Base"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("slicing of unknown intermediate as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test unknownIntermediateAsBase_async:cb response:@selector(response) exception:@selector(unknownIntermediateAsBaseException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("slicing of known intermediate as base... ");
    {
        @try
        {
            [test knownIntermediateAsBase];
            test(NO);
        }
        @catch(TestKnownIntermediate* ki)
        {
            test([ki.b isEqualToString:@"KnownIntermediate.b"]);
            test([ki.ki isEqualToString:@"KnownIntermediate.ki"]);
            test([[ki ice_name] isEqualToString:@"Test::KnownIntermediate"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("slicing of known intermediate as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test knownIntermediateAsBase_async:cb response:@selector(response) exception:@selector(knownIntermediateAsBaseException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("slicing of known most derived as base... ");
    {
        @try
        {
            [test knownMostDerivedAsBase];
            test(NO);
        }
        @catch(TestKnownMostDerived* kmd)
        {
            test([kmd.b isEqualToString:@"KnownMostDerived.b"]);
            test([kmd.ki isEqualToString:@"KnownMostDerived.ki"]);
            test([kmd.kmd isEqualToString:@"KnownMostDerived.kmd"]);
            test([[kmd ice_name] isEqualToString:@"Test::KnownMostDerived"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("slicing of known most derived as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test knownMostDerivedAsBase_async:cb response:@selector(response) exception:@selector(knownMostDerivedException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known intermediate as intermediate... ");
    {
        @try
        {
            [test knownIntermediateAsKnownIntermediate];
            test(NO);
        }
        @catch(TestKnownIntermediate* ki)
        {
            test([ki.b isEqualToString:@"KnownIntermediate.b"]);
            test([ki.ki isEqualToString:@"KnownIntermediate.ki"]);
            test([[ki ice_name] isEqualToString:@"Test::KnownIntermediate"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("non-slicing of known intermediate as intermediate (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test knownIntermediateAsKnownIntermediate_async:cb response:@selector(response) exception:@selector(knownIntermediateAsKnownIntermediateException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known most derived exception as intermediate... ");
    {
        @try
        {
            [test knownMostDerivedAsKnownIntermediate];
            test(NO);
        }
        @catch(TestKnownMostDerived* kmd)
        {
            test([kmd.b isEqualToString:@"KnownMostDerived.b"]);
            test([kmd.ki isEqualToString:@"KnownMostDerived.ki"]);
            test([kmd.kmd isEqualToString:@"KnownMostDerived.kmd"]);
            test([[kmd ice_name] isEqualToString:@"Test::KnownMostDerived"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("non-slicing of known most derived as intermediate (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test knownMostDerivedAsKnownIntermediate_async:cb response:@selector(response) exception:@selector(knownMostDerivedException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known most derived as most derived... ");
    {
        @try
        {
            [test knownMostDerivedAsKnownMostDerived];
            test(NO);
        }
        @catch(TestKnownMostDerived* kmd)
        {
            test([kmd.b isEqualToString:@"KnownMostDerived.b"]);
            test([kmd.ki isEqualToString:@"KnownMostDerived.ki"]);
            test([kmd.kmd isEqualToString:@"KnownMostDerived.kmd"]);
            test([[kmd ice_name] isEqualToString:@"Test::KnownMostDerived"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("non-slicing of known most derived as most derived (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test knownMostDerivedAsKnownMostDerived_async:cb response:@selector(response) exception:@selector(knownMostDerivedException:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, known intermediate as base... ");
    {
        @try
        {
            [test unknownMostDerived1AsBase];
            test(NO);
        }
        @catch(TestKnownIntermediate* ki)
        {
            test([ki.b isEqualToString:@"UnknownMostDerived1.b"]);
            test([ki.ki isEqualToString:@"UnknownMostDerived1.ki"]);
            test([[ki ice_name] isEqualToString:@"Test::KnownIntermediate"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, known intermediate as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test unknownMostDerived1AsBase_async:cb response:@selector(response) exception:@selector(unknownMostDerived1Exception:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, known intermediate as intermediate... ");
    {
        @try
        {
            [test unknownMostDerived1AsKnownIntermediate];
            test(NO);
        }
        @catch(TestKnownIntermediate* ki)
        {
            test([ki.b isEqualToString:@"UnknownMostDerived1.b"]);
            test([ki.ki isEqualToString:@"UnknownMostDerived1.ki"]);
            test([[ki ice_name] isEqualToString:@"Test::KnownIntermediate"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, known intermediate as intermediate (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test unknownMostDerived1AsKnownIntermediate_async:cb response:@selector(response) exception:@selector(unknownMostDerived1Exception:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, unknown intermediate as base... ");
    {
        @try
        {
            [test unknownMostDerived2AsBase];
            test(NO);
        }
        @catch(TestBase* b)
        {
            test([b.b isEqualToString:@"UnknownMostDerived2.b"]);
            test([[b ice_name] isEqualToString:@"Test::Base"]);
        }
        @catch(...)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, unknown intermediate as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test unknownMostDerived2AsBase_async:cb response:@selector(response) exception:@selector(unknownMostDerived2Exception:)];
        test([cb check]);
	[cb release];
    }
    tprintf("ok\n");

    return test;
}
