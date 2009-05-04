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


id<TestObjectsInitialPrx>
allTests(id<ICECommunicator> communicator, BOOL collocated)
{
    tprintf("testing stringToProxy... ");
    NSString* ref = @"initial:default -p 12010";
    id<ICEObjectPrx> base = [communicator stringToProxy:ref];
    test(base);
    tprintf("ok\n");

    tprintf("testing checked cast... ");
    id<TestObjectsInitialPrx> initial = [TestObjectsInitialPrx checkedCast:base];
    test(initial);
    test([initial isEqual:base]);
    tprintf("ok\n");

    tprintf("testing constructor, convenience constructor, and copy... ");

    TestObjectsBase* ba1 = [[[TestObjectsBase alloc] init] autorelease];
    test(ba1.theS.str == nil);
    test(ba1.str == nil);

    TestObjectsS* s = [TestObjectsS s];
    s.str = @"hello";
    TestObjectsBase* ba2 = [TestObjectsBase base:s str:@"hi"];
    test(ba2.theS == s);
    test([ba2.str isEqualToString:@"hi"]);

    TestObjectsBase* ba3 = [ba2 copy];
    test(ba3 != ba2);
    test(ba3.theS == ba2.theS);
    test(ba3.str == ba2.str);

#if 0
    // Can't override assignment operator in Objective-C.
    *ba1 = *ba2;
    test([ba1.theS.str isEqualToString:@"hello"]);
    test([ba1.str isEqualToString:@"hi"]);
    
    TestObjectsBase* bp1 = [TestObjectsBase base];
    *bp1 = *ba2;
    test([bp1.theS.str isEqualToString:@"hello"]);
    test([bp1.str isEqualToString:@"hi"]);
#endif

    tprintf("ok\n");

    tprintf("getting B1... ");
    TestObjectsB* b1 = [initial getB1];
    test(b1);
    tprintf("ok\n");
    
    tprintf("getting B2... ");
    TestObjectsB* b2 = [initial getB2];
    test(b2);
    tprintf("ok\n");
    
    tprintf("getting C... ");
    TestObjectsC* c = [initial getC];
    test(c);
    tprintf("ok\n");
    
    tprintf("getting D... ");
    TestObjectsD* d = [initial getD];
    test(d);
    tprintf("ok\n");
    
    tprintf("checking consistency... ");
    test(b1 != b2);
    test(b1 != (TestObjectsB*)c);
    test(b1 != (TestObjectsB*)d);
    test(b2 != (TestObjectsB*)c);
    test(b2 != (TestObjectsB*)d);
    test(c != (TestObjectsC*)d);
    test(b1.theB == b1);
    test(b1.theC == 0);
    test([b1.theA isKindOfClass:[TestObjectsB class]]);
    test(((TestObjectsB*)b1.theA).theA == b1.theA);
    test(((TestObjectsB*)b1.theA).theB == b1);
    test([((TestObjectsB*)b1.theA).theC isKindOfClass:[TestObjectsC class]]);
    test(((TestObjectsC*)((TestObjectsB*)b1.theA).theC).theB == b1.theA);
    test(b1.preMarshalInvoked);
    test([(id<TestObjectsB>)b1 postUnmarshalInvoked:nil]);
    test(b1.theA.preMarshalInvoked);
    test([(id<TestObjectsA>)b1.theA postUnmarshalInvoked:nil]);
    test(((TestObjectsB*)b1.theA).theC.preMarshalInvoked);
    test([(id<TestObjectsC>)((TestObjectsB*)b1.theA).theC postUnmarshalInvoked:nil]);

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
    test(b1 != (TestObjectsB*)c);
    test(b1 != (TestObjectsB*)d);
    test(b2 != (TestObjectsB*)c);
    test(b2 != (TestObjectsB*)d);
    test(c != (TestObjectsC*)d);
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
    test([(id<TestObjectsD>)d postUnmarshalInvoked:nil]);
    test(d.theA.preMarshalInvoked);
    test([(id<TestObjectsA>)d.theA postUnmarshalInvoked:nil]);
    test(d.theB.preMarshalInvoked);
    test([(id<TestObjectsA>)d.theB postUnmarshalInvoked:nil]);
    test(d.theB.theC.preMarshalInvoked);
    test([(id<TestObjectsC>)d.theB.theC postUnmarshalInvoked:nil]);
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
    TestObjectsE* e = [initial getE];
    test([(id<TestObjectsE>)e checkValues:nil]);
    TestObjectsF* f = [initial getF];
    test([(id<TestObjectsF>)f checkValues:nil]);
    test([(id<TestObjectsE>)f.e2 checkValues:nil]);
    tprintf("ok\n");

    tprintf("getting I, J and H... ");
    TestObjectsI* i = (TestObjectsI*)[initial getI];
    test(i);
    TestObjectsI* j = (TestObjectsI*)[initial getJ];
    test(j && [j isKindOfClass:[TestObjectsJ class]]);
    TestObjectsI* h = (TestObjectsI*)[initial getH];
    test(h && [h isKindOfClass:[TestObjectsH class]]);
    tprintf("ok\n");

    tprintf("setting I... ");
    [initial setI:i];
    [initial setI:j];
    [initial setI:h];
    tprintf("ok\n");

//     if(!collocated)
//     {
    tprintf("testing UnexpectedObjectException... ");
    ref = @"uoet:default -p 12010";
    base = [communicator stringToProxy:ref];
    test(base);
    id<TestObjectsUnexpectedObjectExceptionTestPrx> uoet = [TestObjectsUnexpectedObjectExceptionTestPrx uncheckedCast:base];
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
    // TestObjectss specific to Objective-C.
    //
    {
	tprintf("setting Object sequence... ");
	TestObjectsMutableObjectSeq* seq = [TestObjectsMutableObjectSeq array];

	[seq addObject:[NSNull null]];

	TestObjectsBase* b = [TestObjectsBase base];
	b.theS = [TestObjectsS s];
	b.theS.str = @"Hello";
	b.str = @"World";
	[seq addObject:b];

	@try
	{
	    TestObjectsObjectSeq* r = [initial getObjectSeq:seq];
	    test([r objectAtIndex:0 == [NSNull null]]);
	    TestObjectsBase* br = [r objectAtIndex:1];
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
	tprintf("setting Object proxy sequence... ");
	TestObjectsMutableObjectPrxSeq* seq = [TestObjectsMutableObjectPrxSeq array];

	[seq addObject:[NSNull null]];
	[seq addObject:initial];

	@try
	{
	    TestObjectsObjectPrxSeq* r = [initial getObjectPrxSeq:seq];
	    test([r objectAtIndex:0 == [NSNull null]]);
	    test([[r objectAtIndex:1] isEqual:initial]);
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
	tprintf("setting Base sequence... ");
	TestObjectsMutableBaseSeq* seq = [TestObjectsMutableBaseSeq array];

	[seq addObject:[NSNull null]];

	TestObjectsBase* b = [[[TestObjectsBase alloc] init] autorelease];
	b.theS = [TestObjectsS s];
	b.theS.str = @"Hello";
	b.str = @"World";
	[seq addObject:b];

	@try
	{
	    TestObjectsBaseSeq* r = [initial getBaseSeq:seq];
	    test([r objectAtIndex:0 == [NSNull null]]);
	    TestObjectsBase* br = [r objectAtIndex:1];
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
	tprintf("setting Base proxy sequence... ");
	TestObjectsMutableBasePrxSeq* seq = [TestObjectsMutableBasePrxSeq array];

	[seq addObject:[NSNull null]];
	NSString* ref = @"base:default -p 12010";
	id<ICEObjectPrx> base = [communicator stringToProxy:ref];
	id<TestObjectsBasePrx> b = [TestObjectsBasePrx uncheckedCast:base];
	[seq addObject:b];

	@try
	{
	    TestObjectsBasePrxSeq* r = [initial getBasePrxSeq:seq];
	    test([r objectAtIndex:0 == [NSNull null]]);
	    test([[r objectAtIndex:1] isEqual:b]);
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
	tprintf("setting Object dictionary... ");
	TestObjectsMutableObjectDict* dict = [TestObjectsMutableObjectDict dictionary];

	[dict setObject:[NSNull null] forKey:@"zero"];

	TestObjectsBase* b = [TestObjectsBase base];
	b.theS = [TestObjectsS s];
	b.theS.str = @"Hello";
	b.str = @"World";
	[dict setObject:b forKey:@"one"];

	@try
	{
	    TestObjectsObjectDict* r = [initial getObjectDict:dict];
	    test([r objectForKey:@"zero"] == [NSNull null]);
	    TestObjectsBase* br = [r objectForKey:@"one"];
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
	tprintf("setting Object proxy dictionary... ");
	TestObjectsMutableObjectPrxDict* dict = [TestObjectsMutableObjectPrxDict dictionary];

	[dict setObject:[NSNull null] forKey:@"zero"];
	NSString* ref = @"object:default -p 12010";
	id<ICEObjectPrx> o = [communicator stringToProxy:ref];
	[dict setObject:o forKey:@"one"];

	@try
	{
	    TestObjectsObjectPrxDict* r = [initial getObjectPrxDict:dict];
	    test([r objectForKey:@"zero"] == [NSNull null]);
	    test([[r objectForKey:@"one"] isEqual:o]);
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
	tprintf("setting Base dictionary... ");
	TestObjectsMutableBaseDict* dict = [TestObjectsMutableBaseDict dictionary];

	[dict setObject:[NSNull null] forKey:@"zero"];

	TestObjectsBase* b = [TestObjectsBase base];
	b.theS = [TestObjectsS s];
	b.theS.str = @"Hello";
	b.str = @"World";
	[dict setObject:b forKey:@"one"];

	@try
	{
	    TestObjectsBaseDict* r = [initial getBaseDict:dict];
	    test([r objectForKey:@"zero"] == [NSNull null]);
	    TestObjectsBase* br = [r objectForKey:@"one"];
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
	tprintf("setting Base proxy dictionary... ");
	TestObjectsMutableBasePrxDict* dict = [TestObjectsMutableBasePrxDict dictionary];

	[dict setObject:[NSNull null] forKey:@"zero"];
	NSString* ref = @"base:default -p 12010";
	id<ICEObjectPrx> base = [communicator stringToProxy:ref];
	id<TestObjectsBasePrx> b = [TestObjectsBasePrx uncheckedCast:base];
	[dict setObject:b forKey:@"one"];

	@try
	{
	    TestObjectsObjectPrxDict* r = [initial getObjectPrxDict:dict];
	    test([r objectForKey:@"zero"] == [NSNull null]);
	    test([[r objectForKey:@"one"] isEqual:b]);
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
