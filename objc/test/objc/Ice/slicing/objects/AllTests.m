// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <SlicingObjectsTestClient.h>

#import <Foundation/Foundation.h>

@interface TestSlicingObjectsClientCallback : NSObject
{
    BOOL called;
    NSCondition* cond;
@public
    id r;
    id bout;
}
@property(nonatomic, retain) id r;
@property(nonatomic, retain) id bout;
-(void) check;
-(void) called;
@end

@implementation TestSlicingObjectsClientCallback
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
-(void) SBaseAsObjectResponse:(ICEObject*)o
{
    test(o);
    test([[o ice_id:nil] isEqualToString:@"::Test::SBase"]);
    test([o isKindOfClass:[TestSlicingObjectsClientSBase class]]);
    TestSlicingObjectsClientSBase* sb = (TestSlicingObjectsClientSBase*)o;
    test([sb.sb isEqualToString:@"SBase.sb"]);
    [self called];
}

-(void) SBaseAsObjectException:(ICEException*)exc
{
    test(NO);
}

-(void) SBaseAsSBaseResponse:(TestSlicingObjectsClientSBase*)sb
{
    test([sb.sb isEqualToString:@"SBase.sb"]);
    [self called];
}

-(void) SBaseAsSBaseException:(ICEException*)exc
{
    test(NO);
}

-(void) SBSKnownDerivedAsSBaseResponse:(TestSlicingObjectsClientSBase*)sb
{
    test([sb isKindOfClass:[TestSlicingObjectsClientSBSKnownDerived class]]);
    TestSlicingObjectsClientSBSKnownDerived* sbskd = (TestSlicingObjectsClientSBSKnownDerived*)sb;
    test([sbskd.sbskd isEqualToString:@"SBSKnownDerived.sbskd"]);
    [self called];
}

-(void) SBSKnownDerivedAsSBaseException:(ICEException*)exc
{
    test(NO);
}

-(void) SBSKnownDerivedAsSBSKnownDerivedResponse:(TestSlicingObjectsClientSBSKnownDerived*)sbskd
{
    test([sbskd.sbskd isEqualToString:@"SBSKnownDerived.sbskd"]);
    [self called];
}

-(void) SBSKnownDerivedAsSBSKnownDerivedException:(ICEException*)exc
{
    test(NO);
}

-(void) SBSUnknownDerivedAsSBaseResponse:(TestSlicingObjectsClientSBase*)sb
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

-(void) oneElementCycleResponse:(TestSlicingObjectsClientB*)b
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

