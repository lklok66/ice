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

@implementation ICECommunicator

-(ICECommunicator*) initWithCommunicator:(const Ice::CommunicatorPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    communicator__ = arg.get();
    communicator__->__incRef();
    return self;
}

-(Ice::Communicator*) communicator__
{
    return (Ice::Communicator*)communicator__;
}

-(void) dealloc
{
    communicator__->__decRef();
    communicator__ = 0;
    [super dealloc];
}

+(ICECommunicator*) communicatorWithCommunicator:(const Ice::CommunicatorPtr&)arg
{
    return [[[ICECommunicator alloc] initWithCommunicator:arg] autorelease];
}

//
// Methods from @protocol ICECommunicator
//

-(void) destroy
{
    try
    {
        communicator__->destroy();
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
        communicator__->shutdown();
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
        communicator__->waitForShutdown();
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
        return communicator__->isShutdown();
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:communicator__->stringToProxy(fromNSString(str))];
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
        return [toNSString(communicator__->proxyToString([(ICEObjectPrx*)obj objectPrx__])) autorelease];
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
        return [ICEObjectPrx objectPrxWithObjectPrx__:communicator__->propertyToProxy(fromNSString(property))];
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
        return [ICEIdentity identityWithIdentity:communicator__->stringToIdentity(fromNSString(str))];
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
        return [toNSString(communicator__->identityToString([ident identity__])) autorelease];
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
        return [ICEObjectAdapter objectAdapterWithObjectAdapter:communicator__->createObjectAdapter(fromNSString(name))];
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
        return [ICEObjectAdapter objectAdapterWithObjectAdapter:communicator__->createObjectAdapterWithEndpoints(fromNSString(name), fromNSString(endpoints))];
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
        return [ICEObjectAdapter objectAdapterWithObjectAdapter:communicator__->createObjectAdapterWithRouter(fromNSString(name), router)];
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
        return [ICEProperties propertiesWithProperties:communicator__->getProperties()];
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
        return [ICELogger loggerWithLogger:communicator__->getLogger()];
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
        return (id<ICERouterPrx>)[ICERouterPrx objectPrxWithObjectPrx__:communicator__->getDefaultRouter()];
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
        communicator__->setDefaultRouter(Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)rtr objectPrx__])));
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
        return (id<ICELocatorPrx>)[ICELocatorPrx objectPrxWithObjectPrx__:communicator__->getDefaultLocator()];
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
        communicator__->setDefaultLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
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
        communicator__->flushBatchRequests();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
