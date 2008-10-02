// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>

@implementation TestSlicingObjectsServerI
-(ICEObject*) SBaseAsObject:(ICECurrent*)current
{
    TestSlicingObjectsServerSBase* sb = [[[TestSlicingObjectsServerSBase alloc] init] autorelease];
    sb.sb = @"SBase.sb";
    return sb;
}
-(TestSlicingObjectsServerSBase*) SBaseAsSBase:(ICECurrent*)current
{
    TestSlicingObjectsServerSBase* sb = [[[TestSlicingObjectsServerSBase alloc] init] autorelease];
    sb.sb = @"SBase.sb";
    return sb;
}
-(TestSlicingObjectsServerSBase*) SBSKnownDerivedAsSBase:(ICECurrent*)current
{
    TestSlicingObjectsServerSBSKnownDerived* sbskd = [[[TestSlicingObjectsServerSBSKnownDerived alloc] init] autorelease];
    sbskd.sb = @"SBSKnownDerived.sb";
    sbskd.sbskd = @"SBSKnownDerived.sbskd";
    return sbskd;
}
-(TestSlicingObjectsServerSBSKnownDerived*) SBSKnownDerivedAsSBSKnownDerived:(ICECurrent*)current
{
    TestSlicingObjectsServerSBSKnownDerived* sbskd = [[[TestSlicingObjectsServerSBSKnownDerived alloc] init] autorelease];
    sbskd.sb = @"SBSKnownDerived.sb";
    sbskd.sbskd = @"SBSKnownDerived.sbskd";
    return sbskd;
}
-(TestSlicingObjectsServerSBase*) SBSUnknownDerivedAsSBase:(ICECurrent*)current
{
    TestSlicingObjectsServerSBSUnknownDerived* sbsud = [[[TestSlicingObjectsServerSBSUnknownDerived alloc] init] autorelease];
    sbsud.sb = @"SBSUnknownDerived.sb";
    sbsud.sbsud = @"SBSUnknownDerived.sbsud";
    return sbsud;
}
-(ICEObject*) SUnknownAsObject:(ICECurrent*)current
{
    TestSlicingObjectsServerSUnknown* su = [[[TestSlicingObjectsServerSUnknown alloc] init] autorelease];
    su.su = @"SUnknown.su";
    return su;
}
-(TestSlicingObjectsServerB*) oneElementCycle:(ICECurrent*)current
{
    TestSlicingObjectsServerB* b1 = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    b1.sb = @"B1.sb";
    b1.pb = b1;
    return b1;
}
-(TestSlicingObjectsServerB*) twoElementCycle:(ICECurrent*)current
{
    TestSlicingObjectsServerB* b1 = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    b1.sb = @"B1.sb";
    TestSlicingObjectsServerB* b2 = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    b2.sb = @"B2.sb";
    b2.pb = b1;
    b1.pb = b2;
    return b1;
}
-(TestSlicingObjectsServerB*) D1AsB:(ICECurrent*)current
{
    TestSlicingObjectsServerD1* d1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    TestSlicingObjectsServerD2* d2 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
    d2.pb = d1;
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    d2.pd2 = d1;
    d1.pb = d2;
    d1.pd1 = d2;
    return d1;
}
-(TestSlicingObjectsServerD1*) D1AsD1:(ICECurrent*)current
{
    TestSlicingObjectsServerD1* d1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    TestSlicingObjectsServerD2* d2 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
    d2.pb = d1;
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    d2.pd2 = d1;
    d1.pb = d2;
    d1.pd1 = d2;
    return d1;
}
-(TestSlicingObjectsServerB*) D2AsB:(ICECurrent*)current
{
    TestSlicingObjectsServerD2* d2 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    TestSlicingObjectsServerD1* d1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    d1.pb = d2;
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    d1.pd1 = d2;
    d2.pb = d1;
    d2.pd2 = d1;
    return d2;
}
-(void) paramTest1:(TestSlicingObjectsServerB**)p1 p2:(TestSlicingObjectsServerB**)p2 current:(ICECurrent*)current
{
    TestSlicingObjectsServerD1* d1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    TestSlicingObjectsServerD2* d2 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
    d2.pb = d1;
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    d2.pd2 = d1;
    d1.pb = d2;
    d1.pd1 = d2;
    *p1 = d1;
    *p2 = d2;
}
-(void) paramTest2:(TestSlicingObjectsServerB**)p1 p1:(TestSlicingObjectsServerB**)p2 current:(ICECurrent*)current
{
    [self paramTest1:p2 p2:p1 current:current];
}
-(TestSlicingObjectsServerB*) paramTest3:(TestSlicingObjectsServerB**)p1 p2:(TestSlicingObjectsServerB**)p2 current:(ICECurrent*)current
{
    TestSlicingObjectsServerD2* d2 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
    d2.sb = @"D2.sb (p1 1)";
    d2.pb = 0;
    d2.sd2 = @"D2.sd2 (p1 1)";
    *p1 = d2;

    TestSlicingObjectsServerD1* d1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    d1.sb = @"D1.sb (p1 2)";
    d1.pb = 0;
    d1.sd1 = @"D1.sd2 (p1 2)";
    d1.pd1 = 0;
    d2.pd2 = d1;

    TestSlicingObjectsServerD2* d4 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
    d4.sb = @"D2.sb (p2 1)";
    d4.pb = 0;
    d4.sd2 = @"D2.sd2 (p2 1)";
    *p2 = d4;

    TestSlicingObjectsServerD1* d3 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    d3.sb = @"D1.sb (p2 2)";
    d3.pb = 0;
    d3.sd1 = @"D1.sd2 (p2 2)";
    d3.pd1 = 0;
    d4.pd2 = d3;

    return d3;
}
-(TestSlicingObjectsServerB*) paramTest4:(TestSlicingObjectsServerB**)p1 current:(ICECurrent*)current
{
    TestSlicingObjectsServerD4* d4 = [[[TestSlicingObjectsServerD4 alloc] init] autorelease];
    d4.sb = @"D4.sb (1)";
    d4.pb = 0;
    d4.p1 = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    d4.p1.sb = @"B.sb (1)";
    d4.p2 = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    d4.p2.sb = @"B.sb (2)";
    *p1 = d4;
    return d4.p2;
}
-(TestSlicingObjectsServerB*) returnTest1:(TestSlicingObjectsServerB**)p1 p2:(TestSlicingObjectsServerB**)p2 current:(ICECurrent*)current
{
    [self paramTest1:p1 p2:p2 current:current];
    return *p1;
}
-(TestSlicingObjectsServerB*) returnTest2:(TestSlicingObjectsServerB**)p1 p1:(TestSlicingObjectsServerB**)p2 current:(ICECurrent*)current
{
    [self paramTest1:p2 p2:p1 current:current];
    return *p1;
}
-(TestSlicingObjectsServerB*) returnTest3:(TestSlicingObjectsServerB*)p1 p2:(TestSlicingObjectsServerB*)p2 current:(ICECurrent*)current
{
    return p1;
}
-(TestSlicingObjectsServerSS*) sequenceTest:(TestSlicingObjectsServerSS1*)p1 p2:(TestSlicingObjectsServerSS2*)p2 current:(ICECurrent*)current
{
    TestSlicingObjectsServerSS* ss = [TestSlicingObjectsServerSS sS];
    ss.c1 = p1;
    ss.c2 = p2;
    return ss;
}
-(TestSlicingObjectsServerBDict*) dictionaryTest:(TestSlicingObjectsServerBDict*)bin bout:(TestSlicingObjectsServerBDict**)bout current:(ICECurrent*)current
{
    int i;
    *bout = [TestSlicingObjectsServerMutableBDict dictionary];
    for(i = 0; i < 10; ++i)
    {
        TestSlicingObjectsServerB* b = [bin objectForKey:[NSNumber numberWithInt:i]];
        TestSlicingObjectsServerD2* d2 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
        d2.sb = b.sb;
        d2.pb = b.pb;
        d2.sd2 = @"D2";
        d2.pd2 = d2;
        [(NSMutableDictionary*)*bout setObject:d2 forKey:[NSNumber numberWithInt:(i * 10)]];
    }
    TestSlicingObjectsServerMutableBDict* r = [TestSlicingObjectsServerMutableBDict dictionary];
    for(i = 0; i < 10; ++i)
    {
        TestSlicingObjectsServerD1* d1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
        d1.sb = [NSString stringWithFormat:@"D1.%d",(i * 20)];
        d1.pb = (i == 0 ? nil : [r objectForKey:[NSNumber numberWithInt:((i - 1) * 20)]]);
        d1.sd1 = d1.sb;
        d1.pd1 = d1;
        [r setObject:d1 forKey:[NSNumber numberWithInt:(i * 20)]];
    }
    return r;
}
-(void) throwBaseAsBase:(ICECurrent*)current
{
    TestSlicingObjectsServerBaseException* be = [[[TestSlicingObjectsServerBaseException alloc] init] autorelease];
    be.sbe = @"sbe";
    be.pb = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    be.pb.sb = @"sb";
    be.pb.pb = be.pb;
    @throw be;
}
-(void) throwDerivedAsBase:(ICECurrent*)current
{
    TestSlicingObjectsServerDerivedException* de = [[[TestSlicingObjectsServerDerivedException alloc] init] autorelease];
    de.sbe = @"sbe";
    de.pb = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    de.pb.sb = @"sb1";
    de.pb.pb = de.pb;
    de.sde = @"sde1";
    de.pd1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    de.pd1.sb = @"sb2";
    de.pd1.pb = de.pd1;
    de.pd1.sd1 = @"sd2";
    de.pd1.pd1 = de.pd1;
    @throw de;
}
-(void) throwDerivedAsDerived:(ICECurrent*)current
{
    TestSlicingObjectsServerDerivedException* de = [[[TestSlicingObjectsServerDerivedException alloc] init] autorelease];
    de.sbe = @"sbe";
    de.pb = [[[TestSlicingObjectsServerB alloc] init] autorelease];
    de.pb.sb = @"sb1";
    de.pb.pb = de.pb;
    de.sde = @"sde1";
    de.pd1 = [[[TestSlicingObjectsServerD1 alloc] init] autorelease];
    de.pd1.sb = @"sb2";
    de.pd1.pb = de.pd1;
    de.pd1.sd1 = @"sd2";
    de.pd1.pd1 = de.pd1;
    @throw de;
}
-(void) throwUnknownDerivedAsBase:(ICECurrent*)current
{
    TestSlicingObjectsServerD2* d2 = [[[TestSlicingObjectsServerD2 alloc] init] autorelease];
    d2.sb = @"sb d2";
    d2.pb = d2;
    d2.sd2 = @"sd2 d2";
    d2.pd2 = d2;

    TestSlicingObjectsServerUnknownDerivedException* ude = [[[TestSlicingObjectsServerUnknownDerivedException alloc] init] autorelease];
    ude.sbe = @"sbe";
    ude.pb = d2;
    ude.sude = @"sude";
    ude.pd2 = d2;
    @throw ude;
}
-(void) useForward:(TestSlicingObjectsSharedForward**)f current:(ICECurrent*)current
{
    *f = [[[TestSlicingObjectsSharedForward alloc] init] autorelease];
    (*f).h = [[[TestSlicingObjectsSharedHidden alloc] init] autorelease];
    (*f).h.f = *f;
}
-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end
