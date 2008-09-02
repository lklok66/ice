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

#import <Foundation/Foundation.h>

@interface Callback : NSObject
{
    BOOL called;
    NSCondition* cond;
}
-(BOOL) check;
-(void) called;
@end

@implementation Callback
-(id) init
{
    if(![super init])
    {
        return nil;
    }
    cond = [[NSCondition alloc] init];
}
-(void) dealloc
{
    [cond release];
    [super dealloc];
}
-(BOOL) check
{
    [cond lock];
    while(!called)
    {
        if(![cond waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:50]])
        {
            return NO;
        }
    }
    called = NO;
    [cond unlock];
    return YES;
}
-(void) called
{
    [cond lock];
    called = YES;
    [cond signal];
    [cond unlock];
}
-(void) opVoidExResponse
{
    test(NO);
}
-(void) opVoidExException:(ICEException*)ex
{
    test([ex isKindOfClass:[ICENoEndpointException class]]);
    [self called];
}
-(void) opVoidResponse
{
    [self called];
}
-(void) opVoidException:(ICEException*)ex
{
    test(NO);
}
-(void) opByteExResponse:(ICEByte)ret p3:(ICEByte)p3
{
    test(NO);
}
-(void) opByteExException:(ICEException*)ex
{
    test([ex isKindOfClass:[ICENoEndpointException class]]);
    [self called];
}
-(void) opByteResponse:(ICEByte)ret p3:(ICEByte)p3
{
    [self called];
}
-(void) opByteException:(ICEException*)ex
{
    test(NO);
}
-(void) opBoolResponse:(BOOL)r p3:(BOOL)b
{
    test(b);
    test(!r);
    [self called];
}
-(void) opBoolException:(ICEException*)ex
{
    test(NO);
}
-(void) opShortIntLongResponse:(ICELong)r p4:(ICEShort)s p5:(ICEInt)i p6:(ICELong)l
{
    test(s == 10);
    test(i == 11);
    test(l == 12);
    test(r == 12);
    [self called];
}
-(void) opShortIntLongException:(ICEException*)ex
{
    test(NO);
}
-(void) opFloatDoubleResponse:(ICEDouble)r p3:(ICEFloat)f p4:(ICEDouble)d
{
    test(f == 3.14f);
    test(d == 1.1E10);
    test(r == 1.1E10);
    [self called];
}
-(void) opFloatDoubleException:(ICEException*)ex
{
    test(NO);
}
-(void) opStringResponse:(NSString*)r p3:(NSString*)s
{
    test([s isEqualToString:@"world hello"]);
    test([r isEqualToString:@"hello world"]);
    [self called];
}

-(void) opStringException:(ICEException*)ex
{
    test(NO);
};

-(void) opMyEnumResponse:(TestMyEnum)r p2:(TestMyEnum)e
{
    test(e == Testenum2);
    test(r == Testenum3);
    [self called];
}

-(void) opMyEnumException:(ICEException*)ex
{
    test(NO);
}

// -(void) opMyClassResponse:(id<TestMyClassPrx>)r c1:(id<TestMyClassPrx>)c1 c2:(id<TestMyClassPrx>)c2
// {
//     test([c1 ice_getIdentity]:isEqual:[[c1 ice_getCommunicator] stringToIdentity:@"test"]);
//     test([c2 ice_getIdentity]:isEqual:[[c1 ice_getCommunicator] stringToIdentity:@"noSuchIdentity"]);
//     test([r ice_getIdentity]:isEqual:[[c1 ice_getCommunicator] stringToIdentity:@"test"]);
//     // We can't do the callbacks below in connection serialization mode.
//     if([[[c1 ice_getCommunicator] getProperties] getPropertyAsInt:@"Ice.ThreadPool.Client.Serialize"])
//     {
//         [r opVoid];
//         [c1 opVoid];
//         @try
//         {
//             [c2 opVoid];
//             test(NO);
//         }
//         @catch(ICEObjectNotExistException*)
//         {
//         }
//     }
//     [self called];
// }

-(void) opMyClassException:(ICEException*)ex
{
    test(NO);
}

