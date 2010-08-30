// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/ObjectAdapterI.h>
#import <Ice/CommunicatorI.h>
#import <Ice/ProxyI.h>
#import <Ice/IdentityI.h>
#import <Ice/ObjectI.h>
#import <Ice/Util.h>

#include <IceCpp/Locator.h>
#include <IceCpp/ServantLocator.h>

namespace
{
class DefaultServantLocator : public Ice::ServantLocator
{
public:
    
    DefaultServantLocator(const Ice::ObjectPtr& s) :
        _servant(s)
    {
    }

    virtual Ice::ObjectPtr 
    locate(const Ice::Current&, Ice::LocalObjectPtr&)
    {
        return _servant;
    }

    virtual void 
    finished(const Ice::Current&, const Ice::ObjectPtr&, const Ice::LocalObjectPtr&)
    {
    }

    virtual void 
    deactivate(const std::string&)
    {
    }
    
    const Ice::ObjectPtr& 
    servant() const
    {
        return _servant;
    }

private:
    Ice::ObjectPtr _servant;
};
}

#define OBJECTADAPTER dynamic_cast<Ice::ObjectAdapter*>(static_cast<IceUtil::Shared*>(cxxObject_))

@implementation ICEObjectAdapter
-(Ice::ObjectAdapter*) adapter
{
    return OBJECTADAPTER;
}
//
// @protocol ICEObjectAdapter methods.
//

-(id<ICECommunicator>) getCommunicator
{
    NSException* nsex = nil;
    try
    {
        return [ICECommunicator wrapperWithCxxObject:OBJECTADAPTER->getCommunicator().get()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSString*) getName
{
    NSException* nsex = nil;
    try
    {
        return [toNSString(OBJECTADAPTER->getName()) autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) activate
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->activate();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) hold
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->hold();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) waitForHold
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->waitForHold();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) deactivate
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->deactivate();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) waitForDeactivate
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->waitForDeactivate();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(BOOL) isDeactivated
{
    NSException* nsex = nil;
    try
    {
        return OBJECTADAPTER->isDeactivated();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) destroy
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->destroy();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(id<ICEObjectPrx>) add:(ICEObject*)servant identity:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->add([servant object__], [ident identity])];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) addFacet:(ICEObject*)servant identity:(ICEIdentity*)ident facet:(NSString*)facet
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacet([servant object__], [ident identity],
                                                                              fromNSString(facet))];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) addWithUUID:(ICEObject*)servant
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addWithUUID([servant object__])];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) addFacetWithUUID:(ICEObject*)servant facet:(NSString*)facet
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacetWithUUID([servant object__],
                                                                                      fromNSString(facet))];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) addDefaultServant:(ICEObject*)servant category:(NSString*)category
{
    NSException* nsex = nil;
    try
    {
        Ice::ServantLocatorPtr servantLocator = new DefaultServantLocator([servant object__]);
        OBJECTADAPTER->addServantLocator(servantLocator, fromNSString(category));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(ICEObject*) remove:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->remove([ident identity]);
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}
-(ICEObject*) removeFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    NSException* nsex = nil;
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->removeFacet([ident identity], fromNSString(facet));
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSDictionary*) removeAllFacets:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        Ice::FacetMap wrappers = OBJECTADAPTER->removeAllFacets([ident identity]);
        NSMutableDictionary* servants = [[[NSMutableDictionary alloc] initWithCapacity:wrappers.size()] autorelease];
        for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
        {
            NSObject* key = toObjC(p->first);
            [servants setObject:IceObjC::ObjectWrapperPtr::dynamicCast(p->second)->getObject() forKey:key];
            [key release];
        }
        return servants;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEObject*) find:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->find([ident identity]);
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEObject*) findFacet:(ICEIdentity*)ident facet:(NSString*)facet
{
    NSException* nsex = nil;
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->findFacet([ident identity], fromNSString(facet));
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSDictionary*) findAllFacets:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        Ice::FacetMap wrappers = OBJECTADAPTER->findAllFacets([ident identity]);
        NSMutableDictionary* servants = [[[NSMutableDictionary alloc] initWithCapacity:wrappers.size()] autorelease];
        for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
        {
            NSObject* key = toObjC(p->first);
            [servants setObject:IceObjC::ObjectWrapperPtr::dynamicCast(p->second)->getObject() forKey:key];
            [key release];
        }
        return servants;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEObject*) findByProxy:(id<ICEObjectPrx>)proxy
{
    NSException* nsex = nil;
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->findByProxy([(ICEObjectPrx*)proxy objectPrx__]);
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEObject*) findDefaultServant:(NSString*)category
{
    NSException* nsex = nil;
    try
    {
        Ice::ServantLocatorPtr servantLocator = OBJECTADAPTER->findServantLocator(fromNSString(category));
        if(servantLocator == 0)
        {
            return nil;
        }
        DefaultServantLocator* defaultServantLocator = dynamic_cast<DefaultServantLocator*>(servantLocator.get());
        if(defaultServantLocator == 0)
        {
            return nil; // should never happen!
        }
        else
        {
            Ice::ObjectPtr wrapper = defaultServantLocator->servant();
            return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
        }
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createProxy([ident identity])];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) createDirectProxy:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createDirectProxy([ident identity])];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) createIndirectProxy:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createIndirectProxy([ident identity])];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) setLocator:(id<ICELocatorPrx>)loc
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->setLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

-(void) refreshPublishedEndpoints
{
    NSException* nsex = nil;
    try
    {
        OBJECTADAPTER->refreshPublishedEndpoints();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}

@end
