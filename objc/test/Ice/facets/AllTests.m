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


@interface EmptyI : TestEmpty
@end

@implementation EmptyI
@end

id<TestGPrx>
allTests(id<ICECommunicator> communicator)
{
//     tprintf("testing Ice.Admin.Facets property... ");
//     test([[[communicator getProperties] getPropertyAsList:@"Ice.Admin.Facets"] length] > 0);
//     [[communicator getProperties] setProperty:@"Ice.Admin.Facets" value:@"foobar"];
//     ICEStringSeq facetFilter = [[communicator getProperties] getPropertyAsList:@"Ice.Admin.Facets"];
//     test([[facetFilter count] == 1 && [facetFilter objectAtIndex:0] isEqualToString:@"foobar"]);
//     [[communicator getProperties] setProperty:@"Ice.Admin.Facets" value:@"foo'bar"];
//     facetFilter = [[communicator getProperties] getPropertyAsList:@"Ice.Admin.Facets"];
//     test([[facetFilter count] == 1 && [facetFilter objectAtIndex:0] isEqualToString:@"foo'bar"]);
//     [[communicator getProperties] setProperty:@"Ice.Admin.Facets" value:@"'foo bar' toto 'titi'"];
//     facetFilter = [[communicator getProperties] getPropertyAsList:@"Ice.Admin.Facets"];
//     test([[facetFilter count] == 3 && [facetFilter objectAtIndex:0] isEqualToString:@"foo bar" && [facetFilter objectAtIndex:1]:isEqualToString:@"toto" && [facetFilter objectAtIndex:2]:isEqualToString:@"titi"]);
//     [[communicator getProperties] setProperty:@"Ice.Admin.Facets" value:@"'foo bar\\' toto' 'titi'"];
//     facetFilter = [[communicator getProperties] getPropertyAsList:@"Ice.Admin.Facets"];
//     test([[facetFilter count] == 2 && [facetFilter objectAtIndex:0] isEqualToString:@"foo bar' toto" && [facetFilter objectAtIndex:1]:isEqualToString:@"titi"]);
//     // [[communicator getProperties] setProperty:@"Ice.Admin.Facets" value:@"'foo bar' 'toto titi"];
//     // facetFilter = [[communicator getProperties] getPropertyAsList:@"Ice.Admin.Facets"];
//     // test([facetFilter count] == 0);
//     [[communicator getProperties] setProperty:@"Ice.Admin.Facets" value:@""];
//     tprintf("ok\n");

    tprintf("testing facet registration exceptions... ");
    [[communicator getProperties] setProperty:@"FacetExceptionTestAdapter.Endpoints" value:@"default"];
    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"FacetExceptionTestAdapter"];
    id<ICEObject> obj = [[EmptyI alloc] init];
    [adapter add:obj identity:[communicator stringToIdentity:@"d"]];
    [adapter addFacet:obj identity:[communicator stringToIdentity:@"d"] facet:@"facetABCD"];
    @try
    {
        [adapter addFacet:obj identity:[communicator stringToIdentity:@"d"] facet:@"facetABCD"];
        test(NO);
    }
    @catch(ICEAlreadyRegisteredException*)
    {
    }
    [adapter removeFacet:[communicator stringToIdentity:@"d"] facet:@"facetABCD"];
    @try
    {
        [adapter removeFacet:[communicator stringToIdentity:@"d"] facet:@"facetABCD"];
        test(NO);
    }
    @catch(ICENotRegisteredException*)
    {
    }
    tprintf("ok\n");

    tprintf("testing removeAllFacets... ");
    id<ICEObject> obj1 = [[EmptyI alloc] init];
    id<ICEObject> obj2 = [[EmptyI alloc] init];
    [adapter addFacet:obj1 identity:[communicator stringToIdentity:@"id1"] facet:@"f1"];
    [adapter addFacet:obj2 identity:[communicator stringToIdentity:@"id1"] facet:@"f2"];
    id<ICEObject> obj3 = [[EmptyI alloc] init];
    [adapter addFacet:obj1 identity:[communicator stringToIdentity:@"id2"] facet:@"f1"];
    [adapter addFacet:obj2 identity:[communicator stringToIdentity:@"id2"] facet:@"f2"];
    [adapter addFacet:obj3 identity:[communicator stringToIdentity:@"id2"] facet:@""];
    NSDictionary* fm = [adapter removeAllFacets:[communicator stringToIdentity:@"id1"]];
    test([fm count] == 2);
    test([fm objectForKey:@"f1"] == obj1);
    test([fm objectForKey:@"f2"] == obj2);
    @try
    {
        [adapter removeAllFacets:[communicator stringToIdentity:@"id1"]];
        test(NO);
    }
    @catch(ICENotRegisteredException*)
    {
    }
    fm = [adapter removeAllFacets:[communicator stringToIdentity:@"id2"]];
    test([fm count] == 3);
    test([fm objectForKey:@"f1"] == obj1);
    test([fm objectForKey:@"f2"] == obj2);
    test([fm objectForKey:@""] == obj3);
    tprintf("ok\n");

    [adapter deactivate];

    tprintf("testing stringToProxy... ");
    NSString* ref = @"d:default -p 12010 -t 10000";
    id<ICEObjectPrx> db = [communicator stringToProxy:ref];
    test(db);
    tprintf("ok\n");

    tprintf("testing unchecked cast... ");
    id<ICEObjectPrx> prx = [ICEObjectPrx uncheckedCast:db];
    test([[prx ice_getFacet] length] == 0);
    prx = [ICEObjectPrx uncheckedCast:db facet:@"facetABCD"];
    test([[prx ice_getFacet] isEqualToString:@"facetABCD"]);
    id<ICEObjectPrx> prx2 = [ICEObjectPrx uncheckedCast:prx];
    test([[prx2 ice_getFacet] isEqualToString:@"facetABCD"]);
    id<ICEObjectPrx> prx3 = [ICEObjectPrx uncheckedCast:prx facet:@""];
    test([[prx3 ice_getFacet] length] == 0);
    id<TestDPrx> d = [TestDPrx uncheckedCast:db];
    test([[d ice_getFacet] length] == 0);
    id<TestDPrx> df = [TestDPrx uncheckedCast:db facet:@"facetABCD"];
    test([[df ice_getFacet] isEqualToString:@"facetABCD"]);
    id<TestDPrx> df2 = [TestDPrx uncheckedCast:df];
    test([[df2 ice_getFacet] isEqualToString:@"facetABCD"]);
    id<TestDPrx> df3 = [TestDPrx uncheckedCast:df facet:@""];
    test([[df3 ice_getFacet] length] == 0);
    tprintf("ok\n");

    tprintf("testing checked cast... ");
    prx = [ICEObjectPrx checkedCast:db];
    test([[prx ice_getFacet] length] == 0);
    prx = [ICEObjectPrx checkedCast:db facet:@"facetABCD"];
    test([[prx ice_getFacet] isEqualToString:@"facetABCD"]);
    prx2 = [ICEObjectPrx checkedCast:prx];
    test([[prx2 ice_getFacet] isEqualToString:@"facetABCD"]);
    prx3 = [ICEObjectPrx checkedCast:prx facet:@""];
    test([[prx3 ice_getFacet] length] == 0);
    d = [TestDPrx checkedCast:db];
    test([[d ice_getFacet] length] == 0);
    df = [TestDPrx checkedCast:db facet:@"facetABCD"];
    test([[df ice_getFacet] isEqualToString:@"facetABCD"]);
    df2 = [TestDPrx checkedCast:df];
    test([[df2 ice_getFacet] isEqualToString:@"facetABCD"]);
    df3 = [TestDPrx checkedCast:df facet:@""];
    test([[df3 ice_getFacet] length] == 0);
    tprintf("ok\n");

    tprintf("testing non-facets A, B, C, and D... ");
    d = [TestDPrx checkedCast:db];
    test(d);
    test([d isEqual:db]);
    test([[d callA] isEqualToString:@"A"]);
    test([[d callB] isEqualToString:@"B"]);
    test([[d callC] isEqualToString:@"C"]);
    test([[d callD] isEqualToString:@"D"]);
    tprintf("ok\n");

    tprintf("testing facets A, B, C, and D... ");
    df = [TestDPrx checkedCast:d facet:@"facetABCD"];
    test(df);
    test([[df callA] isEqualToString:@"A"]);
    test([[df callB] isEqualToString:@"B"]);
    test([[df callC] isEqualToString:@"C"]);
    test([[df callD] isEqualToString:@"D"]);
    tprintf("ok\n");

    tprintf("testing facets E and F... ");
    id<TestFPrx> ff = [TestFPrx checkedCast:d facet:@"facetEF"];
    test(ff);
    test([[ff callE] isEqualToString:@"E"]);
    test([[ff callF] isEqualToString:@"F"]);
    tprintf("ok\n");

    tprintf("testing facet G... ");
    id<TestGPrx> gf = [TestGPrx checkedCast:ff facet:@"facetGH"];
    test(gf);
    test([[gf callG] isEqualToString:@"G"]);
    tprintf("ok\n");

    tprintf("testing whether casting preserves the facet... ");
    id<TestHPrx> hf = [TestHPrx checkedCast:gf];
    test(hf);
    test([[hf callG] isEqualToString:@"G"]);
    test([[hf callH] isEqualToString:@"H"]);
    tprintf("ok\n");

    return gf;
}
