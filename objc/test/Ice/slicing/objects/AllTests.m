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
#import <ClientPrivate.h>

#import <Foundation/Foundation.h>

@interface Callback : NSObject
{
    BOOL called;
    NSCondition* cond;
@public
    id r;
    id bout;
}
@property(nonatomic, retain) id r;
@property(nonatomic, retain) id bout;
-(BOOL) check;
-(void) called;
@end

@implementation Callback
@synthesize r;
@synthesize bout;
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
-(void) SBaseAsObjectResponse:(ICEObject*)o
{
    test(o);
    test([[o ice_id:nil] isEqualToString:@"::Test::SBase"]);
    test([o isKindOfClass:[TestSBase class]]);
    TestSBase* sb = (TestSBase*)o;
    test([sb.sb isEqualToString:@"SBase.sb"]);
    [self called];
}

-(void) SBaseAsObjectException:(ICEException*)exc
{
    test(NO);
}

-(void) SBaseAsSBaseResponse:(TestSBase*)sb
{
    test([sb.sb isEqualToString:@"SBase.sb"]);
    [self called];
}

-(void) SBaseAsSBaseException:(ICEException*)exc
{
    test(NO);
}

-(void) SBSKnownDerivedAsSBaseResponse:(TestSBase*)sb
{
    test([sb isKindOfClass:[TestSBSKnownDerived class]]);
    TestSBSKnownDerived* sbskd = (TestSBSKnownDerived*)sb;
    test([sbskd.sbskd isEqualToString:@"SBSKnownDerived.sbskd"]);
    [self called];
}

-(void) SBSKnownDerivedAsSBaseException:(ICEException*)exc
{
    test(NO);
}

-(void) SBSKnownDerivedAsSBSKnownDerivedResponse:(TestSBSKnownDerived*)sbskd
{
    test([sbskd.sbskd isEqualToString:@"SBSKnownDerived.sbskd"]);
    [self called];
}

-(void) SBSKnownDerivedAsSBSKnownDerivedException:(ICEException*)exc
{
    test(NO);
}

-(void) SBSUnknownDerivedAsSBaseResponse:(TestSBase*)sb
{
    test([sb.sb isEqualToString:@"SBSUnknownDerived.sb"]);
    [self called];
}

-(void) SBSUnknownDerivedAsSBaseException:(ICEException*)exc
{
    test(NO);
}

-(void) SUnknownAsObjectResponse:(ICEObject*)o
{
    test(NO);
}

-(void) SUnknownAsObjectException:(ICEException*)exc
{
    test([[exc ice_name] isEqualToString:@"Ice::NoObjectFactoryException"]);
    [self called];
}

-(void) oneElementCycleResponse:(TestB*)b
{
    test(b);
    test([[b ice_id:nil] isEqualToString:@"::Test::B"]);
    test([b.sb isEqualToString:@"B1.sb"]);
    test(b.pb == b);
    [self called];
}

-(void) oneElementCycleException:(ICEException*)exc
{
    test(NO);
}

-(void) twoElementCycleResponse:(TestB*)b1
{
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::B"]);
    test([b1.sb isEqualToString:@"B1.sb"]);

    TestB* b2 = b1.pb;
    test(b2);
    test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);
    test([b2.sb isEqualToString:@"B2.sb"]);
    test(b2.pb == b1);
    [self called];
}

-(void) twoElementCycleException:(ICEException*)exc
{
    test(NO);
}

-(void) D1AsBResponse:(TestB*)b1
{
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([b1.sb isEqualToString:@"D1.sb"]);
    test(b1.pb);
    test(b1.pb != b1);
    test([b1 isKindOfClass:[TestD1 class]]);
    TestD1* d1 = (TestD1*)b1;
    test([d1.sd1 isEqualToString:@"D1.sd1"]);
    test(d1.pd1);
    test(d1.pd1 != b1);
    test(b1.pb == d1.pd1);

    TestB* b2 = b1.pb;
    test(b2);
    test(b2.pb == b1);
    test([b2.sb isEqualToString:@"D2.sb"]);
    test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);
    [self called];
}

-(void) D1AsBException:(ICEException*)exc
{
    test(NO);
}

-(void) D1AsD1Response:(TestD1*)d1
{
    test(d1);
    test([[d1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([d1.sb isEqualToString:@"D1.sb"]);
    test(d1.pb);
    test(d1.pb != d1);

    TestB* b2 = d1.pb;
    test(b2);
    test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);
    test([b2.sb isEqualToString:@"D2.sb"]);
    test(b2.pb == d1);
    [self called];
}

-(void) D1AsD1Exception:(ICEException*)exc
{
    test(NO);
}

-(void) D2AsBResponse:(TestB*)b2
{
    test(b2);
    test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);
    test([b2.sb isEqualToString:@"D2.sb"]);
    test(b2.pb);
    test(b2.pb != b2);

    TestB* b1 = b2.pb;
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([b1.sb isEqualToString:@"D1.sb"]);
    test(b1.pb == b2);
    test([b1 isKindOfClass:[TestD1 class]]);
    TestD1* d1 = (TestD1*)b1;
    test([d1.sd1 isEqualToString:@"D1.sd1"]);
    test(d1.pd1 == b2);
    [self called];
}

-(void) D2AsBException:(ICEException*)exc
{
    test(NO);
}

-(void) paramTest1Response:(TestB*)b1 p2:(TestB*)b2
{
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([b1.sb isEqualToString:@"D1.sb"]);
    test(b1.pb == b2);
    test([b1 isKindOfClass:[TestD1 class]]);
    TestD1* d1 = (TestD1*)b1;
    test([d1.sd1 isEqualToString:@"D1.sd1"]);
    test(d1.pd1 == b2);

    test(b2);
    test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);      // No factory, must be sliced
    test([b2.sb isEqualToString:@"D2.sb"]);
    test(b2.pb == b1);
    [self called];
}

