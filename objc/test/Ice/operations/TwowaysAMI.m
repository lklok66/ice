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
#import <OperationsTest.h>
#import <limits.h>
#import <float.h>

#import <Foundation/Foundation.h>

@interface TestOperationsCallback : NSObject
{
    BOOL called;
    NSCondition* cond;
}
-(BOOL) check;
-(void) called;
@end

@implementation TestOperationsCallback
-(id) init
{
    if(![super init])
    {
        return nil;
    }
    cond = [[NSCondition alloc] init];
    return self;
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

-(void) opMyEnumResponse:(TestOperationsMyEnum)r p2:(TestOperationsMyEnum)e
{
    test(e == TestOperationsenum2);
    test(r == TestOperationsenum3);
    [self called];
}

-(void) opMyEnumException:(ICEException*)ex
{
    test(NO);
}

-(void) opMyClassResponse:(id<TestOperationsMyClassPrx>)r p2:(id<TestOperationsMyClassPrx>)c1 p3:(id<TestOperationsMyClassPrx>)c2
{
    test([[c1 ice_getIdentity] isEqual:[[c1 ice_getCommunicator] stringToIdentity:@"test"]]);
    test([[c2 ice_getIdentity] isEqual:[[c1 ice_getCommunicator] stringToIdentity:@"noSuchIdentity"]]);
    test([[r ice_getIdentity] isEqual:[[c1 ice_getCommunicator] stringToIdentity:@"test"]]);
    // We can't do the callbacks below in connection serialization mode.
    if([[[c1 ice_getCommunicator] getProperties] getPropertyAsInt:@"Ice.ThreadPool.Client.Serialize"])
    {
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
    }
    [self called];
}

-(void) opMyClassException:(ICEException*)ex
{
    test(NO);
}

-(void) opStructResponse:(TestOperationsStructure*)rso p3:(TestOperationsStructure*)so
{
    test(rso.p == nil);
    test(rso.e == TestOperationsenum2);
    test([rso.s.s isEqualToString:@"def"]);
    test([so e] == TestOperationsenum3);
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

-(void) opByteSResponse:(TestOperationsByteS*)rso p3:(TestOperationsByteS*)bso
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

-(void) opBoolSResponse:(TestOperationsBoolS*)rso p3:(TestOperationsBoolS*)bso
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

-(void) opShortIntLongSResponse:(TestOperationsLongS*)rso p4:(TestOperationsShortS*)sso p5:(TestOperationsIntS*)iso p6:(TestOperationsLongS*)lso
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

-(void) opFloatDoubleSResponse:(TestOperationsDoubleS*)rso p3:(TestOperationsFloatS*)fso p4:(TestOperationsDoubleS*)dso
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

-(void) opStringSResponse:(TestOperationsStringS*)rso p3:(TestOperationsStringS*)sso
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

-(void) opByteSSResponse:(TestOperationsByteSS*)rso p3:(TestOperationsByteSS*)bso
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

-(void) opBoolSSResponse:(TestOperationsBoolSS*)sso p3:(TestOperationsBoolSS*)bso
{
    [self called];
}

-(void) opBoolSSException:(ICEException*)ex
{
    test(NO);
}

-(void) opShortIntLongSSResponse:(TestOperationsLongSS*)a p4:(TestOperationsShortSS*)p4 p5:(TestOperationsIntSS*)p5 p6:(TestOperationsLongSS*)p6
{
    [self called];
}

-(void) opShortIntLongSSException:(ICEException*)ex
{
    test(NO);
}

-(void) opFloatDoubleSSResponse:(TestOperationsDoubleSS*)rso p3:(TestOperationsFloatSS*)fso p4:(TestOperationsDoubleSS*)dso
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

-(void) opStringSSResponse:(TestOperationsStringSS*)rso p3:(TestOperationsStringSS*)sso
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

-(void) opStringSSSResponse:(TestOperationsStringSS*)rsso p3:(TestOperationsStringSS*)ssso
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
-(void) opByteBoolDResponse:(TestOperationsMutableByteBoolD*)ro p3:(TestOperationsMutableByteBoolD*)_do
{
    test([_do count] == 2);
    test([[_do objectForKey:[NSNumber numberWithUnsignedChar:10]] boolValue] == YES);
    test([[_do objectForKey:[NSNumber numberWithUnsignedChar:100]] boolValue] == NO);
    test([ro count] == 4);
    test([[ro objectForKey:[NSNumber numberWithUnsignedChar:10]] boolValue] == YES);
    test([[ro objectForKey:[NSNumber numberWithUnsignedChar:11]] boolValue] == NO);
    test([[ro objectForKey:[NSNumber numberWithUnsignedChar:100]] boolValue] == NO);
    test([[ro objectForKey:[NSNumber numberWithUnsignedChar:101]] boolValue] == YES);
    [self called];
}
-(void) opByteBoolDException:(ICEException*)ex
{
    test(NO);
}
-(void) opShortIntDResponse:(TestOperationsShortIntD*)ro p3:(TestOperationsShortIntD*)_do
{
    test([_do count] == 2);
    test([[_do objectForKey:[NSNumber numberWithShort:110]] intValue] == -1);
    test([[_do objectForKey:[NSNumber numberWithShort:1100]] intValue] == 123123);
    test([ro count] == 4);
    test([[ro objectForKey:[NSNumber numberWithShort:110]] intValue] == -1);
    test([[ro objectForKey:[NSNumber numberWithShort:111]] intValue] == -100);
    test([[ro objectForKey:[NSNumber numberWithShort:1100]] intValue] == 123123);
    test([[ro objectForKey:[NSNumber numberWithShort:1101]] intValue] == 0);
    [self called];
}
-(void) opShortIntDException:(ICEException*)ex
{
    test(NO);
}
-(void) opLongFloatDResponse:(TestOperationsLongFloatD*)ro p3:(TestOperationsLongFloatD*)_do
{
    test([_do count] == 2);
    test((ICEFloat)[[_do objectForKey:[NSNumber numberWithLong:999999110]] floatValue] == -1.1f);
    test((ICEFloat)[[_do objectForKey:[NSNumber numberWithLong:999999111]] floatValue] == 123123.2f);
    test([ro count] == 4);
    test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999110]] floatValue] == -1.1f);
    test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999120]] floatValue] == -100.4f);
    test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999111]] floatValue] == 123123.2f);
    test((ICEFloat)[[ro objectForKey:[NSNumber numberWithLong:999999130]] floatValue] == 0.5f);
    [self called];
}
-(void) opLongFloatDException:(ICEException*)ex
{
    test(NO);
}
-(void) opStringStringDResponse:(TestOperationsStringStringD*)ro p3:(TestOperationsStringStringD*)_do
{
    test([_do count] == 2);
    test([[_do objectForKey:@"foo"] isEqualToString:@"abc -1.1"]);
    test([[_do objectForKey:@"bar"] isEqualToString:@"abc 123123.2"]);
    test([ro count] == 4);
    test([[ro objectForKey:@"foo"] isEqualToString:@"abc -1.1"]);
    test([[ro objectForKey:@"FOO"] isEqualToString:@"abc -100.4"]);
    test([[ro objectForKey:@"bar"] isEqualToString:@"abc 123123.2"]);
    test([[ro objectForKey:@"BAR"] isEqualToString:@"abc 0.5"]);
    [self called];
}
-(void) opStringStringDException:(ICEException*)ex
{
    test(NO);
}
-(void) opStringMyEnumDResponse:(TestOperationsStringMyEnumD*)ro p3:(TestOperationsStringMyEnumD*)_do
{
    test([_do count] == 2);
    test([[_do objectForKey:@"abc"] intValue] == TestOperationsenum1);
    test([[_do objectForKey:@""] intValue] == TestOperationsenum2);
    test([ro count] == 4);
    test([[ro objectForKey:@"abc"] intValue] == TestOperationsenum1);
    test([[ro objectForKey:@"querty"] intValue] == TestOperationsenum3);
    test([[ro objectForKey:@""] intValue] == TestOperationsenum2);
    test([[ro objectForKey:@"Hello!!"] intValue] == TestOperationsenum2);
    [self called];
}
-(void) opStringMyEnumDException:(ICEException*)ex
{
    test(NO);
}
-(void) opIntSResponse:(TestOperationsIntS*)r
{
    const ICEInt *rp = [r bytes];
    int j;
    for(j = 0; j < [r length] / sizeof(ICEInt); ++j)
    {
        test(rp[j] == -j);
    }
    [self called];
}
-(void) opIntSException:(ICEException*)ex
{
    test(NO);
}
-(void) opEmptyContextResponse:(ICEContext*)ctx
{
    test([ctx count] == 0);
    [self called];
}
-(void) opNonEmptyContextResponse:(ICEContext*)ctx
{
    test([ctx count] == 3);
    test([[ctx objectForKey:@"one"] isEqualToString:@"ONE"]);
    test([[ctx objectForKey:@"two"] isEqualToString:@"TWO"]);
    test([[ctx objectForKey:@"three"] isEqualToString:@"THREE"]);
    [self called];
}
-(void) opContextException:(ICEException*)ex
{
    test(NO);
}
-(void) opDoubleMarshalingResponse
{
    [self called];
}
-(void) opDoubleMarshalingException:(ICEException*)ex
{
    test(NO);
}
@end

