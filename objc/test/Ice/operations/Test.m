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

#import <IceObjC/LocalException.h>
#import <IceObjC/Stream.h>
#import <Test.h>

@implementation TestAnotherStruct

@synthesize s;

-(id) init:(NSString *)s_
{
    if(![super init])
    {
        return nil;
    }
    s = [s_ retain];
    return self;
}

+(id) anotherStruct:(NSString *)s_
{
    TestAnotherStruct *s__ = [((TestAnotherStruct *)[TestAnotherStruct alloc]) init:s_];
    [s__ autorelease];
    return s__;
}

+(id) anotherStruct
{
    TestAnotherStruct *s__ = [[TestAnotherStruct alloc] init];
    [s__ autorelease];
    return s__;
}

-(id) copyWithZone:(NSZone *)zone
{
    TestAnotherStruct *copy_ = [TestAnotherStruct allocWithZone:zone];
    copy_->s = [s copy];
    return copy_;
}

-(NSUInteger) hash
{
    NSUInteger h_ = 0;
    h_ = (h_ << 1) ^ (!s ? 0 : [s hash]);
    return h_;
}

-(BOOL) isEqual:(id)o_
{
    if(self == o_)
    {
        return YES;
    }
    if(!o_ || ![o_ isKindOfClass:[self class]])
    {
        return NO;
    }
    TestAnotherStruct *obj_ = (TestAnotherStruct *)o_;
    if(!s)
    {
        if(obj_->s)
        {
            return NO;
        }
    }
    else
    {
        if(![s isEqualToString:obj_->s])
        {
            return NO;
        }
    }
    return YES;
}

-(void) dealloc;
{
    [s release];
    [super dealloc];
}
@end

@implementation TestStructure

@synthesize e;
@synthesize s;

-(id) init:(TestMyEnum)e_ s:(TestAnotherStruct *)s_
{
    if(![super init])
    {
        return nil;
    }
    e = e_;
    s = [s_ retain];
    return self;
}

+(id) structure:(TestMyEnum)e_ s:(TestAnotherStruct *)s_
{
    TestStructure *s__ = [((TestStructure *)[TestStructure alloc]) init:e_ s:s_];
    [s__ autorelease];
    return s__;
}

+(id) structure
{
    TestStructure *s__ = [[TestStructure alloc] init];
    [s__ autorelease];
    return s__;
}

-(id) copyWithZone:(NSZone *)zone
{
    TestStructure *copy_ = [TestStructure allocWithZone:zone];
    copy_->e = e;
    copy_->s = [s copy];
    return copy_;
}

-(NSUInteger) hash
{
    NSUInteger h_ = 0;
    h_ = (h_ << 1) ^ e;
    h_ = (h_ << 1) ^ (!s ? 0 : [s hash]);
    return h_;
}

-(BOOL) isEqual:(id)o_
{
    if(self == o_)
    {
        return YES;
    }
    if(!o_ || ![o_ isKindOfClass:[self class]])
    {
        return NO;
    }
    TestStructure *obj_ = (TestStructure *)o_;
    if(e != obj_->e)
    {
        return NO;
    }
    if(!s)
    {
        if(obj_->s)
        {
            return NO;
        }
    }
    else
    {
        if(![s isEqual:obj_->s])
        {
            return NO;
        }
    }
    return YES;
}

-(void) dealloc;
{
    [s release];
    [super dealloc];
}
@end

@implementation TestMyClassPrx

-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3
{
    return [TestMyClassPrx invoke_opByte:p1 p2:p2 p3:p3 prx:self context:nil];
}

-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 context:(ICEContext *)ctx_
{
    return [TestMyClassPrx invoke_opByte:p1 p2:p2 p3:p3 prx:self context:ctx_];
}

-(void) opVoid
{
    [TestMyClassPrx invoke_opVoid:self context:nil];
}

-(void) opVoid:(ICEContext *)ctx_
{
    [TestMyClassPrx invoke_opVoid:self context:ctx_];
}

-(void) shutdown
{
    [TestMyClassPrx invoke_shutdown:self context:nil];
}

-(void) shutdown:(ICEContext *)ctx_
{
    [TestMyClassPrx invoke_shutdown:self context:ctx_];
}

+(NSString *) ice_staticId
{
    return @"::Test::MyClass";
}

+(void) invoke_shutdown:(ICEObjectPrx <TestMyClassPrx> *)prx_ context:(ICEContext *)ctx_
{
    id <ICEOutputStream> os_ = [prx_ createOutputStream__];
    id <ICEInputStream> is_ = nil;
    @try
    {
        [prx_ invoke__:@"shutdown" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}

+(void) invoke_opVoid:(ICEObjectPrx <TestMyClassPrx> *)prx_ context:(ICEContext *)ctx_
{
    id <ICEOutputStream> os_ = [prx_ createOutputStream__];
    id <ICEInputStream> is_ = nil;
    @try
    {
        [prx_ invoke__:@"opVoid" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}

+(ICEByte) invoke_opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 prx:(ICEObjectPrx <TestMyClassPrx> *)prx_ context:(ICEContext *)ctx_
{
    id <ICEOutputStream> os_ = [prx_ createOutputStream__];
    id <ICEInputStream> is_ = nil;
    @try
    {
        [os_ writeByte:p1];
        [os_ writeByte:p2];
        [prx_ invoke__:@"opByte" mode:ICENormal os:os_ is:&is_ context:ctx_];
        *p3 = [is_ readByte];
        ICEByte ret_;
        ret_ = [is_ readByte];
        return ret_;
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}
@end

@implementation TestMyDerivedClassPrx

-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3
{
    return [TestMyClassPrx invoke_opByte:p1 p2:p2 p3:p3 prx:self context:nil];
}

-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 context:(ICEContext *)ctx_
{
    return [TestMyClassPrx invoke_opByte:p1 p2:p2 p3:p3 prx:self context:ctx_];
}

-(void) opVoid
{
    [TestMyClassPrx invoke_opVoid:self context:nil];
}

-(void) opVoid:(ICEContext *)ctx_
{
    [TestMyClassPrx invoke_opVoid:self context:ctx_];
}

-(void) shutdown
{
    [TestMyClassPrx invoke_shutdown:self context:nil];
}

-(void) shutdown:(ICEContext *)ctx_
{
    [TestMyClassPrx invoke_shutdown:self context:ctx_];
}

-(void) opDerived
{
    [TestMyDerivedClassPrx invoke_opDerived:self context:nil];
}

-(void) opDerived:(ICEContext *)ctx_
{
    [TestMyDerivedClassPrx invoke_opDerived:self context:ctx_];
}

+(NSString *) ice_staticId
{
    return @"::Test::MyDerivedClass";
}

+(void) invoke_opDerived:(ICEObjectPrx <TestMyDerivedClassPrx> *)prx_ context:(ICEContext *)ctx_
{
    id <ICEOutputStream> os_ = [prx_ createOutputStream__];
    id <ICEInputStream> is_ = nil;
    @try
    {
        [prx_ invoke__:@"opDerived" mode:ICENormal os:os_ is:&is_ context:ctx_];
    }
    @catch(ICEUserException *ex_)
    {
        @throw [ICEUnknownUserException unknownUserException:__FILE__ line:__LINE__ unknown:[ex_ ice_name]];
    }
    @finally
    {
        [os_ release];
        [is_ release];
    }
}
@end
