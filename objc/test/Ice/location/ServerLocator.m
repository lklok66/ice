// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Ice/BuiltinSequences.h>
#import <ServerLocator.h>

@implementation ServerLocatorRegistry
-(id) init
{
    if(![super init])
    {
        return nil;
    }
    adapters_ = [[NSMutableDictionary alloc] init];
    objects_ = [[NSMutableDictionary alloc] init];
    return self;
}
-(void) dealloc
{
    [adapters_ release];
    [objects_ release];
    [super dealloc];
}
-(void) setAdapterDirectProxy:(NSMutableString *)adapter proxy:(id<ICEObjectPrx>)proxy current:(ICECurrent *)current
{
    if(proxy == nil)
    {
        [adapters_ removeObjectForKey:adapter];
    }
    else
    {
        [adapters_ setObject:proxy forKey:adapter];
    }
}
-(void) setReplicatedAdapterDirectProxy:(NSMutableString *)adapterId 
                         replicaGroupId:(NSMutableString *)replicaGroupId 
                                      p:(id<ICEObjectPrx>)p 
                                current:(ICECurrent *)current
{
    if(p == nil)
    {
        [adapters_ removeObjectForKey:adapterId];
        [adapters_ removeObjectForKey:replicaGroupId];
    }
    else
    {
        [adapters_ setObject:p forKey:adapterId];
        [adapters_ setObject:p forKey:replicaGroupId];
    }
}
-(void) setServerProcessProxy:(NSMutableString *)id_ proxy:(id<ICEProcessPrx>)proxy current:(ICECurrent *)current
{
}
-(void) addObject:(id<ICEObjectPrx>)object current:(ICECurrent*)current
{
    [self addObject:object];
}
-(id<ICEObjectPrx>) getAdapter:(NSString*)adapterId
{
    id<ICEObjectPrx> proxy = [adapters_ objectForKey:adapterId];
    if(proxy == nil)
    {
        @throw [ICEAdapterNotFoundException adapterNotFoundException];
    }
    return proxy;
}
-(id<ICEObjectPrx>) getObject:(ICEIdentity*)ident
{
    id<ICEObjectPrx> proxy = [objects_ objectForKey:ident];
    if(proxy == nil)
    {
        @throw [ICEObjectNotFoundException objectNotFoundException];
    }
    return proxy;
}
-(void) addObject:(id<ICEObjectPrx>)object
{
    [objects_ setObject:object forKey:[object ice_getIdentity]];
}
@end

@implementation ServerLocator
-(id) init:(ServerLocatorRegistry*)registry proxy:(id<ICELocatorRegistryPrx>)registryPrx
{
    if(![super init])
    {
        return nil;
    }
    registry_ = registry;
    registryPrx_ = registryPrx;
    requestCount_ = 0;
    return self;
}
-(id<ICEObjectPrx>) findObjectById:(ICEIdentity *)id_ current:(ICECurrent *)current
{
    ++requestCount_;
    return [registry_ getObject:id_];
}
-(id<ICEObjectPrx>) findAdapterById:(NSMutableString *)id_ current:(ICECurrent *)current
{
    ++requestCount_;
    return [registry_ getAdapter:id_];
}
-(id<ICELocatorRegistryPrx>) getRegistry:(ICECurrent *)current
{
    return registryPrx_;
}
-(int) getRequestCount:(ICECurrent*)current
{
    return requestCount_;
}
@end
