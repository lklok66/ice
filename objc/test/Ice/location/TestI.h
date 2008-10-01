// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>
#import <ServerLocator.h>

@interface ServerManagerI : TestServerManager
{
    NSMutableArray* communicators_;
    ServerLocatorRegistry* registry_;
    ICEInitializationData* initData_;
}
-(id)init:(ServerLocatorRegistry*)registry initData:(ICEInitializationData*)d;

-(void)terminate;
@end

@interface HelloI : TestHello
@end

@interface TestI : TestTestIntf
{
    id<ICEObjectAdapter> adapter1_;
    id<ICEObjectAdapter> adapter2_;
    ServerLocatorRegistry* registry_;
}
-(id) init:(id<ICEObjectAdapter>)adapter1 adapter2:(id<ICEObjectAdapter>)adpt2 registry:(ServerLocatorRegistry*)r;
@end
