// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/ObjectAdapterI.h>
#import <Ice/CommunicatorI.h>
#import <Ice/ProxyI.h>
#import <Ice/IdentityI.h>
#import <Ice/ObjectI.h>
#import <Ice/Util.h>

#include <IceCpp/Locator.h>

@implementation ICEObjectAdapter

-(ICEObjectAdapter*) initWithObjectAdapter:(const Ice::ObjectAdapterPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    objectAdapter__ = arg.get();
    objectAdapter__->__incRef();
    return self;
}

-(Ice::ObjectAdapter*) objectAdapter__
{
    return (Ice::ObjectAdapter*)objectAdapter__;
}

-(void) dealloc
{
    objectAdapter__->__decRef();
    objectAdapter__ = 0;
    [super dealloc];
}

+(ICEObjectAdapter*) objectAdapterWithObjectAdapter:(const Ice::ObjectAdapterPtr&)arg
{
    if(!arg)
    {
        return nil;
    }
    else
    {
        return [[[ICEObjectAdapter alloc] initWithObjectAdapter:arg] autorelease];
    }
}

//
// @protocol ICEObjectAdapter methods.
//

-(id<ICECommunicator>) getCommunicator
{
    try
    {
        return [ICECommunicator communicatorWithCommunicator:objectAdapter__->getCommunicator()];
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
        return [toNSString(objectAdapter__->getName()) autorelease];
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
        objectAdapter__->activate();
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
        objectAdapter__->hold();
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
        objectAdapter__->waitForHold();
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
        objectAdapter__->deactivate();
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
        objectAdapter__->waitForDeactivate();
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
        return objectAdapter__->isDeactivated();
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
        objectAdapter__->destroy();
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:objectAdapter__->add([servant object__], [ident identity__])];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:objectAdapter__->addFacet([servant object__], [ident identity__],
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:objectAdapter__->addWithUUID([servant object__])];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:objectAdapter__->addFacetWithUUID([servant object__],
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
        Ice::ObjectPtr wrapper = objectAdapter__->remove([ident identity__]);
        ICEObject* servant = IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject();
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
        Ice::ObjectPtr wrapper = objectAdapter__->removeFacet([ident identity__], fromNSString(facet));
        ICEObject* servant = IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject();
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
        Ice::FacetMap wrappers = objectAdapter__->removeAllFacets([ident identity__]);
        NSMutableDictionary* servants = [[NSMutableDictionary alloc] initWithCapacity:wrappers.size()];
        for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
        {
            NSObject* key = toObjC(p->first);
            NSObject* value = IceObjC::ObjectWrapperPtr::dynamicCast(p->second)->getObject();
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
        return IceObjC::ObjectWrapperPtr::dynamicCast(objectAdapter__->find([ident identity__]))->getObject();
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
        Ice::ObjectPtr wrapper = objectAdapter__->findFacet([ident identity__], fromNSString(facet));
        return IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject();
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
        Ice::FacetMap wrappers = objectAdapter__->findAllFacets([ident identity__]);
        NSMutableDictionary* servants = [[NSMutableDictionary alloc] initWithCapacity:wrappers.size()];
        for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
        {
            [servants setObject:IceObjC::ObjectWrapperPtr::dynamicCast(p->second)->getObject() forKey:toObjC(p->first)];
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
        return IceObjC::ObjectWrapperPtr::dynamicCast(objectAdapter__->findByProxy(prx))->getObject();
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:objectAdapter__->createProxy([ident identity__])];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:objectAdapter__->createDirectProxy([ident identity__])];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:objectAdapter__->createIndirectProxy([ident identity__])];
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
        objectAdapter__->setLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
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
        objectAdapter__->refreshPublishedEndpoints();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