-(void) twoElementCycleResponse:(TestSlicingObjectsClientB*)b1
{
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::B"]);
    test([b1.sb isEqualToString:@"B1.sb"]);

    TestSlicingObjectsClientB* b2 = b1.pb;
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

-(void) D1AsBResponse:(TestSlicingObjectsClientB*)b1
{
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([b1.sb isEqualToString:@"D1.sb"]);
    test(b1.pb);
    test(b1.pb != b1);
    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
    TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b1;
    test([d1.sd1 isEqualToString:@"D1.sd1"]);
    test(d1.pd1);
    test(d1.pd1 != b1);
    test(b1.pb == d1.pd1);

    TestSlicingObjectsClientB* b2 = b1.pb;
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

-(void) D1AsD1Response:(TestSlicingObjectsClientD1*)d1
{
    test(d1);
    test([[d1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([d1.sb isEqualToString:@"D1.sb"]);
    test(d1.pb);
    test(d1.pb != d1);

    TestSlicingObjectsClientB* b2 = d1.pb;
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

-(void) D2AsBResponse:(TestSlicingObjectsClientB*)b2
{
    test(b2);
    test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);
    test([b2.sb isEqualToString:@"D2.sb"]);
    test(b2.pb);
    test(b2.pb != b2);

    TestSlicingObjectsClientB* b1 = b2.pb;
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([b1.sb isEqualToString:@"D1.sb"]);
    test(b1.pb == b2);
    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
    TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b1;
    test([d1.sd1 isEqualToString:@"D1.sd1"]);
    test(d1.pd1 == b2);
    [self called];
}

-(void) D2AsBException:(ICEException*)exc
{
    test(NO);
}

-(void) paramTest1Response:(TestSlicingObjectsClientB*)b1 p2:(TestSlicingObjectsClientB*)b2
{
    test(b1);
    test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
    test([b1.sb isEqualToString:@"D1.sb"]);
    test(b1.pb == b2);
    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
    TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b1;
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

-(void) returnTest1Response:(TestSlicingObjectsClientB*)r_ p1:(TestSlicingObjectsClientB*)p1 p2:(TestSlicingObjectsClientB*)p2
{
    test(r_ == p1);
    [self called];
}

-(void) returnTest1Exception:(ICEException*)exc
{
    test(NO);
}

-(void) returnTest2Response:(TestSlicingObjectsClientB*)r_ p1:(TestSlicingObjectsClientB*)p1 p2:(TestSlicingObjectsClientB*)p2
{
    test(r_ == p1);
    [self called];
}

-(void) returnTest2Exception:(ICEException*)exc
{
    test(NO);
}

-(void) returnTest3Response:(TestSlicingObjectsClientB*)b
{
    self.r = b;
    [self called];
}

-(void) returnTest3Exception:(ICEException*)exc
{
    test(NO);
}

-(void) paramTest3Response:(TestSlicingObjectsClientB*)ret p1:(TestSlicingObjectsClientB*)p1 p2:(TestSlicingObjectsClientB*)p2
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

-(void) paramTest4Response:(TestSlicingObjectsClientB*)ret p1:(TestSlicingObjectsClientB*)b
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

-(void) sequenceTestResponse:(TestSlicingObjectsClientSS*)ss
{
    self.r = ss;
    [self called];
}

-(void) sequenceTestException:(ICEException*)exc
{
    test(NO);
}

-(void) dictionaryTestResponse:(TestSlicingObjectsClientBDict*)r_ bout:(TestSlicingObjectsClientBDict*)bout_
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
    TestSlicingObjectsClientBaseException* e = (TestSlicingObjectsClientBaseException*)ex;
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
    TestSlicingObjectsClientDerivedException* e = (TestSlicingObjectsClientDerivedException*)ex;
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
    TestSlicingObjectsClientDerivedException* e = (TestSlicingObjectsClientDerivedException*)ex;
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
    TestSlicingObjectsClientBaseException* e = (TestSlicingObjectsClientBaseException*)ex;
    test([e.sbe isEqualToString:@"sbe"]);
    test(e.pb);
    test([e.pb.sb isEqualToString:@"sb d2"]);
    test(e.pb.pb == e.pb);
    [self called];
}

-(void) useForwardResponse:(TestSlicingObjectsSharedForward*)f
{
    test(f);
    [self called];
}

-(void) useForwardException:(ICEException*)exc
{
    test(NO);
}
@end

id<TestSlicingObjectsClientTestIntfPrx>
slicingObjectsAllTests(id<ICECommunicator> communicator)
{
    id<ICEObjectPrx> obj = [communicator stringToProxy:@"Test:default -p 12010"];
    id<TestSlicingObjectsClientTestIntfPrx> test = [TestSlicingObjectsClientTestIntfPrx checkedCast:obj];

    tprintf("base as Object... ");
    {
        ICEObject* o = nil;
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

        test([o isKindOfClass:[TestSlicingObjectsClientSBase class]]);
        test([((TestSlicingObjectsClientSBase*)o).sb isEqualToString:@"SBase.sb"]);
    }
    tprintf("ok\n");

    tprintf("base as Object (AMI)... ");
    {
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_SBaseAsObject:^(ICEObject* o) { [cb SBaseAsObjectResponse:o]; } 
                        exception:^(ICEException* ex) { [cb SBaseAsObjectException:ex]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("base as base... ");
    {
        TestSlicingObjectsClientSBase* sb;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_SBaseAsSBase:^(TestSlicingObjectsClientSBase* o) { [cb SBaseAsSBaseResponse:o]; }
        exception:^(ICEException* e) { [cb SBaseAsSBaseException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("base with known derived as base... ");
    {
        TestSlicingObjectsClientSBase* sb = nil;
        @try
        {
            sb = [test SBSKnownDerivedAsSBase];
            test([sb.sb isEqualToString:@"SBSKnownDerived.sb"]);
        }
        @catch(...)
        {
            test(0);
        }
	test([sb isKindOfClass:[TestSlicingObjectsClientSBSKnownDerived class]]);
        test([((TestSlicingObjectsClientSBSKnownDerived*)sb).sbskd isEqualToString:@"SBSKnownDerived.sbskd"]);
    }
    tprintf("ok\n");

    tprintf("base with known derived as base (AMI)... ");
    {
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_SBSKnownDerivedAsSBase:^(TestSlicingObjectsClientSBase* o) { [cb SBSKnownDerivedAsSBaseResponse:o]; } exception:^(ICEException* e) { [cb SBSKnownDerivedAsSBaseException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("base with known derived as known derived... ");
    {
        TestSlicingObjectsClientSBSKnownDerived* sbskd;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_SBSKnownDerivedAsSBSKnownDerived:^(TestSlicingObjectsClientSBSKnownDerived* o) { [cb SBSKnownDerivedAsSBSKnownDerivedResponse:o]; } exception:^(ICEException* e) { [cb SBSKnownDerivedAsSBSKnownDerivedException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("base with unknown derived as base... ");
    {
        TestSlicingObjectsClientSBase* sb;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_SBSUnknownDerivedAsSBase:^(TestSlicingObjectsClientSBase* o) { [cb SBSUnknownDerivedAsSBaseResponse:o]; } exception:^(ICEException* e) { [cb SBSUnknownDerivedAsSBaseException:e]; }];
        [cb check];
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
//             TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
//             [test begin_SUnknownAsObject:^(ICEObject* o) { [cb SUnknownAsObjectResponse:o]; } exception:^(ICEException* e) { [cb SUnknownAsObjectException:e]; }];
//             [cb check];
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
            TestSlicingObjectsClientB* b = [test oneElementCycle];
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_oneElementCycle:^(TestSlicingObjectsClientB* o) { [cb oneElementCycleResponse:o]; } exception:^(ICEException* e) { [cb oneElementCycleException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("two-element cycle... ");
    {
        @try
        {
            TestSlicingObjectsClientB* b1 = [test twoElementCycle];
            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::B"]);
            test([b1.sb isEqualToString:@"B1.sb"]);

            TestSlicingObjectsClientB* b2 = b1.pb;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_twoElementCycle:^(TestSlicingObjectsClientB* o) { [cb twoElementCycleResponse:o]; } exception:^(ICEException* e) { [cb twoElementCycleException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("known derived pointer slicing as base... ");
    {
        @try
        {
            TestSlicingObjectsClientB* b1;
            b1 = [test D1AsB];
            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb);
            test(b1.pb != b1);
	    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
            TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b1;
            test([d1.sd1 isEqualToString:@"D1.sd1"]);
            test(d1.pd1);
            test(d1.pd1 != b1);
            test(b1.pb == d1.pd1);

            TestSlicingObjectsClientB* b2 = b1.pb;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_D1AsB:^(TestSlicingObjectsClientB* o) { [cb D1AsBResponse:o]; } exception:^(ICEException* e) { [cb D1AsBException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("known derived pointer slicing as derived... ");
    {
        @try
        {
            TestSlicingObjectsClientD1* d1;
            d1 = [test D1AsD1];
            test(d1);
            test([[d1 ice_id] isEqualToString:@"::Test::D1"]);
            test([d1.sb isEqualToString:@"D1.sb"]);
            test(d1.pb);
            test(d1.pb != d1);

            TestSlicingObjectsClientB* b2 = d1.pb;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_D1AsD1:^(TestSlicingObjectsClientD1* o) { [cb D1AsD1Response:o]; } exception:^(ICEException* e) { [cb D1AsD1Exception:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("unknown derived pointer slicing as base... ");
    {
        @try
        {
            TestSlicingObjectsClientB* b2;
            b2 = [test D2AsB];
            test(b2);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);
            test([b2.sb isEqualToString:@"D2.sb"]);
            test(b2.pb);
            test(b2.pb != b2);

            TestSlicingObjectsClientB* b1 = b2.pb;
            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb == b2);
	    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
            TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b1;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_D2AsB:^(TestSlicingObjectsClientB* o) { [cb D2AsBResponse:o]; } exception:^(ICEException* e) { [cb D2AsBException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("param ptr slicing with known first... ");
    {
        @try
        {
            TestSlicingObjectsClientB* b1;
            TestSlicingObjectsClientB* b2;
            [test paramTest1:&b1 p2:&b2];

            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb == b2);
	    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
            TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b1;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_paramTest1:^(TestSlicingObjectsClientB* o, TestSlicingObjectsClientB* b2) { [cb paramTest1Response:o p2:b2]; } exception:^(ICEException* e) { [cb paramTest1Exception:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("param ptr slicing with unknown first... ");
    {
        @try
        {
            TestSlicingObjectsClientB* b2;
            TestSlicingObjectsClientB* b1;
            [test paramTest2:&b2 p1:&b1];

            test(b1);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test(b1.pb == b2);
	    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
            TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b1;
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
            TestSlicingObjectsClientB* p1;
            TestSlicingObjectsClientB* p2;
            TestSlicingObjectsClientB* r = [test returnTest1:&p1 p2:&p2];
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_returnTest1:^(TestSlicingObjectsClientB* o, TestSlicingObjectsClientB* b1, TestSlicingObjectsClientB* b2) { [cb returnTest1Response:o p1:b1 p2:b2]; } exception:^(ICEException* e) { [cb returnTest1Exception:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("return value identity with unknown first... ");
    {
        @try
        {
            TestSlicingObjectsClientB* p1;
            TestSlicingObjectsClientB* p2;
            TestSlicingObjectsClientB* r = [test returnTest2:&p1 p1:&p2];
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_returnTest2:^(TestSlicingObjectsClientB* o, TestSlicingObjectsClientB* b1, TestSlicingObjectsClientB* b2) { [cb returnTest2Response:o p1:b1 p2:b2]; } exception:^(ICEException* e) { [cb returnTest2Exception:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("return value identity for input params known first... ");
    {
        @try
        {
            TestSlicingObjectsClientD1* d1 = [TestSlicingObjectsClientD1 d1];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestSlicingObjectsClientD3* d3 = [TestSlicingObjectsClientD3 d3];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            TestSlicingObjectsClientB* b1 = [test returnTest3:d1 p2:d3];

            test(b1);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test([[b1 ice_id] isEqualToString:@"::Test::D1"]);
	    test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
            TestSlicingObjectsClientD1* p1 = (TestSlicingObjectsClientD1*)b1;
            test([p1.sd1 isEqualToString:@"D1.sd1"]);
            test(p1.pd1 == b1.pb);

            TestSlicingObjectsClientB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D3.sb"]);
            test([[b2 ice_id] isEqualToString:@"::Test::B"]);  // Sliced by server
            test(b2.pb == b1);
	    test(![b2 isKindOfClass:[TestSlicingObjectsClientD3 class]]);

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
            TestSlicingObjectsClientD1* d1 = [[[TestSlicingObjectsClientD1 alloc] init] autorelease];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestSlicingObjectsClientD3* d3 = [[[TestSlicingObjectsClientD3 alloc] init] autorelease];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
            [test begin_returnTest3:d1 p2:d3 response:^(TestSlicingObjectsClientB* o) { [cb returnTest3Response:o]; } exception:^(ICEException* e) { [cb returnTest3Exception:e]; }];
            [cb check];
            TestSlicingObjectsClientB* b1 = cb.r;

            test(b1);
            test([b1.sb isEqualToString:@"D1.sb"]);
            test([[b1 ice_id:nil] isEqualToString:@"::Test::D1"]);
            test([b1 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
            TestSlicingObjectsClientD1* p1 = (TestSlicingObjectsClientD1*)b1;
            test([p1.sd1 isEqualToString:@"D1.sd1"]);
            test(p1.pd1 == b1.pb);

            TestSlicingObjectsClientB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D3.sb"]);
            test([[b2 ice_id:nil] isEqualToString:@"::Test::B"]);  // Sliced by server
            test(b2.pb == b1);
            test(![b2 isKindOfClass:[TestSlicingObjectsClientD3 class]]);

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
            TestSlicingObjectsClientD1* d1 = [TestSlicingObjectsClientD1 d1];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestSlicingObjectsClientD3* d3 = [TestSlicingObjectsClientD3 d3];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            TestSlicingObjectsClientB* b1 = [test returnTest3:d3 p2:d1];

            test(b1);
            test([b1.sb isEqualToString:@"D3.sb"]);
            test([[b1 ice_id] isEqualToString:@"::Test::B"]);  // Sliced by server
	    test(![b1 isKindOfClass:[TestSlicingObjectsClientD3 class]]);

            TestSlicingObjectsClientB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D1.sb"]);
            test([[b2 ice_id] isEqualToString:@"::Test::D1"]);
            test(b2.pb == b1);
	    test([b2 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
	    TestSlicingObjectsClientD1* p3 = (TestSlicingObjectsClientD1*)b2;
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
            TestSlicingObjectsClientD1* d1 = [[[TestSlicingObjectsClientD1 alloc] init] autorelease];
            d1.sb = @"D1.sb";
            d1.sd1 = @"D1.sd1";
            TestSlicingObjectsClientD3* d3 = [[[TestSlicingObjectsClientD3 alloc] init] autorelease];
            d3.pb = d1;
            d3.sb = @"D3.sb";
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d1;
            d1.pb = d3;
            d1.pd1 = d3;

            TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
            [test begin_returnTest3:d3 p2:d1 response:^(TestSlicingObjectsClientB* o) { [cb returnTest3Response:o]; } exception:^(ICEException* e) { [cb returnTest3Exception:e]; }];
            [cb check];
            TestSlicingObjectsClientB* b1 = cb.r;

            test(b1);
            test([b1.sb isEqualToString:@"D3.sb"]);
            test([[b1 ice_id:nil] isEqualToString:@"::Test::B"]);  // Sliced by server
            test(![b1 isKindOfClass:[TestSlicingObjectsClientD3 class]]);

            TestSlicingObjectsClientB* b2 = b1.pb;
            test(b2);
            test([b2.sb isEqualToString:@"D1.sb"]);
            test([[b2 ice_id:nil] isEqualToString:@"::Test::D1"]);
            test(b2.pb == b1);
            test([b2 isKindOfClass:[TestSlicingObjectsClientD1 class]]);
            TestSlicingObjectsClientD1* p3 = (TestSlicingObjectsClientD1*)b2;
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
            TestSlicingObjectsClientB* p1;
            TestSlicingObjectsClientB* p2;
            TestSlicingObjectsClientB* ret = [test paramTest3:&p1 p2:&p2];

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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_paramTest3:^(TestSlicingObjectsClientB* o, TestSlicingObjectsClientB* b1, TestSlicingObjectsClientB* b2) { [cb paramTest3Response:o p1:b1 p2:b2]; } exception:^(ICEException* e) { [cb paramTest3Exception:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("remainder unmarshaling (4 instances)... ");
    {
        @try
        {
            TestSlicingObjectsClientB* b;
            TestSlicingObjectsClientB* ret = [test paramTest4:&b];

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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_paramTest4:^(TestSlicingObjectsClientB* o, TestSlicingObjectsClientB* b) { [cb paramTest4Response:o p1:b]; } exception:^(ICEException* e) { [cb paramTest4Exception:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("param ptr slicing, instance marshaled in unknown derived as base... ");
    {
        @try
        {
            TestSlicingObjectsClientB* b1 = [TestSlicingObjectsClientB b];
            b1.sb = @"B.sb(1)";
            b1.pb = b1;

            TestSlicingObjectsClientD3* d3 = [TestSlicingObjectsClientD3 d3];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = b1;

            TestSlicingObjectsClientB* b2 = [TestSlicingObjectsClientB b];
            b2.sb = @"B.sb(2)";
            b2.pb = b1;

            TestSlicingObjectsClientB* r = [test returnTest3:d3 p2:b2];

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
            TestSlicingObjectsClientB* b1 = [[[TestSlicingObjectsClientB alloc] init] autorelease];
            b1.sb = @"B.sb(1)";
            b1.pb = b1;

            TestSlicingObjectsClientD3* d3 = [[[TestSlicingObjectsClientD3 alloc] init] autorelease];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = b1;

            TestSlicingObjectsClientB* b2 = [[[TestSlicingObjectsClientB alloc] init] autorelease];
            b2.sb = @"B.sb(2)";
            b2.pb = b1;

            TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
            [test begin_returnTest3:d3 p2:b2 response:^(TestSlicingObjectsClientB* o) { [cb returnTest3Response:o]; } exception:^(ICEException* e) { [cb returnTest3Exception:e]; }];
            [cb check];
            TestSlicingObjectsClientB* r = cb.r;

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
            TestSlicingObjectsClientD1* d11 = [TestSlicingObjectsClientD1 d1];
            d11.sb = @"D1.sb(1)";
            d11.pb = d11;
            d11.sd1 = @"D1.sd1(1)";

            TestSlicingObjectsClientD3* d3 = [TestSlicingObjectsClientD3 d3];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d11;

            TestSlicingObjectsClientD1* d12 = [TestSlicingObjectsClientD1 d1];
            d12.sb = @"D1.sb(2)";
            d12.pb = d12;
            d12.sd1 = @"D1.sd1(2)";
            d12.pd1 = d11;

            TestSlicingObjectsClientB* r = [test returnTest3:d3 p2:d12];
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
            TestSlicingObjectsClientD1* d11 = [[[TestSlicingObjectsClientD1 alloc] init] autorelease];
            d11.sb = @"D1.sb(1)";
            d11.pb = d11;
            d11.sd1 = @"D1.sd1(1)";

            TestSlicingObjectsClientD3* d3 = [[[TestSlicingObjectsClientD3 alloc] init] autorelease];
            d3.sb = @"D3.sb";
            d3.pb = d3;
            d3.sd3 = @"D3.sd3";
            d3.pd3 = d11;

            TestSlicingObjectsClientD1* d12 = [[[TestSlicingObjectsClientD1 alloc] init] autorelease];
            d12.sb = @"D1.sb(2)";
            d12.pb = d12;
            d12.sd1 = @"D1.sd1(2)";
            d12.pd1 = d11;

            TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
            [test begin_returnTest3:d3 p2:d12 response:^(TestSlicingObjectsClientB* o) { [cb returnTest3Response:o]; } exception:^(ICEException* e) { [cb returnTest3Exception:e]; }];
            [cb check];
            TestSlicingObjectsClientB* r = cb.r;
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
            TestSlicingObjectsClientSS* ss;
            {
                TestSlicingObjectsClientB* ss1b = [TestSlicingObjectsClientB b];
                ss1b.sb = @"B.sb";
                ss1b.pb = ss1b;

                TestSlicingObjectsClientD1* ss1d1 = [TestSlicingObjectsClientD1 d1];
                ss1d1.sb = @"D1.sb";
                ss1d1.sd1 = @"D1.sd1";
                ss1d1.pb = ss1b;

                TestSlicingObjectsClientD3* ss1d3 = [TestSlicingObjectsClientD3 d3];
                ss1d3.sb = @"D3.sb";
                ss1d3.sd3 = @"D3.sd3";
                ss1d3.pb = ss1b;

                TestSlicingObjectsClientB* ss2b = [TestSlicingObjectsClientB b];
                ss2b.sb = @"B.sb";
                ss2b.pb = ss1b;

                TestSlicingObjectsClientD1* ss2d1 = [TestSlicingObjectsClientD1 d1];
                ss2d1.sb = @"D1.sb";
                ss2d1.sd1 = @"D1.sd1";
                ss2d1.pb = ss2b;

                TestSlicingObjectsClientD3* ss2d3 = [TestSlicingObjectsClientD3 d3];
                ss2d3.sb = @"D3.sb";
                ss2d3.sd3 = @"D3.sd3";
                ss2d3.pb = ss2b;

                ss1d1.pd1 = ss2b;
                ss1d3.pd3 = ss2d1;

                ss2d1.pd1 = ss1d3;
                ss2d3.pd3 = ss1d1;

                TestSlicingObjectsClientSS1* ss1 = [TestSlicingObjectsClientSS1 sS1];
		ss1.s = [TestSlicingObjectsClientMutableBSeq arrayWithCapacity:0];
                [(TestSlicingObjectsClientMutableBSeq*)ss1.s addObject:ss1b];
                [(TestSlicingObjectsClientMutableBSeq*)ss1.s addObject:ss1d1];
                [(TestSlicingObjectsClientMutableBSeq*)ss1.s addObject:ss1d3];

                TestSlicingObjectsClientSS2* ss2 = [TestSlicingObjectsClientSS2 sS2];
		ss2.s = [TestSlicingObjectsClientMutableBSeq arrayWithCapacity:0];
                [(TestSlicingObjectsClientMutableBSeq*)ss2.s addObject:ss2b];
                [(TestSlicingObjectsClientMutableBSeq*)ss2.s addObject:ss2d1];
                [(TestSlicingObjectsClientMutableBSeq*)ss2.s addObject:ss2d3];

                ss = [test sequenceTest:ss1 p2:ss2];
            }

            test(ss.c1);
            TestSlicingObjectsClientB* ss1b = [ss.c1.s objectAtIndex:0];
            TestSlicingObjectsClientB* ss1d1 = [ss.c1.s objectAtIndex:1];
            test(ss.c2);
            TestSlicingObjectsClientB* ss1d3 = [ss.c1.s objectAtIndex:2];

            test(ss.c2);
            TestSlicingObjectsClientB* ss2b = [ss.c2.s objectAtIndex:0];
            TestSlicingObjectsClientB* ss2d1 = [ss.c2.s objectAtIndex:1];
            TestSlicingObjectsClientB* ss2d3 = [ss.c2.s objectAtIndex:2];

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
            TestSlicingObjectsClientSS* ss;
            {
                TestSlicingObjectsClientB* ss1b = [[[TestSlicingObjectsClientB alloc] init] autorelease];
                ss1b.sb = @"B.sb";
                ss1b.pb = ss1b;

                TestSlicingObjectsClientD1* ss1d1 = [[[TestSlicingObjectsClientD1 alloc] init] autorelease];
                ss1d1.sb = @"D1.sb";
                ss1d1.sd1 = @"D1.sd1";
                ss1d1.pb = ss1b;

                TestSlicingObjectsClientD3* ss1d3 = [[[TestSlicingObjectsClientD3 alloc] init] autorelease];
                ss1d3.sb = @"D3.sb";
                ss1d3.sd3 = @"D3.sd3";
                ss1d3.pb = ss1b;

                TestSlicingObjectsClientB* ss2b = [[[TestSlicingObjectsClientB alloc] init] autorelease];
                ss2b.sb = @"B.sb";
                ss2b.pb = ss1b;

                TestSlicingObjectsClientD1* ss2d1 = [[[TestSlicingObjectsClientD1 alloc] init] autorelease];
                ss2d1.sb = @"D1.sb";
                ss2d1.sd1 = @"D1.sd1";
                ss2d1.pb = ss2b;

                TestSlicingObjectsClientD3* ss2d3 = [[[TestSlicingObjectsClientD3 alloc] init] autorelease];
                ss2d3.sb = @"D3.sb";
                ss2d3.sd3 = @"D3.sd3";
                ss2d3.pb = ss2b;

                ss1d1.pd1 = ss2b;
                ss1d3.pd3 = ss2d1;

                ss2d1.pd1 = ss1d3;
                ss2d3.pd3 = ss1d1;

                TestSlicingObjectsClientSS1* ss1 = [[[TestSlicingObjectsClientSS1 alloc] init] autorelease];
                ss1.s = [TestSlicingObjectsClientMutableBSeq array];
                [(NSMutableArray*)ss1.s addObject:ss1b];
                [(NSMutableArray*)ss1.s addObject:ss1d1];
                [(NSMutableArray*)ss1.s addObject:ss1d3];

                TestSlicingObjectsClientSS2* ss2 = [[[TestSlicingObjectsClientSS2 alloc] init] autorelease];
                ss2.s = [TestSlicingObjectsClientMutableBSeq array];
                [(NSMutableArray*)ss2.s addObject:ss2b];
                [(NSMutableArray*)ss2.s addObject:ss2d1];
                [(NSMutableArray*)ss2.s addObject:ss2d3];

                TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
                [test begin_sequenceTest:ss1 p2:ss2 response:^(TestSlicingObjectsClientSS* o) { [cb sequenceTestResponse:o]; } exception:^(ICEException* e) { [cb sequenceTestException:e]; }];
                [cb check];
                ss = cb.r;
            }

            test(ss.c1);
            TestSlicingObjectsClientB* ss1b = [ss.c1.s objectAtIndex:0];
            TestSlicingObjectsClientB* ss1d1 = [ss.c1.s objectAtIndex:1];
            test(ss.c2);
            TestSlicingObjectsClientB* ss1d3 = [ss.c1.s objectAtIndex:2];

            test(ss.c2);
            TestSlicingObjectsClientB* ss2b = [ss.c2.s objectAtIndex:0];
            TestSlicingObjectsClientB* ss2d1 = [ss.c2.s objectAtIndex:1];
            TestSlicingObjectsClientB* ss2d3 = [ss.c2.s objectAtIndex:2];

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
            TestSlicingObjectsClientMutableBDict* bin = [TestSlicingObjectsClientMutableBDict dictionary];
            TestSlicingObjectsClientMutableBDict* bout;
            TestSlicingObjectsClientBDict* r;
            int i;
            for(i = 0; i < 10; ++i)
            {
                TestSlicingObjectsClientD1* d1 = [TestSlicingObjectsClientD1 d1];
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
                TestSlicingObjectsClientB* b = [bout objectForKey:[NSNumber numberWithInt:i * 10]];
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
                TestSlicingObjectsClientB* b = [r objectForKey:[NSNumber numberWithInt:i * 20]];
                test(b);
		NSString *s = [@"D1." stringByAppendingString:[NSString stringWithFormat:@"%d", i * 20]];
                test([b.sb isEqualToString:s]);
                test(b.pb == (i == 0 ? nil : [r objectForKey:[NSNumber numberWithInt:(i - 1) * 20]]));
		test([b isKindOfClass:[TestSlicingObjectsClientD1 class]]);
		TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b;
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
            TestSlicingObjectsClientMutableBDict* bin = [TestSlicingObjectsClientMutableBDict dictionary];
            TestSlicingObjectsClientMutableBDict* bout;
            TestSlicingObjectsClientMutableBDict* r;
            int i;
            for(i = 0; i < 10; ++i)
            {
                TestSlicingObjectsClientD1* d1 = [[[TestSlicingObjectsClientD1 alloc] init] autorelease];
                d1.sb = [NSString stringWithFormat:@"D1.%d",i];
                d1.pb = d1;
                d1.sd1 = d1.sb;
                [bin setObject:d1 forKey:[NSNumber numberWithInt:i]];
            }

            TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
            [test begin_dictionaryTest:bin response:^(TestSlicingObjectsClientMutableBDict* o, TestSlicingObjectsClientMutableBDict* bout) { [cb dictionaryTestResponse:o bout:bout]; } exception:^(ICEException* e) { [cb dictionaryTestException:e]; }];
            [cb check];
            bout = cb.bout;
            r = cb.r;

            test([bout count] == 10);
            for(i = 0; i < 10; ++i)
            {
                TestSlicingObjectsClientB* b = [bout objectForKey:[NSNumber numberWithInt:(i * 10)]];
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
                TestSlicingObjectsClientB* b = [r objectForKey:[NSNumber numberWithInt:(i * 20)]];
                test(b);
                NSString* s = [NSString stringWithFormat:@"D1.%d",(i * 20)];
                test([b.sb isEqualToString:s]);
                test(b.pb == (i == 0 ? nil : [r objectForKey:[NSNumber numberWithInt:((i - 1) * 20)]]));
                test([b isKindOfClass:[TestSlicingObjectsClientD1 class]]);
                TestSlicingObjectsClientD1* d1 = (TestSlicingObjectsClientD1*)b;
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
        @catch(TestSlicingObjectsClientBaseException* e)
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_throwBaseAsBase:^ { [cb throwBaseAsBaseResponse]; } exception:^(ICEException* e) { [cb throwBaseAsBaseException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("derived exception thrown as base exception... ");
    {
        @try
        {
            [test throwDerivedAsBase];
            test(0);
        }
        @catch(TestSlicingObjectsClientDerivedException* e)
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_throwDerivedAsBase:^ { [cb throwDerivedAsBaseResponse]; } exception:^(ICEException* e) { [cb throwDerivedAsBaseException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("derived exception thrown as derived exception... ");
    {
        @try
        {
            [test throwDerivedAsDerived];
            test(0);
        }
        @catch(TestSlicingObjectsClientDerivedException* e)
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_throwDerivedAsDerived:^ { [cb throwDerivedAsDerivedResponse]; } exception:^(ICEException* e) { [cb throwDerivedAsDerivedException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("unknown derived exception thrown as base exception... ");
    {
        @try
        {
            [test throwUnknownDerivedAsBase];
            test(0);
        }
        @catch(TestSlicingObjectsClientBaseException* e)
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_throwUnknownDerivedAsBase:^ { [cb throwUnknownDerivedAsBaseResponse]; } exception:^(ICEException* e) { [cb throwUnknownDerivedAsBaseException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    tprintf("forward-declared class... ");
    {
        @try
        {
            TestSlicingObjectsSharedForward* f;
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
        TestSlicingObjectsClientCallback* cb = [[[TestSlicingObjectsClientCallback alloc] init] autorelease];
        [test begin_useForward:^(TestSlicingObjectsSharedForward* o) { [cb useForwardResponse:o]; } exception:^(ICEException* e) { [cb useForwardException:e]; }];
        [cb check];
    }
    tprintf("ok\n");

    return test;
}
