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
#import <TestCommon.h>

@implementation MyDerivedClassI
-(void) opVoid:(ICECurrent*)current
{
}

-(void) opDerived:(ICECurrent*)current
{
}

-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte *)p3 current:(ICECurrent *)current
{
    *p3 = p1 ^ p2;
    return p1;
}

-(BOOL) opBool:(BOOL)p1 p2:(BOOL)p2 p3:(BOOL *)p3 current:(ICECurrent*)current
{
    *p3 = p1;
    return p2;
}

-(ICELong) opShortIntLong:(ICEShort)p1 p2:(ICEInt)p2 p3:(ICELong)p3
                          p4:(ICEShort *)p4 p5:(ICEInt *)p5 p6:(ICELong *)p6
		          current:(ICECurrent *)current
{
    *p4 = p1;
    *p5 = p2;
    *p6 = p3;
    return p3;
}

-(ICEDouble) opFloatDouble:(ICEFloat)p1 p2:(ICEDouble)p2 p3:(ICEFloat *)p3 p4:(ICEDouble *)p4
                           current:(ICECurrent *)current
{
    *p3 = p1;
    *p4 = p2;
    return p2;
}

-(NSString *) opString:(NSMutableString *)p1 p2:(NSMutableString *)p2 p3:(NSString **)p3 current:(ICECurrent *)current
{
    NSMutableString *sout = [NSMutableString stringWithCapacity:([p2 length] + 1 + [p1 length])];
    [sout appendString:p2];
    [sout appendString:@" "];
    [sout appendString:p1];
    *p3 = sout;

    NSMutableString *ret = [NSMutableString stringWithCapacity:([p1 length] + 1 + [p2 length])];
    [ret appendString:p1];
    [ret appendString:@" "];
    [ret appendString:p2];
    return ret;
}

-(TestMyEnum) opMyEnum:(TestMyEnum)p1 p2:(TestMyEnum *)p2 current:(ICECurrent *)current
{
    *p2 = p1;
    return Testenum3;
}

-(id<TestMyClassPrx>) opMyClass:(id<TestMyClassPrx>)p1 p2:(id<TestMyClassPrx> *)p2 p3:(id<TestMyClassPrx> *)p3 
                        current:(ICECurrent *)current
{
    *p2 = p1;
    *p3 = [TestMyClassPrx uncheckedCast:[[current adapter] 
                                           createProxy:[[[current adapter] getCommunicator] 
                                                           stringToIdentity:@"noSuchIdentity"]]];
    return [TestMyClassPrx uncheckedCast:[[current adapter] createProxy:[current id_]]];
}

-(TestStructure *) opStruct:(TestStructure *)p1 p2:(TestStructure *)p2 p3:(TestStructure **)p3
                            current:(ICECurrent *)current;
{
    *p3 = [[p1 copy] autorelease];
    (*p3).s.s = @"a new string";
    return p2;
}

-(TestByteS *) opByteS:(TestMutableByteS *)p1 p2:(TestMutableByteS *)p2 p3:(TestByteS **)p3
                       current:(ICECurrent *)current
{
    *p3 = [TestMutableByteS dataWithLength:[p1 length]];
    ICEByte *target = (ICEByte *)[*p3 bytes];
    ICEByte *src = (ICEByte *)[p1 bytes] + [p1 length];
    int i;
    for(i = 0; i != [p1 length]; ++i)
    {
        *target++ = *--src;
    }
    TestMutableByteS *r = [TestMutableByteS dataWithData:p1];
    [r appendData:p2];
    return r;
}

-(TestBoolS *) opBoolS:(TestMutableBoolS *)p1 p2:(TestMutableBoolS *)p2 p3:(TestBoolS **)p3
                       current:(ICECurrent *)current
{
    *p3 = [TestMutableBoolS dataWithData:p1];
    [(TestMutableBoolS *)*p3 appendData:p2];

    TestMutableBoolS *r = [TestMutableBoolS dataWithLength:[p1 length] * sizeof(BOOL)];
    BOOL *target = (BOOL *)[r bytes];
    BOOL *src = (BOOL *)([p1 bytes] + [p1 length]);
    int i;
    for(i = 0; i != [p1 length]; ++i)
    {
        *target++ = *--src;
    }
    return r;
}