void
twowaysAMI(id<ICECommunicator> communicator, id<TestOperationsMyClassPrx> p)
{
    {
        // Check that a call to a void operation raises NoEndpointException
        // in the ice_exception() callback instead of at the point of call.
        id<TestOperationsMyClassPrx> indirect = [TestOperationsMyClassPrx uncheckedCast:[p ice_adapterId:@"dummy"]];
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
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
    }

    {
        // Check that a call to a twoway operation raises NoEndpointException
        // in the ice_exception() callback instead of at the point of call.
        id<TestOperationsMyClassPrx> indirect = [TestOperationsMyClassPrx uncheckedCast:[p ice_adapterId:@"dummy"]];
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
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
    }

    {
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)];
        test([cb check]);
        // Let's check if we can reuse the same callback object for another call.
        [p opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)];
        test([cb check]);
    }

    {
        // Check that CommunicatorDestroyedException is raised directly.
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        [initData setProperties:[[communicator getProperties] clone]];
        id<ICECommunicator> ic = [ICEUtil createCommunicator:initData];
        id<ICEObjectPrx> obj = [ic stringToProxy:[p ice_toString]];
        id<TestOperationsMyClassPrx> p2 = [TestOperationsMyClassPrx checkedCast:obj];

        [ic destroy];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        @try
        {
            test(![p2 opVoid_async:cb response:@selector(opVoidResponse) exception:@selector(opVoidException:)]);
            test(NO);
        }
        @catch(ICECommunicatorDestroyedException*)
        {
            // Expected.
        }
    }

    { 
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opByte_async:cb response:@selector(opByteResponse:p3:) exception:@selector(opByteException:) 
           p1:(ICEByte)0xff p2:(ICEByte)0x0f];
        test([cb check]);
    }

    {
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opBool_async:cb response:@selector(opBoolResponse:p3:) exception:@selector(opBoolException:) p1:YES p2:NO];
        test([cb check]);
    }

    {
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opShortIntLong_async:cb response:@selector(opShortIntLongResponse:p4:p5:p6:) exception:@selector(opShortIntLongException:) p1:10 p2:11 p3:12];
        test([cb check]);
    }

    {
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opFloatDouble_async:cb response:@selector(opFloatDoubleResponse:p3:p4:) exception:@selector(opFloatDoubleException:) p1:3.14f p2:1.1E10];
        test([cb check]);
        // Let's check if we can reuse the same callback object for another call.
        [p opFloatDouble_async:cb response:@selector(opFloatDoubleResponse:p3:p4:) exception:@selector(opFloatDoubleException:) p1:3.14f p2:1.1E10];
        test([cb check]);
    }

    {
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opString_async:cb response:@selector(opStringResponse:p3:) exception:@selector(opStringException:) p1:@"hello" p2:@"world"];
        test([cb check]);
    }

    {
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opMyEnum_async:cb response:@selector(opMyEnumResponse:p2:) exception:@selector(opMyEnumException:) p1:TestOperationsenum2];
        test([cb check]);
    }
    
    {
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opMyClass_async:cb response:@selector(opMyClassResponse:p2:p3:) exception:@selector(opMyClassException:) p1:p];
        test([cb check]);
    }

    {
        TestOperationsStructure *si1 = [TestOperationsStructure structure];
	si1.p = p;
	si1.e = TestOperationsenum3;
	si1.s = [TestOperationsAnotherStruct anotherStruct];
	si1.s.s = @"abc";
	TestOperationsStructure *si2 = [TestOperationsStructure structure];
	si2.p = nil;
	si2.e = TestOperationsenum2;
	si2.s = [TestOperationsAnotherStruct anotherStruct];
	si2.s.s = @"def";
        
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opStruct_async:cb response:@selector(opStructResponse:p3:) exception:@selector(opStructException:) p1:si1 p2:si2];
        test([cb check]);
    }

    {
	ICEByte buf1[] = { 0x01, 0x11, 0x12, 0x22 };
	ICEByte buf2[] = { 0xf1, 0xf2, 0xf3, 0xf4 };
        TestOperationsMutableByteS *bsi1 = [[[TestOperationsMutableByteS alloc] init] autorelease];
        TestOperationsMutableByteS *bsi2 = [[[TestOperationsMutableByteS alloc] init] autorelease];
	[bsi1 appendBytes:buf1 length:sizeof(buf1)];
	[bsi2 appendBytes:buf2 length:sizeof(buf2)];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opByteS_async:cb response:@selector(opByteSResponse:p3:) exception:@selector(opByteSException:) p1:bsi1 p2:bsi2];
        test([cb check]);
    }

    {
	BOOL buf1[] = { YES, YES, NO };
	BOOL buf2[] = { NO };
        TestOperationsMutableBoolS *bsi1 = [[[TestOperationsMutableBoolS alloc] init] autorelease];
        TestOperationsMutableBoolS *bsi2 = [[[TestOperationsMutableBoolS alloc] init] autorelease];
	[bsi1 appendBytes:buf1 length:sizeof(buf1)];
	[bsi2 appendBytes:buf2 length:sizeof(buf2)];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opBoolS_async:cb response:@selector(opBoolSResponse:p3:) exception:@selector(opBoolSException:) p1:bsi1 p2:bsi2];
        test([cb check]);
    }

    {
	ICEShort buf1[] = { 1, 2, 3 };
	ICEInt buf2[] = { 5, 6, 7, 8 };
	ICELong buf3[] = { 10, 30, 20 };
        TestOperationsMutableShortS *ssi = [[[TestOperationsMutableShortS alloc] init] autorelease];
	TestOperationsMutableIntS *isi = [[[TestOperationsMutableIntS alloc] init] autorelease];
	TestOperationsMutableLongS *lsi = [[[TestOperationsMutableLongS alloc] init] autorelease];
	[ssi appendBytes:buf1 length:sizeof(buf1)];
	[isi appendBytes:buf2 length:sizeof(buf2)];
	[lsi appendBytes:buf3 length:sizeof(buf3)];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opShortIntLongS_async:cb response:@selector(opShortIntLongSResponse:p4:p5:p6:) exception:@selector(opShortIntLongSException:) p1:ssi p2:isi p3:lsi];
        test([cb check]);
    }

    {
	ICEFloat buf1[] = { 3.14f, 1.11f };
	ICEDouble buf2[] = { 1.1E10, 1.2E10, 1.3E10 };
        TestOperationsMutableFloatS *fsi = [[[TestOperationsMutableFloatS alloc] init] autorelease];
        TestOperationsMutableDoubleS *dsi = [[[TestOperationsMutableDoubleS alloc] init] autorelease];
	[fsi appendBytes:buf1 length:sizeof(buf1)];
	[dsi appendBytes:buf2 length:sizeof(buf2)];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opFloatDoubleS_async:cb response:@selector(opFloatDoubleSResponse:p3:p4:) exception:@selector(opFloatDoubleSException:) p1:fsi p2:dsi];
        test([cb check]);
    }

    {
        TestOperationsMutableStringS *ssi1 = [TestOperationsMutableStringS arrayWithCapacity:3];
        TestOperationsMutableStringS *ssi2 = [TestOperationsMutableStringS arrayWithCapacity:1];

	[ssi1 addObject:@"abc"];
	[ssi1 addObject:@"de"];
	[ssi1 addObject:@"fghi"];

	[ssi2 addObject:@"xyz"];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opStringS_async:cb response:@selector(opStringSResponse:p3:) exception:@selector(opStringSException:) p1:ssi1 p2:ssi2];
        test([cb check]);
    }

    {
        TestOperationsMutableByteSS *bsi1 = [TestOperationsMutableByteSS array];
        TestOperationsMutableByteSS *bsi2 = [TestOperationsMutableByteSS array];

	ICEByte b;
	TestOperationsMutableByteS *tmp = [TestOperationsMutableByteS data];

	b = 0x01;
	[tmp appendBytes:&b length:sizeof(b)];
	b = 0x11;
	[tmp appendBytes:&b length:sizeof(b)];
	b = 0x12;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi1 addObject:tmp];

        tmp = [TestOperationsMutableByteS data];
	b = 0xff;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi1 addObject:tmp];

        tmp = [TestOperationsMutableByteS data];
	b = 0x0e;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi2 addObject:tmp];

        tmp = [TestOperationsMutableByteS data];
	b = 0xf2;
	[tmp appendBytes:&b length:sizeof(b)];
	b = 0xf1;
	[tmp appendBytes:&b length:sizeof(b)];
	[bsi2 addObject:tmp];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opByteSS_async:cb response:@selector(opByteSSResponse:p3:) exception:@selector(opByteSSException:) p1:bsi1 p2:bsi2];
        test([cb check]);
    }

    {
        TestOperationsMutableFloatSS *fsi = [TestOperationsMutableFloatSS array];
        TestOperationsMutableDoubleSS *dsi = [TestOperationsMutableDoubleSS array];

	ICEFloat f;
	TestOperationsMutableFloatS *ftmp;

	ftmp = [TestOperationsMutableFloatS data];
	f = 3.14f;
	[ftmp appendBytes:&f length:sizeof(f)];
	[fsi addObject:ftmp];
	ftmp = [TestOperationsMutableFloatS data];
	f = 1.11f;
	[ftmp appendBytes:&f length:sizeof(f)];
	[fsi addObject:ftmp];
	ftmp = [TestOperationsMutableFloatS data];
	[fsi addObject:ftmp];

	ICEDouble d;
	TestOperationsMutableDoubleS *dtmp;

        dtmp = [TestOperationsMutableDoubleS data];
	d = 1.1E10;
	[dtmp appendBytes:&d length:sizeof(d)];
	d = 1.2E10;
	[dtmp appendBytes:&d length:sizeof(d)];
	d = 1.3E10;
	[dtmp appendBytes:&d length:sizeof(d)];
	[dsi addObject:dtmp];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opFloatDoubleSS_async:cb response:@selector(opFloatDoubleSSResponse:p3:p4:) exception:@selector(opFloatDoubleSSException:) p1:fsi p2:dsi];
        test([cb check]);
    }

    {
        TestOperationsMutableStringSS * ssi1 = [TestOperationsMutableStringSS array];
        TestOperationsMutableStringSS * ssi2 = [TestOperationsMutableStringSS array];

	TestOperationsMutableStringS *tmp;

	tmp = [TestOperationsMutableStringS array];
	[tmp addObject:@"abc"];
	[ssi1 addObject:tmp];
	tmp = [TestOperationsMutableStringS array];
	[tmp addObject:@"de"];
	[tmp addObject:@"fghi"];
	[ssi1 addObject:tmp];

	[ssi2 addObject:[TestOperationsStringS array]];
	[ssi2 addObject:[TestOperationsStringS array]];
	tmp = [TestOperationsMutableStringS array];
	[tmp addObject:@"xyz"];
	[ssi2 addObject:tmp];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opStringSS_async:cb response:@selector(opStringSSResponse:p3:) exception:@selector(opStringSSException:) p1:ssi1 p2:ssi2];
        test([cb check]);
    }

    {
        TestOperationsMutableStringSSS *sssi1 = [TestOperationsMutableStringSSS array];
        TestOperationsMutableStringSSS *sssi2 = [TestOperationsMutableStringSSS array];

	TestOperationsMutableStringSS *tmpss;
	TestOperationsMutableStringS *tmps;

	tmps = [TestOperationsMutableStringS array];
        [tmps addObject:@"abc"];
        [tmps addObject:@"de"];
	tmpss = [TestOperationsMutableStringSS array];
	[tmpss addObject:tmps];
	tmps = [TestOperationsMutableStringS array];
        [tmps addObject:@"xyz"];
	[tmpss addObject:tmps];
	[sssi1 addObject:tmpss];
	tmps = [TestOperationsMutableStringS array];
        [tmps addObject:@"hello"];
	tmpss = [TestOperationsMutableStringSS array];
	[tmpss addObject:tmps];
	[sssi1 addObject:tmpss];

	tmps = [TestOperationsMutableStringS array];
        [tmps addObject:@""];
        [tmps addObject:@""];
	tmpss = [TestOperationsMutableStringSS array];
	[tmpss addObject:tmps];
	tmps = [TestOperationsMutableStringS array];
        [tmps addObject:@"abcd"];
	[tmpss addObject:tmps];
	[sssi2 addObject:tmpss];
	tmps = [TestOperationsMutableStringS array];
        [tmps addObject:@""];
	tmpss = [TestOperationsMutableStringSS array];
	[tmpss addObject:tmps];
	[sssi2 addObject:tmpss];
	tmpss = [TestOperationsMutableStringSS array];
	[sssi2 addObject:tmpss];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opStringSSS_async:cb response:@selector(opStringSSSResponse:p3:) exception:@selector(opStringSSSException:) p1:sssi1 p2:sssi2];
        test([cb check]);
    }

    {
        TestOperationsMutableByteBoolD *di1 = [TestOperationsMutableByteBoolD dictionary];
	[di1 setObject:[NSNumber numberWithBool:YES] forKey:[NSNumber numberWithUnsignedChar:10]];
	[di1 setObject:[NSNumber numberWithBool:NO] forKey:[NSNumber numberWithUnsignedChar:100]];
        TestOperationsMutableByteBoolD *di2 = [TestOperationsMutableByteBoolD dictionary];
	[di2 setObject:[NSNumber numberWithBool:YES] forKey:[NSNumber numberWithUnsignedChar:10]];
	[di2 setObject:[NSNumber numberWithBool:NO] forKey:[NSNumber numberWithUnsignedChar:11]];
	[di2 setObject:[NSNumber numberWithBool:TRUE] forKey:[NSNumber numberWithUnsignedChar:101]];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opByteBoolD_async:cb response:@selector(opByteBoolDResponse:p3:) exception:@selector(opByteBoolDException:) p1:di1 p2:di2];
        test([cb check]);
    }

    {
        TestOperationsMutableShortIntD *di1 = [TestOperationsMutableShortIntD dictionary];
	[di1 setObject:[NSNumber numberWithInt:-1] forKey:[NSNumber numberWithShort:110]];
	[di1 setObject:[NSNumber numberWithInt:123123] forKey:[NSNumber numberWithShort:1100]];
        TestOperationsMutableShortIntD *di2 = [TestOperationsMutableShortIntD dictionary];
	[di2 setObject:[NSNumber numberWithInt:-1] forKey:[NSNumber numberWithShort:110]];
	[di2 setObject:[NSNumber numberWithInt:-100] forKey:[NSNumber numberWithShort:111]];
	[di2 setObject:[NSNumber numberWithInt:0] forKey:[NSNumber numberWithShort:1101]];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opShortIntD_async:cb response:@selector(opShortIntDResponse:p3:) exception:@selector(opShortIntDException:) p1:di1 p2:di2];
        test([cb check]);
    }

    {
        TestOperationsMutableLongFloatD *di1 = [TestOperationsMutableLongFloatD dictionary];
	[di1 setObject:[NSNumber numberWithFloat:-1.1f] forKey:[NSNumber numberWithLong:999999110]];
	[di1 setObject:[NSNumber numberWithFloat:123123.2f] forKey:[NSNumber numberWithLong:999999111]];
        TestOperationsMutableLongFloatD *di2 = [TestOperationsMutableLongFloatD dictionary];
	[di2 setObject:[NSNumber numberWithFloat:-1.1f] forKey:[NSNumber numberWithLong:999999110]];
	[di2 setObject:[NSNumber numberWithFloat:-100.4f] forKey:[NSNumber numberWithLong:999999120]];
	[di2 setObject:[NSNumber numberWithFloat:0.5f] forKey:[NSNumber numberWithLong:999999130]];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opLongFloatD_async:cb response:@selector(opLongFloatDResponse:p3:) exception:@selector(opLongFloatDException:) p1:di1 p2:di2];
        test([cb check]);
    }

    {
        TestOperationsMutableStringStringD *di1 = [TestOperationsMutableStringStringD dictionary];
	[di1 setObject:@"abc -1.1" forKey:@"foo"];
	[di1 setObject:@"abc 123123.2" forKey:@"bar"];
        TestOperationsMutableStringStringD *di2 = [TestOperationsMutableStringStringD dictionary];
	[di2 setObject:@"abc -1.1" forKey:@"foo"];
	[di2 setObject:@"abc -100.4" forKey:@"FOO"];
	[di2 setObject:@"abc 0.5" forKey:@"BAR"];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opStringStringD_async:cb response:@selector(opStringStringDResponse:p3:) exception:@selector(opStringStringDException:) p1:di1 p2:di2];
        test([cb check]);
    }

    {
        TestOperationsMutableStringMyEnumD *di1 = [TestOperationsMutableStringMyEnumD dictionary];
	[di1 setObject:[NSNumber numberWithInt:TestOperationsenum1] forKey:@"abc"];
	[di1 setObject:[NSNumber numberWithInt:TestOperationsenum2] forKey:@""];
        TestOperationsMutableStringMyEnumD *di2 = [TestOperationsMutableStringMyEnumD dictionary];
	[di2 setObject:[NSNumber numberWithInt:TestOperationsenum1] forKey:@"abc"];
	[di2 setObject:[NSNumber numberWithInt:TestOperationsenum3] forKey:@"querty"];
	[di2 setObject:[NSNumber numberWithInt:TestOperationsenum2] forKey:@"Hello!!"];

        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opStringMyEnumD_async:cb response:@selector(opStringMyEnumDResponse:p3:) exception:@selector(opStringMyEnumDException:) p1:di1 p2:di2];
        test([cb check]);
    }

    {
        const int lengths[] = { 0, 1, 2, 126, 127, 128, 129, 253, 254, 255, 256, 257, 1000 };

	int l;
        for(l = 0; l != sizeof(lengths) / sizeof(*lengths); ++l)
        {
            TestOperationsMutableIntS *s = [TestOperationsMutableIntS dataWithLength:(lengths[l] * sizeof(ICEInt))];
	    ICEInt *ip = (ICEInt *)[s bytes];
	    int i;
            for(i = 0; i < lengths[l]; ++i)
            {
                *ip++ = i;
            }
            TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
            [p opIntS_async:cb response:@selector(opIntSResponse:) exception:@selector(opIntSException:) s:s];
            test([cb check]);
        }
    }

    {
        ICEMutableContext *ctx = [ICEMutableContext dictionary];
	[ctx setObject:@"ONE" forKey:@"one"];
	[ctx setObject:@"TWO" forKey:@"two"];
	[ctx setObject:@"THREE" forKey:@"three"];
	{
            TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
            [p opContext_async:cb response:@selector(opEmptyContextResponse:) exception:@selector(opContextException:)];
            test([cb check]);
	}
	{
            TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
            [p opContext_async:cb response:@selector(opNonEmptyContextResponse:) exception:@selector(opContextException:) context:ctx];
            test([cb check]);
	}
	{
	    id<TestOperationsMyClassPrx> p2 = [TestOperationsMyClassPrx checkedCast:[p ice_context:ctx]];
	    test([[p2 ice_getContext] isEqual:ctx]);

            TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
            [p2 opContext_async:cb response:@selector(opNonEmptyContextResponse:) exception:@selector(opContextException:)];
            test([cb check]);

            cb = [[[TestOperationsCallback alloc] init] autorelease];
            [p2 opContext_async:cb response:@selector(opNonEmptyContextResponse:) exception:@selector(opContextException:) context:ctx];
            test([cb check]);
	}
    }