-(void) opStructResponse:(TestStructure*)rso p3:(TestStructure*)so
{
    test(rso.p == nil);
    test(rso.e == Testenum2);
    test([rso.s.s isEqualToString:@"def"]);
    test([so e] == Testenum3);
    test(so.p != nil);
    test([so.s.s isEqualToString:@"a new string"]);
    // We can't do the callbacks below in connection serialization mode.
    if([[[so.p ice_getCommunicator] getProperties] getPropertyAsInt:@"Ice.ThreadPool.Client.Serialize"])
    {
        [so.p opVoid];
    }
    [self called];
}

-(void) opStructException:(ICEException*)ex
{
    test(NO);
}

-(void) opByteSResponse:(TestByteS*)rso p3:(TestByteS*)bso
{
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
    [self called];
}

-(void) opByteSException:(ICEException*)ex
{
    test(NO);
}

-(void) opBoolSResponse:(TestBoolS*)rso p3:(TestBoolS*)bso
{
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
    [self called];
}

-(void) opBoolSException:(ICEException*)ex
{
    test(NO);
}

-(void) opShortIntLongSResponse:(TestLongS*)rso p4:(TestShortS*)sso p5:(TestIntS*)iso p6:(TestLongS*)lso
{
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
    [self called];
}

-(void) opShortIntLongSException:(ICEException*)ex
{
    test(NO);
}

-(void) opFloatDoubleSResponse:(TestDoubleS*)rso p3:(TestFloatS*)fso p4:(TestDoubleS*)dso
{
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
    [self called];
}

-(void) opFloatDoubleSException:(ICEException*)ex
{
    test(NO);
}

-(void) opStringSResponse:(TestStringS*)rso p3:(TestStringS*)sso
{
    test([sso count] == 4);
    test([[sso objectAtIndex:0] isEqualToString:@"abc"]);
    test([[sso objectAtIndex:1] isEqualToString:@"de"]);
    test([[sso objectAtIndex:2] isEqualToString:@"fghi"]);
    test([[sso objectAtIndex:3] isEqualToString:@"xyz"]);
    test([rso count] == 3);
    test([[rso objectAtIndex:0] isEqualToString:@"fghi"]);
    test([[rso objectAtIndex:1] isEqualToString:@"de"]);
    test([[rso objectAtIndex:2] isEqualToString:@"abc"]);
    [self called];
}

-(void) opStringSException:(ICEException*)ex
{
    test(NO);
}

-(void) opByteSSResponse:(TestByteSS*)rso p3:(TestByteSS*)bso
{
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
    [self called];
}

-(void) opByteSSException:(ICEException*)ex
{
    test(NO);
}

-(void) opBoolSSResponse:(TestBoolSS*)sso p3:(TestBoolSS*)bso
{
    [self called];
}

-(void) opBoolSSException:(ICEException*)ex
{
    test(NO);
}

-(void) opShortIntLongSSResponse:(TestLongSS*)a p4:(TestShortSS*)p4 p5:(TestIntSS*)p5 p6:(TestLongSS*)p6
{
    [self called];
}

-(void) opShortIntLongSSException:(ICEException*)ex
{
    test(NO);
}

-(void) opFloatDoubleSSResponse:(TestDoubleSS*)rso p3:(TestFloatSS*)fso p4:(TestDoubleSS*)dso
{
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
    [self called];
}

-(void) opFloatDoubleSSException:(ICEException*)ex
{
    test(NO);
}

-(void) opStringSSResponse:(TestStringSS*)rso p3:(TestStringSS*)sso
{
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
    [self called];
}

-(void) opStringSSException:(ICEException*)ex
{
    test(NO);
}

-(void) opStringSSSResponse:(TestStringSS*)rsso p3:(TestStringSS*)ssso
{
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
    [self called];
}
-(void) opStringSSSException:(ICEException*)ex
{
    test(NO);
}
@end

