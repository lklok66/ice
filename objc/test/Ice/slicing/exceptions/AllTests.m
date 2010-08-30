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
#import <SlicingExceptionsTestClient.h>

#import <Foundation/Foundation.h>

@interface TestSlicingExceptionsClientCallback : NSObject
{
    BOOL called;
    NSCondition* cond;
}
-(void) check;
-(void) called;
@end

@implementation TestSlicingExceptionsClientCallback
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
    @catch(TestSlicingExceptionsClientBase* b)
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
    @catch(TestSlicingExceptionsClientBase* b)
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
    @catch(TestSlicingExceptionsClientKnownDerived* k)
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
    @catch(TestSlicingExceptionsClientBase* b)
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
    @catch(TestSlicingExceptionsClientKnownIntermediate* ki)
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
    @catch(TestSlicingExceptionsClientKnownMostDerived* kmd)
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
    @catch(TestSlicingExceptionsClientKnownIntermediate* ki)
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
    @catch(TestSlicingExceptionsClientKnownIntermediate* ki)
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
    @catch(TestSlicingExceptionsClientBase* b)
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

id<TestSlicingExceptionsClientTestIntfPrx>
slicingExceptionsAllTests(id<ICECommunicator> communicator)
{
    id<ICEObjectPrx> obj = [communicator stringToProxy:@"Test:default -p 12010"];
    id<TestSlicingExceptionsClientTestIntfPrx> test = [TestSlicingExceptionsClientTestIntfPrx checkedCast:obj];
    tprintf("base... ");
    {
        @try
        {
            [test baseAsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientBase* b)
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
        TestSlicingExceptionsClientCallback *cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test baseAsBase_async:cb response:@selector(response) exception:@selector(baseAsBaseException:)];
        [cb check];
    }
    tprintf("ok\n");
   
    tprintf("slicing of unknown derived... ");
    {
        @try
        {
            [test unknownDerivedAsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientBase* b)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test unknownDerivedAsBase_async:cb response:@selector(response) exception:@selector(unknownDerivedAsBaseException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known derived as base... ");
    {
        @try
        {
            [test knownDerivedAsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownDerived* k)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test knownDerivedAsBase_async:cb response:@selector(response) exception:@selector(knownDerivedException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known derived as derived... ");
    {
        @try
        {
            [test knownDerivedAsKnownDerived];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownDerived* k)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test knownDerivedAsKnownDerived_async:cb response:@selector(response) exception:@selector(knownDerivedException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown intermediate as base... ");
    {
        @try
        {
            [test unknownIntermediateAsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientBase* b)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test unknownIntermediateAsBase_async:cb response:@selector(response) exception:@selector(unknownIntermediateAsBaseException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("slicing of known intermediate as base... ");
    {
        @try
        {
            [test knownIntermediateAsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownIntermediate* ki)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test knownIntermediateAsBase_async:cb response:@selector(response) exception:@selector(knownIntermediateAsBaseException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("slicing of known most derived as base... ");
    {
        @try
        {
            [test knownMostDerivedAsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownMostDerived* kmd)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test knownMostDerivedAsBase_async:cb response:@selector(response) exception:@selector(knownMostDerivedException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known intermediate as intermediate... ");
    {
        @try
        {
            [test knownIntermediateAsKnownIntermediate];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownIntermediate* ki)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test knownIntermediateAsKnownIntermediate_async:cb response:@selector(response) exception:@selector(knownIntermediateAsKnownIntermediateException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known most derived exception as intermediate... ");
    {
        @try
        {
            [test knownMostDerivedAsKnownIntermediate];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownMostDerived* kmd)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test knownMostDerivedAsKnownIntermediate_async:cb response:@selector(response) exception:@selector(knownMostDerivedException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("non-slicing of known most derived as most derived... ");
    {
        @try
        {
            [test knownMostDerivedAsKnownMostDerived];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownMostDerived* kmd)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test knownMostDerivedAsKnownMostDerived_async:cb response:@selector(response) exception:@selector(knownMostDerivedException:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, known intermediate as base... ");
    {
        @try
        {
            [test unknownMostDerived1AsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownIntermediate* ki)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test unknownMostDerived1AsBase_async:cb response:@selector(response) exception:@selector(unknownMostDerived1Exception:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, known intermediate as intermediate... ");
    {
        @try
        {
            [test unknownMostDerived1AsKnownIntermediate];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientKnownIntermediate* ki)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init]  autorelease];
        [test unknownMostDerived1AsKnownIntermediate_async:cb response:@selector(response) exception:@selector(unknownMostDerived1Exception:)];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("slicing of unknown most derived, unknown intermediate as base... ");
    {
        @try
        {
            [test unknownMostDerived2AsBase];
            test(NO);
        }
        @catch(TestSlicingExceptionsClientBase* b)
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
        TestSlicingExceptionsClientCallback* cb = [[[TestSlicingExceptionsClientCallback alloc] init] autorelease];
        [test unknownMostDerived2AsBase_async:cb response:@selector(response) exception:@selector(unknownMostDerived2Exception:)];
        [cb check];
    }
    tprintf("ok\n");
    return test;
}
