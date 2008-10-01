// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <Test.h>
#import <limits.h>
#import <float.h>
    
void
twoways(id<ICECommunicator> communicator, id<TestMyClassPrx> p)
{
    {
        [p opVoid];
    }

    {
        ICEByte b;
        ICEByte r;

        r = [p opByte:(ICEByte)0xff p2:(ICEByte)0x0f p3:&b];
        test(b == (ICEByte)0xf0);
        test(r == (ICEByte)0xff);
    }

    {
        BOOL b;
        BOOL r;

        r = [p opBool:YES p2:NO p3:&b];
        test(b);
        test(!r);
    }

    {
        ICEShort s;
	ICEInt i;
	ICELong l;
	ICELong r;

	r = [p opShortIntLong:10 p2:11 p3:12 p4:&s p5:&i p6:&l];
	test(s == 10);
	test(i == 11);
	test(l == 12);
	test(r == 12);

#ifndef SHORT_MIN
#define SHORT_MIN (-0x7fff)
#endif
	r = [p opShortIntLong:SHORT_MIN p2:INT_MIN p3:LONG_MIN p4:&s p5:&i p6:&l];
	test(s == SHORT_MIN);
	test(i == INT_MIN);
	test(l == LONG_MIN);
	test(r == LONG_MIN);

#ifndef SHORT_MAX
#define SHORT_MAX 0x7fff
#endif
	r = [p opShortIntLong:SHORT_MAX p2:INT_MAX p3:LONG_MAX p4:&s p5:&i p6:&l];
	test(s == SHORT_MAX);
	test(i == INT_MAX);
	test(l == LONG_MAX);
	test(r == LONG_MAX);
    }

    {
        ICEFloat f;
	ICEDouble d;
	ICEDouble r;

	r = [p opFloatDouble:3.14f p2:1.1E10 p3:&f p4:&d];
	test(f == 3.14f);
	test(d == 1.1E10);
	test(r == 1.1E10);

	r = [p opFloatDouble:FLT_MIN p2:DBL_MIN p3:&f p4:&d];
	test(f == FLT_MIN);
	test(d == DBL_MIN);
	test(r == DBL_MIN);

	r = [p opFloatDouble:FLT_MAX p2:DBL_MAX p3:&f p4:&d];
	test(f == FLT_MAX);
	test(d == DBL_MAX);
	test(r == DBL_MAX);
    }

    {
        NSMutableString *s = nil;
	NSMutableString *r;

	r = [p opString:@"hello" p2:@"world" p3:&s];
	r = [p opString:@"hello" p2:@"world" p3:&s];
	r = [p opString:@"hello" p2:@"world" p3:&s];
	r = [p opString:@"hello" p2:@"world" p3:&s];
	test([s isEqualToString:@"world hello"]);
	test([r isEqualToString:@"hello world"]);
    }

    {
        TestMyEnum e;
	TestMyEnum r;

	r = [p opMyEnum:Testenum2 p2:&e];
	test(e == Testenum2);
	test(r == Testenum3);
    }

    {
        id<TestMyClassPrx> c1;
        id<TestMyClassPrx> c2;
        id<TestMyClassPrx> r;
        
        r = [p opMyClass:p p2:&c1 p3:&c2];
        test([c1 compareIdentityAndFacet:p] == NSOrderedSame);
        test([c2 compareIdentityAndFacet:p] != NSOrderedSame);
        test([r compareIdentityAndFacet:p] == NSOrderedSame);
        test([[c1 ice_getIdentity] isEqual:[communicator stringToIdentity:@"test"]]);
        test([[c2 ice_getIdentity]isEqual:[communicator stringToIdentity:@"noSuchIdentity"]]);
        test([[r ice_getIdentity] isEqual:[communicator stringToIdentity:@"test"]]);
        [r opVoid];
        [c1 opVoid];
        @try
        {
            [c2 opVoid];
            test(NO);
        }
        @catch(ICEObjectNotExistException*)
        {
        }

        r = [p opMyClass:0 p2:&c1 p3:&c2];
        test(c1 == nil);
        test(c2 != nil);
        test([r compareIdentityAndFacet:p] == NSOrderedSame);
        [r opVoid];
    }

    {
        TestStructure *si1 = [TestStructure structure];
	si1.p = p;
	si1.e = Testenum3;
	si1.s = [TestAnotherStruct anotherStruct];
	si1.s.s = @"abc";
	TestStructure *si2 = [TestStructure structure];
	si2.p = nil;
	si2.e = Testenum2;
	si2.s = [TestAnotherStruct anotherStruct];
	si2.s.s = @"def";

	TestStructure *so;
	TestStructure *rso = [p opStruct:si1 p2:si2 p3:&so];

	test(rso.p == nil);
	test(rso.e == Testenum2);
	test([rso.s.s isEqualToString:@"def"]);
	test(so.p != nil);
	test([so.p isEqual:p]);
	test(so.e == Testenum3);
	test([so.s.s isEqualToString:@"a new string"]);
	[so.p opVoid];
    }

    {
	ICEByte buf1[] = { 0x01, 0x11, 0x12, 0x22 };
	ICEByte buf2[] = { 0xf1, 0xf2, 0xf3, 0xf4 };
        TestMutableByteS *bsi1 = [[[TestMutableByteS alloc] init] autorelease];
        TestMutableByteS *bsi2 = [[[TestMutableByteS alloc] init] autorelease];
	[bsi1 appendBytes:buf1 length:sizeof(buf1)];
	[bsi2 appendBytes:buf2 length:sizeof(buf2)];

	TestMutableByteS *bso;
	TestByteS *rso;

	rso = [p opByteS:bsi1 p2:bsi2 p3:&bso];

        test([bso length] == 4);
	ICEByte *bbso = (ICEByte *)[bso bytes];
        test(bbso[0] == 0x22);
        test(bbso[1] == 0x12);
        test(bbso[2] == 0x11);
        test(bbso[3] == 0x01);
        test([rso length] == 8);
	ICEByte *brso = (ICEByte *)[rso bytes];
        test(brso[0] == 0x01);
        test(brso[1] == 0x11);
        test(brso[2] == 0x12);
        test(brso[3] == 0x22);
        test(brso[4] == 0xf1);
        test(brso[5] == 0xf2);
        test(brso[6] == 0xf3);
        test(brso[7] == 0xf4);
    }

    {
	BOOL buf1[] = { YES, YES, NO };
	BOOL buf2[] = { NO };
        TestMutableBoolS *bsi1 = [[[TestMutableBoolS alloc] init] autorelease];
        TestMutableBoolS *bsi2 = [[[TestMutableBoolS alloc] init] autorelease];
	[bsi1 appendBytes:buf1 length:sizeof(buf1)];
	[bsi2 appendBytes:buf2 length:sizeof(buf2)];

	TestMutableBoolS *bso;
	TestBoolS *rso;

	rso = [p opBoolS:bsi1 p2:bsi2 p3:&bso];

        test([bso length] == 4 * sizeof(BOOL));
	BOOL *bbso = (BOOL *)[bso bytes];
        test(bbso[0]);
        test(bbso[1]);
        test(!bbso[2]);
        test(!bbso[3]);
        test([rso length] == 3 * sizeof(BOOL));
	BOOL *brso = (BOOL *)[rso bytes];
        test(!brso[0]);
        test(brso[1]);
        test(brso[2]);
    }

    {
	ICEShort buf1[] = { 1, 2, 3 };
	ICEInt buf2[] = { 5, 6, 7, 8 };
	ICELong buf3[] = { 10, 30, 20 };
        TestMutableShortS *ssi = [[[TestMutableShortS alloc] init] autorelease];
	TestMutableIntS *isi = [[[TestMutableIntS alloc] init] autorelease];
	TestMutableLongS *lsi = [[[TestMutableLongS alloc] init] autorelease];
	[ssi appendBytes:buf1 length:sizeof(buf1)];
	[isi appendBytes:buf2 length:sizeof(buf2)];
	[lsi appendBytes:buf3 length:sizeof(buf3)];

	TestMutableShortS *sso;
	TestMutableIntS *iso;
	TestMutableLongS *lso;
	TestLongS *rso;

	rso = [p opShortIntLongS:ssi p2:isi p3:lsi p4:&sso p5:&iso p6:&lso];

        test([sso length] == 3 * sizeof(ICEShort));
	ICEShort *bsso = (ICEShort *)[sso bytes];
	test(bsso[0] == 1);
	test(bsso[1] == 2);
	test(bsso[2] == 3);
        test([iso length] == 4 * sizeof(ICEInt));
	ICEInt *biso = (ICEInt *)[iso bytes];
	test(biso[0] == 8);
	test(biso[1] == 7);
	test(biso[2] == 6);
	test(biso[3] == 5);
        test([lso length] == 6 * sizeof(ICELong));
	ICELong *blso = (ICELong *)[lso bytes];
	test(blso[0] == 10);
	test(blso[1] == 30);
	test(blso[2] == 20);
	test(blso[3] == 10);
	test(blso[4] == 30);
	test(blso[5] == 20);
        test([rso length] == 3 * sizeof(ICELong));
	ICELong *brso = (ICELong *)[rso bytes];
	test(brso[0] == 10);
	test(brso[1] == 30);
	test(brso[2] == 20);
    }

    {
	ICEFloat buf1[] = { 3.14f, 1.11f };
	ICEDouble buf2[] = { 1.1E10, 1.2E10, 1.3E10 };
        TestMutableFloatS *fsi = [[[TestMutableFloatS alloc] init] autorelease];
        TestMutableDoubleS *dsi = [[[TestMutableDoubleS alloc] init] autorelease];
	[fsi appendBytes:buf1 length:sizeof(buf1)];
	[dsi appendBytes:buf2 length:sizeof(buf2)];

	TestMutableFloatS *fso;
	TestMutableDoubleS *dso;
	TestDoubleS *rso;

	rso = [p opFloatDoubleS:fsi p2:dsi p3:&fso p4:&dso];

        test([fso length] == 2 * sizeof(ICEFloat));
	ICEFloat *bfso = (ICEFloat *)[fso bytes];
        test(bfso[0] == 3.14f);
        test(bfso[1] == 1.11f);
        test([dso length] == 3 * sizeof(ICEDouble));
	ICEDouble *bdso = (ICEDouble *)[dso bytes];
        test(bdso[0] == 1.3E10);
        test(bdso[1] == 1.2E10);
        test(bdso[2] == 1.1E10);
        test([rso length] == 5 * sizeof(ICEDouble));
	ICEDouble *brso = (ICEDouble *)[rso bytes];
        test(brso[0] == 1.1E10);
        test(brso[1] == 1.2E10);
        test(brso[2] == 1.3E10);
        test((ICEFloat)brso[3] == 3.14f);
        test((ICEFloat)brso[4] == 1.11f);
    }

    {
        TestMutableStringS *ssi1 = [TestMutableStringS arrayWithCapacity:3];
        TestMutableStringS *ssi2 = [TestMutableStringS arrayWithCapacity:1];

	[ssi1 addObject:@"abc"];
	[ssi1 addObject:@"de"];
	[ssi1 addObject:@"fghi"];

	[ssi2 addObject:@"xyz"];

	TestMutableStringS *sso;
	TestStringS *rso;

	rso = [p opStringS:ssi1 p2:ssi2 p3:&sso];

	test([sso count] == 4);
	test([[sso objectAtIndex:0] isEqualToString:@"abc"]);
	test([[sso objectAtIndex:1] isEqualToString:@"de"]);
	test([[sso objectAtIndex:2] isEqualToString:@"fghi"]);
	test([[sso objectAtIndex:3] isEqualToString:@"xyz"]);
        test([rso count] == 3);
	test([[rso objectAtIndex:0] isEqualToString:@"fghi"]);
	test([[rso objectAtIndex:1] isEqualToString:@"de"]);
	test([[rso objectAtIndex:2] isEqualToString:@"abc"]);
    }

    @try
    {
        TestMutableMyClassS *ssi1 = [TestMutableMyClassS arrayWithCapacity:3];
        TestMutableMyClassS *ssi2 = [TestMutableMyClassS arrayWithCapacity:1];

        ICEIdentity *i1 = [ICEIdentity identity:@"abc" category:@""];
        ICEIdentity *i2 = [ICEIdentity identity:@"de" category:@""];
        ICEIdentity *i3 = [ICEIdentity identity:@"fhgi" category:@""];
        ICEIdentity *i4 = [ICEIdentity identity:@"xyz" category:@""];

	[ssi1 addObject:[p ice_identity:i1]];
	[ssi1 addObject:[p ice_identity:i2]];
	[ssi1 addObject:[p ice_identity:i3]];

	[ssi2 addObject:[p ice_identity:i4]];

	TestMutableMyClassS *sso;
	TestMyClassS *rso;

	rso = [p opMyClassS:ssi1 p2:ssi2 p3:&sso];

	test([sso count] == 4);
	test([[[sso objectAtIndex:0] ice_getIdentity] isEqual:i1]);
	test([[[sso objectAtIndex:1] ice_getIdentity] isEqual:i2]);
	test([[[sso objectAtIndex:2] ice_getIdentity] isEqual:i3]);
	test([[[sso objectAtIndex:3] ice_getIdentity] isEqual:i4]);
        test([rso count] == 3);
	test([[[rso objectAtIndex:0] ice_getIdentity] isEqual:i3]);
	test([[[rso objectAtIndex:1] ice_getIdentity] isEqual:i2]);
	test([[[rso objectAtIndex:2] ice_getIdentity] isEqual:i1]);
    }
    @catch(ICEOperationNotExistException*)
    {
        // Some mapping don't implement this method.
    }

    @try
    {
	TestMyEnum buf1[] = { Testenum2, Testenum3, Testenum3 };
	TestMyEnum buf2[] = { Testenum1 };

        TestMutableMyEnumS *esi1 = [TestMutableMyEnumS data];
        TestMutableMyEnumS *esi2 = [TestMutableMyEnumS data];

	[esi1 appendBytes:buf1 length:sizeof(buf1)];
	[esi2 appendBytes:buf2 length:sizeof(buf2)];

	TestMutableMyEnumS *eso;
	TestMyEnumS *rso;

	rso = [p opMyEnumS:esi1 p2:esi2 p3:&eso];

        test([eso length] / sizeof(TestMyEnum) == 3);
	const TestMyEnum *beso = (const TestMyEnum *)[eso bytes];
        test(beso[0] == Testenum3);
        test(beso[1] == Testenum3);
        test(beso[2] == Testenum2);
        test([rso length] / sizeof(TestMyEnum) == 4);
	const TestMyEnum *brso = (const TestMyEnum *)[rso bytes];
        test(brso[0] == Testenum2);
        test(brso[1] == Testenum3);
        test(brso[2] == Testenum3);
        test(brso[3] == Testenum1);
    }
    @catch(ICEOperationNotExistException*)
    {
        // Some mapping don't implement this method.
    }

    {
        TestMutableByteSS *bsi1 = [TestMutableByteSS array];
        TestMutableByteSS *bsi2 = [TestMutableByteSS array];

	ICEByte b;
	TestMutableByteS *tmp = [TestMutableByteS data];

	b = 0x01;
	[tmp appendBytes:&b length:sizeof(b)];
	b = 0x11;
	[tmp appendBytes:&b length:sizeof(b)];
	b = 0x12;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi1 addObject:tmp];

        tmp = [TestMutableByteS data];
	b = 0xff;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi1 addObject:tmp];

        tmp = [TestMutableByteS data];
	b = 0x0e;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi2 addObject:tmp];

        tmp = [TestMutableByteS data];
	b = 0xf2;
	[tmp appendBytes:&b length:sizeof(b)];
	b = 0xf1;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi2 addObject:tmp];

	TestMutableByteSS *bso;
	TestByteSS *rso;

	rso = [p opByteSS:bsi1 p2:bsi2 p3:&bso];

	const ICEByte *p;
	test([bso count] == 2);
	test([[bso objectAtIndex:0] length] / sizeof(ICEByte) == 1);
	p = [[bso objectAtIndex:0] bytes];
	test(p[0] == (ICEByte)0x0ff);
	test([[bso objectAtIndex:1] length] / sizeof(ICEByte) == 3);
	p = [[bso objectAtIndex:1] bytes];
	test(p[0] == (ICEByte)0x01);
	test(p[1] == (ICEByte)0x11);
	test(p[2] == (ICEByte)0x12);
	test([rso count] == 4);
	test([[rso objectAtIndex:0] length] / sizeof(ICEByte) == 3);
	p = [[rso objectAtIndex:0] bytes];
	test(p[0] == (ICEByte)0x01);
	test(p[1] == (ICEByte)0x11);
	test(p[2] == (ICEByte)0x12);
	test([[rso objectAtIndex:1] length] / sizeof(ICEByte) == 1);
	p = [[rso objectAtIndex:1] bytes];
	test(p[0] == (ICEByte)0xff);
	test([[rso objectAtIndex:2] length] / sizeof(ICEByte) == 1);
	p = [[rso objectAtIndex:2] bytes];
	test(p[0] == (ICEByte)0x0e);
	test([[rso objectAtIndex:3] length] / sizeof(ICEByte) == 2);
	p = [[rso objectAtIndex:3] bytes];
	test(p[0] == (ICEByte)0xf2);
	test(p[1] == (ICEByte)0xf1);
    }

    {
        TestMutableFloatSS *fsi = [TestMutableFloatSS array];
        TestMutableDoubleSS *dsi = [TestMutableDoubleSS array];

	ICEFloat f;
	TestMutableFloatS *ftmp;

	ftmp = [TestMutableFloatS data];
	f = 3.14f;
	[ftmp appendBytes:&f length:sizeof(f)];
	[fsi addObject:ftmp];
	ftmp = [TestMutableFloatS data];
	f = 1.11f;
	[ftmp appendBytes:&f length:sizeof(f)];
	[fsi addObject:ftmp];
	ftmp = [TestMutableFloatS data];
	[fsi addObject:ftmp];

	ICEDouble d;
	TestMutableDoubleS *dtmp;

        dtmp = [TestMutableDoubleS data];
	d = 1.1E10;
	[dtmp appendBytes:&d length:sizeof(d)];
	d = 1.2E10;
	[dtmp appendBytes:&d length:sizeof(d)];
	d = 1.3E10;
	[dtmp appendBytes:&d length:sizeof(d)];
	[dsi addObject:dtmp];

	TestMutableFloatSS *fso;
	TestMutableDoubleSS *dso;
	TestDoubleSS *rso;

	rso = [p opFloatDoubleSS:fsi p2:dsi p3:&fso p4:&dso];

	const ICEFloat *fp;
	const ICEDouble *dp;

	test([fso count] == 3);
	test([[fso objectAtIndex:0] length] / sizeof(ICEFloat) == 1);
	fp = [[fso objectAtIndex:0] bytes];
	test(fp[0] == 3.14f);
	test([[fso objectAtIndex:1] length] / sizeof(ICEFloat) == 1);
	fp = [[fso objectAtIndex:1] bytes];
	test(fp[0] == 1.11f);
	test([[fso objectAtIndex:2] length] / sizeof(ICEFloat) == 0);
	test([dso count] == 1);
	test([[dso objectAtIndex:0] length] / sizeof(ICEDouble) == 3);
	dp = [[dso objectAtIndex:0] bytes];
	test(dp[0] == 1.1E10);
	test(dp[1] == 1.2E10);
	test(dp[2] == 1.3E10);
	test([rso count] == 2);
	test([[rso objectAtIndex:0] length] / sizeof(ICEDouble) == 3);
	dp = [[rso objectAtIndex:0] bytes];
	test(dp[0] == 1.1E10);
	test(dp[1] == 1.2E10);
	test(dp[2] == 1.3E10);
	test([[rso objectAtIndex:1] length] / sizeof(ICEDouble) == 3);
	dp = [[rso objectAtIndex:1] bytes];
	test(dp[0] == 1.1E10);
	test(dp[1] == 1.2E10);
	test(dp[2] == 1.3E10);
    }

    {
        TestMutableStringSS * ssi1 = [TestMutableStringSS array];
        TestMutableStringSS * ssi2 = [TestMutableStringSS array];

	TestMutableStringS *tmp;

	tmp = [TestMutableStringS array];
	[tmp addObject:@"abc"];
	[ssi1 addObject:tmp];
	tmp = [TestMutableStringS array];
	[tmp addObject:@"de"];
	[tmp addObject:@"fghi"];
	[ssi1 addObject:tmp];

	[ssi2 addObject:[TestStringS array]];
	[ssi2 addObject:[TestStringS array]];
	tmp = [TestMutableStringS array];
	[tmp addObject:@"xyz"];
	[ssi2 addObject:tmp];

	TestMutableStringSS *sso;
	TestStringSS *rso;

	rso = [p opStringSS:ssi1 p2:ssi2 p3:&sso];

	test([sso count] == 5);
	test([[sso objectAtIndex:0] count] == 1);
	test([[[sso objectAtIndex:0] objectAtIndex:0] isEqualToString:@"abc"]);
	test([[sso objectAtIndex:1] count] == 2);
	test([[[sso objectAtIndex:1] objectAtIndex:0] isEqualToString:@"de"]);
	test([[[sso objectAtIndex:1] objectAtIndex:1] isEqualToString:@"fghi"]);
	test([[sso objectAtIndex:2] count] == 0);
	test([[sso objectAtIndex:3] count] == 0);
	test([[sso objectAtIndex:4] count] == 1);
	test([[[sso objectAtIndex:4] objectAtIndex:0] isEqualToString:@"xyz"]);
	test([rso count] == 3);
	test([[rso objectAtIndex:0] count] == 1);
	test([[[rso objectAtIndex:0] objectAtIndex:0] isEqualToString:@"xyz"]);
	test([[rso objectAtIndex:1] count] == 0);
	test([[rso objectAtIndex:2] count] == 0);
    }

    {
        TestMutableStringSSS *sssi1 = [TestMutableStringSSS array];
        TestMutableStringSSS *sssi2 = [TestMutableStringSSS array];

	TestMutableStringSS *tmpss;
	TestMutableStringS *tmps;

	tmps = [TestMutableStringS array];
        [tmps addObject:@"abc"];
        [tmps addObject:@"de"];
	tmpss = [TestMutableStringSS array];
	[tmpss addObject:tmps];
	tmps = [TestMutableStringS array];
        [tmps addObject:@"xyz"];
	[tmpss addObject:tmps];
	[sssi1 addObject:tmpss];
	tmps = [TestMutableStringS array];
        [tmps addObject:@"hello"];
	tmpss = [TestMutableStringSS array];
	[tmpss addObject:tmps];
	[sssi1 addObject:tmpss];

	tmps = [TestMutableStringS array];
        [tmps addObject:@""];
        [tmps addObject:@""];
	tmpss = [TestMutableStringSS array];
	[tmpss addObject:tmps];
	tmps = [TestMutableStringS array];
        [tmps addObject:@"abcd"];
	[tmpss addObject:tmps];
	[sssi2 addObject:tmpss];
	tmps = [TestMutableStringS array];
        [tmps addObject:@""];
	tmpss = [TestMutableStringSS array];
	[tmpss addObject:tmps];
	[sssi2 addObject:tmpss];
	tmpss = [TestMutableStringSS array];
	[sssi2 addObject:tmpss];

	TestMutableStringSSS *ssso;
	TestStringSSS *rsso;

	rsso = [p opStringSSS:sssi1 p2:sssi2 p3:&ssso];

	test([ssso count] == 5);
	test([[ssso objectAtIndex:0] count] == 2);
	test([[[ssso objectAtIndex:0] objectAtIndex:0] count] == 2);
	test([[[ssso objectAtIndex:0] objectAtIndex:1] count] == 1);
	test([[ssso objectAtIndex:1] count] == 1);
	test([[[ssso objectAtIndex:1] objectAtIndex:0] count] == 1);
	test([[ssso objectAtIndex:2] count] == 2);
	test([[[ssso objectAtIndex:2] objectAtIndex:0] count] == 2);
	test([[[ssso objectAtIndex:2] objectAtIndex:1] count] == 1);
	test([[ssso objectAtIndex:3] count] == 1);
	test([[[ssso objectAtIndex:3] objectAtIndex:0] count] == 1);
	test([[ssso objectAtIndex:4] count] == 0);
	test([[[[ssso objectAtIndex:0] objectAtIndex:0] objectAtIndex:0] isEqualToString:@"abc"]);
	test([[[[ssso objectAtIndex:0] objectAtIndex:0] objectAtIndex:1] isEqualToString:@"de"]);
	test([[[[ssso objectAtIndex:0] objectAtIndex:1] objectAtIndex:0] isEqualToString:@"xyz"]);
	test([[[[ssso objectAtIndex:1] objectAtIndex:0] objectAtIndex:0] isEqualToString:@"hello"]);
	test([[[[ssso objectAtIndex:2] objectAtIndex:0] objectAtIndex:0] isEqualToString:@""]);
	test([[[[ssso objectAtIndex:2] objectAtIndex:0] objectAtIndex:1] isEqualToString:@""]);
	test([[[[ssso objectAtIndex:2] objectAtIndex:1] objectAtIndex:0] isEqualToString:@"abcd"]);
	test([[[[ssso objectAtIndex:3] objectAtIndex:0] objectAtIndex:0] isEqualToString:@""]);

	test([rsso count] == 3);
	test([[rsso objectAtIndex:0] count] == 0);
	test([[rsso objectAtIndex:1] count] == 1);
	test([[[rsso objectAtIndex:1] objectAtIndex:0] count] == 1);
	test([[rsso objectAtIndex:2] count] == 2);
	test([[[rsso objectAtIndex:2] objectAtIndex:0] count] == 2);
	test([[[rsso objectAtIndex:2] objectAtIndex:1] count] == 1);
	test([[[[rsso objectAtIndex:1] objectAtIndex:0] objectAtIndex:0] isEqualToString:@""]);
	test([[[[rsso objectAtIndex:2] objectAtIndex:0] objectAtIndex:0] isEqualToString:@""]);
	test([[[[rsso objectAtIndex:2] objectAtIndex:0] objectAtIndex:1] isEqualToString:@""]);
	test([[[[rsso objectAtIndex:2] objectAtIndex:1] objectAtIndex:0] isEqualToString:@"abcd"]);
    }

    {
        TestMutableByteBoolD *di1 = [TestMutableByteBoolD dictionary];
	[di1 setObject:[NSNumber numberWithBool:YES] forKey:[NSNumber numberWithUnsignedChar:10]];
	[di1 setObject:[NSNumber numberWithBool:NO] forKey:[NSNumber numberWithUnsignedChar:100]];
        TestMutableByteBoolD *di2 = [TestMutableByteBoolD dictionary];
	[di2 setObject:[NSNumber numberWithBool:YES] forKey:[NSNumber numberWithUnsignedChar:10]];
	[di2 setObject:[NSNumber numberWithBool:NO] forKey:[NSNumber numberWithUnsignedChar:11]];
	[di2 setObject:[NSNumber numberWithBool:TRUE] forKey:[NSNumber numberWithUnsignedChar:101]];

        TestMutableByteBoolD *_do;
        TestMutableByteBoolD *ro = [p opByteBoolD:di1 p2:di2 p3:&_do];

	test([_do isEqualToDictionary:di1]);
	test([ro count] == 4);
	test([[ro objectForKey:[NSNumber numberWithUnsignedChar:10]] boolValue] == YES);
	test([[ro objectForKey:[NSNumber numberWithUnsignedChar:11]] boolValue] == NO);
	test([[ro objectForKey:[NSNumber numberWithUnsignedChar:100]] boolValue] == NO);
	test([[ro objectForKey:[NSNumber numberWithUnsignedChar:101]] boolValue] == YES);
    }

    {
        TestMutableShortIntD *di1 = [TestMutableShortIntD dictionary];
	[di1 setObject:[NSNumber numberWithInt:-1] forKey:[NSNumber numberWithShort:110]];
	[di1 setObject:[NSNumber numberWithInt:123123] forKey:[NSNumber numberWithShort:1100]];
        TestMutableShortIntD *di2 = [TestMutableShortIntD dictionary];
	[di2 setObject:[NSNumber numberWithInt:-1] forKey:[NSNumber numberWithShort:110]];
	[di2 setObject:[NSNumber numberWithInt:-100] forKey:[NSNumber numberWithShort:111]];
	[di2 setObject:[NSNumber numberWithInt:0] forKey:[NSNumber numberWithShort:1101]];

        TestMutableShortIntD *_do;
        TestMutableShortIntD *ro = [p opShortIntD:di1 p2:di2 p3:&_do];

	test([_do isEqualToDictionary:di1]);
	test([ro count] == 4);
	test([[ro objectForKey:[NSNumber numberWithShort:110]] intValue] == -1);
	test([[ro objectForKey:[NSNumber numberWithShort:111]] intValue] == -100);
	test([[ro objectForKey:[NSNumber numberWithShort:1100]] intValue] == 123123);
	test([[ro objectForKey:[NSNumber numberWithShort:1101]] intValue] == 0);
    }

    {
        TestMutableLongFloatD *di1 = [TestMutableLongFloatD dictionary];
	[di1 setObject:[NSNumber numberWithFloat:-1.1f] forKey:[NSNumber numberWithLong:999999110]];
	[di1 setObject:[NSNumber numberWithFloat:123123.2f] forKey:[NSNumber numberWithLong:999999111]];
        TestMutableLongFloatD *di2 = [TestMutableLongFloatD dictionary];
	[di2 setObject:[NSNumber numberWithFloat:-1.1f] forKey:[NSNumber numberWithLong:999999110]];
	[di2 setObject:[NSNumber numberWithFloat:-100.4f] forKey:[NSNumber numberWithLong:999999120]];
	[di2 setObject:[NSNumber numberWithFloat:0.5f] forKey:[NSNumber numberWithLong:999999130]];

        TestMutableLongFloatD *_do;
        TestMutableLongFloatD *ro = [p opLongFloatD:di1 p2:di2 p3:&_do];

	test([_do isEqualToDictionary:di1]);
	test([ro count] == 4);
	test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999110]] floatValue] == -1.1f);
	test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999120]] floatValue] == -100.4f);
	test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999111]] floatValue] == 123123.2f);
	test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999130]] floatValue] == 0.5f);
    }

    {
        TestMutableStringStringD *di1 = [TestMutableStringStringD dictionary];
	[di1 setObject:@"abc -1.1" forKey:@"foo"];
	[di1 setObject:@"abc 123123.2" forKey:@"bar"];
        TestMutableStringStringD *di2 = [TestMutableStringStringD dictionary];
	[di2 setObject:@"abc -1.1" forKey:@"foo"];
	[di2 setObject:@"abc -100.4" forKey:@"FOO"];
	[di2 setObject:@"abc 0.5" forKey:@"BAR"];

        TestMutableStringStringD *_do;
        TestMutableStringStringD *ro = [p opStringStringD:di1 p2:di2 p3:&_do];

	test([_do isEqualToDictionary:di1]);
	test([ro count] == 4);
	test([[ro objectForKey:@"foo"] isEqualToString:@"abc -1.1"]);
	test([[ro objectForKey:@"FOO"] isEqualToString:@"abc -100.4"]);
	test([[ro objectForKey:@"bar"] isEqualToString:@"abc 123123.2"]);
	test([[ro objectForKey:@"BAR"] isEqualToString:@"abc 0.5"]);
    }

    {
        TestMutableStringMyEnumD *di1 = [TestMutableStringMyEnumD dictionary];
	[di1 setObject:[NSNumber numberWithInt:Testenum1] forKey:@"abc"];
	[di1 setObject:[NSNumber numberWithInt:Testenum2] forKey:@""];
        TestMutableStringMyEnumD *di2 = [TestMutableStringMyEnumD dictionary];
	[di2 setObject:[NSNumber numberWithInt:Testenum1] forKey:@"abc"];
	[di2 setObject:[NSNumber numberWithInt:Testenum3] forKey:@"querty"];
	[di2 setObject:[NSNumber numberWithInt:Testenum2] forKey:@"Hello!!"];

        TestMutableStringMyEnumD *_do;
        TestMutableStringMyEnumD *ro = [p opStringMyEnumD:di1 p2:di2 p3:&_do];

	test([_do isEqualToDictionary:di1]);
	test([ro count] == 4);
	test([[ro objectForKey:@"abc"] intValue] == Testenum1);
	test([[ro objectForKey:@"querty"] intValue] == Testenum3);
	test([[ro objectForKey:@""] intValue] == Testenum2);
	test([[ro objectForKey:@"Hello!!"] intValue] == Testenum2);
    }

    {
        const int lengths[] = { 0, 1, 2, 126, 127, 128, 129, 253, 254, 255, 256, 257, 1000 };

	int l;
        for(l = 0; l != sizeof(lengths) / sizeof(*lengths); ++l)
        {
            TestMutableIntS *s = [TestMutableIntS dataWithLength:(lengths[l] * sizeof(ICEInt))];
	    ICEInt *ip = (ICEInt *)[s bytes];
	    int i;
            for(i = 0; i < lengths[l]; ++i)
            {
                *ip++ = i;
            }
            TestIntS *r = [p opIntS:s];
            test([r length] == lengths[l] * sizeof(ICEInt));
            const ICEInt *rp = [r bytes];
	    int j;
            for(j = 0; j < [r length] / sizeof(ICEInt); ++j)
            {
                test(rp[j] == -j);
            }
        }
    }

    {
        ICEMutableContext *ctx = [ICEMutableContext dictionary];
	[ctx setObject:@"ONE" forKey:@"one"];
	[ctx setObject:@"TWO" forKey:@"two"];
	[ctx setObject:@"THREE" forKey:@"three"];
	{
	    ICEContext *r = [p opContext];
	    test([[p ice_getContext] count] == 0);
	    test(![r isEqual:ctx]);
	}
	{
	    ICEContext *r = [p opContext:ctx];
	    test([[p ice_getContext] count] == 0);
	    test([r isEqual:ctx]);
	}
	{
	    id<TestMyClassPrx> p2 = [TestMyClassPrx checkedCast:[p ice_context:ctx]];
	    test([[p2 ice_getContext] isEqual:ctx]);
	    ICEContext *r = [p2 opContext];
	    test([r isEqual:ctx]);
	    r = [p2 opContext:ctx];
	    test([r isEqual:ctx]);
	}
    }