-(void) paramTest1Exception:(ICEException*)exc
{
    test(NO);
}

-(void) returnTest1Response:(TestB*)r_ p1:(TestB*)p1 p2:(TestB*)p2
{
    test(r_ == p1);
    [self called];
}

-(void) returnTest1Exception:(ICEException*)exc
{
    test(NO);
}

-(void) returnTest2Response:(TestB*)r_ p1:(TestB*)p1 p2:(TestB*)p2
{
    test(r_ == p1);
    [self called];
}

-(void) returnTest2Exception:(ICEException*)exc
{
    test(NO);
}

-(void) returnTest3Response:(TestB*)b
{
    self.r = b;
    [self called];
}

-(void) returnTest3Exception:(ICEException*)exc
{
    test(NO);
}

-(void) paramTest3Response:(TestB*)ret p1:(TestB*)p1 p2:(TestB*)p2
{
    test(p1);
    test([p1.sb isEqualToString:@"D2.sb (p1 1)"]);
    test(p1.pb == 0);
    test([[p1 ice_id:nil] isEqualToString:@"::Test::B"]);

    test(p2);
    test([p2.sb isEqualToString:@"D2.sb (p2 1)"]);
    test(p2.pb == 0);
    test([[p2 ice_id:nil] isEqualToString:@"::Test::B"]);

    test(ret);
    test([ret.sb isEqualToString:@"D1.sb (p2 2)"]);
    test(ret.pb == 0);
    test([[ret ice_id:nil] isEqualToString:@"::Test::D1"]);
    [self called];
}

-(void) paramTest3Exception:(ICEException*)exc
{
    test(NO);
}

-(void) paramTest4Response:(TestB*)ret p1:(TestB*)b
{
    test(b);
    test([b.sb isEqualToString:@"D4.sb (1)"]);
    test(b.pb == 0);
    test([[b ice_id:nil] isEqualToString:@"::Test::B"]);

    test(ret);
    test([ret.sb isEqualToString:@"B.sb (2)"]);
    test(ret.pb == 0);
    test([[ret ice_id:nil] isEqualToString:@"::Test::B"]);
    [self called];
}

-(void) paramTest4Exception:(ICEException*)exc
{
    test(NO);
}

-(void) sequenceTestResponse:(TestSS*)ss
{
    self.r = ss;
    [self called];
}

-(void) sequenceTestException:(ICEException*)exc
{
    test(NO);
}

-(void) dictionaryTestResponse:(TestBDict*)r_ bout:(TestBDict*)bout_
{
    self.r = r_;
    self.bout = bout_;
    [self called];
}

-(void) dictionaryTestException:(ICEException*)exc
{
    test(NO);
}

-(void) throwBaseAsBaseResponse
{
    test(NO);
}

-(void) throwBaseAsBaseException:(ICEException*)ex
{
    test([[ex ice_name] isEqualToString:@"Test::BaseException"]);
    TestBaseException* e = (TestBaseException*)ex;
    test([e.sbe isEqualToString:@"sbe"]);
    test(e.pb);
    test([e.pb.sb isEqualToString:@"sb"]);
    test(e.pb.pb == e.pb);
    [self called];
}

-(void) throwDerivedAsBaseResponse
{
    test(NO);
}

-(void) throwDerivedAsBaseException:(ICEException*)ex
{
    test([[ex ice_name] isEqualToString:@"Test::DerivedException"]);
    TestDerivedException* e = (TestDerivedException*)ex;
    test([e.sbe isEqualToString:@"sbe"]);
    test(e.pb);
    test([e.pb.sb isEqualToString:@"sb1"]);
    test(e.pb.pb == e.pb);
    test([e.sde isEqualToString:@"sde1"]);
    test(e.pd1);
    test([e.pd1.sb isEqualToString:@"sb2"]);
    test(e.pd1.pb == e.pd1);
    test([e.pd1.sd1 isEqualToString:@"sd2"]);
    test(e.pd1.pd1 == e.pd1);
    [self called];
}

-(void) throwDerivedAsDerivedResponse
{
    test(NO);
}

-(void) throwDerivedAsDerivedException:(ICEException*)ex
{
    test([[ex ice_name] isEqualToString:@"Test::DerivedException"]);
    TestDerivedException* e = (TestDerivedException*)ex;
    test([e.sbe isEqualToString:@"sbe"]);
    test(e.pb);
    test([e.pb.sb isEqualToString:@"sb1"]);
    test(e.pb.pb == e.pb);
    test([e.sde isEqualToString:@"sde1"]);
    test(e.pd1);
    test([e.pd1.sb isEqualToString:@"sb2"]);
    test(e.pd1.pb == e.pd1);
    test([e.pd1.sd1 isEqualToString:@"sd2"]);
    test(e.pd1.pd1 == e.pd1);
    [self called];
}

-(void) throwUnknownDerivedAsBaseResponse
{
    test(NO);
}

