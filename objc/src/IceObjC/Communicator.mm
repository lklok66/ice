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

@implementation Ice_Communicator (Internal)

-(Ice_Communicator*) initWithCommunicator:(const Ice::CommunicatorPtr&)arg
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

+(Ice_Communicator*) communicatorWithCommunicator:(const Ice::CommunicatorPtr&)arg
{
    return [[[Ice_Communicator alloc] initWithCommunicator:arg] autorelease];
}

@end

@implementation Ice_Communicator

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

-(id<Ice_ObjectPrx>) stringToProxy:(NSString*)str
{
    return [Ice_ObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->stringToProxy([str UTF8String])];
}

-(NSString*)proxyToString:(id<Ice_ObjectPrx>)obj
{
    return [NSString stringWithUTF8String:COMMUNICATOR->proxyToString([(Ice_ObjectPrx*)obj objectPrx__]).c_str()];
}

-(id<Ice_ObjectPrx>)propertyToProxy:(NSString*)property
{
    return [Ice_ObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->propertyToProxy([property UTF8String])];
}

-(Ice_Identity*) stringToIdentity:(NSString*)str
{
    return [Ice_Identity identityWithIdentity:COMMUNICATOR->stringToIdentity([str UTF8String])];
}

-(NSString*) identityToString:(Ice_Identity*)ident
{
    return [NSString stringWithUTF8String:COMMUNICATOR->identityToString([ident identity__]).c_str()];
}

-(Ice_ObjectAdapter*) createObjectAdapterWithRouter:(NSString*)name router:(id<Ice_RouterPrx>)rtr
{
    return nil;
}

-(Ice_Properties*) getProperties
{
    return [Ice_Properties propertiesWithProperties:COMMUNICATOR->getProperties()];
}

-(Ice_Logger*) getLogger
{
    return [Ice_Logger loggerWithLogger:COMMUNICATOR->getLogger()];
}

-(id<Ice_RouterPrx>) getDefaultRouter
{
    return (id<Ice_RouterPrx>)[Ice_RouterPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultRouter()];
}

-(void) setDefaultRouter:(id<Ice_RouterPrx>)rtr
{
    COMMUNICATOR->setDefaultRouter(Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(Ice_ObjectPrx*)rtr objectPrx__])));
}

-(id<Ice_LocatorPrx>) getDefaultLocator
{
    return (id<Ice_LocatorPrx>)[Ice_LocatorPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultLocator()];
}

-(void) setDefaultLocator:(id<Ice_LocatorPrx>)loc
{
    COMMUNICATOR->setDefaultLocator(Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(Ice_ObjectPrx*)loc objectPrx__])));
}

-(void) flushBatchRequests
{
    COMMUNICATOR->flushBatchRequests();
}

@end
