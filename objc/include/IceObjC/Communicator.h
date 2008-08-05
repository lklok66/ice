// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

//
// Forward declarations
//
@protocol ICEObjectPrx;
@protocol ICERouterPrx;
@protocol ICELocatorPrx;
@class ICELogger;
@class ICEProperties;
@class ICEIdentity;
@class ICEObjectAdapter;

@interface ICECommunicator : NSObject
{
    void* communicator__;
}

-(void) destroy;
-(void) shutdown;
-(void) waitForShutdown;
-(BOOL) isShutdown;
-(id<ICEObjectPrx>) stringToProxy:(NSString*)str;
-(NSString*)proxyToString:(id<ICEObjectPrx>)obj;
-(id<ICEObjectPrx>)propertyToProxy:(NSString*)property;
-(ICEIdentity*) stringToIdentity:(NSString*)str;
-(NSString*) identityToString:(ICEIdentity*)ident;
-(ICEObjectAdapter*) createObjectAdapter:(NSString*)name;
-(ICEObjectAdapter*) createObjectAdapterWithEndpoints:(NSString*)name endpoints:(NSString*)endpoints;
-(ICEObjectAdapter*) createObjectAdapterWithRouter:(NSString*)name router:(id<ICERouterPrx>)rtr;
//-(void) addObjectFactory(ObjectFactory factory, NSString* id);
//-(ObjectFactory findObjectFactory(NSString* id);
//-(ICEImplicitContext*) getImplicitContext;
-(ICEProperties*) getProperties;
-(ICELogger*) getLogger;
//-(ICEStats*) getStats;
-(id<ICERouterPrx>) getDefaultRouter;
-(void) setDefaultRouter:(id<ICERouterPrx>)rtr;
-(id<ICELocatorPrx>) getDefaultLocator;
-(void) setDefaultLocator:(id<ICELocatorPrx>)loc;
//-(PluginManager*) getPluginManager;
-(void) flushBatchRequests;
//Object* getAdmin;
//void addAdminFacet(Object servant, NSString* facet);
//Object removeAdminFacet(NSString* facet);

@end