void
twowaysAMI(id<ICECommunicator> communicator, id<TestMyClassPrx> p)
{
    {
        // Check that a call to a void operation raises NoEndpointException
        // in the ice_exception() callback instead of at the point of call.
        id<TestMyClassPrx> indirect = [TestMyClassPrx uncheckedCast:[p ice_adapterId:@"dummy"]];
        Callback* cb = [[Callback alloc] init];
        @try 
        {
            test(![indirect opVoid_async:cb response:@selector(opVoidExResponse) 
                            exception:@selector(opVoidExException:)]);
        }
        @catch(ICEException*)
        {
            test(false);
        }
        test([cb check]);
        [cb release];
    }

    {
        // Check that a call to a twoway operation raises NoEndpointException
        // in the ice_exception() callback instead of at the point of call.
        id<TestMyClassPrx> indirect = [TestMyClassPrx uncheckedCast:[p ice_adapterId:@"dummy"]];
        Callback* cb = [[Callback alloc] init];
        @try
        {
            test(![indirect opByte_async:cb response:@selector(opByteExResponse:p3:) 
                            exception:@selector(opByteExException:) p1:0 p2:0]);
        }
        @catch(ICEException*)
        {
            test(NO);
        }
        test([cb check]);
        [cb release];
    }

    {
        Callback* cb = [[Callback alloc] init];
        [p opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)];
        test([cb check]);
        // Let's check if we can reuse the same callback object for another call.
        [p opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)];
        test([cb check]);
        [cb release];
    }

    {
        // Check that CommunicatorDestroyedException is raised directly.
        ICEInitializationData* initData = [[ICEInitializationData alloc] init];
        [initData setProperties:[[communicator getProperties] clone]];
        id<ICECommunicator> ic = [ICEUtil createCommunicator:initData];
        id<ICEObjectPrx> obj = [ic stringToProxy:[p ice_toString]];
        id<TestMyClassPrx> p2 = [TestMyClassPrx checkedCast:obj];

        [ic destroy];

        Callback* cb = [[Callback alloc] init];
        @try
        {
            test(![p2 opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)]);
            test(NO);
        }
        @catch(ICECommunicatorDestroyedException*)
        {
            // Expected.
        }
        [cb release];
    }

    { 
        Callback* cb = [[Callback alloc] init];
        [p opByte_async:cb response:@selector(opByteResponse:p3:) exception:@selector(opByteException:) 
           p1:(ICEByte)0xff p2:(ICEByte)0x0f];
        test([cb check]);
        [cb release];
    }

    {
        Callback* cb = [[Callback alloc] init];
        [p opBool_async:cb response:@selector(opBoolResponse:p3:) exception:@selector(opBoolException:) p1:YES p2:NO];
        test([cb check]);
        [cb release];
    }

    {
        Callback* cb = [[Callback alloc] init];
        [p opShortIntLong_async:cb response:@selector(opShortIntLongResponse:p4:p5:p6:) exception:@selector(opShortIntLongException:) p1:10 p2:11 p3:12];
        test([cb check]);
        [cb release];
    }

    {
        Callback* cb = [[Callback alloc] init];
        [p opFloatDouble_async:cb response:@selector(opFloatDoubleResponse:p3:p4:) exception:@selector(opFloatDoubleException:) p1:3.14f p2:1.1E10];
        test([cb check]);
        // Let's check if we can reuse the same callback object for another call.
        [p opFloatDouble_async:cb response:@selector(opFloatDoubleResponse:p3:p4:) exception:@selector(opFloatDoubleException:) p1:3.14f p2:1.1E10];
        test([cb check]);
        [cb release];
    }

    {
        Callback* cb = [[Callback alloc] init];
        [p opString_async:cb response:@selector(opStringResponse:p3:) exception:@selector(opStringException:) p1:@"hello" p2:@"world"];
        test([cb check]);
        [cb release];
    }

    {
        Callback* cb = [[Callback alloc] init];
        [p opMyEnum_async:cb response:@selector(opMyEnumResponse:p2:) exception:@selector(opMyEnumException:) p1:Testenum2];
        test([cb check]);
        [cb release];
    }

