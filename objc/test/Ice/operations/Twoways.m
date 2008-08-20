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
        NSMutableString *s;
	NSMutableString *r;

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
}
