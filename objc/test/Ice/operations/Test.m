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
