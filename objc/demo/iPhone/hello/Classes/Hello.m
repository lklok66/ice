// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Hello.h>

#import <IceObjC/LocalException.h>

@implementation HelloPrx

+(id<HelloPrx>) uncheckedCast:(id<ICEObjectPrx>)proxy
{
    return (id<HelloPrx>)[self uncheckedCast__:proxy];
}

+(id<HelloPrx>) checkedCast:(id<ICEObjectPrx>)proxy
{
    return (id<HelloPrx>)[self checkedCast__:proxy protocol:@protocol(HelloPrx) sliceId:@"::Demo::Hello"];
}

-(void) sayHello
{
    BOOL ok = [self invoke__:@"sayHello" mode:Idempotent os:nil is:nil];
}

@end

@implementation Hello

-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current
{
    //
    // TODO: Optimize
    //
    if([typeId isEqualToString:@"::Demo::Hello"])
    {
        return TRUE;
    }
    else
    {
        return [super ice_isA:typeId current:current];
    }
}
-(BOOL) sayHello___:(ICECurrent*)current is:(ICEInputStream*)is os:(ICEOutputStream*)os
{
    [(id)self sayHello:current];
    return TRUE;
}
-(BOOL) dispatch__:(ICECurrent*)current is:(ICEInputStream*)is os:(ICEOutputStream*)os
{
    //
    // TODO: Optimize
    //
    if([[current operation] isEqualToString:@"sayHello"])
    {
        return [self sayHello___:current is:is os:os];
    }
    else
    {
        return [super dispatch__:current is:is os:os];
    }
}

@end
