// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/ObjectAdapterI.h>
#import <IceObjC/CommunicatorI.h>
#import <IceObjC/ProxyI.h>
#import <IceObjC/IdentityI.h>
#import <IceObjC/ObjectI.h>
#import <IceObjC/Util.h>

#define OBJECTADAPTER ((Ice::ObjectAdapter*)objectAdapter__)

@implementation Ice_ObjectAdapter (Internal)

-(Ice_ObjectAdapter*) initWithObjectAdapter:(Ice::ObjectAdapter*)arg
{
    if(![super init])
    {
        return nil;
    }
    objectAdapter__ = arg;
    OBJECTADAPTER->__incRef();
    return self;
}

-(Ice::ObjectAdapter*) objectAdapter__
{
    return (Ice::ObjectAdapter*)objectAdapter__;
}

-(void) dealloc
{
    OBJECTADAPTER->__decRef();
    objectAdapter__ = 0;
    [super dealloc];
}

@end

@implementation Ice_ObjectAdapter

-(Ice_Communicator*) getCommunicator
{
    return [Ice_Communicator communicatorWithCommunicator:OBJECTADAPTER->getCommunicator()];
}

-(NSString*) getName
{
    return [NSString stringWithUTF8String:OBJECTADAPTER->getName().c_str()];
}

-(void) activate
{
    OBJECTADAPTER->activate();
}

-(void) hold
{
    OBJECTADAPTER->hold();
}

-(void) waitForHold
{
    OBJECTADAPTER->waitForHold();
}

-(void) deactivate
{
    OBJECTADAPTER->deactivate();
}

-(void) waitForDeactivate
{
    OBJECTADAPTER->waitForDeactivate();
}

-(BOOL) isDeactivated
{
    return OBJECTADAPTER->isDeactivated();
}

-(void) destroy
{
    OBJECTADAPTER->destroy();
}

-(id<Ice_ObjectPrx>) add:(id<Ice_Object>)servant identity:(Ice_Identity*)ident
{
    [(Ice_Object*)servant retain];
    return [Ice_ObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->add([(Ice_Object*)servant object__], [ident identity__])];
}

-(id<Ice_ObjectPrx>) addFacet:(id<Ice_Object>)servant identity:(Ice_Identity*)ident facet:(NSString*)facet
{
    [(Ice_Object*)servant retain];
    return [Ice_ObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacet([(Ice_Object*)servant object__], [ident identity__], [facet UTF8String])];
}

-(id<Ice_ObjectPrx>) addWithUUID:(id<Ice_Object>)servant
{
    [(Ice_Object*)servant retain];
    return [Ice_ObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addWithUUID([(Ice_Object*)servant object__])];
}

-(id<Ice_ObjectPrx>) addFacetWithUUID:(id<Ice_Object>)servant facet:(NSString*)facet
{
    [(Ice_Object*)servant retain];
    return [Ice_ObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacetWithUUID([(Ice_Object*)servant object__], [facet UTF8String])];
}

-(id<Ice_Object>) remove:(Ice_Identity*)ident
{
    Ice::ObjectPtr wrapper = OBJECTADAPTER->remove([ident identity__]);
    id<Ice_Object> servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
    return [(Ice_Object*)servant autorelease];
}
-(id<Ice_Object>) removeFacet:(Ice_Identity*)ident facet:(NSString*)facet
{
    Ice::ObjectPtr wrapper = OBJECTADAPTER->removeFacet([ident identity__], [facet UTF8String]);
    id<Ice_Object> servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
    return [(Ice_Object*)servant autorelease];
}

-(NSDictionary*) removeAllFacets:(Ice_Identity*)ident
{
    Ice::FacetMap wrappers = OBJECTADAPTER->removeAllFacets([ident identity__]);
    NSMutableDictionary* servants = [[NSMutableDictionary alloc] initWithCapacity:wrappers.size()];
    for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
    {
        NSObject* key = toObjC(p->first);
        NSObject* value = IceObjC::ServantWrapperPtr::dynamicCast(p->second)->getServant();
        [servants setObject:value forKey:key];
        [key release];
        [value release];
    }
    return [servants autorelease];
}

-(id<Ice_Object>) find:(Ice_Identity*)ident
{
    return nil;
}

-(id<Ice_Object>) findFacet:(Ice_Identity*)ident facet:(NSString*)facet
{
    return nil;
}

-(NSDictionary*) findAllFacets:(Ice_Identity*)ident
{
    return nil;
}

-(id<Ice_Object>) findByProxy:(id<Ice_ObjectPrx>)proxy
{
    return nil;
}

-(void) addServantLocator:(Ice_ServantLocator*)locator category:(NSString*)category
{
}

-(Ice_ServantLocator*) findServantLocator:(NSString*)category
{
    return nil;
}

-(id<Ice_ObjectPrx>) createProxy:(Ice_Identity*)ident
{
    return nil;
}

-(id<Ice_ObjectPrx>) createDirectProxy:(Ice_Identity*)ident
{
    return nil;
}

-(id<Ice_ObjectPrx>) createIndirectProxy:(Ice_Identity*)ident
{
    return nil;
}

-(void) setLocator:(id<Ice_LocatorPrx>)loc
{
}

-(void) refreshPublishedEndpoints
{
}

@end
