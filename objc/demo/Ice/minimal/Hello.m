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

-(void) sayHello
{
    [self invoke__:@"sayHello" mode:ICEIdempotent os:nil is:nil context:nil];
}

@end

@implementation Hello

static const char* Hello_ids__[2] =
{
    "::Demo::Hello",
    "::Ice::Object"
};

+(const char**) staticIds__:(int*)count
{
    *count = sizeof(Hello_ids__) / sizeof(const char*);
    return Hello_ids__;
}

-(BOOL) sayHello___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    [(id)self sayHello:current];
    return TRUE;
}
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
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
