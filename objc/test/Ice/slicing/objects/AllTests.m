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

#if 0

@interface CallbackBase : IceUtilMonitor<IceUtilMutex>
{
public:

    CallbackBase() :
        _called(NO)
    {
    }

    ~CallbackBase()
    {
    }

-(bool) check
    {
        IceUtilMonitor<IceUtilMutex>Lock sync(*this);
        while(!_called)
        {
            if(!timedWait(IceUtilTimeseconds(5)))
            {
                return NO;
            }
        }
        _called = NO;
        return YES;
    }

protected:

-(void) called
    {
        IceUtilMonitor<IceUtilMutex>Lock sync(*this);
        assert(!_called);
        _called = YES;
        notify();
    }

private:

    BOOL _called;
};

@interface AMI_Test_SBaseAsObjectI : AMI_TestIntf_SBaseAsObject, public CallbackBase
{
    void
    ice_response(id<ICEObject> o)
    {
        test(o);
        test([o ice_id]:isEqualToString:@"::Test::SBase");
        id<SBase> sb = SBasePtrdynamicCast(o);
        test(sb);
        test(sb->sb == @"SBase.sb");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_SBaseAsObjectI> AMI_Test_SBaseAsObjectIPtr;

@interface AMI_Test_SBaseAsSBaseI : AMI_TestIntf_SBaseAsSBase, public CallbackBase
{
    void
    ice_response(id<SBase> sb)
    {
        test(sb->sb == @"SBase.sb");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_SBaseAsSBaseI> AMI_Test_SBaseAsSBaseIPtr;

@interface AMI_Test_SBSKnownDerivedAsSBaseI : AMI_TestIntf_SBSKnownDerivedAsSBase, public CallbackBase
{
    void
    ice_response(id<SBase> sb)
    {
        id<SBSKnownDerived> sbskd = SBSKnownDerivedPtrdynamicCast(sb);
        test(sbskd);
        test(sbskd->sbskd == @"SBSKnownDerived.sbskd");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_SBSKnownDerivedAsSBaseI> AMI_Test_SBSKnownDerivedAsSBaseIPtr;

class AMI_Test_SBSKnownDerivedAsSBSKnownDerivedI
    : public AMI_TestIntf_SBSKnownDerivedAsSBSKnownDerived, public CallbackBase
{
    void
    ice_response(id<SBSKnownDerived> sbskd)
    {
        test(sbskd->sbskd == @"SBSKnownDerived.sbskd");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_SBSKnownDerivedAsSBSKnownDerivedI> AMI_Test_SBSKnownDerivedAsSBSKnownDerivedIPtr;

@interface AMI_Test_SBSUnknownDerivedAsSBaseI : AMI_TestIntf_SBSUnknownDerivedAsSBase, public CallbackBase
{
    void
    ice_response(id<SBase> sb)
    {
        test(sb->sb == @"SBSUnknownDerived.sb");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_SBSUnknownDerivedAsSBaseI> AMI_Test_SBSUnknownDerivedAsSBaseIPtr;

@interface AMI_Test_SUnknownAsObjectI : AMI_TestIntf_SUnknownAsObject, public CallbackBase
{
    void
    ice_response(id<ICEObject> o)
    {
        test(NO);
    }

    void
    ice_exception(ICEException* exc)
    {
        test(exc.ice_name() == @"ICENoObjectFactoryException");
        [self called];
    }
};

typedef IceUtilHandle<AMI_Test_SUnknownAsObjectI> AMI_Test_SUnknownAsObjectIPtr;

@interface AMI_Test_oneElementCycleI : AMI_TestIntf_oneElementCycle, public CallbackBase
{
    void
    ice_response(id<B> b)
    {
        test(b);
        test([b ice_id]:isEqualToString:@"::Test::B");
        test(b->sb == @"B1.sb");
        test(b->pb == b);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_oneElementCycleI> AMI_Test_oneElementCycleIPtr;

@interface AMI_Test_twoElementCycleI : AMI_TestIntf_twoElementCycle, public CallbackBase
{
    void
    ice_response(id<B> b1)
    {
        test(b1);
        test([b1 ice_id]:isEqualToString:@"::Test::B");
        test(b1->sb == @"B1.sb");

        id<B> b2 = b1->pb;
        test(b2);
        test([b2 ice_id]:isEqualToString:@"::Test::B");
        test(b2->sb == @"B2.sb");
        test(b2->pb == b1);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_twoElementCycleI> AMI_Test_twoElementCycleIPtr;

@interface AMI_Test_D1AsBI : AMI_TestIntf_D1AsB, public CallbackBase
{
    void
    ice_response(id<B> b1)
    {
        test(b1);
        test([b1 ice_id]:isEqualToString:@"::Test::D1");
        test(b1->sb == @"D1.sb");
        test(b1->pb);
        test(b1->pb != b1);
        id<D1> d1 = D1PtrdynamicCast(b1);
        test(d1);
        test(d1->sd1 == @"D1.sd1");
        test(d1->pd1);
        test(d1->pd1 != b1);
        test(b1->pb == d1->pd1);

        id<B> b2 = b1->pb;
        test(b2);
        test(b2->pb == b1);
        test(b2->sb == @"D2.sb");
        test([b2 ice_id]:isEqualToString:@"::Test::B");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_D1AsBI> AMI_Test_D1AsBIPtr;

@interface AMI_Test_D1AsD1I : AMI_TestIntf_D1AsD1, public CallbackBase
{
    void
    ice_response(id<D1> d1)
    {
        test(d1);
        test([d1 ice_id]:isEqualToString:@"::Test::D1");
        test(d1->sb == @"D1.sb");
        test(d1->pb);
        test(d1->pb != d1);

        id<B> b2 = d1->pb;
        test(b2);
        test([b2 ice_id]:isEqualToString:@"::Test::B");
        test(b2->sb == @"D2.sb");
        test(b2->pb == d1);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_D1AsD1I> AMI_Test_D1AsD1IPtr;

@interface AMI_Test_D2AsBI : AMI_TestIntf_D2AsB, public CallbackBase
{
    void
    ice_response(id<B> b2)
    {
        test(b2);
        test([b2 ice_id]:isEqualToString:@"::Test::B");
        test(b2->sb == @"D2.sb");
        test(b2->pb);
        test(b2->pb != b2);

        id<B> b1 = b2->pb;
        test(b1);
        test([b1 ice_id]:isEqualToString:@"::Test::D1");
        test(b1->sb == @"D1.sb");
        test(b1->pb == b2);
        id<D1> d1 = D1PtrdynamicCast(b1);
        test(d1);
        test(d1->sd1 == @"D1.sd1");
        test(d1->pd1 == b2);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_D2AsBI> AMI_Test_D2AsBIPtr;

@interface AMI_Test_paramTest1I : AMI_TestIntf_paramTest1, public CallbackBase
{
    void
    ice_response(id<B> b1, id<B> b2)
    {
        test(b1);
        test([b1 ice_id]:isEqualToString:@"::Test::D1");
        test(b1->sb == @"D1.sb");
        test(b1->pb == b2);
        id<D1> d1 = D1PtrdynamicCast(b1);
        test(d1);
        test(d1->sd1 == @"D1.sd1");
        test(d1->pd1 == b2);

        test(b2);
        test([b2 ice_id]:isEqualToString:@"::Test::B");      // No factory, must be sliced
        test(b2->sb == @"D2.sb");
        test(b2->pb == b1);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_paramTest1I> AMI_Test_paramTest1IPtr;

@interface AMI_Test_returnTest1I : AMI_TestIntf_returnTest1, public CallbackBase
{
    void
    ice_response(id<B> r, id<B> p1, id<B> p2)
    {
        test(r == p1);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_returnTest1I> AMI_Test_returnTest1IPtr;

@interface AMI_Test_returnTest2I : AMI_TestIntf_returnTest2, public CallbackBase
{
    void
    ice_response(id<B> r, id<B> p1, id<B> p2)
    {
        test(r == p1);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_returnTest2I> AMI_Test_returnTest2IPtr;

@interface AMI_Test_returnTest3I : AMI_TestIntf_returnTest3, public CallbackBase
{
public:
    void
    ice_response(id<B> b)
    {
        r = b;
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }

    id<B> r;
};

typedef IceUtilHandle<AMI_Test_returnTest3I> AMI_Test_returnTest3IPtr;

@interface AMI_Test_paramTest3I : AMI_TestIntf_paramTest3, public CallbackBase
{
    void
    ice_response(id<B> ret, id<B> p1, id<B> p2)
    {
        test(p1);
        test(p1->sb == @"D2.sb (p1 1)");
        test(p1->pb == 0);
        test([p1 ice_id]:isEqualToString:@"::Test::B");

        test(p2);
        test(p2->sb == @"D2.sb (p2 1)");
        test(p2->pb == 0);
        test([p2 ice_id]:isEqualToString:@"::Test::B");

        test(ret);
        test(ret->sb == @"D1.sb (p2 2)");
        test(ret->pb == 0);
        test([ret ice_id]:isEqualToString:@"::Test::D1");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_paramTest3I> AMI_Test_paramTest3IPtr;

@interface AMI_Test_paramTest4I : AMI_TestIntf_paramTest4, public CallbackBase
{
    void
    ice_response(id<B> ret, id<B> b)
    {
        test(b);
        test(b->sb == @"D4.sb (1)");
        test(b->pb == 0);
        test([b ice_id]:isEqualToString:@"::Test::B");

        test(ret);
        test(ret->sb == @"B.sb (2)");
        test(ret->pb == 0);
        test([ret ice_id]:isEqualToString:@"::Test::B");
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_paramTest4I> AMI_Test_paramTest4IPtr;

@interface AMI_Test_sequenceTestI : AMI_TestIntf_sequenceTest, public CallbackBase
{
    void
    ice_response(SS* ss)
    {
        r = ss;
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }

public:

    SS r;
};

typedef IceUtilHandle<AMI_Test_sequenceTestI> AMI_Test_sequenceTestIPtr;

@interface AMI_Test_dictionaryTestI : AMI_TestIntf_dictionaryTest, public CallbackBase
{
    void
    ice_response(BDict* r, BDict* bout)
    {
        this->r = r;
        this->bout = bout;
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }

public:

    BDict bout;
    BDict r;
};

typedef IceUtilHandle<AMI_Test_dictionaryTestI> AMI_Test_dictionaryTestIPtr;

@interface AMI_Test_throwBaseAsBaseI : AMI_TestIntf_throwBaseAsBase, public CallbackBase
{
    void
    ice_response()
    {
        test(NO);
    }

    void
    ice_exception(ICEException* ex)
    {
        test(ex.ice_name() == @"TestBaseException");
        BaseException* e = dynamic_cast<BaseException*>(ex);
        test(e.sbe == @"sbe");
        test(e.pb);
        test(e.pb->sb == @"sb");
        test(e.pb->pb == e.pb);
        [self called];
    }
};

typedef IceUtilHandle<AMI_Test_throwBaseAsBaseI> AMI_Test_throwBaseAsBaseIPtr;

@interface AMI_Test_throwDerivedAsBaseI : AMI_TestIntf_throwDerivedAsBase, public CallbackBase
{
    void
    ice_response()
    {
        test(NO);
    }

    void
    ice_exception(ICEException* ex)
    {
        test(ex.ice_name() == @"TestDerivedException");
        DerivedException* e = dynamic_cast<DerivedException*>(ex);
        test(e.sbe == @"sbe");
        test(e.pb);
        test(e.pb->sb == @"sb1");
        test(e.pb->pb == e.pb);
        test(e.sde == @"sde1");
        test(e.pd1);
        test(e.pd1->sb == @"sb2");
        test(e.pd1->pb == e.pd1);
        test(e.pd1->sd1 == @"sd2");
        test(e.pd1->pd1 == e.pd1);
        [self called];
    }
};

typedef IceUtilHandle<AMI_Test_throwDerivedAsBaseI> AMI_Test_throwDerivedAsBaseIPtr;

@interface AMI_Test_throwDerivedAsDerivedI : AMI_TestIntf_throwDerivedAsDerived, public CallbackBase
{
    void
    ice_response()
    {
        test(NO);
    }

    void
    ice_exception(ICEException* ex)
    {
        test(ex.ice_name() == @"TestDerivedException");
        DerivedException* e = dynamic_cast<DerivedException*>(ex);
        test(e.sbe == @"sbe");
        test(e.pb);
        test(e.pb->sb == @"sb1");
        test(e.pb->pb == e.pb);
        test(e.sde == @"sde1");
        test(e.pd1);
        test(e.pd1->sb == @"sb2");
        test(e.pd1->pb == e.pd1);
        test(e.pd1->sd1 == @"sd2");
        test(e.pd1->pd1 == e.pd1);
        [self called];
    }
};

typedef IceUtilHandle<AMI_Test_throwDerivedAsDerivedI> AMI_Test_throwDerivedAsDerivedIPtr;

@interface AMI_Test_throwUnknownDerivedAsBaseI : AMI_TestIntf_throwUnknownDerivedAsBase, public CallbackBase
{
    void
    ice_response()
    {
        test(NO);
    }

    void
    ice_exception(ICEException* ex)
    {
        test(ex.ice_name() == @"TestBaseException");
        BaseException* e = dynamic_cast<BaseException*>(ex);
        test(e.sbe == @"sbe");
        test(e.pb);
        test(e.pb->sb == @"sb d2");
        test(e.pb->pb == e.pb);
        [self called];
    }
};

typedef IceUtilHandle<AMI_Test_throwUnknownDerivedAsBaseI> AMI_Test_throwUnknownDerivedAsBaseIPtr;

@interface AMI_Test_useForwardI : AMI_TestIntf_useForward, public CallbackBase
{
    void
    ice_response(id<Forward> f)
    {
        test(f);
        [self called];
    }

    void
    ice_exception(ICEException* exc)
    {
        test(NO);
    }
};

typedef IceUtilHandle<AMI_Test_useForwardI> AMI_Test_useForwardIPtr;

#endif

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

#if 0
    tprintf("base as Object (AMI)... ");
    {
        id<AMI_Test_SBaseAsObjectI> cb = [[AMI_Test_SBaseAsObjectI alloc] init];
        [test SBaseAsObject_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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
 
#if 0
    tprintf("base as base (AMI)... ");
    {
        id<AMI_Test_SBaseAsSBaseI> cb = [[AMI_Test_SBaseAsSBaseI alloc] init];
        [test SBaseAsSBase_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("base with known derived as base (AMI)... ");
    {
        id<AMI_Test_SBSKnownDerivedAsSBaseI> cb = [[AMI_Test_SBSKnownDerivedAsSBaseI alloc] init];
        [test SBSKnownDerivedAsSBase_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("base with known derived as known derived (AMI)... ");
    {
        id<AMI_Test_SBSKnownDerivedAsSBSKnownDerivedI> cb = [[AMI_Test_SBSKnownDerivedAsSBSKnownDerivedI alloc] init];
        [test SBSKnownDerivedAsSBSKnownDerived_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("base with unknown derived as base (AMI)... ");
    {
        id<AMI_Test_SBSUnknownDerivedAsSBaseI> cb = [[AMI_Test_SBSUnknownDerivedAsSBaseI alloc] init];
        [test SBSUnknownDerivedAsSBase_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("unknown with Object as Object (AMI)... ");
    {
        @try
        {
            id<AMI_Test_SUnknownAsObjectI> cb = [[AMI_Test_SUnknownAsObjectI alloc] init];
            [test SUnknownAsObject_async:cb];
            test([cb check]);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("one-element cycle (AMI)... ");
    {
        id<AMI_Test_oneElementCycleI> cb = [[AMI_Test_oneElementCycleI alloc] init];
        [test oneElementCycle_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("two-element cycle (AMI)... ");
    {
        id<AMI_Test_twoElementCycleI> cb = [[AMI_Test_twoElementCycleI alloc] init];
        [test twoElementCycle_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("known derived pointer slicing as base (AMI)... ");
    {
        id<AMI_Test_D1AsBI> cb = [[AMI_Test_D1AsBI alloc] init];
        [test D1AsB_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("known derived pointer slicing as derived (AMI)... ");
    {
        id<AMI_Test_D1AsD1I> cb = [[AMI_Test_D1AsD1I alloc] init];
        [test D1AsD1_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("unknown derived pointer slicing as base (AMI)... ");
    {
        id<AMI_Test_D2AsBI> cb = [[AMI_Test_D2AsBI alloc] init];
        [test D2AsB_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("param ptr slicing with known first (AMI)... ");
    {
        id<AMI_Test_paramTest1I> cb = [[AMI_Test_paramTest1I alloc] init];
        [test paramTest1_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("return value identity with known first (AMI)... ");
    {
        id<AMI_Test_returnTest1I> cb = [[AMI_Test_returnTest1I alloc] init];
        [test returnTest1_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("return value identity with unknown first (AMI)... ");
    {
        id<AMI_Test_returnTest2I> cb = [[AMI_Test_returnTest2I alloc] init];
        [test returnTest2_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("return value identity for input params known first (AMI)... ");
    {
        @try
        {
            id<D1> d1 = [[D1 alloc] init];
            d1->sb = @"D1.sb";
            d1->sd1 = @"D1.sd1";
            id<D3> d3 = [[D3 alloc] init];
            d3->pb = d1;
            d3->sb = @"D3.sb";
            d3->sd3 = @"D3.sd3";
            d3->pd3 = d1;
            d1->pb = d3;
            d1->pd1 = d3;

            id<AMI_Test_returnTest3I> cb = [[AMI_Test_returnTest3I alloc] init];
            [test returnTest3_async:cb XXX:d1 XXX:d3];
            test([cb check]);
            id<B> b1 = cb->r;

            test(b1);
            test(b1->sb == @"D1.sb");
            test([b1 ice_id]:isEqualToString:@"::Test::D1");
            id<D1> p1 = D1PtrdynamicCast(b1);
            test(p1);
            test(p1->sd1 == @"D1.sd1");
            test(p1->pd1 == b1->pb);

            id<B> b2 = b1->pb;
            test(b2);
            test(b2->sb == @"D3.sb");
            test([b2 ice_id]:isEqualToString:@"::Test::B");  // Sliced by server
            test(b2->pb == b1);
            id<D3> p3 = D3PtrdynamicCast(b2);
            test(!p3);

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
#endif

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

#if 0
    tprintf("return value identity for input params unknown first (AMI)... ");
    {
        @try
        {
            id<D1> d1 = [[D1 alloc] init];
            d1->sb = @"D1.sb";
            d1->sd1 = @"D1.sd1";
            id<D3> d3 = [[D3 alloc] init];
            d3->pb = d1;
            d3->sb = @"D3.sb";
            d3->sd3 = @"D3.sd3";
            d3->pd3 = d1;
            d1->pb = d3;
            d1->pd1 = d3;

            id<AMI_Test_returnTest3I> cb = [[AMI_Test_returnTest3I alloc] init];
            [test returnTest3_async:cb XXX:d3 XXX:d1];
            test([cb check]);
            id<B> b1 = cb->r;

            test(b1);
            test(b1->sb == @"D3.sb");
            test([b1 ice_id]:isEqualToString:@"::Test::B");  // Sliced by server
            id<D3> p1 = D3PtrdynamicCast(b1);
            test(!p1);

            id<B> b2 = b1->pb;
            test(b2);
            test(b2->sb == @"D1.sb");
            test([b2 ice_id]:isEqualToString:@"::Test::D1");
            test(b2->pb == b1);
            id<D1> p3 = D1PtrdynamicCast(b2);
            test(p3);
            test(p3->sd1 == @"D1.sd1");
            test(p3->pd1 == b1);

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
#endif

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

#if 0
    tprintf("remainder unmarshaling (3 instances) (AMI)... ");
    {
        id<AMI_Test_paramTest3I> cb = [[AMI_Test_paramTest3I alloc] init];
        [test paramTest3_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("remainder unmarshaling (4 instances) (AMI)... ");
    {
        id<B> b;
        id<AMI_Test_paramTest4I> cb = [[AMI_Test_paramTest4I alloc] init];
        [test paramTest4_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("param ptr slicing, instance marshaled in unknown derived as base (AMI)... ");
    {
        @try
        {
            id<B> b1 = [[B alloc] init];
            b1->sb = @"B.sb(1)";
            b1->pb = b1;

            id<D3> d3 = [[D3 alloc] init];
            d3->sb = @"D3.sb";
            d3->pb = d3;
            d3->sd3 = @"D3.sd3";
            d3->pd3 = b1;

            id<B> b2 = [[B alloc] init];
            b2->sb = @"B.sb(2)";
            b2->pb = b1;

            id<AMI_Test_returnTest3I> cb = [[AMI_Test_returnTest3I alloc] init];
            [test returnTest3_async:cb XXX:d3 XXX:b2];
            test([cb check]);
            id<B> r = cb->r;

            test(r);
            test([r ice_id]:isEqualToString:@"::Test::B");
            test(r->sb == @"D3.sb");
            test(r->pb == r);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");
#endif

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

#if 0
    cout << @"param ptr slicing, instance marshaled in unknown derived as derived (AMI)... "
         << flush;
    {
        @try
        {
            id<D1> d11 = [[D1 alloc] init];
            d11->sb = @"D1.sb(1)";
            d11->pb = d11;
            d11->sd1 = @"D1.sd1(1)";

            id<D3> d3 = [[D3 alloc] init];
            d3->sb = @"D3.sb";
            d3->pb = d3;
            d3->sd3 = @"D3.sd3";
            d3->pd3 = d11;

            id<D1> d12 = [[D1 alloc] init];
            d12->sb = @"D1.sb(2)";
            d12->pb = d12;
            d12->sd1 = @"D1.sd1(2)";
            d12->pd1 = d11;

            id<AMI_Test_returnTest3I> cb = [[AMI_Test_returnTest3I alloc] init];
            [test returnTest3_async:cb XXX:d3 XXX:d12];
            test([cb check]);
            id<B> r = cb->r;
            test(r);
            test([r ice_id]:isEqualToString:@"::Test::B");
            test(r->sb == @"D3.sb");
            test(r->pb == r);
        }
        @catch(...)
        {
            test(0);
        }
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("sequence slicing (AMI)... ");
    {
        @try
        {
            SS ss;
            {
                id<B> ss1b = [[B alloc] init];
                ss1b->sb = @"B.sb";
                ss1b->pb = ss1b;

                id<D1> ss1d1 = [[D1 alloc] init];
                ss1d1->sb = @"D1.sb";
                ss1d1->sd1 = @"D1.sd1";
                ss1d1->pb = ss1b;

                id<D3> ss1d3 = [[D3 alloc] init];
                ss1d3->sb = @"D3.sb";
                ss1d3->sd3 = @"D3.sd3";
                ss1d3->pb = ss1b;

                id<B> ss2b = [[B alloc] init];
                ss2b->sb = @"B.sb";
                ss2b->pb = ss1b;

                id<D1> ss2d1 = [[D1 alloc] init];
                ss2d1->sb = @"D1.sb";
                ss2d1->sd1 = @"D1.sd1";
                ss2d1->pb = ss2b;

                id<D3> ss2d3 = [[D3 alloc] init];
                ss2d3->sb = @"D3.sb";
                ss2d3->sd3 = @"D3.sd3";
                ss2d3->pb = ss2b;

                ss1d1->pd1 = ss2b;
                ss1d3->pd3 = ss2d1;

                ss2d1->pd1 = ss1d3;
                ss2d3->pd3 = ss1d1;

                id<SS1> ss1 = [[SS1 alloc] init];
                ss1->[s addObject:ss1b];
                ss1->[s addObject:ss1d1];
                ss1->[s addObject:ss1d3];

                id<SS2> ss2 = [[SS2 alloc] init];
                ss2->[s addObject:ss2b];
                ss2->[s addObject:ss2d1];
                ss2->[s addObject:ss2d3];

                id<AMI_Test_sequenceTestI> cb = [[AMI_Test_sequenceTestI alloc] init];
                [test sequenceTest_async:cb XXX:ss1 XXX:ss2];
                test([cb check]);
                ss = cb->r;
            }

            test(ss.c1);
            id<B> ss1b = ss.c1->[s objectAtIndex:0];
            id<B> ss1d1 = ss.c1->[s objectAtIndex:1];
            test(ss.c2);
            id<B> ss1d3 = ss.c1->[s objectAtIndex:2];

            test(ss.c2);
            id<B> ss2b = ss.c2->[s objectAtIndex:0];
            id<B> ss2d1 = ss.c2->[s objectAtIndex:1];
            id<B> ss2d3 = ss.c2->[s objectAtIndex:2];

            test(ss1b->pb == ss1b);
            test(ss1d1->pb == ss1b);
            test(ss1d3->pb == ss1b);

            test(ss2b->pb == ss1b);
            test(ss2d1->pb == ss2b);
            test(ss2d3->pb == ss2b);

            test([ss1b ice_id]:isEqualToString:@"::Test::B");
            test([ss1d1 ice_id]:isEqualToString:@"::Test::D1");
            test([ss1d3 ice_id]:isEqualToString:@"::Test::B");

            test([ss2b ice_id]:isEqualToString:@"::Test::B");
            test([ss2d1 ice_id]:isEqualToString:@"::Test::D1");
            test([ss2d3 ice_id]:isEqualToString:@"::Test::B");
        }
        @catch(ICEException*)
        {
            test(0);
        }
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("dictionary slicing (AMI)... ");
    {
        @try
        {
            BDict bin;
            BDict bout;
            BDict r;
            int i;
            for(i = 0; i < 10; ++i)
            {
                ostringstream s;
                s << @"D1." << i;
                id<D1> d1 = [[D1 alloc] init];
                d1->sb = s.str();
                d1->pb = d1;
                d1->sd1 = s.str();
                [bin objectForKey:i] = d1;
            }

            id<AMI_Test_dictionaryTestI> cb = [[AMI_Test_dictionaryTestI alloc] init];
            [test dictionaryTest_async:cb XXX:bin];
            test([cb check]);
            bout = cb->bout;
            r = cb->r;

            test([bout count] == 10);
            for(i = 0; i < 10; ++i)
            {
                id<B> b = [bout objectForKey:i * 10];
                test(b);
                stdostringstream s;
                s << @"D1." << i;
                test(b->sb == s.str());
                test(b->pb);
                test(b->pb != b);
                test(b->pb->sb == s.str());
                test(b->pb->pb == b->pb);
            }

            test([r count] == 10);
            for(i = 0; i < 10; ++i)
            {
                id<B> b = [r objectForKey:i * 20];
                test(b);
                stdostringstream s;
                s << @"D1." << i * 20;
                test(b->sb == s.str());
                test(b->pb == (i == 0 ? BPtr(0) : [r objectForKey:(i - 1) * 20]));
                id<D1> d1 = D1PtrdynamicCast(b);
                test(d1);
                test(d1->sd1 == s.str());
                test(d1->pd1 == d1);
            }
        }
        @catch(ICEException*)
        {
            test(0);
        }
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("base exception thrown as base exception (AMI)... ");
    {
        id<AMI_Test_throwBaseAsBaseI> cb = [[AMI_Test_throwBaseAsBaseI alloc] init];
        [test throwBaseAsBase_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("derived exception thrown as base exception (AMI)... ");
    {
        id<AMI_Test_throwDerivedAsBaseI> cb = [[AMI_Test_throwDerivedAsBaseI alloc] init];
        [test throwDerivedAsBase_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("derived exception thrown as derived exception (AMI)... ");
    {
        id<AMI_Test_throwDerivedAsDerivedI> cb = [[AMI_Test_throwDerivedAsDerivedI alloc] init];
        [test throwDerivedAsDerived_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("unknown derived exception thrown as base exception (AMI)... ");
    {
        id<AMI_Test_throwUnknownDerivedAsBaseI> cb = [[AMI_Test_throwUnknownDerivedAsBaseI alloc] init];
        [test throwUnknownDerivedAsBase_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

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

#if 0
    tprintf("forward-declared class (AMI)... ");
    {
        id<AMI_Test_useForwardI> cb = [[AMI_Test_useForwardI alloc] init];
        [test useForward_async:cb];
        test([cb check]);
    }
    tprintf("ok\n");
#endif

    return test;
}
