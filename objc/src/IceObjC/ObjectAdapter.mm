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

@implementation ICEObjectAdapter (Internal)

-(ICEObjectAdapter*) initWithObjectAdapter:(Ice::ObjectAdapter*)arg
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

-(id<ICEObjectPrx>) add:(id<ICEObject>)servant identity:(ICEIdentity*)ident
{
    [(ICEObject*)servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->add([(ICEObject*)servant object__], [ident identity__])];
}

-(id<ICEObjectPrx>) addFacet:(id<ICEObject>)servant identity:(ICEIdentity*)ident facet:(NSString*)facet
{
    [(ICEObject*)servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacet([(ICEObject*)servant object__], [ident identity__], [facet UTF8String])];
}

-(id<ICEObjectPrx>) addWithUUID:(id<ICEObject>)servant
{
    [(ICEObject*)servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addWithUUID([(ICEObject*)servant object__])];
}

-(id<ICEObjectPrx>) addFacetWithUUID:(id<ICEObject>)servant facet:(NSString*)facet
{
    [(ICEObject*)servant retain];
    return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacetWithUUID([(ICEObject*)servant object__], [facet UTF8String])];
}

-(id<ICEObject>) remove:(ICEIdentity*)ident
{
    Ice::ObjectPtr wrapper = OBJECTADAPTER->remove([ident identity__]);
    id<ICEObject> servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
    return [(ICEObject*)servant autorelease];
}
-(id<ICEObject>) removeFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    Ice::ObjectPtr wrapper = OBJECTADAPTER->removeFacet([ident identity__], [facet UTF8String]);
    id<ICEObject> servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
    return [(ICEObject*)servant autorelease];
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

-(id<ICEObject>) find:(ICEIdentity*)ident
{
    return nil;
}

-(id<ICEObject>) findFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    return nil;
}

-(NSDictionary*) findAllFacets:(ICEIdentity*)ident
{
    return nil;
}

-(id<ICEObject>) findByProxy:(id<ICEObjectPrx>)proxy
{
    return nil;
}

-(void) addServantLocator:(ICEServantLocator*)locator category:(NSString*)category
{
}

-(ICEServantLocator*) findServantLocator:(NSString*)category
{
    return nil;
}

-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)ident
{
    return nil;
}

-(id<ICEObjectPrx>) createDirectProxy:(ICEIdentity*)ident
{
    return nil;
}

-(id<ICEObjectPrx>) createIndirectProxy:(ICEIdentity*)ident
{
    return nil;
}

-(void) setLocator:(id<ICELocatorPrx>)loc
{
}

-(void) refreshPublishedEndpoints
{
}

@end
