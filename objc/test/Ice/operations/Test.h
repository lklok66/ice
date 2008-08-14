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

typedef NSData TestBoolS;

typedef NSData TestShortS;

typedef NSData TestIntS;

typedef NSData TestLongS;

typedef NSData TestFloatS;

typedef NSData TestDoubleS;

typedef NSArray TestStringS;

typedef NSData TestMyEnumS;

typedef NSArray TestByteSS;

typedef NSArray TestBoolSS;

typedef NSArray TestShortSS;

typedef NSArray TestIntSS;

typedef NSArray TestLongSS;

typedef NSArray TestFloatSS;

typedef NSArray TestDoubleSS;

typedef NSArray TestStringSS;

typedef NSArray TestMyEnumSS;

typedef NSArray TestStringSSS;

typedef NSDictionary TestByteBoolD;

typedef NSDictionary TestShortIntD;

typedef NSDictionary TestLongFloatD;

typedef NSDictionary TestStringStringD;

typedef NSDictionary TestStringMyEnumD;
