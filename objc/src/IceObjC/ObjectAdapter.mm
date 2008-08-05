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

#include <Ice/Locator.h>

#define OBJECTADAPTER ((Ice::ObjectAdapter*)objectAdapter__)

@implementation ICEObjectAdapter (Internal)

-(ICEObjectAdapter*) initWithObjectAdapter:(const Ice::ObjectAdapterPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    objectAdapter__ = arg.get();
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

+(ICEObjectAdapter*) objectAdapterWithObjectAdapter:(const Ice::ObjectAdapterPtr&)arg
{
    return [[[ICEObjectAdapter alloc] initWithObjectAdapter:arg] autorelease];
}

@end

@implementation ICEObjectAdapter

-(ICECommunicator*) getCommunicator
{
    return [ICECommunicator communicatorWithCommunicator:OBJECTADAPTER->getCommunicator()];
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

-(id<ICEObjectPrx>) add:(ICEObject*)servant identity:(ICEIdentity*)ident
{
    [servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->add([servant object__], [ident identity__])];
}

-(id<ICEObjectPrx>) addFacet:(ICEObject*)servant identity:(ICEIdentity*)ident facet:(NSString*)facet
{
    [servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacet([servant object__], [ident identity__],
                                                                          [facet UTF8String])];
}

-(id<ICEObjectPrx>) addWithUUID:(ICEObject*)servant
{
    [servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addWithUUID([servant object__])];
}

-(id<ICEObjectPrx>) addFacetWithUUID:(ICEObject*)servant facet:(NSString*)facet
{
    [servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacetWithUUID([servant object__],
                                                                                  [facet UTF8String])];
}

-(ICEObject*) remove:(ICEIdentity*)ident
{
    Ice::ObjectPtr wrapper = OBJECTADAPTER->remove([ident identity__]);
    ICEObject* servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
    return [servant autorelease];
}
-(ICEObject*) removeFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    Ice::ObjectPtr wrapper = OBJECTADAPTER->removeFacet([ident identity__], [facet UTF8String]);
    ICEObject* servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
    return [servant autorelease];
}

-(NSDictionary*) removeAllFacets:(ICEIdentity*)ident
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

-(ICEObject*) find:(ICEIdentity*)ident
{
    return IceObjC::ServantWrapperPtr::dynamicCast(OBJECTADAPTER->find([ident identity__]))->getServant();
}

-(ICEObject*) findFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    Ice::ObjectPtr wrapper = OBJECTADAPTER->findFacet([ident identity__], [facet UTF8String]);
    return IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
}

-(NSDictionary*) findAllFacets:(ICEIdentity*)ident
{
    Ice::FacetMap wrappers = OBJECTADAPTER->findAllFacets([ident identity__]);
    NSMutableDictionary* servants = [[NSMutableDictionary alloc] initWithCapacity:wrappers.size()];
    for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
    {
        [servants setObject:IceObjC::ServantWrapperPtr::dynamicCast(p->second)->getServant() forKey:toObjC(p->first)];
    }
    return servants;
}

-(ICEObject*) findByProxy:(id<ICEObjectPrx>)proxy
{
    Ice::ObjectPrx prx = [(ICEObjectPrx*)proxy objectPrx__];
    return IceObjC::ServantWrapperPtr::dynamicCast(OBJECTADAPTER->findByProxy(prx))->getServant();
}

-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)ident
{
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createProxy([ident identity__])];
}

-(id<ICEObjectPrx>) createDirectProxy:(ICEIdentity*)ident
{
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createDirectProxy([ident identity__])];
}

-(id<ICEObjectPrx>) createIndirectProxy:(ICEIdentity*)ident
{
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createIndirectProxy([ident identity__])];
}

-(void) setLocator:(id<ICELocatorPrx>)loc
{
    OBJECTADAPTER->setLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
}

-(void) refreshPublishedEndpoints
{
    OBJECTADAPTER->refreshPublishedEndpoints();
}

@end