//     {
//         Callback* cb = [[Callback alloc] init];
//         [p opMyClass_async:cb response:@selector(opMyClassResponse:p2:p3:) exception:@selector(opMyClassException:) p1:p];
//         test([cb check]);
//         [cb release];
//     }

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
        
        Callback* cb = [[Callback alloc] init];
        [p opStruct_async:cb response:@selector(opStructResponse:p3:) exception:@selector(opStructException:) p1:si1 p2:si2];
        test([cb check]);
        [cb release];
    }

    {
	ICEByte buf1[] = { 0x01, 0x11, 0x12, 0x22 };
	ICEByte buf2[] = { 0xf1, 0xf2, 0xf3, 0xf4 };
        TestMutableByteS *bsi1 = [[[TestMutableByteS alloc] init] autorelease];
        TestMutableByteS *bsi2 = [[[TestMutableByteS alloc] init] autorelease];
	[bsi1 appendBytes:buf1 length:sizeof(buf1)];
	[bsi2 appendBytes:buf2 length:sizeof(buf2)];

        Callback* cb = [[Callback alloc] init];
        [p opByteS_async:cb response:@selector(opByteSResponse:p3:) exception:@selector(opByteSException:) p1:bsi1 p2:bsi2];
        test([cb check]);
        [cb release];
    }

    {
	BOOL buf1[] = { YES, YES, NO };
	BOOL buf2[] = { NO };
        TestMutableBoolS *bsi1 = [[[TestMutableBoolS alloc] init] autorelease];
        TestMutableBoolS *bsi2 = [[[TestMutableBoolS alloc] init] autorelease];
	[bsi1 appendBytes:buf1 length:sizeof(buf1)];
	[bsi2 appendBytes:buf2 length:sizeof(buf2)];

        Callback* cb = [[Callback alloc] init];
        [p opBoolS_async:cb response:@selector(opBoolSResponse:p3:) exception:@selector(opBoolSException:) p1:bsi1 p2:bsi2];
        test([cb check]);
        [cb release];
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

        Callback* cb = [[Callback alloc] init];
        [p opShortIntLongS_async:cb response:@selector(opShortIntLongSResponse:p4:p5:p6:) exception:@selector(opShortIntLongSException:) p1:ssi p2:isi p3:lsi];
        test([cb check]);
        [cb release];
    }

    {
	ICEFloat buf1[] = { 3.14f, 1.11f };
	ICEDouble buf2[] = { 1.1E10, 1.2E10, 1.3E10 };
        TestMutableFloatS *fsi = [[[TestMutableFloatS alloc] init] autorelease];
        TestMutableDoubleS *dsi = [[[TestMutableDoubleS alloc] init] autorelease];
	[fsi appendBytes:buf1 length:sizeof(buf1)];
	[dsi appendBytes:buf2 length:sizeof(buf2)];

        Callback* cb = [[Callback alloc] init];
        [p opFloatDoubleS_async:cb response:@selector(opFloatDoubleSResponse:p3:p4:) exception:@selector(opFloatDoubleSException:) p1:fsi p2:dsi];
        test([cb check]);
        [cb release];
    }

    {
        TestMutableStringS *ssi1 = [TestMutableStringS arrayWithCapacity:3];
        TestMutableStringS *ssi2 = [TestMutableStringS arrayWithCapacity:1];

	[ssi1 addObject:@"abc"];
	[ssi1 addObject:@"de"];
	[ssi1 addObject:@"fghi"];

	[ssi2 addObject:@"xyz"];

        Callback* cb = [[Callback alloc] init];
        [p opStringS_async:cb response:@selector(opStringSResponse:p3:) exception:@selector(opStringSException:) p1:ssi1 p2:ssi2];
        test([cb check]);
        [cb release];
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

        Callback* cb = [[Callback alloc] init];
        [p opByteSS_async:cb response:@selector(opByteSSResponse:p3:) exception:@selector(opByteSSException:) p1:bsi1 p2:bsi2];
        test([cb check]);
        [cb release];
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

        Callback* cb = [[Callback alloc] init];
        [p opFloatDoubleSS_async:cb response:@selector(opFloatDoubleSSResponse:p3:p4:) exception:@selector(opFloatDoubleSSException:) p1:fsi p2:dsi];
        test([cb check]);
        [cb release];
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

        Callback* cb = [[Callback alloc] init];
        [p opStringSS_async:cb response:@selector(opStringSSResponse:p3:) exception:@selector(opStringSSException:) p1:ssi1 p2:ssi2];
        test([cb check]);
        [cb release];
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

        Callback* cb = [[Callback alloc] init];
        [p opStringSSS_async:cb response:@selector(opStringSSSResponse:p3:) exception:@selector(opStringSSSException:) p1:sssi1 p2:sssi2];
        test([cb check]);
        [cb release];
    }
}