-(TestLongS *) opShortIntLongS:(TestMutableShortS *)p1 p2:(TestMutableIntS *)p2 p3:(TestMutableLongS *)p3
                               p4:(TestShortS **)p4 p5:(TestIntS **)p5 p6:(TestLongS **)p6
			       current:(ICECurrent *)current
{
    *p4 = [TestMutableShortS dataWithData:p1];
    *p5 = [TestMutableIntS dataWithLength:[p2 length]];
    ICEInt *target = (ICEInt *)[*p5 bytes];
    ICEInt *src = (ICEInt *)([p2 bytes] + [p2 length]);
    int i;
    for(i = 0; i != [p2 length] / sizeof(ICEInt); ++i)
    {
        *target++ = *--src;
    }
    *p6 = [TestMutableLongS dataWithData:p3];
    [(TestMutableLongS *)*p6 appendData:p3];
    return p3;
}

-(TestDoubleS *) opFloatDoubleS:(TestMutableFloatS *)p1 p2:(TestMutableDoubleS *)p2
                                p3:(TestFloatS **)p3 p4:(TestDoubleS **)p4 current:(ICECurrent *)current
{
    *p3 = [TestMutableFloatS dataWithData:p1];
    *p4 = [TestMutableDoubleS dataWithLength:[p2 length]];
    ICEDouble *target = (ICEDouble *)[*p4 bytes];
    ICEDouble *src = (ICEDouble *)([p2 bytes] + [p2 length]);
    int i;
    for(i = 0; i != [p2 length] / sizeof(ICEDouble); ++i)
    {
        *target++ = *--src;
    }
    TestDoubleS *r = [TestMutableDoubleS dataWithLength:([p2 length]
                                                         + ([p1 length] / sizeof(ICEFloat) * sizeof(ICEDouble)))];
    ICEDouble *rp = (ICEDouble *)[r bytes];
    ICEDouble *p2p = (ICEDouble *)[p2 bytes];
    for(i = 0; i < [p2 length] / sizeof(ICEDouble); ++i)
    {
        *rp++ = *p2p++;
    }
    ICEFloat *bp1 = (ICEFloat *)[p1 bytes];
    for(i = 0; i < [p1 length] / sizeof(ICEFloat); ++i)
    {
        *rp++ = bp1[i];
    }
    return r;
}

-(TestStringS *) opStringS:(TestMutableStringS *)p1 p2:(TestMutableStringS *)p2
                           p3:(TestStringS **)p3 current:(ICECurrent *)current
{
    *p3 = [TestMutableStringS arrayWithArray:p1];
    [(TestMutableStringS *)*p3 addObjectsFromArray:p2];
    TestMutableStringS *r = [TestMutableStringS arrayWithCapacity:[p1 count]];
    NSEnumerator *enumerator = [p1 reverseObjectEnumerator];
    for(NSString *element in enumerator)
    {
        [r addObject:element];
    } 
    return r;
}

-(TestMyEnumS *) opMyEnumS:(TestMutableMyEnumS *)p1 p2:(TestMutableMyEnumS *)p2
                           p3:(TestMyEnumS **)p3 current:(ICECurrent *)current
{
    *p3 = [TestMutableMyEnumS dataWithLength:[p1 length]];
    TestMyEnum *target = (TestMyEnum *)[*p3 bytes];
    TestMyEnum *src = (TestMyEnum *)([p1 bytes] + [p1 length]);
    int i;
    for(i = 0; i != [p1 length] / sizeof(TestMyEnum); ++i)
    {
        *target++ = *--src;
    }
    TestMutableMyEnumS *r = [TestMutableMyEnumS dataWithData:p1];
    [r appendData:p2];
    return r;
}

-(TestMyClassS *) opMyClassS:(TestMutableMyClassS *)p1 p2:(TestMutableMyClassS *)p2
                          p3:(TestMyClassS **)p3 current:(ICECurrent *)current
{
    *p3 = [TestMutableMyClassS arrayWithArray:p1];
    [(TestMutableMyClassS *)*p3 addObjectsFromArray:p2];
    TestMutableMyClassS *r = [TestMutableMyClassS arrayWithCapacity:[p1 count]];
    NSEnumerator *enumerator = [p1 reverseObjectEnumerator];
    for(NSString *element in enumerator)
    {
        [r addObject:element];
    } 
    return r;
}

-(TestByteSS *) opByteSS:(TestMutableByteSS *)p1 p2:(TestMutableByteSS *)p2 p3:(TestByteSS * *)p3
                         current:(ICECurrent *)current
{
    *p3 = [TestMutableByteSS array];
    NSEnumerator *enumerator = [p1 reverseObjectEnumerator]; 
    for(TestByteS *element in enumerator)
    { 
        [(TestMutableByteSS *)*p3 addObject:element];
    }

    TestMutableByteSS *r = [TestMutableByteSS arrayWithArray:p1];
    [r addObjectsFromArray:p2];
    return r;
}