//         {
//             //
//             // TestOperations implicit context propagation
//             //
            
//             string impls[] = {"Shared" :@"PerThread"};
//             for(int i = 0; i < 2; i++)
//             {
//                 ICEInitializationData* initData = [ICEInitializationData initializationData];
//                 [initData setProperties:[[communicator getProperties] clone]];
//                 [[initData properties] setProperty:@"Ice.ImplicitContext" value:@[impls objectForKey:i]];
                
//                 id<ICECommunicator> ic = [ICEUtil createCommunicator:initData];

//                 ICEContext ctx;
//                 [ctx objectForKey:@"one"] = @"ONE";
//                 [ctx objectForKey:@"two"] = @"TWO";
//                 [ctx objectForKey:@"three"] = @"THREE";


//                 id<TestOperationsMyClassPrx> p = TestOperationsMyClassPrxuncheckedCast(
//                                         [ic stringToProxy:@"test:default -p 12010"]);
                
                
//                 [[ic getImplicitContext] setContext:(ctx)];
//                 test([[ic getImplicitContext] getContext] == ctx);
//                 {
//                     id<AMI_MyClass_opContextEqualI> cb = [[AMI_MyClass_opContextEqualI alloc] init](ctx);
//                     [p opContext_async:cb];
//                     test([cb check]);
//                 }

