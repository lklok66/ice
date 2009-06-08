// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>

#import <exceptions/TestI.h>
#import <TestCommon.h>
#import <Foundation/NSException.h>

@interface FooException : NSException
@end

@implementation FooException
-(id)init
{
    if(![super initWithName:@"FooException" reason:@"no reason" userInfo:nil])
    {
        return nil;
    }
    return self;
}
@end

@implementation ThrowerI

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}

-(BOOL) supportsUndeclaredExceptions:(ICECurrent*)current
{
    return YES;
}

-(BOOL) supportsAssertException:(ICECurrent*)current
{
    return NO;
}

-(void) throwAasA:(ICEInt)a current:(ICECurrent*)current
{
    @throw [TestExceptionsA a:a];
}

-(void) throwAorDasAorD:(ICEInt)a current:(ICECurrent*)current
{
    if(a > 0)
    {
        @throw [TestExceptionsA a:a];
    }
    else
    {
        @throw [TestExceptionsD d:a];
    }
}

-(void) throwBasA:(ICEInt)a b:(ICEInt)b current:(ICECurrent*)current
{
    [self throwBasB:a b:b current:current];
}

-(void) throwCasA:(ICEInt)a b:(ICEInt)b c:(ICEInt) c current:(ICECurrent*)current
{
    [self throwCasC:a b:b c:c current:current];
}

-(void) throwBasB:(ICEInt)a b:(ICEInt)b current:(ICECurrent*)current
{
    @throw [TestExceptionsB b:a bMem:b];
}

-(void) throwCasB:(ICEInt)a b:(ICEInt)b c:(ICEInt)c current:(ICECurrent*)current
{
    [self throwCasC:a b:b c:c current:current];
}

-(void) throwCasC:(ICEInt)a b:(ICEInt)b c:(ICEInt)c current:(ICECurrent*)current
{
    @throw [TestExceptionsC c:a bMem:b cMem:c];
}

-(void) throwModA:(ICEInt)a a2:(ICEInt)a2 current:(ICECurrent*)current
{
    @throw [TestExceptionsModA a:a a2Mem:a2];
}

-(void) throwUndeclaredA:(ICEInt)a current:(ICECurrent*)current
{
    @throw [TestExceptionsA a:a];
}

-(void) throwUndeclaredB:(ICEInt)a b:(ICEInt)b current:(ICECurrent*)current
{
    @throw [TestExceptionsB b:a bMem:b];
}

-(void) throwUndeclaredC:(ICEInt)a b:(ICEInt)b c:(ICEInt)c current:(ICECurrent*)current
{
    @throw [TestExceptionsC c:a bMem:b cMem:c];
}

-(void) throwLocalException:(ICECurrent*)current
{
    @throw [ICETimeoutException timeoutException:__FILE__ line:__LINE__];
}

-(void) throwNonIceException:(ICECurrent*)current
{
    @throw [[[FooException alloc] init] autorelease];
}

-(void) throwAssertException:(ICECurrent*)current
{
    // Not supported.
}
@end