#if 0 // TODO: commented out until context stuff is implemented
    {
        //
	// Test implicit context propagation
	//

	NSString *impls[] = { @"Shared", @"PerThread" };
	int i;
	for(i = 0; i < 2; i++)
	{
	    ICEInitializationData *initData = [ICEInitializationData initializationData];
	    initData.properties = [[communicator getProperties] clone];
	    [initData.properties setProperty:@"Ice.Implicitcontext" value:impls[i]];

	    id<ICECommunicator> ic = [ICEUtil createCommunicator:initData];

	    ICEMutableContext *ctx = [ICEContext dictionary];
	    [ctx setObject:@"ONE" forKey:@"one"];
	    [ctx setObject:@"TWO" forKey:@"two"];
	    [ctx setObject:@"THREE" forKey:@"three"];

	    id<TestMyClassPrx> p = [TestMyClassPrx uncheckedCast:[ic stringToProxy:@"test:default -p 12010 -t 10000"]];

	    [[ic getImplicitContext] setContext:ctx];
	    test([[[ic getImplicitContext] getContext] isEqual:ctx]);
	    test([[p opContext] isEqual:ctx]);

	    test([[ic getImplicitContext] objectForKey:@"zero"] == nil);
	    [[ic getImplicitContext] setObject:@"ZERO" forKey:@"zero"];
	    test([[[ic getImplicitContext] objectForKey:@"zero"] isEqualToString:@"ZERO"]);

	    ctx = [[ic getImplicitContext] getContext];
	    test([[p opContext] isEqual:ctx]);

	    ICEMutableContext *prxContext = [ICEContext dictionary];
	    [prxContext setObject:@"UN" forKey:@"one"];
	    [prxContext setObject:@"QUATRE" forKey:@"four"];

	    ICEMutableContext *combined = [ICEMutableContext dictionaryWithDictionary:prxContext];
	    [combined setObject:@"UN" forKey:@"one"];

	    p = [TestMyClassPrx uncheckedCast:[p ice_context:prxContext]];

	    [[ic getImplicitContext] setContext:[ICEContext dictionary]];
	    test([[p opContext] isEqualToDictionary:prxContext]);

	    [[ic getImplicitContext] setContext:ctx];
	    test([[p opContext] isEqualToDictionary:combined]);

	    test([[[ic getImplicitContext] objectForKey:@"one"] isEqualToString:@"ONE"]);
	    [[ic getImplicitContext] removeObjectForKey:@"one"];

	    if([impls[i] isEqualToString:@"PerThread"])
	    {
	        // TODO: create thred with context
	    }

	    [[ic getImplicitContext] setContext:[ICEContext dictionary]];
	    [ic destroy];
	}
    }
