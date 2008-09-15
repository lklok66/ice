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


id<TestInitialPrx>
allTests(id<ICECommunicator> communicator, BOOL collocated)
{
    tprintf("testing stringToProxy... ");
    NSString* ref = @"initial:default -p 12010 -t 10000";
    id<ICEObjectPrx> base = [communicator stringToProxy:ref];
    test(base);
    tprintf("ok\n");

    tprintf("testing checked cast... ");
    id<TestInitialPrx> initial = [TestInitialPrx checkedCast:base];
    test(initial);
    test([initial isEqual:base]);
    tprintf("ok\n");

    tprintf("testing constructor, copy constructor, and assignment operator... ");

    TestBase* ba1 = [[[TestBase alloc] init] autorelease];
    test(ba1.theS.str == nil);
    test(ba1.str == nil);

    TestS* s = [TestS s];
    s.str = @"hello";
    TestBase* ba2 = [TestBase base:s str:@"hi"];
    test([ba2.theS.str isEqualToString:@"hello"]);
    test([ba2.str isEqualToString:@"hi"]);

    *ba1 = *ba2;
    test([ba1.theS.str isEqualToString:@"hello"]);
    test([ba1.str isEqualToString:@"hi"]);
    
    TestBase* bp1 = [TestBase base];
    *bp1 = *ba2;
    test([bp1.theS.str isEqualToString:@"hello"]);
    test([bp1.str isEqualToString:@"hi"]);

    tprintf("ok\n");

    tprintf("getting B1... ");
    TestB* b1 = [initial getB1];
    test(b1);
    tprintf("ok\n");
    
    tprintf("getting B2... ");
    TestB* b2 = [initial getB2];
    test(b2);
    tprintf("ok\n");
    
    tprintf("getting C... ");
    TestC* c = [initial getC];
    test(c);
    tprintf("ok\n");
    
    tprintf("getting D... ");
    TestD* d = [initial getD];
    test(d);
    tprintf("ok\n");
    
    tprintf("checking consistency... ");
    test(b1 != b2);
    test(b1 != (TestB*)c);
    test(b1 != (TestB*)d);
    test(b2 != (TestB*)c);
    test(b2 != (TestB*)d);
    test(c != (TestC*)d);
    test(b1.theB == b1);
    test(b1.theC == 0);
    test([b1.theA isKindOfClass:[TestB class]]);
    test(((TestB*)b1.theA).theA == b1.theA);
    test(((TestB*)b1.theA).theB == b1);
    test([((TestB*)b1.theA).theC isKindOfClass:[TestC class]]);
    test(((TestC*)((TestB*)b1.theA).theC).theB == b1.theA);
    test(b1.preMarshalInvoked);
    test([(id<TestB>)b1 postUnmarshalInvoked:nil]);
    test(b1.theA.preMarshalInvoked);
    test([(id<TestA>)b1.theA postUnmarshalInvoked:nil]);
    test(((TestB*)b1.theA).theC.preMarshalInvoked);
    test([(id<TestC>)((TestB*)b1.theA).theC postUnmarshalInvoked:nil]);

    // More tests possible for b2 and d, but I think this is already sufficient.
    test(b2.theA == b2);
    test(d.theC == 0);

    b1.theA = nil; // Break cyclic reference.
    b1.theB = nil; // Break cyclic reference.

    b2.theA = nil; // Break cyclic reference.
    b2.theB = nil; // Break cyclic reference.
    b2.theC = nil; // Break cyclic reference.

    c.theB = nil; // Break cyclic reference.

    d.theA = nil; // Break cyclic reference.
    d.theB = nil; // Break cyclic reference.
    d.theC = nil; // Break cyclic reference.

    tprintf("ok\n");

    tprintf("getting B1, B2, C, and D all at once... ");
    [initial getAll:&b1 b2:&b2 theC:&c theD:&d];
    test(b1);
    test(b2);
    test(c);
    test(d);
    tprintf("ok\n");
    
    tprintf("checking consistency... ");
    test(b1 != b2);
    test(b1 != (TestB*)c);
    test(b1 != (TestB*)d);
    test(b2 != (TestB*)c);
    test(b2 != (TestB*)d);
    test(c != (TestC*)d);
    test(b1.theA == b2);
    test(b1.theB == b1);
    test(b1.theC == 0);
    test(b2.theA == b2);
    test(b2.theB == b1);
    test(b2.theC == c);
    test(c.theB == b2);
    test(d.theA == b1);
    test(d.theB == b2);
    test(d.theC == 0);
//    if(!collocated)
//    {
    test(d.preMarshalInvoked);
    test([(id<TestD>)d postUnmarshalInvoked:nil]);
    test(d.theA.preMarshalInvoked);
    test([(id<TestA>)d.theA postUnmarshalInvoked:nil]);
    test(d.theB.preMarshalInvoked);
    test([(id<TestA>)d.theB postUnmarshalInvoked:nil]);
    test(d.theB.theC.preMarshalInvoked);
    test([(id<TestC>)d.theB.theC postUnmarshalInvoked:nil]);
//    }

    b1.theA = nil; // Break cyclic reference.
    b1.theB = nil; // Break cyclic reference.

    b2.theA = nil; // Break cyclic reference.
    b2.theB = nil; // Break cyclic reference.
    b2.theC = nil; // Break cyclic reference.

    c.theB = nil; // Break cyclic reference.

    d.theA = nil; // Break cyclic reference.
    d.theB = nil; // Break cyclic reference.
    d.theC = nil; // Break cyclic reference.

    tprintf("ok\n");

    tprintf("testing protected members... ");
    TestE* e = [initial getE];
    test([(id<TestE>)e checkValues:nil]);
    TestF* f = [initial getF];
    test([(id<TestF>)f checkValues:nil]);
    test([(id<TestE>)f.e2 checkValues:nil]);
    tprintf("ok\n");

    tprintf("getting I, J and H... ");
    TestI* i = (TestI*)[initial getI];
    test(i);
    TestI* j = (TestI*)[initial getJ];
    test(j && [j isKindOfClass:[TestJ class]]);
    TestI* h = (TestI*)[initial getH];
    test(h && [h isKindOfClass:[TestH class]]);
    tprintf("ok\n");

    tprintf("setting I... ");
    [initial setI:i];
    [initial setI:j];
    [initial setI:h];
    tprintf("ok\n");

//     if(!collocated)
//     {
    tprintf("testing UnexpectedObjectException... ");
    ref = @"uoet:default -p 12010 -t 10000";
    base = [communicator stringToProxy:ref];
    test(base);
    id<TestUnexpectedObjectExceptionTestPrx> uoet = [TestUnexpectedObjectExceptionTestPrx uncheckedCast:base];
    test(uoet);
    @try
    {
        [uoet op];
        test(NO);
    }
    @catch(ICEUnexpectedObjectException* ex)
    {
        test([ex.type isEqualToString:@"::Test::AlsoEmpty"]);
        test([ex.expectedType isEqualToString:@"::Test::Empty"]);
    }
    @catch(ICEException* ex)
    {
        test(NO);
    }
    tprintf("ok\n");
//     }

    //
    // Tests specific to Objective-C.
    //
    {
	tprintf("setting object sequence... ");
	TestMutableObjectSeq* seq = [TestMutableObjectSeq array];

	[seq addObject:[NSNull null]];

	TestBase* b = [TestBase base];
	b.theS = [TestS s];
	b.theS.str = @"Hello";
	b.str = @"World";
	[seq addObject:b];

	@try
	{
	    TestObjectSeq* r = [initial getObjectSeq:seq];
	    test([r objectAtIndex:0 == [NSNull null]]);
	    TestBase* br = [r objectAtIndex:1];
	    test([br.theS.str isEqualToString:@"Hello"]);
	    test([br.str isEqualToString:@"World"]);
	}
	@catch(ICEOperationNotExistException*)
	{
	    // Expected if we are testing against a non-Objective-C server.
	}
	@catch(...)
	{
	    test(NO);
	}
	tprintf("ok\n");
    }

    {
	tprintf("setting object dictionary... ");
	TestMutableObjectDict* dict = [TestMutableObjectDict dictionary];

	[dict setObject:[NSNull null] forKey:@"zero"];

	TestBase* b = [TestBase base];
	b.theS = [TestS s];
	b.theS.str = @"Hello";
	b.str = @"World";
	[dict setObject:b forKey:@"one"];

	@try
	{
	    TestObjectDict* r = [initial getObjectDict:dict];
	    test([r objectForKey:@"zero"] == [NSNull null]);
	    TestBase* br = [r objectForKey:@"one"];
	    test([br.theS.str isEqualToString:@"Hello"]);
	    test([br.str isEqualToString:@"World"]);
	}
	@catch(ICEOperationNotExistException*)
	{
	    // Expected if we are testing against a non-Objective-C server.
	}
	@catch(...)
	{
	    test(NO);
	}
	tprintf("ok\n");
    }

    return initial;
}