-(void) throwUnknownDerivedAsBaseException:(ICEException*)ex
{
    test([[ex ice_name] isEqualToString:@"Test::BaseException"]);
    TestBaseException* e = (TestBaseException*)ex;
    test([e.sbe isEqualToString:@"sbe"]);
    test(e.pb);
    test([e.pb.sb isEqualToString:@"sb d2"]);
    test(e.pb.pb == e.pb);
    [self called];
}

-(void) useForwardResponse:(TestForward*)f
{
    test(f);
    [self called];
}

-(void) useForwardException:(ICEException*)exc
{
    test(NO);
}
@end

id<TestTestIntfPrx>
allTests(id<ICECommunicator> communicator)
{
    id<ICEObjectPrx> obj = [communicator stringToProxy:@"Test:default -p 12010"];
    id<TestTestIntfPrx> test = [TestTestIntfPrx checkedCast:obj];

    tprintf("base as Object... ");
    {
        ICEObject* o;
        @try
        {
            o = [test SBaseAsObject];
            test(o);
            test([[o ice_id] isEqualToString:@"::Test::SBase"]);
        }
        @catch(...)
        {
            test(0);
        }

	test([o isKindOfClass:[TestSBase class]]);
        test([((TestSBase*)o).sb isEqualToString:@"SBase.sb"]);
    }
    tprintf("ok\n");

    tprintf("base as Object (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test SBaseAsObject_async:cb response:@selector(SBaseAsObjectResponse:) exception:@selector(SBaseAsObjectException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("base as base... ");
    {
        TestSBase* sb;
        @try
        {
            sb = [test SBaseAsSBase];
            test([sb.sb isEqualToString:@"SBase.sb"]);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");
 
    tprintf("base as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test SBaseAsSBase_async:cb response:@selector(SBaseAsSBaseResponse:) exception:@selector(SBaseAsSBaseException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("base with known derived as base... ");
    {
        TestSBase* sb;
        @try
        {
            sb = [test SBSKnownDerivedAsSBase];
            test([sb.sb isEqualToString:@"SBSKnownDerived.sb"]);
        }
        @catch(...)
        {
            test(0);
        }
	test([sb isKindOfClass:[TestSBSKnownDerived class]]);
        test([((TestSBSKnownDerived*)sb).sbskd isEqualToString:@"SBSKnownDerived.sbskd"]);
    }
    tprintf("ok\n");

    tprintf("base with known derived as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test SBSKnownDerivedAsSBase_async:cb response:@selector(SBSKnownDerivedAsSBaseResponse:) exception:@selector(SBSKnownDerivedAsSBaseException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("base with known derived as known derived... ");
    {
        TestSBSKnownDerived* sbskd;
        @try
        {
            sbskd = [test SBSKnownDerivedAsSBSKnownDerived];
            test([sbskd.sbskd isEqualToString:@"SBSKnownDerived.sbskd"]);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("base with known derived as known derived (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test SBSKnownDerivedAsSBSKnownDerived_async:cb response:@selector(SBSKnownDerivedAsSBSKnownDerivedResponse:) exception:@selector(SBSKnownDerivedAsSBSKnownDerivedException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("base with unknown derived as base... ");
    {
        TestSBase* sb;
        @try
        {
            sb = [test SBSUnknownDerivedAsSBase];
            test([sb.sb isEqualToString:@"SBSUnknownDerived.sb"]);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("base with unknown derived as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test SBSUnknownDerivedAsSBase_async:cb response:@selector(SBSUnknownDerivedAsSBaseResponse:) exception:@selector(SBSUnknownDerivedAsSBaseException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("unknown with Object as Object... ");
    {
        id<ICEObject> o;
        @try
        {
            o = [test SUnknownAsObject];
            test(0);
        }
        @catch(ICENoObjectFactoryException*)
        {
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

//     tprintf("unknown with Object as Object (AMI)... ");
//     {
//         @try
//         {
//             Callback* cb = [[Callback alloc] init];
//             [test SUnknownAsObject_async:cb response:@selector(SUnknownAsObjectResponse:) exception:@selector(SUnknownAsObjectException:)];
//             test([cb check]);
//             [cb release];
//         }
//         @catch(NSException*)
//         {
//             test(NO);
//         }
//     }
//     tprintf("ok\n");

    tprintf("one-element cycle... ");
    {
        @try
        {
            TestB* b = [test oneElementCycle];
            test(b);
            test([[b ice_id] isEqualToString:@"::Test::B"]);
            test([b.sb isEqualToString:@"B1.sb"]);
            test(b.pb == b);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("one-element cycle (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test oneElementCycle_async:cb response:@selector(oneElementCycleResponse:) exception:@selector(oneElementCycleException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("two-element cycle... ");
    {
        @try
        {
            TestB* b1 = [test twoElementCycle];
            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::B"]);
            test([b1.sb isEqualToString:@"B1.sb"]);

            TestB* b2 = b1.pb;
            test(b2);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);
            test([b2.sb isEqualToString:@"B2.sb"]);
            test(b2.pb == b1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("two-element cycle (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test twoElementCycle_async:cb response:@selector(twoElementCycleResponse:) exception:@selector(twoElementCycleException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("known derived pointer slicing as base... ");
    {
        @try
        {
            TestB* b1;
            b1 = [test D1AsB];
            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb);
            test(b1.pb != b1);
	    test([b1 isKindOfClass:[TestD1 class]]);
            TestD1* d1 = (TestD1*)b1;
            test([d1.sd1 isEqualToString:@"D1.sd1"]);
            test(d1.pd1);
            test(d1.pd1 != b1);
            test(b1.pb == d1.pd1);

            TestB* b2 = b1.pb;
            test(b2);
            test(b2.pb == b1);
            test([b2.sb isEqualToString:@"D2.sb"]);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("known derived pointer slicing as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test D1AsB_async:cb response:@selector(D1AsBResponse:) exception:@selector(D1AsBException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("known derived pointer slicing as derived... ");
    {
        @try
        {
            TestD1* d1;
            d1 = [test D1AsD1];
            test(d1);
            test([[d1 ice_id] isEqualToString:@"::Test::D1"]);
            test([d1.sb isEqualToString:@"D1.sb"]);
            test(d1.pb);
            test(d1.pb != d1);

            TestB* b2 = d1.pb;
            test(b2);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);
            test([b2.sb isEqualToString:@"D2.sb"]);
            test(b2.pb == d1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("known derived pointer slicing as derived (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test D1AsD1_async:cb response:@selector(D1AsD1Response:) exception:@selector(D1AsD1Exception:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("unknown derived pointer slicing as base... ");
    {
        @try
        {
            TestB* b2;
            b2 = [test D2AsB];
            test(b2);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);
            test([b2.sb isEqualToString:@"D2.sb"]);
            test(b2.pb);
            test(b2.pb != b2);

            TestB* b1 = b2.pb;
            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb == b2);
	    test([b1 isKindOfClass:[TestD1 class]]);
            TestD1* d1 = (TestD1*)b1;
            test([d1.sd1 isEqualToString:@"D1.sd1"]);
            test(d1.pd1 == b2);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("unknown derived pointer slicing as base (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test D2AsB_async:cb response:@selector(D2AsBResponse:) exception:@selector(D2AsBException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("param ptr slicing with known first... ");
    {
        @try
        {
            TestB* b1;
            TestB* b2;
            [test paramTest1:&b1 p2:&b2];

            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb == b2);
	    test([b1 isKindOfClass:[TestD1 class]]);
            TestD1* d1 = (TestD1*)b1;
            test([d1.sd1 isEqualToString:@"D1.sd1"]);
            test(d1.pd1 == b2);

            test(b2);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);  // No factory, must be sliced
            test([b2.sb isEqualToString:@"D2.sb"]);
            test(b2.pb == b1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("param ptr slicing with known first (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test paramTest1_async:cb response:@selector(paramTest1Response:p2:) exception:@selector(paramTest1Exception:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("param ptr slicing with unknown first... ");
    {
        @try
        {
            TestB* b2;
            TestB* b1;
            [test paramTest2:&b2 p1:&b1];

            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb == b2);
	    test([b1 isKindOfClass:[TestD1 class]]);
            TestD1* d1 = (TestD1*)b1;
            test([d1.sd1 isEqualToString:@"D1.sd1"]);
            test(d1.pd1 == b2);

            test(b2);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);  // No factory, must be sliced
            test([b2.sb isEqualToString:@"D2.sb"]);
            test(b2.pb == b1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("return value identity with known first... ");
    {
        @try
        {
            TestB* p1;
            TestB* p2;
            TestB* r = [test returnTest1:&p1 p2:&p2];
            test(r == p1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("return value identity with known first (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test returnTest1_async:cb response:@selector(returnTest1Response:p1:p2:) exception:@selector(returnTest1Exception:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("return value identity with unknown first... ");
    {
        @try
        {
            TestB* p1;
            TestB* p2;
            TestB* r = [test returnTest2:&p1 p1:&p2];
            test(r == p1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("return value identity with unknown first (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test returnTest2_async:cb response:@selector(returnTest2Response:p1:p2:) exception:@selector(returnTest2Exception:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("return value identity for input params known first... ");
    {
        @try
        {
            TestD1* d1 = [TestD1 d1];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestD3* d3 = [TestD3 d3];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            TestB* b1 = [test returnTest3:d1 p2:d3];

            test(b1);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
	    test([b1 isKindOfClass:[TestD1 class]]);
            TestD1* p1 = (TestD1*)b1;
            test([p1.sd1 isEqualToString:@"D1.sd1"]);
            test(p1.pd1 == b1.pb);

            TestB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D3.sb"]);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);  // Sliced by server
            test(b2.pb == b1);
	    test(![b2 isKindOfClass:[TestD3 class]]);

            test(b1 != d1);
            test(b1 != d3);
            test(b2 != d1);
            test(b2 != d3);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("return value identity for input params known first (AMI)... ");
    {
        @try
        {
            TestD1* d1 = [[[TestD1 alloc] init] autorelease];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestD3* d3 = [[[TestD3 alloc] init] autorelease];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            Callback* cb = [[Callback alloc] init];
            [test returnTest3_async:cb response:@selector(returnTest3Response:) exception:@selector(returnTest3Exception:) p1:d1 p2:d3];
            test([cb check]);
            TestB* b1 = cb.r;
            [cb release];

            test(b1);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
            test([b1 isKindOfClass:[TestD1 class]]);
            TestD1* p1 = (TestD1*)b1;
            test([p1.sd1 isEqualToString:@"D1.sd1"]);
            test(p1.pd1 == b1.pb);

            TestB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D3.sb"]);
            test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);  // Sliced by server
            test(b2.pb == b1);
            test(![b2 isKindOfClass:[TestD3 class]]);

            test(b1 != d1);
            test(b1 != d3);
            test(b2 != d1);
            test(b2 != d3);
        }
        @catch(NSException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("return value identity for input params unknown first... ");
    {
        @try
        {
            TestD1* d1 = [TestD1 d1];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestD3* d3 = [TestD3 d3];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            TestB* b1 = [test returnTest3:d3 p2:d1];

            test(b1);
            test([b1.sb isEqualToString:@"D3.sb"]);
            test([[b1 ice_id] isEqualToString:@"::Test::B"]);  // Sliced by server
	    test(![b1 isKindOfClass:[TestD3 class]]);

            TestB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D1.sb"]);
            test([[b2 ice_id] isEqualToString:@"::Test::D1"]);
            test(b2.pb == b1);
	    test([b2 isKindOfClass:[TestD1 class]]);
	    TestD1* p3 = (TestD1*)b2;
            test([p3.sd1 isEqualToString:@"D1.sd1"]);
            test(p3.pd1 == b1);

            test(b1 != d1);
            test(b1 != d3);
            test(b2 != d1);
            test(b2 != d3);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("return value identity for input params unknown first (AMI)... ");
    {
        @try
        {
            TestD1* d1 = [[[TestD1 alloc] init] autorelease];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestD3* d3 = [[[TestD3 alloc] init] autorelease];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            Callback* cb = [[Callback alloc] init];
            [test returnTest3_async:cb response:@selector(returnTest3Response:) exception:@selector(returnTest3Exception:) p1:d3 p2:d1];
            test([cb check]);
            TestB* b1 = cb.r;
            [cb release];

            test(b1);
            test([b1.sb isEqualToString:@"D3.sb"]);
            test([[b1 ice_id:nil] isEqualToString:@"::Test::B"]);  // Sliced by server
            test(![b1 isKindOfClass:[TestD3 class]]);

            TestB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D1.sb"]);
            test([[b2 ice_id:nil] isEqualToString:@"::Test::D1"]);
            test(b2.pb == b1);
            test([b2 isKindOfClass:[TestD1 class]]);
            TestD1* p3 = (TestD1*)b2;
            test([p3.sd1 isEqualToString:@"D1.sd1"]);
            test(p3.pd1 == b1);

            test(b1 != d1);
            test(b1 != d3);
            test(b2 != d1);
            test(b2 != d3);
        }
        @catch(NSException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("remainder unmarshaling (3 instances)... ");
    {
        @try
        {
            TestB* p1;
            TestB* p2;
            TestB* ret = [test paramTest3:&p1 p2:&p2];

            test(p1);
            test([p1.sb isEqualToString:@"D2.sb (p1 1)"]);
            test(!p1.pb);
            test([[p1 ice_id] isEqualToString:@"::Test::B"]);

            test(p2);
            test([p2.sb isEqualToString:@"D2.sb (p2 1)"]);
            test(!p2.pb);
            test([[p2 ice_id] isEqualToString:@"::Test::B"]);

            test(ret);
            test([ret.sb isEqualToString:@"D1.sb (p2 2)"]);
            test(!ret.pb);
            test([[ret ice_id] isEqualToString:@"::Test::D1"]);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("remainder unmarshaling (3 instances) (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test paramTest3_async:cb response:@selector(paramTest3Response:p1:p2:) exception:@selector(paramTest3Exception:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("remainder unmarshaling (4 instances)... ");
    {
        @try
        {
            TestB* b;
            TestB* ret = [test paramTest4:&b];

            test(b);
            test([b.sb isEqualToString:@"D4.sb (1)"]);
            test(!b.pb);
            test([[b ice_id] isEqualToString:@"::Test::B"]);

            test(ret);
            test([ret.sb isEqualToString:@"B.sb (2)"]);
            test(!ret.pb);
            test([[ret ice_id] isEqualToString:@"::Test::B"]);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("remainder unmarshaling (4 instances) (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test paramTest4_async:cb response:@selector(paramTest4Response:p1:) exception:@selector(paramTest4Exception:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("param ptr slicing, instance marshaled in unknown derived as base... ");
    {
        @try
        {
            TestB* b1 = [TestB b];
            b1.sb = @"B.sb(1)";
            b1.pb = b1;

            TestD3* d3 = [TestD3 d3];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = b1;

            TestB* b2 = [TestB b];
            b2.sb = @"B.sb(2)";
            b2.pb = b1;

            TestB* r = [test returnTest3:d3 p2:b2];

            test(r);
            test([[r ice_id] isEqualToString:@"::Test::B"]);
            test([r.sb isEqualToString:@"D3.sb"]);
            test(r.pb == r);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("param ptr slicing, instance marshaled in unknown derived as base (AMI)... ");
    {
        @try
        {
            TestB* b1 = [[[TestB alloc] init] autorelease];
            b1.sb = @"B.sb(1)";
            b1.pb = b1;

            TestD3* d3 = [[[TestD3 alloc] init] autorelease];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = b1;

            TestB* b2 = [[[TestB alloc] init] autorelease];
            b2.sb = @"B.sb(2)";
            b2.pb = b1;

            Callback* cb = [[Callback alloc] init];
            [test returnTest3_async:cb response:@selector(returnTest3Response:) exception:@selector(returnTest3Exception:) p1:d3 p2:b2];
            test([cb check]);
            TestB* r = cb.r;
            [cb release];

            test(r);
            test([[r ice_id:nil] isEqualToString:@"::Test::B"]);
            test([r.sb isEqualToString:@"D3.sb"]);
            test(r.pb == r);
        }
        @catch(NSException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("param ptr slicing, instance marshaled in unknown derived as derived... ");
    {
        @try
        {
            TestD1* d11 = [TestD1 d1];
            d11.sb = @"D1.sb(1)";
            d11.pb = d11;
            d11.sd1 = @"D1.sd1(1)";

            TestD3* d3 = [TestD3 d3];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d11;

            TestD1* d12 = [TestD1 d1];
            d12.sb = @"D1.sb(2)";
            d12.pb = d12;
            d12.sd1 = @"D1.sd1(2)";
            d12.pd1 = d11;

            TestB* r = [test returnTest3:d3 p2:d12];
            test(r);
            test([[r ice_id] isEqualToString:@"::Test::B"]);
            test([r.sb isEqualToString:@"D3.sb"]);
            test(r.pb == r);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("param ptr slicing, instance marshaled in unknown derived as derived (AMI)... ");
    {
        @try
        {
            TestD1* d11 = [[[TestD1 alloc] init] autorelease];
            d11.sb = @"D1.sb(1)";
            d11.pb = d11;
            d11.sd1 = @"D1.sd1(1)";

            TestD3* d3 = [[[TestD3 alloc] init] autorelease];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d11;

            TestD1* d12 = [[[TestD1 alloc] init] autorelease];
            d12.sb = @"D1.sb(2)";
            d12.pb = d12;
            d12.sd1 = @"D1.sd1(2)";
            d12.pd1 = d11;

            Callback* cb = [[Callback alloc] init];
            [test returnTest3_async:cb response:@selector(returnTest3Response:) exception:@selector(returnTest3Exception:) p1:d3 p2:d12];
            test([cb check]);
            TestB* r = cb.r;
            [cb release];
            test(r);
            test([[r ice_id:nil] isEqualToString:@"::Test::B"]);
            test([r.sb isEqualToString:@"D3.sb"]);
            test(r.pb == r);
        }
        @catch(NSException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("sequence slicing... ");
    {
        @try
        {
            TestSS* ss;
            {
                TestB* ss1b = [TestB b];
                ss1b.sb = @"B.sb";
                ss1b.pb = ss1b;

                TestD1* ss1d1 = [TestD1 d1];
                ss1d1.sb = @"D1.sb";
                ss1d1.sd1 = @"D1.sd1";
                ss1d1.pb = ss1b;

                TestD3* ss1d3 = [TestD3 d3];
                ss1d3.sb = @"D3.sb";
                ss1d3.sd3 = @"D3.sd3";
                ss1d3.pb = ss1b;

                TestB* ss2b = [TestB b];
                ss2b.sb = @"B.sb";
                ss2b.pb = ss1b;

                TestD1* ss2d1 = [TestD1 d1];
                ss2d1.sb = @"D1.sb";
                ss2d1.sd1 = @"D1.sd1";
                ss2d1.pb = ss2b;

                TestD3* ss2d3 = [TestD3 d3];
                ss2d3.sb = @"D3.sb";
                ss2d3.sd3 = @"D3.sd3";
                ss2d3.pb = ss2b;

                ss1d1.pd1 = ss2b;
                ss1d3.pd3 = ss2d1;

                ss2d1.pd1 = ss1d3;
                ss2d3.pd3 = ss1d1;

                TestSS1* ss1 = [TestSS1 sS1];
		ss1.s = [TestMutableBSeq arrayWithCapacity:0];
                [(TestMutableBSeq*)ss1.s addObject:ss1b];
                [(TestMutableBSeq*)ss1.s addObject:ss1d1];
                [(TestMutableBSeq*)ss1.s addObject:ss1d3];

                TestSS2* ss2 = [TestSS2 sS2];
		ss2.s = [TestMutableBSeq arrayWithCapacity:0];
                [(TestMutableBSeq*)ss2.s addObject:ss2b];
                [(TestMutableBSeq*)ss2.s addObject:ss2d1];
                [(TestMutableBSeq*)ss2.s addObject:ss2d3];

                ss = [test sequenceTest:ss1 p2:ss2];
            }

            test(ss.c1);
            TestB* ss1b = [ss.c1.s objectAtIndex:0];
            TestB* ss1d1 = [ss.c1.s objectAtIndex:1];
            test(ss.c2);
            TestB* ss1d3 = [ss.c1.s objectAtIndex:2];

            test(ss.c2);
            TestB* ss2b = [ss.c2.s objectAtIndex:0];
            TestB* ss2d1 = [ss.c2.s objectAtIndex:1];
            TestB* ss2d3 = [ss.c2.s objectAtIndex:2];

            test(ss1b.pb == ss1b);
            test(ss1d1.pb == ss1b);
            test(ss1d3.pb == ss1b);

            test(ss2b.pb == ss1b);
            test(ss2d1.pb == ss2b);
            test(ss2d3.pb == ss2b);

            test([[ss1b ice_id] isEqualToString:@"::Test::B"]);
            test([[ss1d1 ice_id] isEqualToString:@"::Test::D1"]);
            test([[ss1d3 ice_id] isEqualToString:@"::Test::B"]);

            test([[ss2b ice_id] isEqualToString:@"::Test::B"]);
            test([[ss2d1 ice_id] isEqualToString:@"::Test::D1"]);
            test([[ss2d3 ice_id] isEqualToString:@"::Test::B"]);
        }
        @catch(ICEException*)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("sequence slicing (AMI)... ");
    {
        @try
        {
            TestSS* ss;
            {
                TestB* ss1b = [[[TestB alloc] init] autorelease];
                ss1b.sb = @"B.sb";
                ss1b.pb = ss1b;

                TestD1* ss1d1 = [[[TestD1 alloc] init] autorelease];
                ss1d1.sb = @"D1.sb";
                ss1d1.sd1 = @"D1.sd1";
                ss1d1.pb = ss1b;

                TestD3* ss1d3 = [[[TestD3 alloc] init] autorelease];
                ss1d3.sb = @"D3.sb";
                ss1d3.sd3 = @"D3.sd3";
                ss1d3.pb = ss1b;

                TestB* ss2b = [[[TestB alloc] init] autorelease];
                ss2b.sb = @"B.sb";
                ss2b.pb = ss1b;

                TestD1* ss2d1 = [[[TestD1 alloc] init] autorelease];
                ss2d1.sb = @"D1.sb";
                ss2d1.sd1 = @"D1.sd1";
                ss2d1.pb = ss2b;

                TestD3* ss2d3 = [[[TestD3 alloc] init] autorelease];
                ss2d3.sb = @"D3.sb";
                ss2d3.sd3 = @"D3.sd3";
                ss2d3.pb = ss2b;

                ss1d1.pd1 = ss2b;
                ss1d3.pd3 = ss2d1;

                ss2d1.pd1 = ss1d3;
                ss2d3.pd3 = ss1d1;

                TestSS1* ss1 = [[[TestSS1 alloc] init] autorelease];
                ss1.s = [TestMutableBSeq array];
                [(NSMutableArray*)ss1.s addObject:ss1b];
                [(NSMutableArray*)ss1.s addObject:ss1d1];
                [(NSMutableArray*)ss1.s addObject:ss1d3];

                TestSS2* ss2 = [[[TestSS2 alloc] init] autorelease];
                ss2.s = [TestMutableBSeq array];
                [(NSMutableArray*)ss2.s addObject:ss2b];
                [(NSMutableArray*)ss2.s addObject:ss2d1];
                [(NSMutableArray*)ss2.s addObject:ss2d3];

                Callback* cb = [[Callback alloc] init];
                [test sequenceTest_async:cb response:@selector(sequenceTestResponse:) exception:@selector(sequenceTestException:) p1:ss1 p2:ss2];
                test([cb check]);
                ss = cb.r;
                [cb release];
            }

            test(ss.c1);
            TestB* ss1b = [ss.c1.s objectAtIndex:0];
            TestB* ss1d1 = [ss.c1.s objectAtIndex:1];
            test(ss.c2);
            TestB* ss1d3 = [ss.c1.s objectAtIndex:2];

            test(ss.c2);
            TestB* ss2b = [ss.c2.s objectAtIndex:0];
            TestB* ss2d1 = [ss.c2.s objectAtIndex:1];
            TestB* ss2d3 = [ss.c2.s objectAtIndex:2];

            test(ss1b.pb == ss1b);
            test(ss1d1.pb == ss1b);
            test(ss1d3.pb == ss1b);

            test(ss2b.pb == ss1b);
            test(ss2d1.pb == ss2b);
            test(ss2d3.pb == ss2b);

            test([[ss1b ice_id:nil] isEqualToString:@"::Test::B"]);
            test([[ss1d1 ice_id:nil] isEqualToString:@"::Test::D1"]);
            test([[ss1d3 ice_id:nil] isEqualToString:@"::Test::B"]);

            test([[ss2b ice_id:nil] isEqualToString:@"::Test::B"]);
            test([[ss2d1 ice_id:nil] isEqualToString:@"::Test::D1"]);
            test([[ss2d3 ice_id:nil] isEqualToString:@"::Test::B"]);
        }
        @catch(ICEException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("dictionary slicing... ");
    {
        @try
        {
            TestMutableBDict* bin = [TestMutableBDict dictionary];
            TestMutableBDict* bout;
            TestBDict* r;
            int i;
            for(i = 0; i < 10; ++i)
            {
                TestD1* d1 = [TestD1 d1];
                NSString *s = [@"D1." stringByAppendingString:[NSString stringWithFormat:@"%d", i]];
                d1.sb = s;
                d1.pb = d1;
                d1.sd1 = s;
                [bin setObject:d1 forKey:[NSNumber numberWithInt:i]];
            }

            r = [test dictionaryTest:bin bout:&bout];

            test([bout count] == 10);
            for(i = 0; i < 10; ++i)
            {
                TestB* b = [bout objectForKey:[NSNumber numberWithInt:i * 10]];
                test(b);
                NSString *s = [@"D1." stringByAppendingString:[NSString stringWithFormat:@"%d", i]];
                test([b.sb isEqualToString:s]);
                test(b.pb);
                test(b.pb != b);
                test([b.pb.sb isEqualToString:s]);
                test(b.pb.pb == b.pb);
            }

            test([r count] == 10);
            for(i = 0; i < 10; ++i)
            {
                TestB* b = [r objectForKey:[NSNumber numberWithInt:i * 20]];
                test(b);
		NSString *s = [@"D1." stringByAppendingString:[NSString stringWithFormat:@"%d", i * 20]];
                test([b.sb isEqualToString:s]);
                test(b.pb == (i == 0 ? nil : [r objectForKey:[NSNumber numberWithInt:(i - 1) * 20]]));
		test([b isKindOfClass:[TestD1 class]]);
		TestD1* d1 = (TestD1*)b;
                test([d1.sd1 isEqualToString:s]);
                test(d1.pd1 == d1);
            }
        }
        @catch(ICEException*)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("dictionary slicing (AMI)... ");
    {
        @try
        {
            TestMutableBDict* bin = [TestMutableBDict dictionary];
            TestMutableBDict* bout;
            TestMutableBDict* r;
            int i;
            for(i = 0; i < 10; ++i)
            {
                TestD1* d1 = [[[TestD1 alloc] init] autorelease];
                d1.sb = [NSString stringWithFormat:@"D1.%d",i];
                d1.pb = d1;
                d1.sd1 = d1.sb;
                [bin setObject:d1 forKey:[NSNumber numberWithInt:i]];
            }

            Callback* cb = [[Callback alloc] init];
            [test dictionaryTest_async:cb response:@selector(dictionaryTestResponse:bout:) exception:@selector(dictionaryTestException:) bin:bin];
            test([cb check]);
            bout = cb.bout;
            r = cb.r;
            [cb release];

            test([bout count] == 10);
            for(i = 0; i < 10; ++i)
            {
                TestB* b = [bout objectForKey:[NSNumber numberWithInt:(i * 10)]];
                test(b);
                NSString* s = [NSString stringWithFormat:@"D1.%d",i];
                test([b.sb isEqualToString:s]);
                test(b.pb);
                test(b.pb != b);
                test([b.pb.sb isEqualToString:s]);
                test(b.pb.pb == b.pb);
            }

            test([r count] == 10);
            for(i = 0; i < 10; ++i)
            {
                TestB* b = [r objectForKey:[NSNumber numberWithInt:(i * 20)]];
                test(b);
                NSString* s = [NSString stringWithFormat:@"D1.%d",(i * 20)];
                test([b.sb isEqualToString:s]);
                test(b.pb == (i == 0 ? nil : [r objectForKey:[NSNumber numberWithInt:((i - 1) * 20)]]));
                test([b isKindOfClass:[TestD1 class]]);
                TestD1* d1 = (TestD1*)b;
                test([d1.sd1 isEqualToString:s]);
                test(d1.pd1 == d1);
            }
        }
        @catch(ICEException*)
        {
            test(NO);
        }
    }
    tprintf("ok\n");

    tprintf("base exception thrown as base exception... ");
    {
        @try
        {
            [test throwBaseAsBase];
            test(0);
        }
        @catch(TestBaseException* e)
        {
            test([[e ice_name] isEqualToString: @"Test::BaseException"]);
            test([e.sbe isEqualToString:@"sbe"]);
            test(e.pb);
            test([e.pb.sb isEqualToString:@"sb"]);
            test(e.pb.pb == e.pb);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("base exception thrown as base exception (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test throwBaseAsBase_async:cb response:@selector(throwBaseAsBaseResponse) exception:@selector(throwBaseAsBaseException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("derived exception thrown as base exception... ");
    {
        @try
        {
            [test throwDerivedAsBase];
            test(0);
        }
        @catch(TestDerivedException* e)
        {
            test([[e ice_name] isEqualToString:@"Test::DerivedException"]);
            test([e.sbe isEqualToString:@"sbe"]);
            test(e.pb);
            test([e.pb.sb isEqualToString:@"sb1"]);
            test(e.pb.pb == e.pb);
            test([e.sde isEqualToString:@"sde1"]);
            test(e.pd1);
            test([e.pd1.sb isEqualToString:@"sb2"]);
            test(e.pd1.pb == e.pd1);
            test([e.pd1.sd1 isEqualToString:@"sd2"]);
            test(e.pd1.pd1 == e.pd1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("derived exception thrown as base exception (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test throwDerivedAsBase_async:cb response:@selector(throwDerivedAsBaseResponse) exception:@selector(throwDerivedAsBaseException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("derived exception thrown as derived exception... ");
    {
        @try
        {
            [test throwDerivedAsDerived];
            test(0);
        }
        @catch(TestDerivedException* e)
        {
            test([[e ice_name] isEqualToString:@"Test::DerivedException"]);
            test([e.sbe isEqualToString:@"sbe"]);
            test(e.pb);
            test([e.pb.sb isEqualToString:@"sb1"]);
            test(e.pb.pb == e.pb);
            test([e.sde isEqualToString:@"sde1"]);
            test(e.pd1);
            test([e.pd1.sb isEqualToString:@"sb2"]);
            test(e.pd1.pb == e.pd1);
            test([e.pd1.sd1 isEqualToString:@"sd2"]);
            test(e.pd1.pd1 == e.pd1);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("derived exception thrown as derived exception (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test throwDerivedAsDerived_async:cb response:@selector(throwDerivedAsDerivedResponse) exception:@selector(throwDerivedAsDerivedException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("unknown derived exception thrown as base exception... ");
    {
        @try
        {
            [test throwUnknownDerivedAsBase];
            test(0);
        }
        @catch(TestBaseException* e)
        {
            test([[e ice_name] isEqualToString:@"Test::BaseException"]);
            test([e.sbe isEqualToString:@"sbe"]);
            test(e.pb);
            test([e.pb.sb isEqualToString:@"sb d2"]);
            test(e.pb.pb == e.pb);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("unknown derived exception thrown as base exception (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test throwUnknownDerivedAsBase_async:cb response:@selector(throwUnknownDerivedAsBaseResponse) exception:@selector(throwUnknownDerivedAsBaseException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    tprintf("forward-declared class... ");
    {
        @try
        {
            TestForward* f;
            [test useForward:&f];
            test(f);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");

    tprintf("forward-declared class (AMI)... ");
    {
        Callback* cb = [[Callback alloc] init];
        [test useForward_async:cb response:@selector(useForwardResponse:) exception:@selector(useForwardException:)];
        test([cb check]);
        [cb release];
    }
    tprintf("ok\n");

    return test;
}
