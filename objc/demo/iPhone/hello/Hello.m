// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import "Hello.h"

@implementation HelloPrx

+(HelloPrx*) uncheckedCast:(ICEObjectPrx*)proxy
{
    return (HelloPrx*)[self uncheckedCast__:proxy];
}

+(HelloPrx*) checkedCast:(ICEObjectPrx*)proxy
{
    return (HelloPrx*)[self checkedCast__:proxy protocol:@protocol(HelloPrxProtocol) sliceId:@"::Demo::Hello"];
}

-(void) sayHello
{
    ICEOutputStream* os = [self createOutputStream__];
    ICEInputStream* is;
    BOOL ok = [self invoke__:@"sayHello" mode:Idempotent os:os is:&is];
    [is release];
}

@end
