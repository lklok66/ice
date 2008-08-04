// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Hello.h>

@implementation HelloPrx

+(id<HelloPrx>) uncheckedCast:(id<Ice_ObjectPrx>)proxy
{
    return (id<HelloPrx>)[self uncheckedCast__:proxy];
}

+(id<HelloPrx>) checkedCast:(id<Ice_ObjectPrx>)proxy
{
    return (id<HelloPrx>)[self checkedCast__:proxy protocol:@protocol(HelloPrx) sliceId:@"::Demo::Hello"];
}

-(void) sayHello
{
    Ice_OutputStream* os = [self createOutputStream__];
    Ice_InputStream* is;
    BOOL ok = [self invoke__:@"sayHello" mode:Idempotent os:os is:&is];
    [is release];
}

@end