-(TestBoolSS *) opBoolSS:(TestMutableBoolSS *)p1 p2:(TestMutableBoolSS *)p2 p3:(TestBoolSS * *)p3
                         current:(ICECurrent *)current
{
    *p3 = [TestMutableBoolSS arrayWithArray:p1];
    [(TestMutableBoolSS *)*p3 addObjectsFromArray:p2];

    TestMutableBoolSS *r = [TestMutableBoolSS array];
    NSEnumerator *enumerator = [p1 reverseObjectEnumerator]; 
    for(TestBoolS *element in enumerator)
    { 
        [r addObject:element];
    }
    return r;
}

-(TestLongSS *) opShortIntLongSS:(TestMutableShortSS *)p1 p2:(TestMutableIntSS *)p2 p3:(TestMutableLongSS *)p3
                                 p4:(TestShortSS **)p4 p5:(TestIntSS **)p5 p6:(TestLongSS **)p6
			         current:(ICECurrent *)current
{
    *p4 = [TestShortSS arrayWithArray:p1];
    *p5 = [TestMutableIntSS array];
    NSEnumerator *enumerator = [p2 reverseObjectEnumerator]; 
    for(TestIntS *element in enumerator)
    { 
        [(TestMutableIntSS *)*p5 addObject:element];
    }
    *p6 = [TestMutableLongSS arrayWithArray:p3];
    [(TestMutableLongSS *)*p6 addObjectsFromArray:p3];
    return p3;
}

-(TestDoubleSS *) opFloatDoubleSS:(TestMutableFloatSS *)p1 p2:(TestMutableDoubleSS *)p2
                                  p3:(TestFloatSS **)p3 p4:(TestDoubleSS **)p4 current:(ICECurrent *)current
{
    *p3 = [TestFloatSS arrayWithArray:p1];
    *p4 = [TestMutableDoubleSS array];
    NSEnumerator *enumerator = [p2 reverseObjectEnumerator]; 
    for(TestDoubleS *element in enumerator)
    { 
        [(TestMutableDoubleSS *)*p4 addObject:element];
    }
    TestMutableDoubleSS *r = [TestMutableDoubleSS arrayWithArray:p2];
    [r addObjectsFromArray:p2];
    return r;
}

-(TestStringSS *) opStringSS:(TestMutableStringSS *)p1 p2:(TestMutableStringSS *)p2 p3:(TestStringSS **)p3
                             current:(ICECurrent *)current
{
    *p3 = [TestMutableStringSS arrayWithArray:p1];
    [(TestMutableStringSS *)*p3 addObjectsFromArray:p2];
    TestMutableStringSS *r = [TestMutableStringSS array];
    NSEnumerator *enumerator = [p2 reverseObjectEnumerator]; 
    for(TestStringS *element in enumerator)
    { 
        [r addObject:element];
    }
    return r;
}

-(TestStringSSS *) opStringSSS:(TestMutableStringSSS *)p1 p2:(TestMutableStringSSS *)p2 p3:(TestStringSSS **)p3
                               current:(ICECurrent *)current
{
    *p3 = [TestMutableStringSSS arrayWithArray:p1];
    [(TestMutableStringSSS *)*p3 addObjectsFromArray:p2];
    TestMutableStringSSS *r = [TestMutableStringSSS array];
    NSEnumerator *enumerator = [p2 reverseObjectEnumerator];
    for(TestStringSS *element in enumerator)
    { 
        [r addObject:element];
    }
    return r;
}

-(TestByteBoolD *) opByteBoolD:(TestMutableByteBoolD *)p1 p2:(TestMutableByteBoolD *)p2 p3:(TestByteBoolD **)p3
                               current:(ICECurrent *)current
{
    *p3 = [TestMutableByteBoolD dictionaryWithDictionary:p1];
    TestMutableByteBoolD *r = [TestMutableByteBoolD dictionaryWithDictionary:p1];
    [r addEntriesFromDictionary:p2];
    return r;
}

-(TestShortIntD *) opShortIntD:(TestMutableShortIntD *)p1 p2:(TestMutableShortIntD *)p2 p3:(TestShortIntD **)p3
                               current:(ICECurrent *)current
{
    *p3 = [TestMutableShortIntD dictionaryWithDictionary:p1];
    TestMutableShortIntD *r = [TestMutableShortIntD dictionaryWithDictionary:p1];
    [r addEntriesFromDictionary:p2];
    return r;
}

