// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.0
// Generated from file `Test.ice'

#import <IceObjC/Config.h>
#import <IceObjC/Proxy.h>
#import <IceObjC/Exception.h>
#import <IceObjC/Current.h>

typedef enum
{
    Testenum1,
    Testenum2,
    Testenum3
} TestMyEnum;

@interface TestAnotherStruct : NSObject <NSCopying>
{
    @private
        NSString *s;
}

@property(nonatomic, retain) NSString *s;

-(id) init:(NSString *)s;
+(id) anotherStruct:(NSString *)s;
+(id) anotherStruct;
-(id) copyWithZone:(NSZone *)zone;
-(NSUInteger) hash;
-(BOOL) isEqual:(id)anObject;
-(void) dealloc;
@end

@interface TestStructure : NSObject <NSCopying>
{
    @private
        TestMyEnum e;
        TestAnotherStruct *s;
}

@property(nonatomic, assign) TestMyEnum e;
@property(nonatomic, retain) TestAnotherStruct *s;

-(id) init:(TestMyEnum)e s:(TestAnotherStruct *)s;
+(id) structure:(TestMyEnum)e s:(TestAnotherStruct *)s;
+(id) structure;
-(id) copyWithZone:(NSZone *)zone;
-(NSUInteger) hash;
-(BOOL) isEqual:(id)anObject;
-(void) dealloc;
@end

typedef NSData TestByteS;
typedef NSMutableData TestMutableByteS;

typedef NSData TestBoolS;
typedef NSMutableData TestMutableBoolS;

typedef NSData TestShortS;
typedef NSMutableData TestMutableShortS;

typedef NSData TestIntS;
typedef NSMutableData TestMutableIntS;

typedef NSData TestLongS;
typedef NSMutableData TestMutableLongS;

typedef NSData TestFloatS;
typedef NSMutableData TestMutableFloatS;

typedef NSData TestDoubleS;
typedef NSMutableData TestMutableDoubleS;

typedef NSArray TestStringS;
typedef NSMutableArray TestMutableStringS;

typedef NSData TestMyEnumS;
typedef NSMutableData TestMutableMyEnumS;

typedef NSArray TestByteSS;
typedef NSMutableArray TestMutableByteSS;

typedef NSArray TestBoolSS;
typedef NSMutableArray TestMutableBoolSS;

typedef NSArray TestShortSS;
typedef NSMutableArray TestMutableShortSS;

typedef NSArray TestIntSS;
typedef NSMutableArray TestMutableIntSS;

typedef NSArray TestLongSS;
typedef NSMutableArray TestMutableLongSS;

typedef NSArray TestFloatSS;
typedef NSMutableArray TestMutableFloatSS;

typedef NSArray TestDoubleSS;
typedef NSMutableArray TestMutableDoubleSS;

typedef NSArray TestStringSS;
typedef NSMutableArray TestMutableStringSS;

typedef NSArray TestMyEnumSS;
typedef NSMutableArray TestMutableMyEnumSS;

typedef NSArray TestStringSSS;
typedef NSMutableArray TestMutableStringSSS;

typedef NSDictionary TestByteBoolD;

typedef NSDictionary TestShortIntD;

typedef NSDictionary TestLongFloatD;

typedef NSDictionary TestStringStringD;

typedef NSDictionary TestStringMyEnumD;

@protocol TestMyClassPrx <ICEObjectPrx>
-(void) shutdown;
-(void) shutdown:(ICEContext *)context;
-(void) opVoid;
-(void) opVoid:(ICEContext *)context;
-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3;
-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 context:(ICEContext *)context;
@end;

@protocol TestMyDerivedClassPrx <TestMyClassPrx>
-(void) opDerived;
-(void) opDerived:(ICEContext *)context;
@end;

@interface TestMyClassPrx : ICEObjectPrx <TestMyClassPrx>
+(NSString *) ice_staticId;
+(void) invoke_shutdown:(ICEObjectPrx <TestMyClassPrx> *)prx context:(ICEContext *)ctx;
+(void) invoke_opVoid:(ICEObjectPrx <TestMyClassPrx> *)prx context:(ICEContext *)ctx;
+(ICEByte) invoke_opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 prx:(ICEObjectPrx <TestMyClassPrx> *)prx context:(ICEContext *)ctx;
@end

@interface TestMyDerivedClassPrx : ICEObjectPrx <TestMyDerivedClassPrx>
+(NSString *) ice_staticId;
+(void) invoke_opDerived:(ICEObjectPrx <TestMyDerivedClassPrx> *)prx context:(ICEContext *)ctx;
@end
