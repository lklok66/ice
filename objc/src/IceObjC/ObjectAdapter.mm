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
    try
    {
        return [ICECommunicator communicatorWithCommunicator:OBJECTADAPTER->getCommunicator()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSString*) getName
{
    try
    {
        return [toNSString(OBJECTADAPTER->getName()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) activate
{
    try
    {
        OBJECTADAPTER->activate();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) hold
{
    try
    {
        OBJECTADAPTER->hold();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) waitForHold
{
    try
    {
        OBJECTADAPTER->waitForHold();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) deactivate
{
    try
    {
        OBJECTADAPTER->deactivate();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) waitForDeactivate
{
    try
    {
        OBJECTADAPTER->waitForDeactivate();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(BOOL) isDeactivated
{
    try
    {
        return OBJECTADAPTER->isDeactivated();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) destroy
{
    try
    {
        OBJECTADAPTER->destroy();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(id<ICEObjectPrx>) add:(ICEObject*)servant identity:(ICEIdentity*)ident
{
    try
    {
        [servant retain];
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->add([servant object__], [ident identity__])];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) addFacet:(ICEObject*)servant identity:(ICEIdentity*)ident facet:(NSString*)facet
{
    try
    {
        [servant retain];
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacet([servant object__], [ident identity__],
                                                                              fromNSString(facet))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) addWithUUID:(ICEObject*)servant
{
    try
    {
        [servant retain];
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addWithUUID([servant object__])];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) addFacetWithUUID:(ICEObject*)servant facet:(NSString*)facet
{
    try
    {
        [servant retain];
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacetWithUUID([servant object__],
                                                                                      fromNSString(facet))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObject*) remove:(ICEIdentity*)ident
{
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->remove([ident identity__]);
        ICEObject* servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
        return [servant autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(ICEObject*) removeFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->removeFacet([ident identity__], fromNSString(facet));
        ICEObject* servant = IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
        return [servant autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSDictionary*) removeAllFacets:(ICEIdentity*)ident
{
    try
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
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObject*) find:(ICEIdentity*)ident
{
    try
    {
        return IceObjC::ServantWrapperPtr::dynamicCast(OBJECTADAPTER->find([ident identity__]))->getServant();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObject*) findFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->findFacet([ident identity__], fromNSString(facet));
        return IceObjC::ServantWrapperPtr::dynamicCast(wrapper)->getServant();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSDictionary*) findAllFacets:(ICEIdentity*)ident
{
    try
    {
        Ice::FacetMap wrappers = OBJECTADAPTER->findAllFacets([ident identity__]);
        NSMutableDictionary* servants = [[NSMutableDictionary alloc] initWithCapacity:wrappers.size()];
        for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
        {
            [servants setObject:IceObjC::ServantWrapperPtr::dynamicCast(p->second)->getServant() forKey:toObjC(p->first)];
        }
        return servants;
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObject*) findByProxy:(id<ICEObjectPrx>)proxy
{
    try
    {
        Ice::ObjectPrx prx = [(ICEObjectPrx*)proxy objectPrx__];
        return IceObjC::ServantWrapperPtr::dynamicCast(OBJECTADAPTER->findByProxy(prx))->getServant();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)ident
{
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createProxy([ident identity__])];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) createDirectProxy:(ICEIdentity*)ident
{
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createDirectProxy([ident identity__])];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) createIndirectProxy:(ICEIdentity*)ident
{
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createIndirectProxy([ident identity__])];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) setLocator:(id<ICELocatorPrx>)loc
{
    try
    {
        OBJECTADAPTER->setLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) refreshPublishedEndpoints
{
    try
    {
        OBJECTADAPTER->refreshPublishedEndpoints();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
