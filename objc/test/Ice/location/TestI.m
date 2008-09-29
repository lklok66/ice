// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Ice/Locator.h>
#import <TestI.h>

@implementation ServerManagerI
-(id) init:(ServerLocatorRegistry*)registry initData:(ICEInitializationData*)initData
{
    if(![super init])
    {
        return nil;
    }
    registry_ = registry;
    initData_ = initData;

    [initData_.properties setProperty:@"TestAdapter.Endpoints" value:@"default"];
    [initData_.properties setProperty:@"TestAdapter.AdapterId" value:@"TestAdapter"];
    [initData_.properties setProperty:@"TestAdapter.ReplicaGroupId" value:@"ReplicatedAdapter"];
    
    [initData_.properties setProperty:@"TestAdapter2.Endpoints" value:@"default"];
    [initData_.properties setProperty:@"TestAdapter2.AdapterId" value:@"TestAdapter2"];

    [initData_.properties setProperty:@"Ice.PrintAdapterReady" value:@"0"];
    return self;
}
-(void) dealloc
{
    [communicators_ release];
    [super dealloc];
}
-(void) startServer:(ICECurrent*)current
{
    for(id<ICECommunicator> c in communicators_)
    {
        [c waitForShutdown];
        [c destroy];
    }
    [communicators_ release];
    communicators_ = [[NSMutableArray alloc] init];

    //
    // Simulate a server: create a [[communicator alloc] init] and object
    // adapter. The object adapter is started on a system allocated
    // port. The configuration used here contains the Ice.Locator
    // configuration variable. The [[object alloc] init] adapter will register
    // its endpoints with the locator and create references containing
    // the adapter id instead of the endpoints.
    //
    
    id<ICECommunicator> serverCommunicator = [ICEUtil createCommunicator:initData_];
    [communicators_ addObject:serverCommunicator];

    id<ICEObjectAdapter> adapter = [serverCommunicator createObjectAdapter:@"TestAdapter"];
    id<ICEObjectAdapter> adapter2 = [serverCommunicator createObjectAdapter:@"TestAdapter2"];

    id<ICEObjectPrx> locator = [serverCommunicator stringToProxy:@"locator:default -p 12010"];
    [adapter setLocator:[ICELocatorPrx uncheckedCast:locator]];
    [adapter2 setLocator:[ICELocatorPrx uncheckedCast:locator]];

    ICEObject* object = [[[TestI alloc] init:adapter adapter2:adapter2 registry:registry_] autorelease];
    [registry_ addObject:[adapter add:object identity:[serverCommunicator stringToIdentity:@"test"]]];
    [registry_ addObject:[adapter add:object identity:[serverCommunicator stringToIdentity:@"test2"]]];

    [adapter activate];
    [adapter2 activate];
}

-(void) shutdown:(ICECurrent*)current
{
    for(id<ICECommunicator> c in communicators_)
    {
        [c destroy];
    }
    [[[current adapter] getCommunicator] shutdown];
}
@end

@implementation TestI
-(id) init:(id<ICEObjectAdapter>)adapter 
  adapter2:(id<ICEObjectAdapter>)adapter2 
  registry:(ServerLocatorRegistry*)registry
{
    if(![super init])
    {
        return nil;
    }
    adapter1_ = [adapter retain];
    adapter2_ = [adapter2 retain];
    registry_ = registry;
    [registry_ addObject:[adapter1_ add:[[[HelloI alloc] init] autorelease]
                                    identity:[[adapter1_ getCommunicator] stringToIdentity:@"hello"]]];
    return self;
}
-(void) dealloc
{
    [adapter1_ release];
    [adapter2_ release];
    [super dealloc];
}
-(void) shutdown:(ICECurrent*)current
{
    [[adapter1_ getCommunicator] shutdown];
}

-(id<TestHelloPrx>) getHello:(ICECurrent*)current
{
    return [TestHelloPrx uncheckedCast:[adapter1_ createIndirectProxy:[[adapter1_ getCommunicator] 
                                                                          stringToIdentity:@"hello"]]];
}

-(id<TestHelloPrx>) getReplicatedHello:(ICECurrent*)current
{
    return [TestHelloPrx uncheckedCast:[adapter1_ createProxy:[[adapter1_ getCommunicator] stringToIdentity:@"hello"]]];
}

-(void) migrateHello:(ICECurrent*)current
{
    ICEIdentity* ident = [[adapter1_ getCommunicator] stringToIdentity:@"hello"];
    @try
    {
        [registry_ addObject:[adapter2_ add:[adapter1_ remove:ident] identity:ident]];
    }
    @catch(ICENotRegisteredException*)
    {
        [registry_ addObject:[adapter1_ add:[adapter2_ remove:ident] identity:ident]];
    }
}
@end

@implementation HelloI
-(void) sayHello:(ICECurrent*)current
{
}
@end
