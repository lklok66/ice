// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>

@implementation TestI
-(ICEObject*) SBaseAsObject:(ICECurrent*)current
{
    TestSBase* sb = [[[TestSBase alloc] init] autorelease];
    sb.sb = @"SBase.sb";
    return sb;
}
-(TestSBase*) SBaseAsSBase:(ICECurrent*)current
{
    TestSBase* sb = [[[TestSBase alloc] init] autorelease];
    sb.sb = @"SBase.sb";
    return sb;
}
-(TestSBase*) SBSKnownDerivedAsSBase:(ICECurrent*)current
{
    TestSBSKnownDerived* sbskd = [[[TestSBSKnownDerived alloc] init] autorelease];
    sbskd.sb = @"SBSKnownDerived.sb";
    sbskd.sbskd = @"SBSKnownDerived.sbskd";
    return sbskd;
}
-(TestSBSKnownDerived*) SBSKnownDerivedAsSBSKnownDerived:(ICECurrent*)current
{
    TestSBSKnownDerived* sbskd = [[[TestSBSKnownDerived alloc] init] autorelease];
    sbskd.sb = @"SBSKnownDerived.sb";
    sbskd.sbskd = @"SBSKnownDerived.sbskd";
    return sbskd;
}
-(TestSBase*) SBSUnknownDerivedAsSBase:(ICECurrent*)current
{
    TestSBSUnknownDerived* sbsud = [[[TestSBSUnknownDerived alloc] init] autorelease];
    sbsud.sb = @"SBSUnknownDerived.sb";
    sbsud.sbsud = @"SBSUnknownDerived.sbsud";
    return sbsud;
}
-(ICEObject*) SUnknownAsObject:(ICECurrent*)current
{
    TestSUnknown* su = [[[TestSUnknown alloc] init] autorelease];
    su.su = @"SUnknown.su";
    return su;
}
-(TestB*) oneElementCycle:(ICECurrent*)current
{
    TestB* b1 = [[[TestB alloc] init] autorelease];
    b1.sb = @"B1.sb";
    b1.pb = b1;
    return b1;
}
-(TestB*) twoElementCycle:(ICECurrent*)current
{
    TestB* b1 = [[[TestB alloc] init] autorelease];
    b1.sb = @"B1.sb";
    TestB* b2 = [[[TestB alloc] init] autorelease];
    b2.sb = @"B2.sb";
    b2.pb = b1;
    b1.pb = b2;
    return b1;
}
-(TestB*) D1AsB:(ICECurrent*)current
{
    TestD1* d1 = [[[TestD1 alloc] init] autorelease];
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    TestD2* d2 = [[[TestD2 alloc] init] autorelease];
    d2.pb = d1;
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    d2.pd2 = d1;
    d1.pb = d2;
    d1.pd1 = d2;
    return d1;
}
-(TestD1*) D1AsD1:(ICECurrent*)current
{
    TestD1* d1 = [[[TestD1 alloc] init] autorelease];
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    TestD2* d2 = [[[TestD2 alloc] init] autorelease];
    d2.pb = d1;
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    d2.pd2 = d1;
    d1.pb = d2;
    d1.pd1 = d2;
    return d1;
}
-(TestB*) D2AsB:(ICECurrent*)current
{
    TestD2* d2 = [[[TestD2 alloc] init] autorelease];
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    TestD1* d1 = [[[TestD1 alloc] init] autorelease];
    d1.pb = d2;
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    d1.pd1 = d2;
    d2.pb = d1;
    d2.pd2 = d1;
    return d2;
}
-(void) paramTest1:(TestB**)p1 p2:(TestB**)p2 current:(ICECurrent*)current
{
    TestD1* d1 = [[[TestD1 alloc] init] autorelease];
    d1.sb = @"D1.sb";
    d1.sd1 = @"D1.sd1";
    TestD2* d2 = [[[TestD2 alloc] init] autorelease];
    d2.pb = d1;
    d2.sb = @"D2.sb";
    d2.sd2 = @"D2.sd2";
    d2.pd2 = d1;
    d1.pb = d2;
    d1.pd1 = d2;
    *p1 = d1;
    *p2 = d2;
}
-(void) paramTest2:(TestB**)p1 p1:(TestB**)p2 current:(ICECurrent*)current
{
    [self paramTest1:p2 p2:p1 current:current];
}
-(TestB*) paramTest3:(TestB**)p1 p2:(TestB**)p2 current:(ICECurrent*)current
{
    TestD2* d2 = [[[TestD2 alloc] init] autorelease];
    d2.sb = @"D2.sb (p1 1)";
    d2.pb = 0;
    d2.sd2 = @"D2.sd2 (p1 1)";
    *p1 = d2;

    TestD1* d1 = [[[TestD1 alloc] init] autorelease];
    d1.sb = @"D1.sb (p1 2)";
    d1.pb = 0;
    d1.sd1 = @"D1.sd2 (p1 2)";
    d1.pd1 = 0;
    d2.pd2 = d1;

    TestD2* d4 = [[[TestD2 alloc] init] autorelease];
    d4.sb = @"D2.sb (p2 1)";
    d4.pb = 0;
    d4.sd2 = @"D2.sd2 (p2 1)";
    *p2 = d4;

    TestD1* d3 = [[[TestD1 alloc] init] autorelease];
    d3.sb = @"D1.sb (p2 2)";
    d3.pb = 0;
    d3.sd1 = @"D1.sd2 (p2 2)";
    d3.pd1 = 0;
    d4.pd2 = d3;

    return d3;
}
-(TestB*) paramTest4:(TestB**)p1 current:(ICECurrent*)current
{
    TestD4* d4 = [[[TestD4 alloc] init] autorelease];
    d4.sb = @"D4.sb (1)";
    d4.pb = 0;
    d4.p1 = [[[TestB alloc] init] autorelease];
    d4.p1.sb = @"B.sb (1)";
    d4.p2 = [[[TestB alloc] init] autorelease];
    d4.p2.sb = @"B.sb (2)";
    *p1 = d4;
    return d4.p2;
}
-(TestB*) returnTest1:(TestB**)p1 p2:(TestB**)p2 current:(ICECurrent*)current
{
    [self paramTest1:p1 p2:p2 current:current];
    return *p1;
}
-(TestB*) returnTest2:(TestB**)p1 p1:(TestB**)p2 current:(ICECurrent*)current
{
    [self paramTest1:p2 p2:p1 current:current];
    return *p1;
}
-(TestB*) returnTest3:(TestB*)p1 p2:(TestB*)p2 current:(ICECurrent*)current
{
    return p1;
}
-(TestSS*) sequenceTest:(TestSS1*)p1 p2:(TestSS2*)p2 current:(ICECurrent*)current
{
    TestSS* ss = [TestSS sS];
    ss.c1 = p1;
    ss.c2 = p2;
    return ss;
}
-(TestBDict*) dictionaryTest:(TestBDict*)bin bout:(TestBDict**)bout current:(ICECurrent*)current
{
    int i;
    *bout = [TestMutableBDict dictionary];
    for(i = 0; i < 10; ++i)
    {
        TestB* b = [bin objectForKey:[NSNumber numberWithInt:i]];
        TestD2* d2 = [[[TestD2 alloc] init] autorelease];
        d2.sb = b.sb;
        d2.pb = b.pb;
        d2.sd2 = @"D2";
        d2.pd2 = d2;
        [(NSMutableDictionary*)*bout setObject:d2 forKey:[NSNumber numberWithInt:(i * 10)]];
    }
    TestMutableBDict* r = [TestMutableBDict dictionary];
    for(i = 0; i < 10; ++i)
    {
        TestD1* d1 = [[[TestD1 alloc] init] autorelease];
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
    TestBaseException* be = [[[TestBaseException alloc] init] autorelease];
    be.sbe = @"sbe";
    be.pb = [[[TestB alloc] init] autorelease];
    be.pb.sb = @"sb";
    be.pb.pb = be.pb;
    @throw be;
}
-(void) throwDerivedAsBase:(ICECurrent*)current
{
    TestDerivedException* de = [[[TestDerivedException alloc] init] autorelease];
    de.sbe = @"sbe";
    de.pb = [[[TestB alloc] init] autorelease];
    de.pb.sb = @"sb1";
    de.pb.pb = de.pb;
    de.sde = @"sde1";
    de.pd1 = [[[TestD1 alloc] init] autorelease];
    de.pd1.sb = @"sb2";
    de.pd1.pb = de.pd1;
    de.pd1.sd1 = @"sd2";
    de.pd1.pd1 = de.pd1;
    @throw de;
}
-(void) throwDerivedAsDerived:(ICECurrent*)current
{
    TestDerivedException* de = [[[TestDerivedException alloc] init] autorelease];
    de.sbe = @"sbe";
    de.pb = [[[TestB alloc] init] autorelease];
    de.pb.sb = @"sb1";
    de.pb.pb = de.pb;
    de.sde = @"sde1";
    de.pd1 = [[[TestD1 alloc] init] autorelease];
    de.pd1.sb = @"sb2";
    de.pd1.pb = de.pd1;
    de.pd1.sd1 = @"sd2";
    de.pd1.pd1 = de.pd1;
    @throw de;
}
-(void) throwUnknownDerivedAsBase:(ICECurrent*)current
{
    TestD2* d2 = [[[TestD2 alloc] init] autorelease];
    d2.sb = @"sb d2";
    d2.pb = d2;
    d2.sd2 = @"sd2 d2";
    d2.pd2 = d2;

    TestUnknownDerivedException* ude = [[[TestUnknownDerivedException alloc] init] autorelease];
    ude.sbe = @"sbe";
    ude.pb = d2;
    ude.sude = @"sude";
    ude.pd2 = d2;
    @throw ude;
}
-(void) useForward:(TestForward**)f current:(ICECurrent*)current
{
    *f = [[[TestForward alloc] init] autorelease];
    (*f).h = [[[TestHidden alloc] init] autorelease];
    (*f).h.f = *f;
}
-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end