//                 [[ic getImplicitContext] put:@"zero" :@"ZERO"];
          
//                 ctx = [[ic getImplicitContext] getContext];
//                 {
//                     id<AMI_MyClass_opContextEqualI> cb = [[AMI_MyClass_opContextEqualI alloc] init](ctx);
//                     [p opContext_async:cb];
//                     test([cb check]);
//                 }
                
//                 ICEContext prxContext;
//                 [prxContext objectForKey:@"one"] = @"UN";
//                 [prxContext objectForKey:@"four"] = @"QUATRE";
                
//                 ICEContext* combined = prxContext;
//                 combined.insert(ctx.begin(), ctx.end());
//                 test([[combined objectForKey:@"one"] isEqualToString:@"UN"]);
                
//                 p = [TestOperationsMyClassPrx uncheckedCast:[p ice_context:prxContext]];
                
//                 [[ic getImplicitContext] setContext:(ICEContext()]);
//                 {
//                     id<AMI_MyClass_opContextEqualI> cb = [[AMI_MyClass_opContextEqualI alloc] init](prxContext);
//                     [p opContext_async:cb];
//                     test([cb check]);
//                 }

//                 [[ic getImplicitContext] setContext:(ctx)];
//                 {
//                     id<AMI_MyClass_opContextEqualI> cb = [[AMI_MyClass_opContextEqualI alloc] init](combined);
//                     [p opContext_async:cb];
//                     test([cb check]);
//                 }

//                 [[ic getImplicitContext] setContext:(ICEContext()]);
//                 [ic destroy];
//             }
//         }
//     }



    {
        ICEDouble d = 1278312346.0 / 13.0;
	TestOperationsMutableDoubleS *ds = [TestOperationsMutableDoubleS dataWithLength:(5 * sizeof(ICEDouble))];
	ICEDouble *pb = (ICEDouble *)[ds bytes];
	int i = 5;
	while(i-- > 0)
	{
	    *pb++ = d;
	}
        TestOperationsCallback* cb = [[[TestOperationsCallback alloc] init] autorelease];
        [p opDoubleMarshaling_async:cb response:@selector(opDoubleMarshalingResponse) exception:@selector(opDoubleMarshalingException:) p1:d p2:ds];
        test([cb check]);
    }

    // Marshaling tests for NSNull are present only in synchronous test because testing asynchronously
    // would only test the same marshaling code that's been tested already.
}

