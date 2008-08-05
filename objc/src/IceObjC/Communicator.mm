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
    return self;
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
    COMMUNICATOR->destroy();
}

-(void) shutdown
{
    COMMUNICATOR->shutdown();
}

-(void) waitForShutdown
{
    COMMUNICATOR->waitForShutdown();
}

-(BOOL) isShutdown
{
    return COMMUNICATOR->isShutdown();
}

-(id<ICEObjectPrx>) stringToProxy:(NSString*)str
{
    return [ICEObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->stringToProxy([str UTF8String])];
}

-(NSString*)proxyToString:(id<ICEObjectPrx>)obj
{
    return [NSString stringWithUTF8String:COMMUNICATOR->proxyToString([(ICEObjectPrx*)obj objectPrx__]).c_str()];
}

-(id<ICEObjectPrx>)propertyToProxy:(NSString*)property
{
    return [ICEObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->propertyToProxy([property UTF8String])];
}

-(ICEIdentity*) stringToIdentity:(NSString*)str
{
    return [ICEIdentity identityWithIdentity:COMMUNICATOR->stringToIdentity([str UTF8String])];
}

-(NSString*) identityToString:(ICEIdentity*)ident
{
    return [NSString stringWithUTF8String:COMMUNICATOR->identityToString([ident identity__]).c_str()];
}

-(ICEObjectAdapter*) createObjectAdapterWithRouter:(NSString*)name router:(id<ICERouterPrx>)rtr
{
    return nil;
}

-(ICEProperties*) getProperties
{
    return [ICEProperties propertiesWithProperties:COMMUNICATOR->getProperties()];
}

-(ICELogger*) getLogger
{
    return [ICELogger loggerWithLogger:COMMUNICATOR->getLogger()];
}

-(id<ICERouterPrx>) getDefaultRouter
{
    return (id<ICERouterPrx>)[ICERouterPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultRouter()];
}

-(void) setDefaultRouter:(id<ICERouterPrx>)rtr
{
    COMMUNICATOR->setDefaultRouter(Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)rtr objectPrx__])));
}

-(id<ICELocatorPrx>) getDefaultLocator
{
    return (id<ICELocatorPrx>)[ICELocatorPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultLocator()];
}

-(void) setDefaultLocator:(id<ICELocatorPrx>)loc
{
    COMMUNICATOR->setDefaultLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
}

-(void) flushBatchRequests
{
    COMMUNICATOR->flushBatchRequests();
}

@end
