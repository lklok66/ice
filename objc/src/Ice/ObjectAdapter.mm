// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
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
    try
    {
        return [ICECommunicator wrapperWithCxxObject:OBJECTADAPTER->getCommunicator().get()];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->add([servant object__], [ident identity])];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacet([servant object__], [ident identity],
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->addFacetWithUUID([servant object__],
                                                                                      fromNSString(facet))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) addDefaultServant:(ICEObject*)servant category:(NSString*)category
{
    try
    {
        Ice::ServantLocatorPtr servantLocator = new DefaultServantLocator([servant object__]);
        OBJECTADAPTER->addServantLocator(servantLocator, fromNSString(category));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(ICEObject*) remove:(ICEIdentity*)ident
{
    try
    {
        Ice::ObjectPtr wrapper = OBJECTADAPTER->remove([ident identity]);
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
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
        Ice::ObjectPtr wrapper = OBJECTADAPTER->removeFacet([ident identity], fromNSString(facet));
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
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
        Ice::FacetMap wrappers = OBJECTADAPTER->removeAllFacets([ident identity]);
        NSMutableDictionary* servants = [[NSMutableDictionary alloc] initWithCapacity:wrappers.size()];
        for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
        {
            NSObject* key = toObjC(p->first);
            [servants setObject:IceObjC::ObjectWrapperPtr::dynamicCast(p->second)->getObject() forKey:key];
            [key release];
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
        Ice::ObjectPtr wrapper = OBJECTADAPTER->find([ident identity]);
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
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
        Ice::ObjectPtr wrapper = OBJECTADAPTER->findFacet([ident identity], fromNSString(facet));
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
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
        Ice::FacetMap wrappers = OBJECTADAPTER->findAllFacets([ident identity]);
        NSMutableDictionary* servants = [[NSMutableDictionary alloc] initWithCapacity:wrappers.size()];
        for(Ice::FacetMap::const_iterator p = wrappers.begin(); p != wrappers.end(); ++p)
        {
            NSObject* key = toObjC(p->first);
            [servants setObject:IceObjC::ObjectWrapperPtr::dynamicCast(p->second)->getObject() forKey:key];
            [key release];
        }
        return [servants autorelease];
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
        Ice::ObjectPtr wrapper = OBJECTADAPTER->findByProxy([(ICEObjectPrx*)proxy objectPrx__]);
        return [[IceObjC::ObjectWrapperPtr::dynamicCast(wrapper)->getObject() retain] autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObject*) findDefaultServant:(NSString*)category
{
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
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)ident
{
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createProxy([ident identity])];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createDirectProxy([ident identity])];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:OBJECTADAPTER->createIndirectProxy([ident identity])];
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
