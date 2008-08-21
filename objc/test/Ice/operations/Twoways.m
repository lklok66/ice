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
#import <limits.h>
#import <float.h>
    
void
twoways(id<ICECommunicator> communicator, TestMyClassPrx* p)
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
        // TODO: opMyClass
    }

    {
        TestStructure *si1 = [TestStructure structure];
	[si1 setP:p];
	[si1 setE:Testenum3];
	[si1 setS:[TestAnotherStruct anotherStruct]];
	[[si1 s] setS:@"abc"];
	TestStructure *si2 = [TestStructure structure];
	[si2 setP:nil];
	[si2 setE:Testenum2];
	[si2 setS:[TestAnotherStruct anotherStruct]];
	[[si2 s] setS:@"def"];

	TestStructure *so;
	TestStructure *rso = [p opStruct:si1 p2:si2 p3:&so];

	test([rso p] == nil);
	test([rso e] == Testenum2);
	test([[[rso s] s] isEqualToString:@"def"]);
	test([so p] != nil);
	//test([[so p] isEqual:p]);
	test([so e] == Testenum3);
	test([[[so s] s] isEqualToString:@"a new string"]);
	[[so p] opVoid];
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
}
