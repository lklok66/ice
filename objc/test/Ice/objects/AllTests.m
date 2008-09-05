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

    // TODO
//    *ba1 = *ba2;
//    test([ba1.theS.str isEqualToString:@"hello"]);
//    test([ba1.str isEqualToString:@"hi"]);

//    TestBase* bp1 = [TestBase base];
//    *bp1 = *ba2;
//    test([bp1.theS.str isEqualToString:@"hello"]);
//    test([bp1.str isEqualToString:@"hi"]);

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
//    test(b1 != c);
//    test(b1 != d);
//    test(b2 != c);
//    test(b2 != d);
//    test(c != d);
    test(b1.theB == b1);
    test(b1.theC == 0);
    test([b1.theA isKindOfClass:[TestB class]]);
    test(((TestB*)b1.theA).theA == b1.theA);
    test(((TestB*)b1.theA).theB == b1);
    test([((TestB*)b1.theA).theC isKindOfClass:[TestC class]]);
    test(((TestC*)((TestB*)b1.theA).theC).theB == b1.theA);
//         test(b1.preMarshalInvoked);
//         test([b1 postUnmarshalInvoked]);
//         test(b1.theA.preMarshalInvoked);
//         test(b1.[theA postUnmarshalInvoked]);
//         test(BPtrdynamicCast(b1.theA).theC.preMarshalInvoked);
//         test(BPtrdynamicCast(b1.theA).[theC postUnmarshalInvoked]);

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
//    test(b1 != c);
//    test(b1 != d);
//    test(b2 != c);
//    test(b2 != d);
//    test(c != d);
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
//     if(!collocated)
//     {
//         test(d.preMarshalInvoked);
//         test([d postUnmarshalInvoked]);
//         test(d.theA.preMarshalInvoked);
//         test(d.[theA postUnmarshalInvoked]);
//         test(d.theB.preMarshalInvoked);
//         test(d.[theB postUnmarshalInvoked]);
//         test(d.theB.theC.preMarshalInvoked);
//         test(d.theB.[theC postUnmarshalInvoked]);
//     }

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

//    tprintf("testing protected members... ");
//    TestE* e = [initial getE];
//    test([e checkValues]);
//    TestF* f = [initial getF];
//    test([f checkValues]);
//    test(f.[e2 checkValues]);
//    tprintf("ok\n");

    tprintf("getting I, J and H... ");
    TestI* i = [initial getI];
    test(i);
    TestI* j = [initial getJ];
    test(j && [j isKindOfClass:[TestJ class]]);
    TestI* h = [initial getH];
    test(h && [h isKindOfClass:[TestH class]]);
    tprintf("ok\n");

    tprintf("setting I... ");
    [initial setI:i];
    [initial setI:j];
    [initial setI:h];
    tprintf("ok\n");

//     if(!collocated)
//     {
//         tprintf("testing UnexpectedObjectException... ");
//         ref = @"uoet:default -p 12010 -t 10000";
//         base = [communicator stringToProxy:ref];
//         test(base);
//         id<UnexpectedObjectExceptionTestPrx> uoet = [UnexpectedObjectExceptionTestPrx uncheckedCast:base];
//         test(uoet);
//         @try
//         {
//             [uoet op];
//             test(NO);
//         }
//         @catch(ICEUnexpectedObjectException* ex)
//         {
//             test(ex.type == @"TestAlsoEmpty");
//             test(ex.expectedType == @"TestEmpty");
//         }
//         @catch(ICEException* ex)
//         {
//             cout << ex << endl;
//             test(NO);
//         }
//         catch(...)
//         {
//             test(NO);
//         }
//         tprintf("ok\n");
//     }

    return initial;
}
