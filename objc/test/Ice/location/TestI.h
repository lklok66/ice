// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>
#import <ServerLocator.h>

@interface ServerManagerI : TestLocationServerManager
{
    NSMutableArray* communicators_;
    ServerLocatorRegistry* registry_;
    ICEInitializationData* initData_;
}
-(id)init:(ServerLocatorRegistry*)registry initData:(ICEInitializationData*)d;

-(void)terminate;
@end

@interface HelloI : TestLocationHello
@end

@interface TestLocationI : TestLocationTestIntf
{
    id<ICEObjectAdapter> adapter1_;
    id<ICEObjectAdapter> adapter2_;
    ServerLocatorRegistry* registry_;
}
-(id) init:(id<ICEObjectAdapter>)adapter1 adapter2:(id<ICEObjectAdapter>)adpt2 registry:(ServerLocatorRegistry*)r;
@end
