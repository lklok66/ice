// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/CommunicatorI.h>
#import <Ice/PropertiesI.h>
#import <Ice/ProxyI.h>
#import <Ice/IdentityI.h>
#import <Ice/LoggerI.h>
#import <Ice/ObjectAdapterI.h>
#import <Ice/Router.h>
#import <Ice/Locator.h>
#import <Ice/Util.h>

#include <IceCpp/Router.h>
#include <IceCpp/Locator.h>

#define COMMUNICATOR dynamic_cast<Ice::Communicator*>(static_cast<IceUtil::Shared*>(cxxObject_))

@implementation ICECommunicator
-(id) initWithCxxObject:(IceUtil::Shared*)cxxObject
{
    if(![super initWithCxxObject:cxxObject])
    {
        return nil;
    }
    adapters_ = [[NSMutableSet alloc] init];
    return self;
}
-(Ice::Communicator*) communicator
{
    return (Ice::Communicator*)cxxObject_;
}
-(void)removeObjectAdapter:(ICEObjectAdapter*)adapter
{
    [adapters_ removeObject:adapter];
}
-(void) dealloc
{
    [adapters_ release];
    [super dealloc];
}
//
// Methods from @protocol ICECommunicator
//

-(void) destroy
{
    try
    {
        COMMUNICATOR->destroy();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
    [adapters_ removeAllObjects];
}

-(void) shutdown
{
    try
    {
        COMMUNICATOR->shutdown();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) waitForShutdown
{
    try
    {
        COMMUNICATOR->waitForShutdown();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(BOOL) isShutdown
{
    try
    {
        return COMMUNICATOR->isShutdown();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>) stringToProxy:(NSString*)str
{
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->stringToProxy(fromNSString(str))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSString*)proxyToString:(id<ICEObjectPrx>)obj
{
    try
    {
        return [toNSString(COMMUNICATOR->proxyToString([(ICEObjectPrx*)obj objectPrx__])) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectPrx>)propertyToProxy:(NSString*)property
{
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->propertyToProxy(fromNSString(property))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEIdentity*) stringToIdentity:(NSString*)str
{
    try
    {
        return [ICEIdentity identityWithIdentity:COMMUNICATOR->stringToIdentity(fromNSString(str))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(NSString*) identityToString:(ICEIdentity*)ident
{
    try
    {
        return [toNSString(COMMUNICATOR->identityToString([ident identity])) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectAdapter>) createObjectAdapter:(NSString*)name;
{
    try
    {
        ICEObjectAdapter* adapter = [ICEObjectAdapter wrapperWithCxxObject:
                                                          COMMUNICATOR->createObjectAdapter(
                                                              fromNSString(name)).get()];
        [adapters_ addObject:adapter];
        return adapter;
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectAdapter>) createObjectAdapterWithEndpoints:(NSString*)name endpoints:(NSString*)endpoints;
{
    try
    {
        ICEObjectAdapter* adapter = [ICEObjectAdapter wrapperWithCxxObject:
                                                          COMMUNICATOR->createObjectAdapterWithEndpoints(
                                                              fromNSString(name), fromNSString(endpoints)).get()];
        [adapters_ addObject:adapter];
        return adapter;
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEObjectAdapter>) createObjectAdapterWithRouter:(NSString*)name router:(id<ICERouterPrx>)rtr
{
    try
    {
        Ice::RouterPrx router = Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)rtr objectPrx__]));
        ICEObjectAdapter* adapter = [ICEObjectAdapter wrapperWithCxxObject:
                                                          COMMUNICATOR->createObjectAdapterWithRouter(
                                                              fromNSString(name), router).get()];
        [adapters_ addObject:adapter];
        return adapter;
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICEProperties>) getProperties
{
    try
    {
        return [ICEProperties wrapperWithCxxObject:COMMUNICATOR->getProperties().get()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICELogger>) getLogger
{
    try
    {
        return [ICELogger wrapperWithCxxObject:COMMUNICATOR->getLogger().get()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(id<ICERouterPrx>) getDefaultRouter
{
    try
    {
        return (id<ICERouterPrx>)[ICERouterPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultRouter()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) setDefaultRouter:(id<ICERouterPrx>)rtr
{
    try
    {
        COMMUNICATOR->setDefaultRouter(Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)rtr objectPrx__])));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(id<ICELocatorPrx>) getDefaultLocator
{
    try
    {
        return (id<ICELocatorPrx>)[ICELocatorPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultLocator()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) setDefaultLocator:(id<ICELocatorPrx>)loc
{
    try
    {
        COMMUNICATOR->setDefaultLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

-(void) flushBatchRequests
{
    try
    {
        COMMUNICATOR->flushBatchRequests();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