#endif

    {
        ICEDouble d = 1278312346.0 / 13.0;
	TestMutableDoubleS *ds = [TestMutableDoubleS dataWithLength:(5 * sizeof(ICEDouble))];
	ICEDouble *pb = (ICEDouble *)[ds bytes];
	int i = 5;
	while(i-- > 0)
	{
	    *pb++ = d;
	}
        [p opDoubleMarshaling:d p2:ds];
    }

    //
    // Tests below are for Objective-C only. They test that we do the right thing if NSNull
    // is passed as part of the sequence or dictionary.
    //
    @try
    {
	{
	    TestStringS *s = [p getNSNullStringSeq];
	    test([s count] == 2);
	    test([[s objectAtIndex:0] isEqualToString:@"first"]);
	    test([[s objectAtIndex:1] isEqualToString:@""]);
	}

	{
	    TestAS *s = [p getNSNullASeq];
	    test([s count] == 2);
	    test(((TestA *)[s objectAtIndex:0]).i == 99);
	    test([s objectAtIndex:1] == [NSNull null]);
	}

	{
	    TestStructS *seq = [p getNSNullStructSeq];
	    test([seq count] == 2);
	    TestStructure *s = [seq objectAtIndex:0];
	    test(s.p == nil);
	    test(s.e == Testenum2);
	    test([s.s.s isEqualToString:@"Hello"]);
	    s = [seq objectAtIndex:1];
	    test(s.p == nil);
	    test(s.e == Testenum1);
	    test([s.s.s isEqualToString:@""]);
	}

	{
	    TestStringSS *seq = [p getNSNullStringSeqSeq];
	    test([seq count] == 2);
	    TestStringS *s = [seq objectAtIndex:0];
	    test([s count] == 1);
	    test([(NSString *)[s objectAtIndex:0] isEqualToString:@"first"]);
	    s = [seq objectAtIndex:1];
	    test([s count] == 0);
	}

	{
	    TestStringStringD *d = [p getNSNullStringStringDict];
	    test([d count] == 2);
	    test([(NSString *)[d objectForKey:@"one"] isEqualToString:@"ONE"]);
	    test([(NSString *)[d objectForKey:@"two"] isEqualToString:@""]);
	}

	{
	    @try
	    {
		TestMutableStringStringD *d = [TestMutableStringStringD dictionary];
		[d setObject:@"bad key" forKey:[NSNull null]];
		[p putNSNullStringStringDict:d];
		test(NO);
	    }
	    @catch(ICEMarshalException *)
	    {
	        // Expected
	    }
	}

	{
	    @try
	    {
		TestMutableShortIntD *d = [TestMutableShortIntD dictionary];
		[d setObject:[NSNull null] forKey:[NSNumber numberWithInt:1]];
		[p putNSNullShortIntDict:d];
		test(NO);
	    }
	    @catch(ICEMarshalException *)
	    {
	        // Expected
	    }
	}

	{
	    @try
	    {
		TestMutableStringMyEnumD *d = [TestMutableStringMyEnumD dictionary];
		[d setObject:[NSNull null] forKey:@"key"];
		[p putNSNullStringMyEnumDict:d];
		test(NO);
	    }
	    @catch(ICEMarshalException *)
	    {
	        // Expected
	    }
	}
    }
    @catch(ICEOperationNotExistException *)
    {
       // Client is talking to non-Objective-C server.
    }
}