-(TestLongFloatD *) opLongFloatD:(TestMutableLongFloatD *)p1 p2:(TestMutableLongFloatD *)p2 p3:(TestLongFloatD **)p3
                               current:(ICECurrent *)current
{
    *p3 = [TestMutableLongFloatD dictionaryWithDictionary:p1];
    TestMutableLongFloatD *r = [TestMutableLongFloatD dictionaryWithDictionary:p1];
    [r addEntriesFromDictionary:p2];
    return r;
}

-(TestStringStringD *) opStringStringD:(TestMutableStringStringD *)p1 p2:(TestMutableStringStringD *)p2
                                       p3:(TestStringStringD **)p3 current:(ICECurrent *)current
{
    *p3 = [TestMutableStringStringD dictionaryWithDictionary:p1];
    TestMutableStringStringD *r = [TestMutableStringStringD dictionaryWithDictionary:p1];
    [r addEntriesFromDictionary:p2];
    return r;
}

-(TestStringMyEnumD *) opStringMyEnumD:(TestMutableStringMyEnumD *)p1 p2:(TestMutableStringMyEnumD *)p2
                                       p3:(TestStringMyEnumD **)p3 current:(ICECurrent *)current
{
    *p3 = [TestMutableStringMyEnumD dictionaryWithDictionary:p1];
    TestMutableStringMyEnumD *r = [TestMutableStringMyEnumD dictionaryWithDictionary:p1];
    [r addEntriesFromDictionary:p2];
    return r;
}

-(TestIntS *) opIntS:(TestMutableIntS *)p1 current:(ICECurrent *)current
{
    int count = [p1 length] / sizeof(ICEInt);
    TestMutableIntS *r = [TestMutableIntS dataWithLength:[p1 length]];
    const int *src = [p1 bytes];
    int *target = (int *)[r bytes];
    while(count-- > 0)
    {
	*target++ = -*src++;
    }
    return r;
}

-(void) opByteSOneway:(TestMutableByteS *)p1 current:(ICECurrent *)current
{
}

-(ICEContext *) opContext:(ICECurrent *)current
{
    return current.ctx;
}

-(void) opDoubleMarshaling:(ICEDouble)p1 p2:(TestMutableDoubleS *)p2 current:(ICECurrent *)current
{
    ICEDouble d = 1278312346.0 / 13.0;
    test(p1 == d);
    const ICEDouble *p = [p2 bytes];
    int i;
    for(i = 0; i < [p2 length] / sizeof(ICEDouble); ++i)
    {
	test(p[i] == d);
    }
}

-(TestStringS *) getNSNullStringSeq:(ICECurrent *)current
{
    return [NSArray arrayWithObjects:@"first", [NSNull null], nil];
}

-(TestMyClassS *) getNSNullASeq:(ICECurrent *)current
{
    return [NSArray arrayWithObjects:[TestA a:99], [NSNull null], nil];
}

-(TestStructS *) getNSNullStructSeq:(ICECurrent *)current
{
    TestStructure *s = [TestStructure structure:nil e:Testenum2 s:[TestAnotherStruct anotherStruct:@"Hello"]];
    return [NSArray arrayWithObjects:s, [NSNull null], nil];
}

-(TestStringSS *) getNSNullStringSeqSeq:(ICECurrent *)current
{
    TestStringSS *s = [NSArray arrayWithObjects:@"first", nil];
    return [NSArray arrayWithObjects:s, [NSNull null], nil];
}

-(TestStringStringD *) getNSNullStringStringDict:(ICECurrent *)current
{
    TestMutableStringStringD *d = [TestMutableStringStringD dictionary];
    [d setObject:@"ONE" forKey:@"one"];
    [d setObject:[NSNull null] forKey:@"two"];
    return d;
}

-(void) putNSNullStringStringDict:(TestMutableStringStringD *)d current:(ICECurrent *)current
{
    // Nothing to do because this tests that an exception is thrown on the client side.
}

-(void) putNSNullShortIntDict:(TestMutableShortIntD *)d current:(ICECurrent *)current
{
    // Nothing to do because this tests that an exception is thrown on the client side.
}

-(void) putNSNullStringMyEnumDict:(TestMutableStringMyEnumD *)d current:(ICECurrent *)current
{
    // Nothing to do because this tests that an exception is thrown on the client side.
}

-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}
@end
