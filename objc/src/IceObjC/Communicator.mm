// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/CommunicatorI.h>
#import <IceObjC/PropertiesI.h>
#import <IceObjC/ProxyI.h>
#import <IceObjC/IdentityI.h>
#import <IceObjC/LoggerI.h>
#import <IceObjC/ObjectAdapterI.h>
#import <IceObjC/Router.h>
#import <IceObjC/Locator.h>
#import <IceObjC/Util.h>

#include <Ice/Router.h>
#include <Ice/Locator.h>

#define COMMUNICATOR ((Ice::Communicator*)communicator__)

@implementation ICECommunicator (Internal)

-(ICECommunicator*) initWithCommunicator:(const Ice::CommunicatorPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    communicator__ = arg.get();
    COMMUNICATOR->__incRef();
    return [[ExceptionHandlerForwarder alloc] init:self];
}

-(Ice::Communicator*) communicator__
{
    return (Ice::Communicator*)communicator__;
}

-(void) dealloc
{
    COMMUNICATOR->__decRef();
    communicator__ = 0;
    [super dealloc];
}

+(ICECommunicator*) communicatorWithCommunicator:(const Ice::CommunicatorPtr&)arg
{
    return [[[ICECommunicator alloc] initWithCommunicator:arg] autorelease];
}

@end

@implementation ICECommunicator

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
        return FALSE; // Keep the compiler happy.
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
        return [toNSString(COMMUNICATOR->identityToString([ident identity__])) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObjectAdapter*) createObjectAdapter:(NSString*)name;
{
    try
    {
        return [ICEObjectAdapter objectAdapterWithObjectAdapter:COMMUNICATOR->createObjectAdapter(fromNSString(name))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObjectAdapter*) createObjectAdapterWithEndpoints:(NSString*)name endpoints:(NSString*)endpoints;
{
    try
    {
        return [ICEObjectAdapter objectAdapterWithObjectAdapter:COMMUNICATOR->createObjectAdapterWithEndpoints(fromNSString(name), fromNSString(endpoints))];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEObjectAdapter*) createObjectAdapterWithRouter:(NSString*)name router:(id<ICERouterPrx>)rtr
{
    try
    {
        Ice::RouterPrx router = Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)rtr objectPrx__]));
        return [ICEObjectAdapter objectAdapterWithObjectAdapter:COMMUNICATOR->createObjectAdapterWithRouter(fromNSString(name), router)];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICEProperties*) getProperties
{
    try
    {
        return [ICEProperties propertiesWithProperties:COMMUNICATOR->getProperties()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(ICELogger*) getLogger
{
    try
    {
        return [ICELogger loggerWithLogger:COMMUNICATOR->getLogger()];
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
